
from numpy.ctypeslib import ndpointer
import ctypes

# Setup the argument types for the library's functions
milleC = ctypes.CDLL("libMilleC.so")
milleC.open_mille.restype = None  
milleC.open_mille.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.POINTER(ctypes.c_int), ctypes.c_int]
milleC.read_mille.restype = None  
milleC.read_mille.argtypes = [ndpointer(ctypes.c_double, flags="C_CONTIGUOUS"),
                                    ndpointer(ctypes.c_float, flags="C_CONTIGUOUS"),
                                    ndpointer(ctypes.c_int, flags="C_CONTIGUOUS"),
                                    ctypes.POINTER(ctypes.c_int), 
                                    ctypes.POINTER(ctypes.c_int), 
                                    ctypes.c_int]
milleC.last_mille_in_file_index.restype = None  
milleC.last_mille_in_file_index.argtypes = [ctypes.POINTER(ctypes.c_int)]
milleC.close_mille.restype = None  
milleC.close_mille.argtypes = [ctypes.c_int]
milleC.reset_mille_entry.restype = None  
milleC.reset_mille_entry.argtypes = [ctypes.c_int]
milleC.reset_mille.restype = None  
milleC.reset_mille.argtypes = [ctypes.c_int]
milleC.skip_mille.restype = None  
milleC.skip_mille.argtypes = [ctypes.c_int]

milleC.open_mille_out.restype = None  
milleC.open_mille_out.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_bool, ctypes.c_bool, ctypes.c_int]
milleC.add_mille_data.restype = None  
milleC.add_mille_data.argtypes = [ctypes.c_double, ctypes.c_double, ctypes.c_uint, 
                                    ndpointer(ctypes.c_uint, flags="C_CONTIGUOUS"),
                                    ndpointer(ctypes.c_double, flags="C_CONTIGUOUS"),
                                    ctypes.c_int, 
                                    ndpointer(ctypes.c_int, flags="C_CONTIGUOUS"),
                                    ndpointer(ctypes.c_double, flags="C_CONTIGUOUS"),
                                    ctypes.c_int]
milleC.add_mille_special.restype = None  
milleC.add_mille_special.argtypes = [ctypes.c_int, 
                                    ndpointer(ctypes.c_double, flags="C_CONTIGUOUS"),
                                    ndpointer(ctypes.c_int, flags="C_CONTIGUOUS"),
                                    ctypes.c_int]
milleC.copy_to_mille_buffer.restype = None  
milleC.copy_to_mille_buffer.argtypes = [ctypes.c_uint, 
                                    ndpointer(ctypes.c_int, flags="C_CONTIGUOUS"),
                                    ndpointer(ctypes.c_double, flags="C_CONTIGUOUS"),
                                    ctypes.c_int]
milleC.abort_mille_record.restype = None  
milleC.abort_mille_record.argtypes = [ctypes.c_int]
milleC.flush_mille_output.restype = None  
milleC.flush_mille_output.argtypes = [ctypes.c_int]
milleC.reset_mille_output.restype = None  
milleC.reset_mille_output.argtypes = [ctypes.c_int]
milleC.last_mille_out_file_index.restype = None  
milleC.last_mille_out_file_index.argtypes = [ctypes.POINTER(ctypes.c_int)]
milleC.close_mille_output.restype = None  
milleC.close_mille_output.argtypes = [ctypes.c_int]
