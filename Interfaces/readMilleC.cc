
/** \file
*  C bindings for reading Mille - definitions
*
* \author Max Goblirsch-Kolb, DESY (2026)
*
*  \copyright
*  Copyright (c) 2026 Deutsches Elektronen-Synchroton,
*  Member of the Helmholtz Association, (DESY), HAMBURG, GERMANY \n\n
*  This library is free software; you can redistribute it and/or modify
*  it under the terms of the GNU Library General Public License as
*  published by the Free Software Foundation; either version 2 of the
*  License, or (at your option) any later version. \n\n
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Library General Public License for more details. \n\n
*  You should have received a copy of the GNU Library General Public
*  License along with this program (see the file COPYING.LIB for more
*  details); if not, write to the Free Software Foundation, Inc.,
*  675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "Mille/readMilleC.h"
#include "Mille/MilleFactory.h"
#include <vector>
#include <iostream>
#include <atomic>

using Mille::spawnMilleReader; 

/* ________ global variables used for file handling __________ */

std::vector<std::unique_ptr<Mille::IMilleReader>> g_readFileHandles;   ///< pointer to list of pointers to opened binary files
std::atomic<int> g_lastInFileIndex = 0;                         ///< index of the last opened file handle
/*______________________________________________________________*/

/// helper to derive last opened file index
namespace{
    int resolveFileIndex(int nFileIn){
        if (nFileIn <= 0){
            return g_lastInFileIndex - 1;
        }
        else return nFileIn - 1; 
    }
    bool validFileIndex(int fileIndex){
        if (fileIndex < 0) return false;
        if (fileIndex > (int)g_readFileHandles.size()) return false;
        if (!g_readFileHandles.at(fileIndex)) return false;
        return true; 
    }
}

/// Initialises the 'global' variables used for file handling.
void initMille() {
    g_readFileHandles.reserve(100); 
}

/*______________________________________________________________*/
/// Open file.
void open_mille(const char *fileName, int lengthFileName, int *errorFlag, int nFileIn)
{
    if (!errorFlag)
        return; /* 'printout' error? */
    if (nFileIn <= 0) nFileIn = g_readFileHandles.size()+1; 
    int fileIndex = nFileIn - 1; /* index of specific file */

    // some sanitation of the input file name
    std::string fname = fileName;
    fname = fname.substr(0,lengthFileName); 
    fname[lengthFileName] = '\0';

    // now access the file 
    if (fileIndex >= (int)g_readFileHandles.size()) g_readFileHandles.resize(fileIndex+1); 
    g_readFileHandles.at(fileIndex) = spawnMilleReader(fname); 
    if (!g_readFileHandles.at(fileIndex) || !g_readFileHandles.at(fileIndex)->open(fname)){
        *errorFlag = 2;     // failed to open file 
        return; 
    }
    g_lastInFileIndex = nFileIn;
    // all good!
    *errorFlag = 0; 
}

/*______________________________________________________________*/
/// Close file.
void close_mille(int nFileIn) {
    int fileIndex = resolveFileIndex(nFileIn); /* index of current file */
    if (!validFileIndex(fileIndex)) return; 
    g_readFileHandles.at(fileIndex)->close();
}

/*______________________________________________________________*/
/// Rewind file.
void reset_mille(int nFileIn) {
    int fileIndex = resolveFileIndex(nFileIn); /* index of current file */
    if (!validFileIndex(fileIndex)) return; 
    g_readFileHandles.at(fileIndex)->reset();
}

/*______________________________________________________________*/
/// Rewind file.
void reset_mille_entry(int nFileIn) {
    int fileIndex = resolveFileIndex(nFileIn); /* index of current file */
    if (!validFileIndex(fileIndex)) return; 
    g_readFileHandles.at(fileIndex)->rewindEntry();
}

void skip_mille(int nFileIn){
    int fileIndex = resolveFileIndex(nFileIn); /* index of current file */
    if (!validFileIndex(fileIndex)) return; 
    g_readFileHandles.at(fileIndex)->skipEntry();
}

/**______________________________________________________________*/
// Read record from file.

void read_mille(double *bufferDouble, float *bufferFloat, int *bufferInt,
    int *lengthBuffers, int *errorFlag, int nFileIn) {
        /* No return value since to be called as subroutine from fortran,
        negative *errorFlag are errors, otherwise fine.
        
        *nFileIn: number of the file the record is read from,
        starting from 1 (not 0)
        */
    if (!errorFlag)
        return;
    *errorFlag = 0;

    int fileIndex = resolveFileIndex(nFileIn); /* index of current file */
    if (!validFileIndex(fileIndex)){
        std::cerr <<" File index " <<fileIndex<<" from nFileIn "<<nFileIn<<" is either invalid or points to invalid reader instance. Will not read anything."<<std::endl;
        *errorFlag = -3; 
        return; 
    }
    // access the requested handler 
    Mille::IMilleReader* reader = g_readFileHandles.at(fileIndex).get(); 
    // read the next record via the IMilleReader
    std::pair<int,int> readResult = reader->readToBuffer(bufferDouble, bufferFloat, bufferInt, *lengthBuffers); 
    *errorFlag = readResult.second; 
    *lengthBuffers = readResult.first; 
}

void last_mille_in_file_index(int* index){
    if (!index) return; 
    *index = g_lastInFileIndex; 
}
