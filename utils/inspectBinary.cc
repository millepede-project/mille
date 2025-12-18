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

#include "MilleReaderCcompressed.h"
#include "MilleReaderC.h"
#include "MilleReaderROOT.h"
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
    else if (readCGZ) reader = std::make_unique<MilleReaderCcompressed>(); 
    else if (readROOT) reader = std::make_unique<MilleReaderROOT>(); 

    if (!reader->open(inFile)){
        std::cerr <<" Failed to open input file "<<inFile<<std::endl;
        return 1; 
    }

    // fast forward
    for (long long k = 0; k < first; ++k){
        reader->skipEntry(); 
    }
    MilleDecoder d; 
    long long record = first; 
    do{
        if (last > 0 && record >= last) break; 
        auto measurements = d.decode(*reader); 
        if (measurements.empty()) break; 
        std::cout <<" ========== Record "<<record<<" ========== " <<" ----"<<std::endl; 
        for (size_t m = 0; m < measurements.size(); ++m){
            if (measurements[m].specialData.empty()){
                std::cout << " ---- Measurement "<<m<<"/"<<measurements.size()<<std::endl; 
                std::cout << "    Value      : "<<measurements[m].measurement<<std::endl; 
                std::cout << "    Uncertainty: "<<measurements[m].uncertainty<<std::endl; 
                std::cout << "    Local derivatives: " <<std::endl; 
                for (auto & [label, value] : measurements[m].localDerivatives){
                    std::cout << "        "<<label<<" -- "<<value <<std::endl; 
                }
                std::cout << "    Global derivatives: " <<std::endl; 
                for (auto & [label, value] : measurements[m].globalDerivatives){
                    std::cout << "        "<<label<<" -- "<<value <<std::endl; 
                }
            }
            else{
                std::cout << " ---- Special block "<<m<<"/"<<measurements.size()<<" ----"<<std::endl; 
                for (auto & [label, value] : measurements[m].specialData){
                    std::cout << "        "<<label<<" -- "<<value <<std::endl; 
                }
            }
            std::cout << std::endl; 
        }
        ++record;
        std::cout << std::endl; 
    } while (true); 
    return 0;
}
