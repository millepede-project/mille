CMAKE_MINIMUM_REQUIRED(VERSION 3.26.5)

PROJECT(Mille LANGUAGES Fortran CXX)
set(Mille_VERSION 01.00.03)
include(CTest)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set_property(CACHE CMAKE_INSTALL_PREFIX PROPERTY VALUE "${CMAKE_BINARY_DIR}/MilleInstall")
endif()

# Arguments for CMake
option(DEBUG "Debug" OFF)
option(SUPPORT_ROOT "build ROOT file I/O and root plotting scripts" ON)
option(SUPPORT_ZLIB "support Z lib for file compression" ON)
option(BUILD_TESTS "compile unit tests" OFF)

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
set(CMAKE_Fortran_MODULE_DIRECTORY ${CMAKE_BINARY_DIR}/modules)

if (DEBUG)
    message("Setting flags for debug build")    
    set(CMAKE_CXX_FLAGS "-Wall -Wno-unused-function -O3 -g ")  

else()
    set(CMAKE_CXX_FLAGS "-Wall -Wno-unused-function -O3")
endif()

# add our main targets 

# ===== Mille2 ======= 
add_library(Mille2 SHARED)
# Require at least C++17
target_compile_features(Mille2 PUBLIC cxx_std_17)
target_sources(Mille2 
            PRIVATE src/MilleRecord.cc src/MilleRecordCSV.cc src/MilleRecordC.cc src/MilleReaderC.cc src/MilleReaderCSV.cc src/MilleDecoder.cc src/MilleFactory.cc
            PUBLIC 
                FILE_SET HEADERS 
                FILES Mille/MilleDataStructures.h Mille/MilleRecord.h Mille/MilleRecord.ixx  Mille/MilleRecordC.h Mille/MilleRecordCSV.h Mille/MilleReaderC.h Mille/MilleReaderCSV.h Mille/IMilleReader.h Mille/MilleDecoder.h  Mille/MilleFactory.h 
                 )


add_executable(inspectMilleBinary)
target_sources(inspectMilleBinary PRIVATE util/inspectMilleBinary.cc)
target_link_libraries(inspectMilleBinary PUBLIC Mille2)
install(TARGETS inspectMilleBinary 
        DESTINATION bin )


add_executable(convertMilleFormats)
target_sources(convertMilleFormats PRIVATE util/convertMilleFormats.cc)
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
  EXPORT Mille-targets
  LIBRARY DESTINATION lib
  FILE_SET HEADERS 
  DESTINATION include
)
# F90-bindings
add_library(MilleF OBJECT Fortran/MilleF.f90)
target_include_directories(MilleF 
                        INTERFACE  $<BUILD_INTERFACE:${CMAKE_Fortran_MODULE_DIRECTORY}/> $<INSTALL_INTERFACE:modules>)
target_link_libraries(MilleF INTERFACE MilleC )

install (FILES ${CMAKE_Fortran_MODULE_DIRECTORY}/millef.mod DESTINATION modules )
install (
    TARGETS MilleF
    EXPORT Mille-targets
)

# Unit tests 
if (BUILD_TESTS)
    subdirs(test)
endif()


# install components for Mille
install(
  TARGETS Mille2
  EXPORT Mille-targets
  LIBRARY DESTINATION lib
  FILE_SET HEADERS 
  DESTINATION include
)

# install python modules 
install (FILES python/milleLibBindings.py python/milleReader.py python/milleWriter.py python/__init__.py 
        DESTINATION python/Mille)
        

install (EXPORT Mille-targets
        DESTINATION cmake 
)

if (NOT MILLE_AS_SUBDIR)
    # configure and install a setup script for standalone Mille 
    configure_file(
        ${Mille_SOURCE_DIR}/cmake/setup.sh.cmake.in
        ${Mille_BINARY_DIR}/milleStandaloneSetup.sh @ONLY)

    install(FILES ${Mille_BINARY_DIR}/milleStandaloneSetup.sh
            DESTINATION . )

    # add a target to generate API documentation with Doxygen
    FIND_PACKAGE(Doxygen)

    if(DOXYGEN_FOUND)

        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)

        ADD_CUSTOM_TARGET(doc
            ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
            COMMENT "Generating API documentation with Doxygen" VERBATIM
        )

    ELSE()
        MESSAGE( STATUS "Doxygen not installed or found -- API documentation can't be generatred!" )

    ENDIF(DOXYGEN_FOUND)
endif()

# Create and install package configuration and version files.
configure_file(
${Mille_SOURCE_DIR}/cmake/Mille-config.cmake.in
${Mille_BINARY_DIR}/cmake/MilleConfig.cmake @ONLY)

configure_file(
${Mille_SOURCE_DIR}/cmake/Mille-config-version.cmake.in
${Mille_BINARY_DIR}/cmake/MilleConfig-version.cmake @ONLY)

install(FILES ${Mille_BINARY_DIR}/cmake/MilleConfig.cmake
        ${Mille_BINARY_DIR}/cmake/MilleConfig-version.cmake
        DESTINATION cmake/)
