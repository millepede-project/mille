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
from milleLibBindings import milleC

## Class to wrap the C-Mille interface back into 
# object-oriented behaviour at the python level. 
class milleWriter:

    # Constructor - will directly open the output file. 
    # @param fname: Desired file name 
    # @param doublePrec: Enable double-precision storage of floats
    # @param keepZeros: Store zero-derivatives (not tested in pede) 
    # @param recordLengthHint: Initial write buffer size 
    # Will spawn a MilleRecord in the background, of a type 
    # determined based on the chosen file extension. 
    def __init__(self, fname: str, 
             doublePrec: bool = False, 
             keepZeros: bool = False, 
             recordLengthHint: int = 2000):
        
        self.fileIndex = -1
        # open the file using the C interface 
        fstr = fname.encode(encoding="utf8")
        fsize = len(fname) 
        milleC.open_mille_out(fstr, fsize, -1, doublePrec, keepZeros, recordLengthHint)
        # read the assigned index of the file handle, to internally use
        # for future calls 
        newIndex = ctypes.c_int(0)
        milleC.last_mille_out_file_index(ctypes.pointer(newIndex))
        self.fileIndex = newIndex.value
    
    # Close the attached file
    def close (self):
        milleC.close_mille_output(ctypes.c_int(self.fileIndex))
        self.fileIndex = -1

    # Add data (a measurement) to the current record
    # @param measurement: The measurement
    # @param error: The uncertainty associated to the measurement
    # @param localLabels: List of local parameter labels (type: unit32)
    # @param localDerivatives: List of local parameter derivatives (type: float64)
    # @param globalLabels: List of global parameter labels (type: int32)
    # @param globalDerivatives: List of global parameter derivatives (type: float64)
    def addData(self, measurement: float, error: float, 
                localLabels: np.array, 
                localDerivatives: np.array, 
                globalLabels: np.array, 
                globalDerivatives: np.array):
        milleC.add_mille_data(measurement, error, len(localLabels), localLabels, localDerivatives, len(globalDerivatives), globalLabels, globalDerivatives, self.fileIndex)


    # Add a special data segment to the current record
    # @param specialInt: List of integer values (type: int32)
    # @param specialFloat: List of floating point values (type: float64)
    # Both arrays are expected to be of the same size. 
    def addSpecial(self, specialInt: np.array,  specialFloat: np.array):
        milleC.add_mille_special(len(specialInt), specialInt, specialFloat, self.fileIndex) 

    # Directly copy (encoded) data into the Mille buffer
    # @param ints: Content for the integer buffer (type: int32)
    # @param floats: Content for the integer buffer  (type: float64)
    # Both arrays are expected to be of the same size. 
    def copyToBuffer(self,  ints: np.array, floats: np.array):
        milleC.copy_to_mille_buffer(len(ints), ints, floats, self.fileIndex) 

    # Finish the current record, write to storage, and prepare next record
    def write(self):
        milleC.write_mille_out(self.fileIndex)
    
    # Abort the current record
    def abort(self):
        milleC.abort_mille_record(self.fileIndex)

    # Flush the file to disk 
    def flush(self):
        milleC.flush_mille_output(self.fileIndex)

    # Close and re-open the output file. 
    # This will discard existing content. 
    def reset(self):
        milleC.reset_mille_output(self.fileIndex)



if __name__ == "__main__":

    ## DEMO! 

    # instantiate a writer - here with ROOT output
    writer = milleWriter("testMille.root")
    
    # dummy data
    measurement = 1.e-4
    error = 1.e-5 
    localLabels = np.array([1,3,4,5,6], dtype=np.uint32)
    localDeriv = np.array([1.,0.2, 0.2, 1.,-1.], dtype=np.float64)
    globalLabels = np.array([1001, 2001], dtype=np.int32)
    globalDeriv = np.array([1.,-1.], dtype=np.float64)

    # write our measurement 
    writer.addData(measurement, error, localLabels, localDeriv, globalLabels, globalDeriv) 

    # end the record, write it 
    writer.write()
    # and close the file, writing to disk. 
    writer.close()

    