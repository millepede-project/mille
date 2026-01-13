CMAKE_MINIMUM_REQUIRED(VERSION 3.26.5)

# discover the compilers from PATH (when system install is overridden)
find_program(gcc_loc gcc)
find_program(gpp_loc g++)

set(CMAKE_C_COMPILER ${gcc_loc})
set(CMAKE_CXX_COMPILER ${gpp_loc})
set(CMAKE_Fortran_COMPILER ${gf_loc})

PROJECT(mille LANGUAGES Fortran CXX)
set(mille_VERSION 00.01.00)
include(CTest)

# Arguments for CMake
option(DEBUG "Debug" OFF)
option(SUPPORT_ROOT "build ROOT file I/O and root plotting scripts" ON)
option(SUPPORT_ZLIB "support Z lib for file compression" ON)
option(BUILD_TESTS "compile unit tests" ON)

set(build_ROOT_comps false)
if(SUPPORT_ROOT)
    # check for ROOT 
    if (DEFINED ENV{ROOTSYS})
        # follow ROOTSYS if it has been defined 
        set (ROOT_DIR $ENV{ROOTSYS}/cmake) 
        message ("Taking ROOT from ROOTSYS: $ENV{ROOTSYS}")
    endif()
    find_package(ROOT CONFIG QUIET )
    if (${ROOT_FOUND})
        message("Building ROOT related components")
        set(build_ROOT_comps true)
    endif()
endif()

# Check dependencies
if (SUPPORT_ZLIB)
    find_package(ZLIB  REQUIRED)
endif()
# Destination targets for build results
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_Fortran_MODULE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/modules)

if (DEBUG)
    message("Setting flags for debug build")    
    set(CMAKE_CXX_FLAGS "-Wall -Wno-unused-function -O3 -g ")  

else()
    set(CMAKE_CXX_FLAGS "-Wall -Wno-unused-function -O3")
endif()

# add our main targets 

# ===== Mille2 ======= 
add_library(Mille2 SHARED)
target_sources(Mille2 
            PRIVATE src/MilleRecord.cc src/MilleRecordCSV.cc src/MilleRecordC.cc src/MilleReaderC.cc src/MilleReaderCSV.cc src/MilleDecoder.cc src/MilleFactory.cc
            PUBLIC 
                FILE_SET HEADERS 
                FILES Mille/MilleDataStructures.h Mille/MilleRecord.h Mille/MilleRecordC.h Mille/MilleRecordCSV.h Mille/MilleReaderC.h Mille/MilleReaderCSV.h Mille/IMilleReader.h Mille/MilleDecoder.h  Mille/MilleFactory.h 
                 )


add_executable(inspectMilleBinary)
target_sources(inspectMilleBinary PRIVATE utils/inspectMilleBinary.cc)
target_link_libraries(inspectMilleBinary PUBLIC Mille2)
install(TARGETS inspectMilleBinary 
        DESTINATION bin )


add_executable(convertMilleFormats)
target_sources(convertMilleFormats PRIVATE utils/convertMilleFormats.cc)
target_link_libraries(convertMilleFormats PUBLIC Mille2)
install(TARGETS convertMilleFormats 
        DESTINATION bin )

# ===== LibZ for file compression ===== 
if (SUPPORT_ZLIB)
    target_link_libraries(Mille2 PUBLIC z )
    target_compile_definitions(Mille2 PUBLIC SUPPORT_LZ)
    target_sources(Mille2 PRIVATE src/MilleReaderCcompressed.cc
            PUBLIC 
                FILE_SET HEADERS 
                FILES  Mille/MilleReaderCcompressed.h
                 )
endif()


if (build_ROOT_comps)
    message ("Found ROOT - will build ROOT I/O lib") 

    set(CMAKE_CXX_STANDARD ${ROOT_CXX_STANDARD})

    # enable ROOT support in Mille2
    target_compile_definitions(Mille2 PUBLIC SUPPORT_ROOTIO)
    target_compile_definitions(inspectMilleBinary PUBLIC SUPPORT_ROOTIO)
    target_link_libraries(Mille2 PUBLIC ROOT::Core ROOT::Tree ROOT::RIO )
    target_sources(Mille2 PRIVATE src/MilleRecordROOT.cc src/MilleReaderROOT.cc
            PUBLIC 
                FILE_SET HEADERS 
                FILES Mille/MilleRecordROOT.h Mille/MilleReaderROOT.h
                 )

endif()

# C-bindings
add_library(MilleC SHARED)
target_sources(MilleC PRIVATE Interfaces/readMilleC.cc Interfaces/writeMilleC.cc
            PUBLIC 
                FILE_SET HEADERS 
                FILES Mille/readMilleC.h Mille/writeMilleC.h
                )
target_link_libraries(MilleC PUBLIC Mille2 )

# install C-bindings
install(
  TARGETS MilleC
  EXPORT mille-targets
  LIBRARY DESTINATION lib
  FILE_SET HEADERS 
  DESTINATION include
)
# F90-bindings
add_library(MilleF INTERFACE)
target_sources(MilleF PRIVATE Fortran/MilleF.f90)
target_link_libraries(MilleF INTERFACE MilleC )

# install F90-bindings
install(
  TARGETS MilleF
  EXPORT mille-targets
  LIBRARY DESTINATION lib
)
install (FILES ${CMAKE_Fortran_MODULE_DIRECTORY}/millef.mod DESTINATION modules )

# Unit tests 
if (BUILD_TESTS)
    subdirs(test)
endif()


# install components for Mille
install(
  TARGETS Mille2
  EXPORT mille-targets
  LIBRARY DESTINATION lib
  FILE_SET HEADERS 
  DESTINATION include
)

# install python modules 
install (FILES python/milleLibBindings.py python/milleReader.py python/milleWriter.py python/__init__.py 
        DESTINATION python/Mille)
        

install (EXPORT mille-targets
        DESTINATION cmake 
)

# Create and install package configuration and version files.
configure_file(
${mille_SOURCE_DIR}/cmake/mille-config.cmake.in
${mille_BINARY_DIR}/cmake/milleConfig.cmake @ONLY)

configure_file(
${mille_SOURCE_DIR}/cmake/mille-config-version.cmake.in
${mille_BINARY_DIR}/cmake/milleConfig-version.cmake @ONLY)

install(FILES ${mille_BINARY_DIR}/cmake/milleConfig.cmake
        ${mille_BINARY_DIR}/cmake/milleConfig-version.cmake
        DESTINATION cmake/)
