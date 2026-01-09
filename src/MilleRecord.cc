/**
 *  \file Implementation of the MilleRecord class. 
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

#include "MilleRecord.h"
#include <iostream> 

MilleRecord::MilleRecord(bool doublePrecision, bool writeZero, std::size_t recordLengthHint):
    globalDerKeepZeros_(writeZero),
    recordLengthHint_(recordLengthHint){
        buffer_.doublePrecision = doublePrecision;
        buffer_.reserve(recordLengthHint); 
}

MilleRecord::~MilleRecord(){

}

void MilleRecord::addData(double aMeas, double aErr, 
			MilleArrayView<unsigned int> indLocal, MilleArrayView<double> derLocal,
			MilleArrayView<int> labGlobal,
			MilleArrayView<double> derGlobal){
    // ensure the header is in place 
    newSetIfNeeded(); 
    // add measurement
    buffer_.fill(0, aMeas);
    // add local derivative block
    for (unsigned int i = 0; i < derLocal.size(); ++i){
        buffer_.fill(indLocal[i], derLocal[i]); 
    }
    // add uncertainty 
    buffer_.fill(0, aErr); 
    // add global derivative block 
    for (std::size_t k = 0; k < derGlobal.size(); ++k){
        if (derGlobal[k] != 0 or globalDerKeepZeros_){
            buffer_.fill(labGlobal[k], derGlobal[k]);
        } 
    }
}
void MilleRecord::copyToBuffer(MilleArrayView<int> integers, MilleArrayView<double> floats){
    if (integers.size() != floats.size()){
        std::cerr << " MilleRecord::copyToBuffer expects both arrays to have same size, but received "<<integers.size()<<" int and "<<floats.size()<<" float. "<<std::endl; 
        return ;   
    }
    for (size_t k = 0; k < integers.size(); ++k){
        buffer_.fill(integers[k], floats[k]);
    }
}

void MilleRecord::addSpecial(int nSpecial, MilleArrayView<double> specialFloat, MilleArrayView<int> specialInt){
    if (!nSpecial) return;  // trivial empty special block 
    newSetIfNeeded(); // header 
    if (hasSpecialInRecord_){
        std::cerr << "MilleRecord::addSpecial: Special values already stored for this record."
            << std::endl; 
        return;
    }
    hasSpecialInRecord_ = true; 
    // A special record is indicated by:
    buffer_.fill(0,0.); // a zero measurement, 
                    // followed by no local derivatives,
    buffer_.fill(0, -1. * nSpecial);  // and a negative uncertainty, with the magnitude nSpecial 
    for (int i = 0; i < nSpecial; ++i){
        buffer_.fill(specialInt[i],specialFloat[i]); 
    }
}

void MilleRecord::abort(){
    // clear the current content
    resetBuffers(); 
    // and write a fresh header 
    newSetIfNeeded(); 
}
void MilleRecord::newSetIfNeeded(){
    /// if we are in an active record, do not add a new header
    if (!buffer_.empty()){
        return; 
    }
    /// The header of a new record is a pair (0,0) 
    buffer_.fill(0,0.); 
}
void MilleRecord::resetBuffers(){
    buffer_.clear(); 
    buffer_.reserve(recordLengthHint_); 
    hasSpecialInRecord_ = false; 
}

bool MilleRecord::copyRecord(IMilleReader & source){
    buffer_.clear(); 
    const size_t defaultBuffer = 5000; 
    buffer_.resize(defaultBuffer,true); 
    auto [nRead, status] = source.readToBuffer(buffer_); 
    if (status == -4){
        buffer_.resize(nRead,true); 
        source.rewindEntry();
        auto try2 = source.readToBuffer(buffer_); 
        nRead = try2.first;
        status = try2.second;
    }
    if (status < 0){
        std::cerr <<" Read error "<<std::endl;
        return false;
    }
    if (status == 0) return false; 
    buffer_.resize(nRead); 
    buffer_.clearUnused();  
    return true; 
}
