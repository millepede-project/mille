
 \file
 Mille library, usage instructions.

 \page usage_page Usage instructions


# Usage of Mille (in standalone code) 

Mille interaction inside Millepede-II or GBL is usually handled automatically by the respective packages. 
Please refer to the corresponding package documentation for details. 


## General principle 
The information stored by Mille consists of a series of *records*. Each record contains a number of *measurements*. 
For example, in High Energy Physics detector alignment, a record would correspond to a charged particle track, and each measurement would represent one detector measurement contributing to the track.
Each measurement is expected to contain the following information:
- A single (one-dimensional) **measured value** - in HEP alignment, a track residual. 
- An associated **uncertainty** - in HEP alignment, the position uncertainty  
- A set of **local parameters** the measurement depends on 
    - each has a *label* identifying it and a *derivative* containing the linearised impact on the measurement 
- A set of **global parameters** the measurement depends on 
    - each has a *label* identifying it and a *derivative* containing the linearised impact on the measurement 

This information is *encoded* when writing to a file - into two arrays, one of integers, and one of real numbers (either single- or double-precision). 
The details of the array storage then depend on the file type. 

 In general, `Mille` provides two main components,
 - the `MilleRecord` is responsible for encoding and writing information. The user typically passes measurement, uncertainty, parameter labels, and derivatives separately. 
 - the `MilleReader` is responsible for reading back the encoded information. Typically, the decoding is expected to be done by the client, and the encoded arrays are returned. 

 Specialisations of both exist for each of the supported file formats. 

 In addition, a `MilleDecoder` is provided to decode the output of a `MilleReader` on the fly if required. 

## Interacting with Mille to write or retrieve information

 The following operations are supported: 

#### Writing (`MilleRecord`): 
 - instantiation: Will open the output file and prepare internal buffers
 - add data: Add one measurement to the current record. Will encode the data in the internal buffer, but not yet write it out. 
 - write record: Will write the current record to the output file and reset the internal buffer to start a new record when subsequent measurements are added.
 - abort record: Will drop the current buffer content, aborting the in-progress record without writing it out. Subsequent measurements will land in a fresh record.
 - flush: Will flush the target file to disk
 - close: Will close (and finalise) the output 

#### Reading(`MilleReader`): 
 - open: Opens a file for reading (and return a status code). 
 - close: Close a file 
 - reset: Rewind a file to start reading from the first entry 
 - rewind: Rewind a file by one record, going back to the start of the record last read. 
 - skip: Skip one record. 
 - read: Read one record in encoded format to a user-provided buffer. This is described in more detail below.

This functionality is identical across all target languages, only the interfaces change subtly:

 ### C++ 

 In C++, the recommended way to instantiate a `MilleReader` or `MilleRecord` are the `spawnMilleReader` and `spawnMilleRecord` functions defined in `MilleFactory.h`. 
 You can either specify the file format to use via an enum, or it will be auto-deduced based on the file extension.

 The two methods will return a `unique_ptr` to the reader / writer instance that the user can interact with. 
 Please see the `IMilleReader.h` and `MilleRecord.h` headers to identify the methods corresponding to the API components described above. 
 
 For the MilleRecord, closing the output is achieved by the destructor of the object - for example by `resetting` the `unique_ptr` or letting it go out of scope. 

 To have multiple in-progress files simultaneously, simply instantiate multiple readers / writers. 

 ### C 

 For C, the interface in `Mille/readMilleC.h` and `Mille/writeMilleC.h` provide a wrapper for interacting with the `MilleReader` and `MilleRecord`. 

 Instead of an object-oriented interface, a set of functions with `void` return type provide the same functionality, returning information by means of pointer and reference-type arguments. 

 Internally, these functions manage instances of the C++ interface objects in the background.

 To handle multiple in-flight files, a **file index** (integer) is used to identify each given file. You can either explicitly assign a number when calling the `open_mille` / `open_mille_out` functions, or let the API automatically assign a number by passing `-1` for the file number (default argument). In the latter case, the assigned number can be retrieved by calling `last_mille_in_file_index` / `last_mille_out_file_index` after the file was opened. When working with a single file, the file index can be ignored. 
  
 A `libMilleC` library implementing this interface with C-style linkage is built in addition to the `libMille2` library implementing the C++ interface. 

 ### Fortran

 The Fortran bindings are identical to the C-bindings. A `MilleF` interface module is provided that can be used in client code. `See Fortran/MilleF.f90`. Fortran projects using this should link against both the `MilleC` (for the API implementation) and the `MilleF` (for the interface layer) libraries.

 ### Python 3 

 The python3 bindings are created using the `ctypes` module to keep external dependencies to a minimum. 
 Two modules, `milleWriter.py` and `milleReader.py`, provide object-oriented interfaces that internally talk to the C bindings. The user does not have to pass file indices, this is handled by the bindings. The interface to the arrays stored in the records is based on `numpy` arrays to ensure adequate performance. The user can either pass arrays of their own (and expect them to be filled when reading a record), or receive arrays as a function return value (extra overhead).  

## Status codes and conventions when reading a Mille record

The standard interface for reading a Mille record involves the user passing three buffers, 
- an integer buffer
- a single-precision float buffer
- a double-precision float buffer.

All should be pre-allocated by the user and sized sufficiently to accommodate the information to be read. 

In the C++ API, there is a `MilleBuffer` convenience struct that contains the three arrays plus a buffer size integer, and allows them to be resized together as required. 

There are additional arguments that are used to transport status information: 
- an integer **buffer size** variable 
- an integer **status code** variable. 

The buffer size variable should be set by the user to the size of the passed buffers before calling the function. 

When reading a `Mille` record, the following outcomes are possible. As a general rule of thumb, a negative return code means something went wrong. 

#### Successful read
- the **status code** is either **4** (single-precision real array detected) or **8** (double-precision real array detected) 
- the returned **buffer size** (`recordLength` in the following) is the number of entries that were written into the buffers (smaller than or equal to the value passed by the user). 
- the first `recordLength` elements of the **integer buffer** will be populated with the encoded integer array representing the record 
- the first `recordLength` elements of the **double-precision float buffer** will always be populated with the encoded real array representing the record. This is the array recommended for use to process the real-value information, as it is guaranteed to always be populated. 
- Only if the status code is 4, the first `recordLength` elements of the **single-precision float buffer** will also be populated with the encoded real array representing the record. For double-precision float storage, this buffer will not be touched. 

After the successful read, the internal record counter of the file is incremented, and the next read operation will yield the subsequent record. 

#### End-of-file 

If the end of the file has been reached, a **status code of 0** will be returned. The buffers will not be touched, and the output buffer size will be reported as 0. 

#### Undersized buffers

If the buffers passed by the user (capacity indicated by the value of the **buffer size** when calling the read function) are smaller than the number of entries in the current record, 
- the **status code** will be set to **-4** 
- the returned **buffer size** will be set to the **negative number of entries in the record** - you can use this as a hint to resize the buffer to accommodate the data
- None of the buffers will be touched.

The internal record counter of the fill will still be incremented, and the next read operation would yield the subsequent record. You can use the *rewind entry* function of the record for a new read attempt after increasing the buffer size. 

#### Read errors 

Several error states are indicated by the **status code**:
- **-1**: Either the pointer to a needed buffer or the passed buffer size are invalid / null. Indicates an incorrect function call / bug on the user side.
- **-2**: There was a problem reading the length of the record. This indicates a corrupt input file. 
- **-8**: There was a problem reading the single-precision float information. This indicates a corrupt input file. 
- **-16**: There was a problem reading the integer information. This indicates a corrupt input file. 
- **-32**: There was a problem reading the double-precision float information. This indicates a corrupt input file. 

#### Putting it together 

You can find a typical use case for reading a record in `milleReaderTests.h`. 

In pseudo-code: 
```
myIntBuffer, myFloatBuffer, myDoubleBuffer = allocateSomeBuffers(); 
inBufferSize = myIntBuffer.size(); // assumed to be the same for all buffers

/// in some interfaces, the input buffer size is instead passed by reference and overwritten inside the call  
/// in some interfaces, the out status code is passed by pointer and overwritten inside the call
outBufferSize, outStatusCode = myMilleRecord.readToBuffer(myDoubleBuffer, myFloatBuffer, myIntBuffer, inBufferSize); 

/// good read 
if (outStatusCode == 4 || outStatusCode == 8) processMyReadRecordFurther(); 

/// done with the file - load the next one or finalise... 
else if (outStatusCode == 0) doSomethingForEndOfFile();

/// buffers too small 
else if (outStatusCode == -4){
    /// resize the buffers
    myIntBuffer.resize(outBufferSize); 
    myFloatBuffer.resize(outBufferSize); 
    myDoubleBuffer.resize(outBufferSize); 
    inBufferSize = outBufferSize; 
    /// rewind
    myMilleRecord.rewindEntry();

    /// now it should work! 
    outBufferSize, outStatusCode = myMilleRecord.readToBuffer(myDoubleBuffer, myFloatBuffer, myIntBuffer, inBufferSize); 
}

else if (outStatusCode < 0){
    /// corrupt input file or incorrect function call - can not recover
    raise_error(); 
}


```
