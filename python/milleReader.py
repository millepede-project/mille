import ctypes 
import pathlib 
import numpy as np 

class milleReader:
    def __init__(self):
        from numpy.ctypeslib import ndpointer
        self.milleC = ctypes.CDLL("libMilleC.so")
        self.milleC.openMille.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_int)]
        self.milleC.readMille.argtypes = [ndpointer(ctypes.c_double, flags="C_CONTIGUOUS"),
                                          ndpointer(ctypes.c_float, flags="C_CONTIGUOUS"),
                                          ndpointer(ctypes.c_int, flags="C_CONTIGUOUS"),
                                          ctypes.POINTER(ctypes.c_int), 
                                          ctypes.c_int, 
                                          ctypes.POINTER(ctypes.c_int)]
        self.milleC.lastMilleFileIndex.argtypes = [ctypes.POINTER(ctypes.c_int)]
        self.milleC.closeMille.argtypes = [ctypes.c_int]
        self.milleC.resetMilleEntry.argtypes = [ctypes.c_int]
        self.milleC.resetMille.argtypes = [ctypes.c_int]
        self.openFiles = []
        pass

    def open(self, fname: str, fileIndex = -1):
        err = ctypes.c_int(0) 
        fstr = fname.encode(encoding="utf8")
        fsize = len(fname) 
        self.milleC.openMille(fstr, fsize,fileIndex, err)
        newIndex = ctypes.c_int(0)
        self.milleC.lastMilleFileIndex(ctypes.pointer(newIndex))
        self.openFiles += [newIndex.value]
        return err.value
    
    def close (self, fileIndex: int):
        self.milleC.closeMille(ctypes.c_int(fileIndex))
        self.openFiles = [f for f in self.openFiles if f is not fileIndex]

    def readRecordToBuffers(self, fileIndex: int, doubleBuffer: np.array, floatBuffer: np.array, intBuffer:np.array):
        stat = ctypes.c_int(0)
        nRecords = ctypes.c_int(len(doubleBuffer)) 
        self.milleC.readMille(doubleBuffer, floatBuffer, intBuffer, nRecords, fileIndex, stat)
        return stat.value, nRecords.value

    def readRecord(self, fileIndex: int, bufferSize=1000):

        stat = ctypes.c_int(0)
        nRecords = ctypes.c_int(bufferSize) 
        dnp = np.zeros(bufferSize, dtype=np.double)
        fnp = np.zeros(bufferSize, dtype=np.float32)
        inp = np.zeros(bufferSize, dtype=np.int32)

        stat, nRec = self.readRecordToBuffers(fileIndex, dnp, fnp, inp)

        # for undersized buffers, re-read
        if (stat == -4):
            self.milleC.resetMilleEntry(fileIndex)
            dnp.resize(nRec)
            fnp.resize(nRec)
            inp.resize(nRec)
            stat, nRec = self.readRecordToBuffers(fileIndex, dnp, fnp, inp)

        return (stat, inp[:nRec],  dnp[:nRec] )
    
    def rewindToStart(self, fileIndex):
        return self.milleC.resetMille(fileIndex)
    
    def rewindLastEntry(self, fileIndex):
        return self.milleC.resetMilleEntry(fileIndex)


if __name__ == "__main__":
    reader = milleReader ()
    res = reader.open("mp2tst.root")
    f = reader.openFiles[-1]

    stat, ints, doubles = reader.readRecord(f)
    print ("Read outcome: Status ",stat,"read",len(doubles),"entries")

    reader.rewindLastEntry(f)

    bufDouble = np.zeros(1000, dtype=np.double)
    bufFloat = np.zeros(1000, dtype=np.float32)
    bufInt = np.zeros(1000, dtype=np.int32)

    stat2, nRead2 = reader.readRecordToBuffers(f, bufDouble, bufFloat, bufInt)
    print ("Alternative read outcome: Status ",stat2,"read",nRead2,"entries")
    assert((bufDouble[:nRead2] == doubles).all())
    reader.close(f)
