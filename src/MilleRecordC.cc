    /**
 *  \file Implementation of the MilleRecordC class. 
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

#include "Mille/MilleRecordC.h"
#include <iostream> 

namespace Mille{
    MilleRecordC::MilleRecordC( const std::string & fname, 
                    bool doublePrecision, 
                    bool writeZero, 
                    std::size_t recordLengthHint):
        MilleRecord(doublePrecision, writeZero, recordLengthHint),
        fname_(fname){
        openC(); 
    }
    /// @brief Destructor 
    MilleRecordC::~MilleRecordC(){
        closeC();     
    } 
    void MilleRecordC::writeRecord(){
        // skip empty records 
        if (buffer_.empty()) return;
        const int recordLength =
                (buffer_.doublePrecision) ? -buffer_.size() * 2 : buffer_.size() * 2;
        // header word indicating record size and type 
        myOutFile_.write(reinterpret_cast<const char*>(&recordLength),
                sizeof(recordLength));
        // floating point content 
        if (buffer_.doublePrecision)
            myOutFile_.write(reinterpret_cast<char*>(buffer_.doubleBuffer.data()),
                    buffer_.size() * sizeof(double));
        else
            myOutFile_.write(reinterpret_cast<char*>(buffer_.floatBuffer.data()),
                    buffer_.size() * sizeof(float));
        // integer content 
        myOutFile_.write(reinterpret_cast<char*>(buffer_.intBuffer.data()),
                buffer_.size() * sizeof(int));

        // prepare for next record 
        resetBuffers();
        newSetIfNeeded();     
    }

    void MilleRecordC::flushOutputFile(){
        myOutFile_.flush(); 
    } 

    void MilleRecordC::resetOutputFile(){
        closeC();
        openC();
    } 

    void MilleRecordC::openC(){
        myOutFile_.open(fname_, std::ios::binary | std::ios::out); 
    } 
    void MilleRecordC::closeC(){
        myOutFile_.close(); 
    } 
}
