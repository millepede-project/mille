/*
 * MilleReaderCSV.h
 *
 *  Created on: Jan 6, 2026
 *      Author: goblirsc
 */

/** \file
 *  MilleReaderCSV declaration. Reader for plain text files 
 *
 *  \author Maximilian Goblirsch-Kolb, DESY, 2026 
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

 #ifndef MILLE_READER_CSV__H
 #define MILLE_READER_CSV__H
 
 #include "Mille/IMilleReader.h" 
 #include "fstream" 

namespace Mille{
    /// @brief Reader for uncompressed C binaries. 
    class MilleReaderCSV : public IMilleReader{
        public: 
            MilleReaderCSV()=default; 
            MilleReaderCSV(const std::string & fin){fname_ = fin;} 
            virtual ~MilleReaderCSV(){close();}
            /// @brief Open a file for reading
            /// @param fname path to the file 
            /// @return true on success, false on error. 
            virtual bool open(const std::string & fname="") override final;
            /// @brief  Close a file 
            virtual void close() override final;
            /// @brief reset a file to the first entry 
            virtual void reset() override final;
            /// @brief rewind to the start the last entry read 
            virtual void rewindEntry() override final; 
            /// @brief skip to the next entry
            virtual void skipEntry() override final; 
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
                                int bufferSize) override final;

            /// @brief helper function - extract the next meaningful line from the file 
            /// Skips all comments and whitespaces. 
            /// Will keep reading past comment lines until encountering nontrivial comment 
            /// @param str: Input stream to read from
            /// @return A string containing the stripped next line. 
            ///         Upon reaching EOF, will return empty string. 
            static std::string nextLine(std::ifstream & str);
        private: 
            std::ifstream infile_;
            std::string fname_=""; 
            std::streampos lastPos_=0; 
    };
}


 
 
 
 #endif // MILLE_READER_CSV__H
