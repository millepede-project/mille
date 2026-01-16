## \file
#  python interface to Mille reader 
#
# \author Maximilian Goblirsch-Kolb, DESY (2026) 
#
#  \copyright
#  Copyright (c) 2026 Deutsches Elektronen-Synchroton,
#  Member of the Helmholtz Association, (DESY), HAMBURG, GERMANY \n\n
#  This library is free software; you can redistribute it and/or modify
#  it under the terms of the GNU Library General Public License as
#  published by the Free Software Foundation; either version 2 of the
#  License, or (at your option) any later version. \n\n
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Library General Public License for more details. \n\n
#  You should have received a copy of the GNU Library General Public
#  License along with this program (see the file COPYING.LIB for more
#  details); if not, write to the Free Software Foundation, Inc.,
#  675 Mass Ave, Cambridge, MA 02139, USA.


import ctypes 
import numpy as np 
# In the CI, we do not have the Mille module in place yet 
try: 
    from milleLibBindings import milleC
except ModuleNotFoundError:
    # this is the import expected to happen outside the CI! 
    try: 
        from Mille.milleLibBindings import milleC
    except ModuleNotFoundError:
        raise


## Class to wrap the C-Mille interface back into 
# object-oriented behaviour at the python level. 
class milleReader:

    # Construct an (empty) instance. File opening 
    # happens separately to be able to check the outcome. 
    def __init__(self):
        self.fileIndex = -1

    # Open a file for reading. Mille will try to 
    # instantiate an appropriate reader class in the background,
    # based on the extension of the file. 
    # The return code will indicate the outcome. A value of 0 
    # indicates a success. 
    # @param fname: file name 
    def open(self, fname: str):
        # jump through a few hoops to satisfy the (fortran-compatible...) interface
        err = ctypes.c_int(0) 
        fstr = fname.encode(encoding="utf8")
        fsize = len(fname) 
        # open the file - will spawn a MilleReader in the background
        milleC.open_mille(fstr, fsize, err, -1)
        # find out the index assigned to "our" reader in the C-interface, 
        # and keep track of it internally 
        newIndex = ctypes.c_int(0)
        milleC.last_mille_in_file_index(ctypes.pointer(newIndex))
        self.fileIndex = newIndex.value
        # finally, return the status code 
        return err.value
    
    # Closes the attached file 
    def close (self):
        milleC.close_mille(ctypes.c_int(self.fileIndex))

    # read the next record from the file to the user- supplied buffers.
    # @param doubleBuffer: An array of type float64 
    # @param floatBuffer: An array of type float32 
    # @param intBuffer: An array of type int32 
    # The Buffers need to have sufficent size to fit the record. 
    # The integer record will be written into intBuffer, and the 
    # floating point record into doubleBuffer. 
    # The function returns a status code and the number of read entries. 
    # For status code -4 (buffers too small), the second return value
    # indicates the minimum required buffer size. 
    def readRecordToBuffers(self, doubleBuffer: np.array, floatBuffer: np.array, intBuffer:np.array):
        stat = ctypes.c_int(0)
        nRecords = ctypes.c_int(len(doubleBuffer)) 
        milleC.read_mille(doubleBuffer, floatBuffer, intBuffer, nRecords, stat, self.fileIndex)
        return stat.value, nRecords.value

    # read the next record from the file - automatically generated buffers. 
    # @param bufferSize: Default buffer size to try. 
    # Will return a 3-tuple: Status code, integer content, floating-point content. 
    def readRecord(self, bufferSize=1000):

        stat = ctypes.c_int(0)
        nRecords = ctypes.c_int(bufferSize) 
        dnp = np.zeros(bufferSize, dtype=np.double)
        fnp = np.zeros(bufferSize, dtype=np.float32)
        inp = np.zeros(bufferSize, dtype=np.int32)

        stat, nRec = self.readRecordToBuffers(dnp, fnp, inp)

        # for undersized buffers, re-read with enlarged buffers
        if (stat == -4):
            milleC.reset_mille_entry(self.fileIndex)
            # the nRec value from the failed attempt tells us the minimum
            # required buffer size. 
            dnp.resize(nRec)
            fnp.resize(nRec)
            inp.resize(nRec)
            stat, nRec = self.readRecordToBuffers(dnp, fnp, inp)
        # truncate buffers to the number of read entries when returning
        return (stat, inp[:nRec],  dnp[:nRec] )
    
    # rewind the file to the first entry 
    def skipRecord(self):
        return milleC.skip_mille(self.fileIndex)
    
    # rewind the file to the first entry 
    def rewindToStart(self):
        return milleC.reset_mille(self.fileIndex)
    
    # rewind the file to the start of the previously accessed entry. 
    def rewindLastEntry(self):
        return milleC.reset_mille_entry(self.fileIndex)


if __name__ == "__main__":
    # DEMO: 

    # instantiate a reader and connect to the file
    reader = milleReader ()
    res = reader.open("testMille.root")
    assert(res == 0) # check for valid file open 

    # read one record 
    stat, ints, doubles = reader.readRecord()
    print ("Read outcome: Status ",stat,"read",len(doubles),"entries")

    # rewind to read the same record again, using the second read interface
    reader.rewindLastEntry()

    # assign buffers on the user side
    bufDouble = np.zeros(1000, dtype=np.double)
    bufFloat = np.zeros(1000, dtype=np.float32)
    bufInt = np.zeros(1000, dtype=np.int32)

    # and read into the buffers
    stat2, nRead2 = reader.readRecordToBuffers(bufDouble, bufFloat, bufInt)
    print ("Alternative read outcome: Status ",stat2,"read",nRead2,"entries")
    # ensure consistency of the output 
    assert((bufDouble[:nRead2] == doubles).all())
    # close the file
    reader.close()
