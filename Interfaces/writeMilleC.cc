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

#include "Mille/writeMilleC.h"
#include "Mille/MilleFactory.h"
#include <vector>
#include <atomic>
#include <iostream>
#include <cstring>



using Mille::MilleArrayView; 
using Mille::spawnMilleRecord; 


/// global variables used for file handling 
std::vector<std::unique_ptr<Mille::MilleRecord>> g_writeFileHandles; 
std::atomic<int> g_lastOutFileIndex = 0; 

/// helper to derive last opened file index
namespace {
    int resolveFileIndex(int nFileOut){
        if (nFileOut <= 0){
            return g_lastOutFileIndex - 1;
        }
        else return nFileOut - 1; 
    }
    bool validFileIndex(int fileIndex){
        if (fileIndex < 0) return false;
        if (fileIndex > (int)g_writeFileHandles.size()) return false;
        if (!g_writeFileHandles.at(fileIndex)) return false;
        return true; 
    }
}



void open_mille_out(const char *fileName, int lengthFileName, int nFileOut, bool doublePrec, bool keepZeros,
                int recordLengthHint){
    if (nFileOut <= 0) nFileOut = g_writeFileHandles.size()+1; 
    int fileIndex = nFileOut - 1; 

    // some sanitation of the output file name
    std::string fname = fileName;
    fname = fname.substr(0,lengthFileName); 
    fname[lengthFileName] = '\0';

    // now access the file 
    if (fileIndex >= (int)g_writeFileHandles.size()) g_writeFileHandles.resize(fileIndex+1); 
    g_writeFileHandles.at(fileIndex) = spawnMilleRecord(fname,doublePrec,keepZeros,recordLengthHint); 
    if (!g_writeFileHandles.at(fileIndex)){
        std::cerr << "FAILED to open output file!" <<std::endl;
        return; 
    }
    g_lastOutFileIndex = nFileOut;
}

void write_mille_out(int nFileOut){
    int fileIndex = resolveFileIndex(nFileOut); 
    if (!validFileIndex(fileIndex)) return; 
    g_writeFileHandles[fileIndex]->writeRecord(); 
}

void add_mille_data( double aMeas, double aErr, 
        unsigned int numLocal,
        const unsigned int* indLocal, const double* derLocal,
        int nGlobal, 
        const int* labGlobal, const double* derGlobal, 
        int nFileOut){
    int fileIndex = resolveFileIndex(nFileOut);  
    if (!validFileIndex(fileIndex)) return; 
    
    g_writeFileHandles[fileIndex]->addData(aMeas, aErr, 
        MilleArrayView<unsigned int>(indLocal,numLocal), 
        MilleArrayView<double>(derLocal,numLocal),
        MilleArrayView<int>(labGlobal, nGlobal), 
        MilleArrayView<double>(derGlobal, nGlobal));
}
void copy_to_mille_buffer(unsigned int nEntries,
        const int* indices, const double* derivatives,
        int nFileOut){
    int fileIndex = resolveFileIndex(nFileOut);  
    if (!validFileIndex(fileIndex)) return; 
    g_writeFileHandles[fileIndex]->copyToBuffer(MilleArrayView<int>(indices, nEntries), 
                                                MilleArrayView<double>(derivatives, nEntries)); 
}

        
void add_mille_special(int nSpecial, const int* specialInt, 
                    const double* specialFloat, 
                    int nFileOut){
    int fileIndex = resolveFileIndex(nFileOut);  
    if (!validFileIndex(fileIndex)) return; 
    g_writeFileHandles[fileIndex]->addSpecial( 
        MilleArrayView<int>(specialInt, nSpecial),
        MilleArrayView<double>(specialFloat, nSpecial)
    ); 
}


void abort_mille_record(int nFileOut){
    int fileIndex = resolveFileIndex(nFileOut);  
    if (!validFileIndex(fileIndex)) return; 
    g_writeFileHandles[fileIndex]->abort();
}

void flush_mille_output(int nFileOut){
    int fileIndex = resolveFileIndex(nFileOut);  
    if (!validFileIndex(fileIndex)) return; 
    g_writeFileHandles[fileIndex]->flushOutputFile();
}

void reset_mille_output(int nFileOut){
    int fileIndex = resolveFileIndex(nFileOut);  
    if (!validFileIndex(fileIndex)) return; 
    g_writeFileHandles[fileIndex]->resetOutputFile();
}

void last_mille_out_file_index(int* index){
    *index = g_lastOutFileIndex;     
}

void close_mille_output(int nFileOut){
    int fileIndex = resolveFileIndex(nFileOut);  
    if (!validFileIndex(fileIndex)) return; 
    g_writeFileHandles[fileIndex].reset(nullptr);
}
