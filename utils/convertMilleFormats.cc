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

#include "Mille/MilleFactory.h"
#include <iostream>

int decodeFile(Mille::IMilleReader & reader, Mille::MilleRecord & writer, const std::string & fname, long long int & nRecords, long long int & nSkip){
    int nentries = 0; 
    if(!reader.open(fname)){
        std::cerr <<" Failed to import "<<fname<<" due to open errors"<<std::endl;    
    } 
    do{
        if (nSkip > 0 && nSkip-- > 0){
            reader.skipEntry();
            continue; 
        }
        if (nRecords >= 0 && nentries+1 > nRecords){
            nRecords = 0; 
            break; 
        }
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
    using Mille::MilleFormat; 
    using Mille::MilleRecord; 
    using Mille::spawnMilleRecord; 
    using Mille::spawnMilleReader; 
    using Mille::IMilleReader; 

    if (argc < 3){
        std::cerr << "Usage: convertMilleFormats [-r / -c / -z / -t to force input file format to ROOT / C / zipped C / plain text]"
                  << " [-s <Nskip> to skip first Nskip records]"
                  << " [-n <nRec> to only write nRec records] "
                  << " <destination file name> <one or many files to convert> "<<std::endl;
        return 1; 
    }
    int iarg = 1; 
    long long int nRecords = -1; 
    long long int nSkip = -1; 
    MilleFormat userOutputFormat = MilleFormat::Unknown; 
    while (std::string(argv[iarg]).find("-") == 0){
        std::string arg = argv[iarg]; 
        if (arg == "-z"){
            userOutputFormat = MilleFormat::Ccompressed; 
            ++iarg;
        }
        else if (arg == "-r"){
            userOutputFormat = MilleFormat::ROOT; 
            ++iarg;
        }
        else if (arg == "-c"){
            userOutputFormat = MilleFormat::C; 
            ++iarg;
        }
        else if (arg == "-t"){
            userOutputFormat = MilleFormat::CSV; 
            ++iarg;
        }
        else if (arg == "-s"){
            ++iarg; 
            nSkip = std::atoll(argv[iarg++]); 
        }
        else if (arg == "-n"){
            ++iarg; 
            nRecords = std::atoll(argv[iarg++]); 
        }
        else{
            ++iarg; 
        }
    }
    if (nSkip > 0 ){
        std::cout <<" Will skip the first "<<nSkip<<" Records"<<std::endl;
    }
    if (nRecords > 0 ){
        std::cout <<" Will copy at most "<<nRecords<<" Records"<<std::endl;
    }
    const std::string outFile = argv[iarg++]; 

    std::unique_ptr<MilleRecord> out(nullptr); 
    if (userOutputFormat == MilleFormat::Unknown){
        out = spawnMilleRecord(outFile); 
    }
    else{
        out = spawnMilleRecord(userOutputFormat, outFile);
    }
    if (!out){
        std::cerr <<" Failed to deduce output file format - bailing out "<<std::endl; 
        return 1; 
    }
    for (int i = iarg; i < argc; ++i){
        std::unique_ptr<IMilleReader> reader = spawnMilleReader(std::string(argv[i])); 
        if (!reader){
            std::cout <<" Unable to deduce file type for " <<argv[i]<<std::endl; 
            continue; 
        }
        int nentries = decodeFile(*reader, *out, argv[i],nRecords, nSkip);
        std::cout << " Imported "<<nentries<<" entries from file "<<argv[i]<<std::endl; 
    }
    return 0;
}
