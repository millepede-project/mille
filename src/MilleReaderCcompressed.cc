/**
 *  \file Implementation of the MilleReaderCcompressed class. 
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
#include <iostream>

bool MilleReaderCcompressed::open(const std::string & fname){
    if (!fname_.empty()){
        std::cerr << " Careful, calling open() on an already active file handler!"<<std::endl;
        close(); 
    }
    fname_ = fname; 
    infile_ = gzopen(fname_.c_str(),"rb"); 
    if (!infile_){
        std::cerr <<" Failed to open '"<<fname_<<"'"<<std::endl;
        return false; 
    }
    return true; 
}
void MilleReaderCcompressed::close(){
    gzclose(infile_);
    fname_ = "";
}
void MilleReaderCcompressed::reset(){
    gzrewind(infile_); 
}
void MilleReaderCcompressed::rewindEntry(){
    gzseek(infile_,lastPos_,SEEK_SET); 
}
std::pair<int,int> MilleReaderCcompressed::readToBuffer(  double *bufferDouble, 
                            float *bufferFloat, 
                            int *bufferInt,
                            int bufferSize){
    if (!bufferDouble || !bufferFloat || !bufferInt || !bufferSize) {
        return {0,-1};
    }
    lastPos_ = gztell(infile_); 
    int recordLength = 0; // get size of the next record 
    int nR = gzread(infile_, &recordLength, sizeof(recordLength)); 
	if (gzeof(infile_)) {
		return {0,0};
	}
	else if (sizeof(recordLength) != nR) {
		return {0,-2};
	}
    bool doublePrecision = (recordLength < 0); 
    // divide by two to get length per field 
    recordLength = std::abs(recordLength) / 2 ; 

    /// catch if we exceed the buffer size 
    if (recordLength > bufferSize){
        // skip forward
        if (doublePrecision){
            int ret = gzseek(infile_, recordLength * sizeof(double), SEEK_CUR); 
            if(ret < 0){
                std::cerr << "Problem with stream or EOF skipping doubles"<<std::endl;
                return {0,-32}; 
            }
        }
        else{
            int ret = gzseek(infile_,recordLength * sizeof(float), SEEK_CUR); 
            if (ret < 0){
                std::cerr << "Problem with stream or EOF skipping floats"<<std::endl;
                return {0,-8}; 
            }
        }
        int ret = gzseek(infile_,recordLength * sizeof(int), SEEK_CUR); 
        if (ret < 0){
            std::cerr << "Problem with stream or EOF skipping ints"<<std::endl;
            return {0,-16}; 
        }
        return {recordLength, -4};
    }

    if (doublePrecision){
        int nRead = gzread(infile_,reinterpret_cast<char*>(bufferDouble), sizeof(double) * recordLength); 
        if (nRead != recordLength * sizeof(double)){
            std::cerr << "Problem with stream or EOF reading doubles"<<std::endl;
            return {recordLength,-32}; 
        }
    }
    else{
        int nRead = gzread(infile_,reinterpret_cast<char*>(bufferFloat), sizeof(float) * recordLength); 
        if (nRead != recordLength * sizeof(float)){
            std::cerr << "Problem with stream or EOF reading floats"<<std::endl;
            return {recordLength,-8}; 
        }
        // convention: Double buffer shall always be populated with derivatives
        for (int k = 0; k < recordLength; ++k){
            bufferDouble[k] = bufferFloat[k]; 
        }
    }
    int nread = gzread(infile_,reinterpret_cast<char*>(bufferInt), sizeof(int) * recordLength); 
    if (nread != sizeof(int)*recordLength){
        std::cerr << "Problem with stream or EOF reading ints"<<std::endl;
        return {recordLength,-16}; 
    }
    return {recordLength, (doublePrecision ? 8 : 4)}; 
}
