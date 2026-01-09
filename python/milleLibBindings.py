
from numpy.ctypeslib import ndpointer
import ctypes

# Setup the argument types for the library's functions
milleC = ctypes.CDLL("libMilleC.so")
milleC.openMille.restype = None  
milleC.openMille.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.POINTER(ctypes.c_int), ctypes.c_int]
milleC.readMille.restype = None  
milleC.readMille.argtypes = [ndpointer(ctypes.c_double, flags="C_CONTIGUOUS"),
                                    ndpointer(ctypes.c_float, flags="C_CONTIGUOUS"),
                                    ndpointer(ctypes.c_int, flags="C_CONTIGUOUS"),
                                    ctypes.POINTER(ctypes.c_int), 
                                    ctypes.POINTER(ctypes.c_int), 
                                    ctypes.c_int]
milleC.lastMilleInFileIndex.restype = None  
milleC.lastMilleInFileIndex.argtypes = [ctypes.POINTER(ctypes.c_int)]
milleC.closeMille.restype = None  
milleC.closeMille.argtypes = [ctypes.c_int]
milleC.resetMilleEntry.restype = None  
milleC.resetMilleEntry.argtypes = [ctypes.c_int]
milleC.resetMille.restype = None  
milleC.resetMille.argtypes = [ctypes.c_int]
milleC.skipMille.restype = None  
milleC.skipMille.argtypes = [ctypes.c_int]

milleC.openMilleOut.restype = None  
milleC.openMilleOut.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_bool, ctypes.c_bool, ctypes.c_int]
milleC.addMilleData.restype = None  
milleC.addMilleData.argtypes = [ctypes.c_double, ctypes.c_double, ctypes.c_uint, 
                                    ndpointer(ctypes.c_uint, flags="C_CONTIGUOUS"),
                                    ndpointer(ctypes.c_double, flags="C_CONTIGUOUS"),
                                    ctypes.c_int, 
                                    ndpointer(ctypes.c_int, flags="C_CONTIGUOUS"),
                                    ndpointer(ctypes.c_double, flags="C_CONTIGUOUS"),
                                    ctypes.c_int]
milleC.addMilleSpecial.restype = None  
milleC.addMilleSpecial.argtypes = [ctypes.c_int, 
                                    ndpointer(ctypes.c_double, flags="C_CONTIGUOUS"),
                                    ndpointer(ctypes.c_int, flags="C_CONTIGUOUS"),
                                    ctypes.c_int]
milleC.copyToMilleBuffer.restype = None  
milleC.copyToMilleBuffer.argtypes = [ctypes.c_uint, 
                                    ndpointer(ctypes.c_int, flags="C_CONTIGUOUS"),
                                    ndpointer(ctypes.c_double, flags="C_CONTIGUOUS"),
                                    ctypes.c_int]
milleC.abortMilleRecord.restype = None  
milleC.abortMilleRecord.argtypes = [ctypes.c_int]
milleC.flushMilleOutput.restype = None  
milleC.flushMilleOutput.argtypes = [ctypes.c_int]
milleC.resetMilleOutput.restype = None  
milleC.resetMilleOutput.argtypes = [ctypes.c_int]
milleC.lastMilleOutFileIndex.restype = None  
milleC.lastMilleOutFileIndex.argtypes = [ctypes.POINTER(ctypes.c_int)]
milleC.closeMilleOutput.restype = None  
milleC.closeMilleOutput.argtypes = [ctypes.c_int]
