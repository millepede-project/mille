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
#include "MilleDataStructures.h" 
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

        /// @brief constructor
        MilleDecoder()=default;
        /// @brief accessor - retrieve the next record 
        /// @param[in] reader: A reader connected to a file to read from 
        /// @param[out] record: A vector to be filled with the measurements from the next record 
        /// @return A status code indicating the outcome of the operation 
        ReadResult decode (IMilleReader & reader, std::vector<MilleMeasurement> & record);  

    private:
        /// @brief Find the next measurement in the buffer. 
        /// @param [in] current Starting index in the buffer to search 
        /// @param [out] startLocal Will be set to the index storing the measured value
        /// @param [out] startGlobal Will be set to the index storing the uncertainty on the measurement
        /// @param [out] lastGlobal Will be set to the index storing the last entry of this measurement (last global derivative or uncertainty if no such)
        /// @return a bool - true indicates a new measurement was found, false indicates we are done with the buffer 
        bool nextMeasurement(long int current, long int & startLocal, long int  & startGlobal, long int & lastGlobal); 
        
        /// @brief Decodes an entry and extracts a measurement.
        ///        Expects the indices to be determined by a prior call to nextMeasurement. 
        /// @param [out] startLocal Index storing the measured value
        /// @param [out] startGlobal Index storing the uncertainty on the measurement
        /// @param [out] lastGlobal Index storing the last entry of this measurement (last global derivative or uncertainty if no such)
        MilleMeasurement decodeEntry(long int startLocal, long int startGlobal, long int lastGlobal); 

        MilleBuffer buffer_; 
        const size_t defaultBufferSize_ = 5000;  ///< starting buffer size

};




#endif // MILLE__DECODER__H
