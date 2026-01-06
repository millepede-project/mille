CMAKE_MINIMUM_REQUIRED(VERSION 3.26.5)

# discover the compilers from PATH (when system install is overridden)
find_program(gcc_loc gcc)
find_program(gpp_loc g++)

set(CMAKE_C_COMPILER ${gcc_loc})
set(CMAKE_CXX_COMPILER ${gpp_loc})

PROJECT(mille LANGUAGES CXX)
set(mille_VERSION 01.00.00)
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


if (DEBUG)
    message("Setting flags for debug build")    
    set(CMAKE_C_FLAGS "-Wall -Wno-unused-function -O3")
else()
    set(CMAKE_C_FLAGS "-Wall -Wno-unused-function -O3")
endif()

# add our main targets 

# ===== Mille2 ======= 
add_library(Mille2 SHARED)
target_sources(Mille2 
            PRIVATE src/MilleRecord.cc src/MilleRecordCSV.cc src/MilleRecordC.cc src/MilleReaderC.cc src/MilleReaderCSV.cc src/MilleDecoder.cc src/MilleFactory.cc
            PUBLIC 
                FILE_SET HEADERS 
                FILES Interface/MilleDataStructures.h Interface/MilleRecord.h Interface/MilleRecordC.h Interface/MilleRecordCSV.h Interface/MilleReaderC.h Interface/MilleReaderCSV.h Interface/IMilleReader.h Interface/MilleDecoder.h  Interface/MilleFactory.h 
                BASE_DIRS Interface )


add_executable(inspectMilleBinary)
target_sources(inspectMilleBinary PRIVATE utils/inspectMilleBinary.cc)
target_link_libraries(inspectMilleBinary PUBLIC Mille2)
install(TARGETS inspectMilleBinary 
        DESTINATION bin )

# ===== LibZ for file compression ===== 
if (SUPPORT_ZLIB)
    target_link_libraries(Mille2 PUBLIC z )
    target_compile_definitions(Mille2 PUBLIC SUPPORT_LZ)
    target_sources(Mille2 PRIVATE src/MilleReaderCcompressed.cc
            PUBLIC 
                FILE_SET HEADERS 
                FILES  Interface/MilleReaderCcompressed.h
                BASE_DIRS Interface )
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
                FILES Interface/MilleRecordROOT.h Interface/MilleReaderROOT.h
                BASE_DIRS Interface )

    add_executable(convertMilleFormats)
    target_sources(convertMilleFormats PRIVATE utils/convertMilleFormats.cc)
    target_link_libraries(convertMilleFormats PUBLIC Mille2)
    install(TARGETS convertMilleFormats 
            DESTINATION bin )

endif()


# install headers for Mille2 
install(
  TARGETS Mille2
  EXPORT mille-targets
  LIBRARY DESTINATION lib
  FILE_SET HEADERS 
  DESTINATION include
)


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


# Unit tests 
if (BUILD_TESTS)
    subdirs(test)
endif()
