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
#include "Mille/MilleDecoder.h"
#include "Mille/MilleFactory.h"
#include <iostream>
#include <filesystem>


int main(int argc, char** argv){

    if (argc < 2){
        std::cerr << "Usage: inspectMilleBinary [-r / -c / -z / -t to force file format to ROOT / C / zipped C / plain text] <file name> [first record to print][last record to print] "<<std::endl;
        return 1; 
    }
    int iarg = 1; 
    MilleFormat userFormat = MilleFormat::Unknown; 
    if (std::string(argv[iarg]).find("-") == 0){
        std::string arg = argv[iarg]; 
        if (arg == "-z"){
            userFormat = MilleFormat::Ccompressed; 
            ++iarg;
        }
        else if (arg == "-r"){
            userFormat = MilleFormat::ROOT; 
            ++iarg;
        }
        else if (arg == "-c"){
            userFormat = MilleFormat::C; 
            ++iarg;
        }
        else if (arg == "-t"){
            userFormat = MilleFormat::CSV; 
            ++iarg;
        }
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
    // if the user has not set an option, try to auto-detect
    if (userFormat == MilleFormat::Unknown){
        reader = spawnMilleReader(inFile); 
    }
    else {
        /// hint type using user-specified option
        reader = spawnMilleReader(userFormat, inFile);
    }
    if (!reader){
        std::cerr << "Unable to spawn reader for file "<<inFile<<", bailing out"<<std::endl; 
        return 1; 
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
