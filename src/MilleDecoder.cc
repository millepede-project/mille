/**
 *  \file Implementation of the MilleDecoder class. 
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

#include "MilleDecoder.h"
#include <iostream>

std::vector<MilleDecoder::Measurement> MilleDecoder::decode (IMilleReader & reader){
    intBuffer_.clear();
    floatBuffer_.clear();
    doubleBuffer_.clear();
    intBuffer_.resize(defaultBufferSize_,0);
    floatBuffer_.resize(defaultBufferSize_,0.);
    doubleBuffer_.resize(defaultBufferSize_,0.);
    // attempt to read 
    auto [nRead, status] = reader.readToBuffer(doubleBuffer_.data(), 
                                               floatBuffer_.data(), 
                                               intBuffer_.data(), 
                                               intBuffer_.size()); 

    if (status == 0){
        return {}; 
    }
    if (status == -4){
        intBuffer_.resize(nRead);
        floatBuffer_.resize(nRead);
        doubleBuffer_.resize(nRead);
        reader.rewindEntry(); 
        auto res = reader.readToBuffer(doubleBuffer_.data(), 
                                    floatBuffer_.data(), 
                                    intBuffer_.data(), 
                                    intBuffer_.size()); 
        nRead = res.first;
        status = res.second;
    }
    if (status < 0){
        std::cerr << "MilleDecoder: Failed to read the next entry " <<std::endl;
        return {}; 
    }
    // read was OK, now we decode the content and populate the passed data object 
    long int current = 1;   // skip record header
    long int startLocal = 0; 
    long int startGlobal = 0; 
    long int lastGlobal = 0; 
    std::vector<Measurement> result; 
    while (nextMeasurement(current, startLocal, startGlobal, lastGlobal) && current < nRead){
        result.push_back(decodeEntry(current, startLocal, startGlobal, lastGlobal)); 
        current = lastGlobal+1;
    }
    return result; 
}
   
MilleDecoder::Measurement MilleDecoder::decodeEntry(long int start, long int startLocal, long int startGlobal, long int lastGlobal){
    MilleDecoder::Measurement meas; 
    // special blocks
    if (    doubleBuffer_.at(startLocal) == 0 
            && doubleBuffer_.at(startGlobal) < 0 
            && startGlobal == startLocal + 1 ){
        std::cout <<" reading a special block" <<std::endl; 
        int nSpecial = int(-doubleBuffer_.at(startGlobal)); 
        for (int k = startGlobal+1; k <= startGlobal+nSpecial; ++k){
            meas.specialData.push_back(std::make_pair(intBuffer_[k],doubleBuffer_[k])); 
        }
        return meas; 
    }
    // normal blocks 
    meas.measurement = doubleBuffer_.at(startLocal); 
    meas.uncertainty = doubleBuffer_.at(startGlobal); 
    if (startGlobal > startLocal + 1){
        for (int ix = startLocal+1; ix < startGlobal; ++ix){
            meas.localDerivatives.push_back(std::make_pair(intBuffer_[ix], doubleBuffer_[ix]));
        }
    }
    if (lastGlobal > startGlobal){
        for (int ix = startGlobal+1; ix <= lastGlobal; ++ix){
            meas.globalDerivatives.push_back(std::make_pair(intBuffer_[ix], doubleBuffer_[ix]));
        }
    }
    return meas; 
}
bool MilleDecoder::nextMeasurement(long int current, long int & startLocal, long int  & startGlobal, long int & lastGlobal){
    startLocal = -1;
    startGlobal = -1;
    lastGlobal = -1; 
    for (long int index = current; index < intBuffer_.size(); ++index){
        if (intBuffer_[index] == 0 && startLocal < 0) startLocal = index; 
        else if (intBuffer_[index] == 0 && startLocal >=0 && startGlobal < 0) startGlobal = index;
        else if (intBuffer_[index] == 0 && startLocal >=0 && startGlobal >=0) {
            lastGlobal = index - 1;
            break;
        }
    }
    if (startLocal >= 0 && startGlobal >=0 && lastGlobal < 0) lastGlobal = intBuffer_.size() - 1; 
    return (startLocal >=0 && startGlobal >=0 && lastGlobal >=0);
}
