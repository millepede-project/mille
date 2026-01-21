
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
 - read: Read one record in encoded format to a user-provided buffer.  

This functionality is identical across all target languages, only the interfaces change subtly:

 ### C++ 

 In C++, the recommended way to instantiate a `MilleReader` or `MilleRecord` are the `spawnMilleReader` and `spawnMilleRecord` functions defined in `MilleFactory.h`. 
 You can either specify the file format to use via an enum, or it will be auto-deduced based on the file extension.

 The two methods will return a `unique_ptr` to the reader / writer instance that the user can interact with. 
 Please see the `IMilleReader.h` and `MilleRecord.h` headers to identify the methods corresponding to the API components described above. 

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

