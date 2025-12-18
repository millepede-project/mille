/**
 *  \file C-binary to ROOT file conversion  
 *  \author Maximilian Goblirsch-Kolb, DESY, 2025 
 *  \copyright
 *  Copyright (c) 2025 Deutsches Elektronen-Synchroton,
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

#include "MilleReaderCcompressed.h"
#include "MilleReaderC.h"
#include "MilleRecordROOT.h"
#include "MilleDecoder.h"
#include <iostream>

int decodeFile(IMilleReader & reader, MilleRecordROOT & writer, const std::string & fname){
    int nentries = 0; 
    MilleDecoder decoder; 
    if(!reader.open(fname)){
        std::cerr <<" Failed to import "<<fname<<" due to open errors"<<std::endl;    
    } 
    do{
        if (!writer.copyRecord(reader)) break; 
        writer.writeRecord(); 
        ++nentries;
        if (nentries % 10000 == 0) std::cout <<" Done copying entry "<<nentries<<std::endl; 
    } while(true); 
    writer.flushOutputFile(); 
    reader.close(); 
    return nentries; 
}

int main(int argc, char** argv){

    if (argc < 3){
        std::cerr << "Usage: cToRoot <target ROOT file> <one or many mille binaries>"<<std::endl;
        return 1; 
    }
    const std::string outFile = argv[1]; 
    MilleRecordROOT out (outFile); 
    std::vector<std::string> inMilleFiles{};
    inMilleFiles.reserve(argc - 2); 
    MilleReaderC readC;
    MilleReaderCcompressed readCGZ; 
    for (size_t i = 2; i < argc; ++i){
        if(std::string(argv[i]).find(".gz")!= std::string::npos){
            int nentries = decodeFile(readCGZ, out, argv[i]);
            std::cout << " Imported "<<nentries<<" entries from file "<<argv[i]<<std::endl; 
        }
        else{
            int nentries = decodeFile(readC, out, argv[i]);
            std::cout << " Imported "<<nentries<<" entries from file "<<argv[i]<<std::endl; 
        }
    }
    return 0;
}
