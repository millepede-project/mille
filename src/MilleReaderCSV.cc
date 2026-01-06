/**
 *  \file Implementation of the MilleReaderCSV class. 
 *  \author Maximilian Goblirsch-Kolb, DESY, 2026 
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

#include "MilleReaderCSV.h"
#include <iostream>
#include <sstream>

std::string MilleReaderCSV::nextLine(std::ifstream & input){
    std::string line;  
    std::getline(input, line ); 
    if (input.eof()) return ""; 
    line = line.substr(0,line.find_first_of("!#")); 
    auto start = line.find_first_not_of(" "); 
    auto end = line.find_last_not_of(" "); 
    if (start == std::string::npos) line = ""; 
    else{
        line = line.substr(start,end-start+1); 
    }
    if (line.empty()) return nextLine(input); 
    return line;
}

bool MilleReaderCSV::open(const std::string & fname){
    if (infile_.is_open() || !fname_.empty()){
        std::cerr << " Careful, calling open() on an already active file handler!"<<std::endl;
        close(); 
    }
    fname_ = fname; 
    infile_.open(fname, std::ios::in); 
    return infile_.is_open(); 
}
void MilleReaderCSV::close(){
    if (infile_.is_open()) infile_.close();
    fname_ = "";
}
void MilleReaderCSV::reset(){
    infile_.clear();
    infile_.seekg(0, std::ios::beg);
}
void MilleReaderCSV::rewindEntry(){
    infile_.clear();
    infile_.seekg(lastPos_,std::ios::beg); 
}
void MilleReaderCSV::skipEntry(){
    lastPos_ = infile_.tellg(); 
    int recordLength = 0; // get size of the next record 
    std::string line = nextLine(infile_); 
    std::stringstream sst(line); 
    sst >> recordLength; 
    recordLength /=2; 
    for (size_t nentries = 0; nentries < recordLength; ++nentries){
        line = nextLine(infile_); 
    }
}

std::pair<int,int> MilleReaderCSV::readToBuffer(  double *bufferDouble, 
                            float *bufferFloat, 
                            int *bufferInt,
                            int bufferSize){
    if (!bufferDouble || !bufferFloat || !bufferInt || !bufferSize) {
        return {0,-1};
    }
    lastPos_ = infile_.tellg(); 
    int recordLength = 0; // get size of the next record 
    std::string line = nextLine(infile_);
    if (!infile_.good()){
        if (infile_.eof()){
            // empty file 
            return {0,0}; 
        }
        else{
            std::cerr << " Read error - record length for file " <<fname_<<std::endl;
            return {0, -2}; 
        }
    }
    std::stringstream sstr(line); 
    sstr >> recordLength; 
    bool doublePrecision = (recordLength < 0); 
    recordLength = std::abs(recordLength) / 2 ; 

    /// catch if we exceed the buffer size 
    if (recordLength > bufferSize){
        // skip forward
        for (size_t k = 0; k < recordLength; ++k){
            (void)nextLine(infile_);
        }
        if (infile_.bad()){
            std::cerr << "Problem with stream or EOF skipping entries"<<std::endl;
            return {0,-32}; 
        }
        return {recordLength, -4};
    }
    for (size_t entry = 0; entry < recordLength; ++entry){
        line = nextLine(infile_); 
        if (infile_.bad()){
            std::cerr << "Problem with stream or EOF reading CSV entries"<<std::endl;
            return {recordLength,-32}; 
        }
        sstr.clear(); 
        sstr << line; 
        if (doublePrecision) 
            sstr >> bufferInt[entry] >> bufferDouble[entry]; 
        else{
            sstr >> bufferInt[entry] >> bufferDouble[entry];
            bufferFloat[entry] = bufferDouble[entry];  
        }
    }
    return {recordLength, doublePrecision ? 8 : 4}; 
}
