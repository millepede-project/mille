import ctypes 
import pathlib 
import numpy as np 
from milleLibBindings import milleC

class milleReader:
    def __init__(self):
        from numpy.ctypeslib import ndpointer
        # Setup the argument types for the library's functions
        # init the list of open files
        self.fileIndex = -1
        pass

    def open(self, fname: str):
        err = ctypes.c_int(0) 
        fstr = fname.encode(encoding="utf8")
        fsize = len(fname) 
        milleC.openMille(fstr, fsize, err, -1)
        newIndex = ctypes.c_int(0)
        milleC.lastMilleInFileIndex(ctypes.pointer(newIndex))
        self.fileIndex = newIndex.value
        return err.value
    
    def close (self):
        milleC.closeMille(ctypes.c_int(self.fileIndex))

    def readRecordToBuffers(self, doubleBuffer: np.array, floatBuffer: np.array, intBuffer:np.array):
        stat = ctypes.c_int(0)
        nRecords = ctypes.c_int(len(doubleBuffer)) 
        milleC.readMille(doubleBuffer, floatBuffer, intBuffer, nRecords, stat, self.fileIndex)
        return stat.value, nRecords.value

    def readRecord(self, bufferSize=1000):

        stat = ctypes.c_int(0)
        nRecords = ctypes.c_int(bufferSize) 
        dnp = np.zeros(bufferSize, dtype=np.double)
        fnp = np.zeros(bufferSize, dtype=np.float32)
        inp = np.zeros(bufferSize, dtype=np.int32)

        stat, nRec = self.readRecordToBuffers(dnp, fnp, inp)

        # for undersized buffers, re-read
        if (stat == -4):
            milleC.resetMilleEntry(self.fileIndex)
            dnp.resize(nRec)
            fnp.resize(nRec)
            inp.resize(nRec)
            stat, nRec = self.readRecordToBuffers(dnp, fnp, inp)

        return (stat, inp[:nRec],  dnp[:nRec] )
    
    def rewindToStart(self):
        return milleC.resetMille(self.fileIndex)
    
    def rewindLastEntry(self):
        return milleC.resetMilleEntry(self.fileIndex)


if __name__ == "__main__":
    reader = milleReader ()
    res = reader.open("testMille.root")

    stat, ints, doubles = reader.readRecord()
    print ("Read outcome: Status ",stat,"read",len(doubles),"entries")

    reader.rewindLastEntry()

    bufDouble = np.zeros(1000, dtype=np.double)
    bufFloat = np.zeros(1000, dtype=np.float32)
    bufInt = np.zeros(1000, dtype=np.int32)

    stat2, nRead2 = reader.readRecordToBuffers(bufDouble, bufFloat, bufInt)
    print ("Alternative read outcome: Status ",stat2,"read",nRead2,"entries")
    assert((bufDouble[:nRead2] == doubles).all())
    reader.close()
