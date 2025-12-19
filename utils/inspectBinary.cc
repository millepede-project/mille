/**
 *  \file Print out binary records
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
#ifdef SUPPORT_LZ
    #include "MilleReaderCcompressed.h"
#endif
#include "MilleReaderC.h"
#ifdef SUPPORT_ROOTIO
    #include "MilleReaderROOT.h"
#endif 
#include "MilleDecoder.h"
#include <iostream>
#include <filesystem>


int main(int argc, char** argv){

    if (argc < 2){
        std::cerr << "Usage: inspectBinary [-r / -c / -z to force file format] <file name> [first record to print][last record to print] "<<std::endl;
        return 1; 
    }
    bool readROOT = false;
    bool readC = false;
    bool readCGZ = false; 
    int iarg = 1; 
    if (std::string(argv[iarg]).find("-") == 0){
        if (argv[iarg] == "-z"){
            readCGZ=true;  
            ++iarg;
        }
        else if (argv[iarg] == "-r"){
            readROOT=true;  
            ++iarg;
        }
        else if (argv[iarg] == "-c"){
            readC=true;  
            ++iarg;
        }
    }
    if (iarg >= argc){
        std::cerr << "Usage: inspectBinary [-r / -c / -z to force file format] <file name> [first record to print][last record to print] "<<std::endl;
        return 1; 
    }
    const std::filesystem::path inFile = argv[iarg++];
    long long first = 0; 
    long long last = -1 ; 
    if (iarg < argc){
        first = std::atoll(argv[iarg++]);    
    }
    if (iarg < argc){
        last = std::atoll(argv[iarg++]);    
    }
    std::unique_ptr<IMilleReader> reader = nullptr; 
    if (!readC && !readCGZ && !readROOT){
        if (inFile.extension() == ".dat" || inFile.extension() == ".bin"){
            std::cout <<" Auto-detected C binary from file extension"<<std::endl; 
            readC = true; 
        }
        else if (inFile.extension() == ".gz"){
            std::cout <<" Auto-detected compressed C binary from file extension"<<std::endl; 
            readCGZ = true; 
        }
        else if (inFile.extension() == ".root" ){
            std::cout <<" Auto-detected ROOT binary from file extension"<<std::endl; 
            readROOT = true; 
        }
        else{
            std::cerr <<" Unable to auto-detect file type from name "<<inFile<<" - please specify the format using the -c (C-binary) / -r (ROOT binary) / -z (compressed C binary) CLI args"<<std::endl;
            return 1; 
        }
    }
    if (readC) reader = std::make_unique<MilleReaderC>(); 
    else if (readCGZ){
#ifdef SUPPORT_LZ
        reader = std::make_unique<MilleReaderCcompressed>(); 
#else
        std::cerr << "Sorry! gzip-compressed binary reading not compiled into this build of Mille :-( Please rebuild with libz support to inspect this file. "<<std::endl;
        return 1; 
#endif 
    }
    else if (readROOT){
#ifdef SUPPORT_ROOTIO
        reader = std::make_unique<MilleReaderROOT>(); 
#else
        std::cerr << "Sorry! ROOT binary reading not compiled into this build of Mille :-( Please rebuild with ROOT support to inspect this file. "<<std::endl;
        return 1; 
#endif 
    }

    if (!reader->open(inFile)){
        std::cerr <<" Failed to open input file "<<inFile<<std::endl;
        return 1; 
    }

    // fast forward
    for (long long k = 0; k < first; ++k){
        reader->skipEntry(); 
    }
    MilleDecoder d; 
    long long recordIndex = first; 
    std::vector<MilleMeasurement> myRecord; 
    do{
        if (last > 0 && recordIndex >= last) break; 
        myRecord.clear(); 
        auto result = d.decode(*reader,myRecord); 
        if (result == MilleDecoder::ReadResult::error){
            std::cerr <<" Careful! Encountered decoding error. "<<std::endl;
            break;
        }
        else if (result == MilleDecoder::ReadResult::atEof){
            break; // done with file 
        }
        if (myRecord.empty()) continue; // empty record?!
        std::cout <<" ========== Record "<<recordIndex<<" ========== " <<" ----"<<std::endl; 
        size_t nMeasurements = 0; 
        for (const MilleMeasurement & measurement : myRecord){
            if (measurement.nSpecial() == 0){
                std::cout << " ---- Measurement "<<++nMeasurements<<":"<<std::endl; 
                std::cout << "    Value      : "<<measurement.measurement<<std::endl; 
                std::cout << "    Uncertainty: "<<measurement.uncertainty<<std::endl; 
                std::cout << "    Local derivatives: " <<std::endl; 
                for (size_t k = 0; k < measurement.nLocal(); ++k){
                    const auto & [label,value] = measurement.localEntry(k); 
                    std::cout << "        "<<label<<" -- "<<value <<std::endl; 
                }
                std::cout << "    Global derivatives: " <<std::endl; 
                for (size_t k = 0; k < measurement.nGlobal(); ++k){
                    const auto & [label,value] = measurement.globalEntry(k); 
                    std::cout << "        "<<label<<" -- "<<value <<std::endl; 
                }
            }
            else{
                std::cout << " ---- Special block ----"<<std::endl; 
                for (size_t k = 0; k < measurement.nSpecial(); ++k){
                    const auto & [specInt,specFloat] = measurement.specialEntry(k); 
                    std::cout << "        "<<specInt<<" -- "<<specFloat <<std::endl; 
                }
            }
            std::cout << std::endl; 
        }
        ++recordIndex;
        std::cout << std::endl; 
    } while (true); 
    return 0;
}
