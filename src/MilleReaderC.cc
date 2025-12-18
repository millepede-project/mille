/**
 *  \file Implementation of the MilleReaderC class. 
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

#include "MilleReaderC.h"
#include <iostream>

bool MilleReaderC::open(const std::string & fname){
    if (infile_.is_open() || !fname_.empty()){
        std::cerr << " Careful, calling open() on an already active file handler!"<<std::endl;
        close(); 
    }
    fname_ = fname; 
    infile_.open(fname, std::ios::in | std::ios::binary); 
    return infile_.is_open(); 
}
void MilleReaderC::close(){
    infile_.close();
    fname_ = "";
}
void MilleReaderC::reset(){
    infile_.clear();
    infile_.seekg(0, std::ios::beg);
}
void MilleReaderC::rewindEntry(){
    infile_.clear();
    infile_.seekg(lastPos_,std::ios::beg); 
}
std::pair<int,int> MilleReaderC::readToBuffer(  double *bufferDouble, 
                            float *bufferFloat, 
                            int *bufferInt,
                            int bufferSize){
    if (!bufferDouble || !bufferFloat || !bufferInt || !bufferSize) {
        return {0,-1};
    }
    lastPos_ = infile_.tellg(); 
    int recordLength = 0; // get size of the next record 
    infile_.read(reinterpret_cast<char*>(&recordLength), sizeof(recordLength)); 
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
    bool doublePrecision = (recordLength < 0); 
    // divide by two to get length per field 
    recordLength = std::abs(recordLength) / 2 ; 

    /// catch if we exceed the buffer size 
    if (recordLength > bufferSize){
        // skip forward
        if (doublePrecision){
            infile_.seekg(recordLength * sizeof(double), std::ios_base::cur); 
            if (infile_.bad()){
                std::cerr << "Problem with stream or EOF skipping doubles"<<std::endl;
                return {0,-32}; 
            }
        }
        else{
            infile_.seekg(recordLength * sizeof(float), std::ios_base::cur); 
            if (infile_.bad()){
                std::cerr << "Problem with stream or EOF skipping floats"<<std::endl;
                return {0,-8}; 
            }
        }
        infile_.seekg(recordLength * sizeof(int), std::ios_base::cur); 
        if (infile_.bad()){
            std::cerr << "Problem with stream or EOF skipping ints"<<std::endl;
            return {0,-16}; 
        }
        return {recordLength, -4};
    }

    if (doublePrecision){
        infile_.read(reinterpret_cast<char*>(bufferDouble), sizeof(double) * recordLength); 
        if (infile_.bad()){
            std::cerr << "Problem with stream or EOF reading doubles"<<std::endl;
            return {recordLength,-32}; 
        }
    }
    else{
        infile_.read(reinterpret_cast<char*>(bufferFloat), sizeof(float) * recordLength); 
        if (infile_.bad()){
            std::cerr << "Problem with stream or EOF reading floats"<<std::endl;
            return {recordLength,-8}; 
        }
        // convention: Double buffer shall always be populated with derivatives
        for (int k = 0; k < recordLength; ++k){
            bufferDouble[k] = bufferFloat[k]; 
        }
    }
    infile_.read(reinterpret_cast<char*>(bufferInt), sizeof(int) * recordLength); 
    if (infile_.bad()){
        std::cerr << "Problem with stream or EOF reading ints"<<std::endl;
        return {recordLength,-16}; 
    }
    return {recordLength, (doublePrecision ? 8 : 4)}; 
}
