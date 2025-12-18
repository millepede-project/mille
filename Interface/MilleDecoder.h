/*
 * MilleDecoder.h
 *
 *  Created on: Dec 18, 2025
 *      Author: goblirsc
 */

/** \file
 *  MilleDecoder declaration. Helper class to decode a Mille record. 
 *
 *  \author Maximilian Goblirsch-Kolb, DESY, 2025 
 *
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

#include "IMilleReader.h" 
#include <vector> 

#ifndef MILLE__DECODER__H
#define MILLE__DECODER__H

/// @brief Helper class to decode a Mille record 
class MilleDecoder{
    public:
        enum class ReadResult{
            OK=0,
            atEof=1,
            error=2
        };
        /// @brief Data structure for the decoded record.
        struct Measurement{
            ReadResult res=ReadResult::OK; 
            double measurement; 
            double uncertainty;
            std::vector<std::pair<int, double>> localDerivatives={};
            std::vector<std::pair<int, double>> globalDerivatives={};
            std::vector<std::pair<int, double>> specialData={};
        };

        /// @brief constructor
        MilleDecoder()=default;
        /// @brief accessor - retrieve the next entry 
        /// @param[in] reader: A reader connected to a file to read from 
        /// @return A vector of decoded measurements for the current record
        std::vector<Measurement> decode (IMilleReader & reader);  

    private:
        bool nextMeasurement(long int current, long int & startLocal, long int  & startGlobal, long int & lastGlobal); 
        Measurement decodeEntry(long int start, long int startLocal, long int startGlobal, long int lastGlobal); 

        std::vector<int> intBuffer_ = {};       ///< read buffer for ints
        std::vector<float> floatBuffer_ = {};     ///< read buffer for floats
        std::vector<double> doubleBuffer_ = {};    ///< read buffer for doubles
        const size_t defaultBufferSize_ = 5000;  ///< starting buffer size

};




#endif // MILLE__DECODER__H
