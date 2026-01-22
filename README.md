# Mille

![MP2 logo](./mp2-logo.png)

**Mille** is the common I/O component of the [Millepede-II](https://gitlab.desy.de/millepede/millepede-ii) and [GeneralBrokenLines](https://gitlab.desy.de/millepede/general-broken-lines/) packages. 

It is used to store the equations defining a linear(ised) least squares problem into binary files that can then be processed within Millepede-II.

In addition to the brief summary below, please check out the extended [documentation page](https://millepede.pages.desy.de/mille/). 


## Availability 

The source code of Mille is provided by DESY under the terms of the [LGPLv2 license](http://www.gnu.org/licenses/old-licenses/lgpl-2.0-standalone.html).

The software can be freely used for research and education. We expect that all publications describing work using this software quote at least one reference (see [here](http://www.desy.de/~blobel/mptalks.html) or [references](references)).

**Disclaimer**: This software is provided without any expressed or implied warranty. In particular there is no warranty of any kind concerning the fitness of this software for any particular purpose.

## Installation

The package can be built using CMake:
- clone the package sources into a location of your choosing
- make a folder `build`, ideally outside of the cloned package.
- enter `build` 
- Configure the build using `cmake <path_to_cloned_sources>`
- Compile using `make` and install using `make install`. 
The default installation location is a subfolder `MilleInstall` within your build directory.
There, a script `milleStandaloneSetup.sh` will be created for you, which you can call to make the installed programs and libraries available to other packages on your system. 

Note that you do not need to manually install `Mille` when you want to use `Millepede-II` or `GeneralBrokenLines` - these packages will automatically download and compile `Mille` for you if you do not yet have an installation. 

## Contact

For information exchange the **Millepede** mailing list
anacentre-millepede2 < at > desy.de should be used.
You can subscribe under this [link](https://lists.desy.de/sympa/info/anacentre-millepede2)
