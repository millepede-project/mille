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
    doublePrecision_ (doublePrecision),
    globalDerKeepZeros_(writeZero),
    recordLengthHint_(recordLengthHint){
    intBuffer_.reserve(recordLengthHint_); 
    if (doublePrecision) doubleBuffer_.reserve(recordLengthHint_); 
    else floatBuffer_.reserve(recordLengthHint_); 
}

MilleRecord::~MilleRecord(){

}

void MilleRecord::addData(double aMeas, double aErr, unsigned int numLocal,
			unsigned int* indLocal, double* derLocal,
			const std::vector<int> &labGlobal,
			const std::vector<double> &derGlobal){
    // ensure the header is in place 
    newSetIfNeeded(); 
    // add measurement
    addEntry(0, aMeas);
    // add local derivative block
    for (unsigned int i = 0; i < numLocal; ++i){
        addEntry(indLocal[i], derLocal[i]); 
    }
    // add uncertainty 
    addEntry(0, aErr); 
    // add global derivative block 
    for (std::size_t k = 0; k < derGlobal.size(); ++k){
        if (derGlobal[k] != 0 or globalDerKeepZeros_){
            addEntry(labGlobal[k], derGlobal[k]);
        } 
    }
}

void MilleRecord::addSpecial(int nSpecial, const std::vector<double> & specialFloat, const std::vector<int> specialInt){
    if (!nSpecial) return;  // trivial empty special block 
    newSetIfNeeded(); // header 
    if (hasSpecialInRecord_){
        std::cerr << "MilleRecord::addSpecial: Special values already stored for this record."
            << std::endl; 
        return;
    }
    hasSpecialInRecord_ = true; 
    // A special record is indicted by:
    addEntry(0,0.); // a zero measurement, 
                    // followed by no local derivatives,
    addEntry(0, -1. * nSpecial);  // and a negative uncertainty, with the magnitude nSpecial 
    for (int i = 0; i < nSpecial; ++i){
        addEntry(specialInt[i],specialFloat[i]); 
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
    if (intBuffer_.size() != 0){
        return; 
    }
    /// The header of a new record is a pair (0,0) 
    addEntry(0,0.); 
}
void MilleRecord::resetBuffers(){
    intBuffer_.clear();
    floatBuffer_.clear();
    doubleBuffer_.clear();
    intBuffer_.reserve(recordLengthHint_); 
    if (doublePrecision_) doubleBuffer_.reserve(recordLengthHint_); 
    else floatBuffer_.reserve(recordLengthHint_); 
    hasSpecialInRecord_ = false; 
}

void MilleRecord::addEntry(int i, double k){
    intBuffer_.push_back(i);
    if (doublePrecision_) doubleBuffer_.push_back(k);
    else floatBuffer_.push_back(k); 
}

bool MilleRecord::copyRecord(IMilleReader & source){
    doubleBuffer_.clear(); 
    floatBuffer_.clear(); 
    intBuffer_.clear(); 
    const size_t defaultBuffer = 5000; 
    doubleBuffer_.resize(defaultBuffer); 
    floatBuffer_.resize(defaultBuffer); 
    intBuffer_.resize(defaultBuffer); 
    auto [nRead, status] = source.readToBuffer(doubleBuffer_.data(), floatBuffer_.data(), intBuffer_.data(), doubleBuffer_.size()); 
    if (status == -4){
        doubleBuffer_.resize(nRead);
        floatBuffer_.resize(nRead);
        intBuffer_.resize(nRead);
        source.rewindEntry();
        auto try2 = source.readToBuffer(doubleBuffer_.data(), floatBuffer_.data(), intBuffer_.data(), doubleBuffer_.size()); 
        nRead = try2.first;
        status = try2.second;
    }
    if (status < 0){
        std::cerr <<" Read error "<<std::endl;
        return false;
    }
    if (status == 0) return false; 
    if (status == 4){
        doubleBuffer_.clear();
        floatBuffer_.resize(nRead); 
    }
    else if (status == 8){
        floatBuffer_.clear();
        doubleBuffer_.resize(nRead);  
    }
    intBuffer_.resize(nRead); 
    return true; 
}
