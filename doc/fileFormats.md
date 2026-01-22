
\page formats_page Supported file formats

# Mille file formats

Currently, four formats are defined.

Each is by default associated to a file extension. Files with the same extension will be automatically assumed to belong to said format. 

The following formats exist, 
1. **C-binary** - extension **.dat**
    - The original `Millepede-II` C-binary format. 
    - Backward compatible to existing binaries.
2. **Compressed C-binary** - extension **.dat.gz**: 
    - Original C-binary format, compressed with GZip. 
    - Most disk-space-efficient, good reading performance. 
    - Backward compatible to existing compressed binaries. 
    - Not directly available for writing - instead, write into `.dat` and then manually run `gzip`. 
3. **ROOT** - extension **.root**: 
    - ROOT `TTree` closely following the C-binary format. 
    - Slightly larger disk usage, but support for direct reading from the network using the `xrootd` protocol. 
    - Useful if local storage for the binaries is limited, or if the files are inteded to be inspected using standalone ROOT scripts. 
4. **Plain text**- extension **.csv**: 
    - Plain text format. Intentionally verbose with inline decoding hints, for pedagogical purposes. Maximum disk usage, poor reading performance - intended as an immediately human-readable and editeable format to experiment with Mille and inspect / debug certain records. *Not recommended for production use*. 

The API is agnostic of the file format - as long as you provide one of the four provided file extensions when passing the file name, the correct type will be deduced and instantiated in the background. If you feel corageous and insist on using different extensions, the C++ API allows to manually override the file format. In this case, no successful deduction of the file type should be assumed by e.g. Millepede-II! 

