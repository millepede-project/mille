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

#include "Mille/MilleDecoder.h"
#include <iostream>

namespace Mille{
    MilleDecoder::ReadResult MilleDecoder::decode (IMilleReader & reader, std::vector<MilleMeasurement> & record){
        buffer_.clear();
        buffer_.resize(defaultBufferSize_,true); 
        // attempt to read 
        auto [nRead, status] = reader.readToBuffer(buffer_); 

        if (status == 0){
            return ReadResult::atEof; 
        }
        if (status == -4){
            buffer_.resize(nRead,true);
            reader.rewindEntry(); 
            auto res = reader.readToBuffer(buffer_); 
            nRead = res.first;
            status = res.second;
        }
        if (status < 0){
            std::cerr << "MilleDecoder: Failed to read the next entry " <<std::endl;
            return ReadResult::error; 
        }
        return decode(buffer_, nRead, record); 
    }
    MilleDecoder::ReadResult MilleDecoder::decode (const MilleBuffer & buffer, int recordLength, std::vector<MilleMeasurement> & record) const{
        long int current = 1;   // skip record header
        long int startLocal = 0; 
        long int startGlobal = 0; 
        long int lastGlobal = 0; 
        while (nextMeasurement(buffer, current, startLocal, startGlobal, lastGlobal) && current < recordLength){
            record.push_back(decodeEntry(buffer, startLocal, startGlobal, lastGlobal)); 
            current = lastGlobal+1;
        }
        return ReadResult::OK; 
    }
    
    MilleMeasurement MilleDecoder::decodeEntry(const MilleBuffer & buffer,long int startLocal, long int startGlobal, long int lastGlobal) const {
        MilleMeasurement meas; 
        // special blocks
        if (    buffer.doubleBuffer.at(startLocal) == 0 
                && buffer.doubleBuffer.at(startGlobal) < 0 
                && startGlobal == startLocal + 1 ){
            int nSpecial = int(-buffer.doubleBuffer.at(startGlobal)); 
            for (int k = startGlobal+1; k <= startGlobal+nSpecial; ++k){
                meas.specialInts.push_back(buffer.intBuffer[k]);
                meas.specialFloat.push_back(buffer.doubleBuffer[k]); 
            }
            return meas; 
        }
        // normal blocks 
        meas.measurement = buffer.doubleBuffer.at(startLocal); 
        meas.uncertainty = buffer.doubleBuffer.at(startGlobal); 
        if (startGlobal > startLocal + 1){
            for (int ix = startLocal+1; ix < startGlobal; ++ix){
                meas.localLabels.push_back(buffer.intBuffer[ix]);
                meas.localDerivatives.push_back(buffer.doubleBuffer[ix]);
            }
        }
        if (lastGlobal > startGlobal){
            for (int ix = startGlobal+1; ix <= lastGlobal; ++ix){
                meas.globalLabels.push_back(buffer.intBuffer[ix]);
                meas.globalDerivatives.push_back(buffer.doubleBuffer[ix]);
            }
        }
        return meas; 
    }
    bool MilleDecoder::nextMeasurement(const MilleBuffer & buffer,long int current, long int & startLocal, long int  & startGlobal, long int & lastGlobal) const {
        startLocal = -1;
        startGlobal = -1;
        lastGlobal = -1; 
        for (size_t index = current; index < buffer.intBuffer.size(); ++index){
            if (buffer.intBuffer[index] == 0 && startLocal < 0) startLocal = index; 
            else if (buffer.intBuffer[index] == 0 && startLocal >=0 && startGlobal < 0) startGlobal = index;
            else if (buffer.intBuffer[index] == 0 && startLocal >=0 && startGlobal >=0) {
                lastGlobal = index - 1;
                break;
            }
        }
        if (startLocal >= 0 && startGlobal >=0 && lastGlobal < 0) lastGlobal = buffer.intBuffer.size() - 1; 
        return (startLocal >=0 && startGlobal >=0 && lastGlobal >=0);
    }
}
