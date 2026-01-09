
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
/* ________ global variables used for file handling __________ */

std::vector<std::unique_ptr<IMilleReader>> g_readFileHandles;   ///< pointer to list of pointers to opened binary files
std::atomic<int> g_lastInFileIndex = 0;                         ///< index of the last opened file handle
/*______________________________________________________________*/
/// Initialises the 'global' variables used for file handling.
void initMille() {
    g_readFileHandles.reserve(100); 
}

/*______________________________________________________________*/
/// Open file.
void openMille(const char *fileName, int lengthFileName, int *errorFlag, int nFileIn)
{
    if (!errorFlag)
        return; /* 'printout' error? */
    if (nFileIn < 0) nFileIn = g_readFileHandles.size()+1; 
    int fileIndex = nFileIn - 1; /* index of specific file */

    // some sanitation of the input file name
    std::string fname = fileName;
    fname = fname.substr(0,lengthFileName); 
    fname[lengthFileName] = '\0';

    // now access the file 
    if (fileIndex >= g_readFileHandles.size()) g_readFileHandles.resize(fileIndex+1); 
    g_readFileHandles.at(fileIndex) = std::move(spawnMilleReader(fname)); 
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
void closeMille(int nFileIn) {
    int fileIndex = nFileIn - 1; /* index of current file */
    if (fileIndex < 0 || fileIndex >= g_readFileHandles.size())
        return; /* no file opened at all... */
    if (g_readFileHandles.at(fileIndex)) g_readFileHandles.at(fileIndex)->close();
}

/*______________________________________________________________*/
/// Rewind file.
void resetMille(int nFileIn) {
    int fileIndex = nFileIn - 1; /* index of current file */
    if (fileIndex < 0 || fileIndex >= g_readFileHandles.size())
        return; /* no file opened at all... */
    if (g_readFileHandles.at(fileIndex)) g_readFileHandles.at(fileIndex)->reset();
}

/*______________________________________________________________*/
/// Rewind file.
void resetMilleEntry(int nFileIn) {
    int fileIndex = nFileIn - 1; /* index of current file */
    if (fileIndex < 0 || fileIndex >= g_readFileHandles.size())
        return; /* no file opened at all... */
    if (g_readFileHandles.at(fileIndex)) g_readFileHandles.at(fileIndex)->rewindEntry();
}

void skipMille(int nFileIn){
    int fileIndex = nFileIn - 1; /* index of current file */
    if (fileIndex < 0 || fileIndex >= g_readFileHandles.size())
        return; /* no file opened at all... */
    if (g_readFileHandles.at(fileIndex)) g_readFileHandles.at(fileIndex)->skipEntry();
}

/**______________________________________________________________*/
// Read record from file.

void readMille(double *bufferDouble, float *bufferFloat, int *bufferInt,
    int *lengthBuffers, int *errorFlag, int nFileIn) {
        /* No return value since to be called as subroutine from fortran,
        negative *errorFlag are errors, otherwise fine.
        
        *nFileIn: number of the file the record is read from,
        starting from 1 (not 0)
        */
	if (!errorFlag)
		return;
	*errorFlag = 0;

	int fileIndex = nFileIn - 1; /* index of current file */
	if (fileIndex < 0)
		return; /* no file opened at all... */
    // check for a valid file index
    if (fileIndex >= g_readFileHandles.size()){
        std::cerr <<" Requested file " <<nFileIn<<" out of " <<g_readFileHandles.size()<<std::endl;
		*errorFlag = -3; 
        return; 
    }
    /// check if the file handler is well-defined
    if (!g_readFileHandles.at(fileIndex)){
        std::cerr <<" File " <<nFileIn<<" out of " <<g_readFileHandles.size()<<" is in invalid state"<<std::endl;
		*errorFlag = -3; 
        return; 
    }
    // access the requested handler 
    IMilleReader* reader = g_readFileHandles.at(fileIndex).get(); 
    // read the next record via ROOT's TTree I/O 
    bool foundDouble = false; 
    std::pair<int,int> readResult = reader->readToBuffer(bufferDouble, bufferFloat, bufferInt, *lengthBuffers); 
    *errorFlag = readResult.second; 
    *lengthBuffers = readResult.first; 
}

void lastMilleInFileIndex(int* index){
    if (!index) return; 
    *index = g_lastInFileIndex; 
}


