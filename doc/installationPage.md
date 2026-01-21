 \page installation_page Installation and compilation

## Requirements

### Fortran and C++ standards

At least Fortran90 and C++17 are recommended. 

### Optional: ROOT

Optionally, if you have ROOT available on your system, the package will be built to support ROOT file storage / reading. 

### Optional: libZ 

If you have `libz` on your system (most systems do), support for reading directly from gzip-compressed C-binaries will be enabled by default. 

### Documentation

To build the documentation, [doxygen](https://www.doxygen.nl/) version 1.9.8 or later is required to correctly generate internal links between the pages. 
This is for example available in the `gitlab.desy.de:5555/millepede/millepede-ii/mp2dev:latest` docker image.

## Installation

 To install **Mille** (on a linux system):

### Download the sources 
Download the software package from the DESY \c gitlab server to
    \a source directory (in the following, assumed to be "Mille"), e.g.:

         git clone --depth 1  \
             https://gitlab.desy.de/millepede/Mille.git Mille

Then compile using the following steps:
- Create a `build` folder, outside the Mille folder (for example, it can be in the same top level folder as the `Mille` one). 
- navigate into `build` and invoke `cmake <path_to_Mille>`. Here you can set additional options (see next sub-section) to configure the build.  
- then compile the sources by calling `make` followed by `make install`. 
- If you wish to compile the documentation, additionally do `make doc`. 

#### Configuring the CMake build
You can pass additional options / flags to CMake: 
- `-DCMAKE_INSTALL_PREFIX=<...>`: Set the install location for the binaries. The default is a subfolder `MilleInstall` within your build folder. 
- `-DDEBUG=on`: Enable debug build 
- `-DSUPPORT_ROOT=off`: Disable (default: on) support for ROOT-files (if dependency is met)
- `-DSUPPORT_ZLIB=off`: Disable (default: on) support for z-lib compression (if dependency is met)
- `-DBUILD_TESTS=off`: Disable (default: on) compilation of unit tests

#### Bonus: Setup script
The build will place a `milleStandaloneSetup.sh` script in your chosen installation folder.
If this is not a folder on the default path, you can call this script to make Mille and its programs visible to the system. 
This removes the need to point by hand to the `Mille` location. 
Note that when Mille is automatically built as part of `Millepede-II`, the setup for `Mille` will instead be added to the `mp2setup.sh` script from Millepede.

### Testing your build

 Optionally check the installation by running the unit tests with

         ctest -V
