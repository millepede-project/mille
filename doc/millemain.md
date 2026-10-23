
 \file
 Mille library, documentation.

 \author Maximilian Goblirsch-Kolb, DESY

 \copyright
 Copyright (c) 2026 Deutsches Elektronen-Synchroton,
 Member of the Helmholtz Association, (DESY), HAMBURG, GERMANY \n\n
 This library is free software; you can redistribute it and/or modify
 it under the terms of the GNU Library General Public License as
 published by the Free Software Foundation; either version 2 of the
 License, or (at your option) any later version. \n\n
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Library General Public License for more details. \n\n
 You should have received a copy of the GNU Library General Public
 License along with this program (see the file COPYING.LIB for more
 details); if not, write to the Free Software Foundation, Inc.,
 675 Mass Ave, Cambridge, MA 02139, USA.


 \mainpage Overview

# Introduction
 The **Mille** library is responsible for handling the I/O operations of the **Millepede-II** program and the **GeneralBrokenLines** toolkit. 
  
 It supports writing and reading the encoded sets of measurements (referred to as records) using several different file formats. 

 The core components are implemented in C++, and bindings exist to allow their use within C, Fortran, and python3. 

 The code is maintained and developed by the [CMS group](https://cms.desy.de/) of the [Particle Physics division](https://particle-physics.desy.de/index_eng.html) at DESY using [GitLab](https://about.gitlab.com) ([code and wiki](https://gitlab.desy.de)).

 The Mille software is provided by DESY under the terms of the
 [LGPLv2 license](http://www.gnu.org/licenses/old-licenses/lgpl-2.0-standalone.html).


 # Getting started 

 \subpage installation_page

 \subpage usage_page

 \subpage formats_page

 # News 

 \subpage changelog_page 

 \subpage release_page

# Package structure 

- The `cmake` folder contains helper files and macros used to build the package.
- The `doc` folder contains sources and auxiliary files to build this documentation.
- The `Fortran` folder contains the interface to Fortran
- The `Interfaces` folder contains implementations of the C bindings 
- The `Mille` folder includes C and C++ headers for all components of the package. 
- The `python` folder contains the python modules and C-python bindings. It also provides example scripts.
- The `src` folder contains the implementation of the C++ API 
- The `test` folder implements unit tests, which can also be used as implementation examples. 
- The `util` folder implements executable C++ utility programs for inspecting and translating Mille files. 

# Helper programs

Two helper programs are provided with the package
- `inspectMilleBinary`: Allows to read a given Mille file and print its content to the output. Command line arguments allow to specify a sub-set of the records to print. 
- `convertMilleFormats`: Converts the entirity or a sub-set of an existing Mille file into a new file of arbitrary supported format. 

 # Contact

 For information exchange the **Millepede** mailing list
 anacentre-millepede2@desy.de should be used.
 You can subscribe under this [link](https://lists.desy.de/sympa/info/anacentre-millepede2)

