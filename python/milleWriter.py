import ctypes 
import pathlib 
import numpy as np 
from milleLibBindings import milleC

class milleWriter:
    def __init__(self, fname: str, 
             doublePrec: bool = False, 
             keepZeros: bool = False, 
             recordLengthHint: int = 2000):
        
        # init the list of open files
        self.fileIndex = -1

        fstr = fname.encode(encoding="utf8")
        fsize = len(fname) 
        milleC.openMilleOut(fstr, fsize, -1, doublePrec, keepZeros, recordLengthHint)
        newIndex = ctypes.c_int(0)
        milleC.lastMilleOutFileIndex(ctypes.pointer(newIndex))
        self.fileIndex = newIndex.value
    
    def close (self):
        milleC.closeMilleOutput(ctypes.c_int(self.fileIndex))

    def addData(self, measurement: float, error: float, 
                localLabels: np.array, 
                localDerivatives: np.array, 
                globalLabels: np.array, 
                globalDerivatives: np.array):
        milleC.addMilleData(measurement, error, len(localLabels), localLabels, localDerivatives, len(globalDerivatives), globalLabels, globalDerivatives, self.fileIndex)

    def addSpecial(self,  specialFloat: np.array, specialInt: np.array):
        milleC.addMilleSpecial(len(specialInt), specialFloat, specialInt, self.fileIndex) 

    def write(self):
        milleC.writeMilleOut(self.fileIndex)

    def abort(self):
        milleC.abortMilleRecord(self.fileIndex)

    def flush(self):
        milleC.flushMilleOutput(self.fileIndex)

    def reset(self):
        milleC.resetMilleOutput(self.fileIndex)

    def close(self):
        milleC.closeMilleOutput(self.fileIndex)


if __name__ == "__main__":
    writer = milleWriter("testMille.root")
    # dummy data
    measurement = 1.e-4
    error = 1.e-5 
    localLabels = np.array([1,3,4,5,6], dtype=np.uint32)
    localDeriv = np.array([1.,0.2, 0.2, 1.,-1.], dtype=np.float64)
    globalLabels = np.array([1001, 2001], dtype=np.int32)
    globalDeriv = np.array([1.,-1.], dtype=np.float64)

    writer.addData(measurement, error, localLabels, localDeriv, globalLabels, globalDeriv) 

    writer.write()
    writer.close()

    