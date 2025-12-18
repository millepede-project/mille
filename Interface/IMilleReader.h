/*
 * IMilleReader.h
 *
 *  Created on: Dec 17, 2025
 *      Author: goblirsc
 */

/** \file
 *  IMilleReader declaration. Abstract interface class for all Mille readers. 
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

#ifndef IMILLEREADER__H
#define IMILLEREADER__H

#include <string> 

/// @brief Abstract base class for readers for Mille binaries. 
class IMilleReader{
    public: 
        virtual ~IMilleReader(){}
        /// @brief Open a file for reading
        /// @param fname path to the file 
        /// @return true on success, false on error. 
        virtual bool open(const std::string & fname)=0;
        /// @brief  Close a file 
        virtual void close()=0;
        /// @brief reset a file to the first entry 
        virtual void reset()=0;
        /// @brief rewind to the start the last entry read 
        virtual void rewindEntry()=0; 
        /// @brief skip to the next entry 
        virtual void skipEntry()=0; 
        /// @brief read the next record in the file to the provided buffers. 
        ///        The labels are guaranteed to be stored in the int buffer,
        ///        and the derivatives in the *double* buffer.
        ///        Depending on the file format, the float buffer may or may not 
        ///        be filled with the derivatives as well. 
        /// @param [out] bufferDouble: buffer to store the double array   
        /// @param [out] bufferFloat: buffer to store the float array   
        /// @param [out] bufferInt: buffer to store the integer array   
        /// @param [in] bufferSize: maximum available buffer size 
        /// @return Pair of ints: 
        ///             First element indicates number of read entries (= used buffer size), 
        ///                 Sizes larger than buffer size indicate entry was skipped. 
        ///             Second element is a status code: 
        ///                  -1: pointer to a buffer or lengthBuffers are null
        ///                  -2: problem reading record length
        ///                  -4: given buffers too short for record
        ///                  -8: problem with stream or EOF reading floats
        ///                 -16: problem with stream or EOF reading ints
        ///                 -32: problem with stream or EOF reading doubles
        ///                  =0: reached end of file (or read empty record?!)
        ///                  =4: found floats
        ///                  =8: found doubles   
        ///                 All non-negative values indicate a success
        virtual std::pair<int,int> readToBuffer(  double *bufferDouble, 
                            float *bufferFloat, 
                            int *bufferInt,
                            int bufferSize)=0;
};


#endif // IMILLEREADER__H
