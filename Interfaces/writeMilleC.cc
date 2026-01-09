/** \file
 *  C-binding for Mille writing - Definitions
 *
 * \author Maximilian Goblirsch-Kolb, DESY (2026) 
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
 *
 */

#include "writeMilleC.h"
#include "MilleFactory.h"
#include <vector>
#include <atomic>
#include <iostream>
#include <cstring>
/// global variables used for file handling 
std::vector<std::unique_ptr<MilleRecord>> g_writeFileHandles; 
std::atomic<int> g_lastOutFileIndex = 0; 


void openMilleOut(const char *fileName, int lengthFileName, int nFileOut, bool doublePrec, bool keepZeros,
			    int recordLengthHint){
    if (nFileOut < 0) nFileOut = g_writeFileHandles.size()+1; 
    int fileIndex = nFileOut - 1; 

    // some sanitation of the output file name
    std::string fname = fileName;
    fname = fname.substr(0,lengthFileName); 
    fname[lengthFileName] = '\0';

    // now access the file 
    if (fileIndex >= g_writeFileHandles.size()) g_writeFileHandles.resize(fileIndex+1); 
    g_writeFileHandles.at(fileIndex) = std::move(spawnMilleRecord(fname,doublePrec,keepZeros,recordLengthHint)); 
    if (!g_writeFileHandles.at(fileIndex)){
        std::cerr << "FAILED to open output file!" <<std::endl;
        return; 
    }
    g_lastOutFileIndex = nFileOut;
}

void writeMilleOut(int nFileOut){
    if (nFileOut < 0) nFileOut = g_lastOutFileIndex; 
    int fileIndex = nFileOut - 1; 
    if (fileIndex < g_writeFileHandles.size() && g_writeFileHandles[fileIndex] != nullptr){
        g_writeFileHandles[fileIndex]->writeRecord(); 
    }
}

void addMilleData( double aMeas, double aErr, 
        unsigned int numLocal,
        const unsigned int* indLocal, const double* derLocal,
        int nGlobal, 
        const int* labGlobal, const double* derGlobal, 
        int nFileOut){
    if (nFileOut < 0) nFileOut = g_lastOutFileIndex; 
    int fileIndex = nFileOut - 1; 
    if (fileIndex < g_writeFileHandles.size() && g_writeFileHandles[fileIndex] != nullptr){
        g_writeFileHandles[fileIndex]->addData(aMeas, aErr, 
            std::span<const unsigned int>(indLocal,numLocal), 
            std::span<const double>(derLocal,numLocal),
            std::span<const int>(labGlobal, nGlobal), 
            std::span<const double>(derGlobal, nGlobal));
    }
}

        
void addMilleSpecial(int nSpecial, 
                     const double* specialFloat, const int* specialInt, 
                     int nFileOut){
    if (nFileOut < 0) nFileOut = g_lastOutFileIndex; 
    int fileIndex = nFileOut - 1; 
    if (fileIndex < g_writeFileHandles.size() && g_writeFileHandles[fileIndex] != nullptr){
        g_writeFileHandles[fileIndex]->addSpecial(nSpecial, 
            std::span<const double>(specialFloat, nSpecial),
            std::span<const int>(specialInt, nSpecial)
        ); 
    }
}


void abortMilleRecord(int nFileOut){
    if (nFileOut < 0) nFileOut = g_lastOutFileIndex; 
    int fileIndex = nFileOut - 1; 
    if (fileIndex < g_writeFileHandles.size() && g_writeFileHandles[fileIndex] != nullptr){
        g_writeFileHandles[fileIndex]->abort();
    }
}

void flushMilleOutput(int nFileOut){
    if (nFileOut < 0) nFileOut = g_lastOutFileIndex; 
    int fileIndex = nFileOut - 1; 
    if (fileIndex < g_writeFileHandles.size() && g_writeFileHandles[fileIndex] != nullptr){
        g_writeFileHandles[fileIndex]->flushOutputFile();
    }
}

void resetMilleOutput(int nFileOut){
    if (nFileOut < 0) nFileOut = g_lastOutFileIndex; 
    int fileIndex = nFileOut - 1; 
    if (fileIndex < g_writeFileHandles.size() && g_writeFileHandles[fileIndex] != nullptr){
        g_writeFileHandles[fileIndex]->resetOutputFile();
    }
}

void lastMilleOutFileIndex(int* index){
    *index = g_lastOutFileIndex;     
}

void closeMilleOutput(int nFileOut){
    if (nFileOut < 0) nFileOut = g_lastOutFileIndex; 
    int fileIndex = nFileOut - 1; 
    if (fileIndex < g_writeFileHandles.size() && g_writeFileHandles[fileIndex] != nullptr){
        g_writeFileHandles[fileIndex].reset(nullptr);
    }
}
