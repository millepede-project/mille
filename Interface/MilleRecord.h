/*
 * MilleRecord.h
 *
 *  Created on: Dec 15, 2025
 *      Author: goblirsc
 */

/** \file
 *  MilleRecord definition. Base class for the writeable Mille payload. 
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

#ifndef MILLERECORD__H
#define MILLERECORD__H

#include <vector>
#include "IMilleReader.h"


/// @brief  Base class for a writeable Mille record. 
///         Defines the data payload and the abstract 
///         I/O interface methods. 
class MilleRecord{
    public:
    /// @brief  Constructor
    /// @param writeZero If enabled, will write entries with global derivatives of 0 into the output array. Note that using these within pede has not been tested. 
    /// @param doublePrecision: Store derivatives with double precision. 
    /// @param recordLengthHint: Hint for expected record length (will reserve buffer space). 
    MilleRecord(bool doublePrecision = false, bool writeZero = false,
			    std::size_t recordLengthHint = 2000); 
    
    /// @brief Destructor 
    virtual ~MilleRecord(); 
	
    /// @brief Interface to add an entry to the current record. 
	/// @param aMeas Measured residual (or other measurement) 
	/// @param aErr Uncertainty of the measurement
	/// @param numLocal Number of local parameters / derivatives 
	/// @param indLocal Indices for local derivatives 
	/// @param derLocal Local derivatives 
	/// @param labGlobal Labels for global parameters
	/// @param derGlobal  Global derivatives 
	void addData(double aMeas, double aErr, unsigned int numLocal,
			unsigned int* indLocal, double* derLocal,
			const std::vector<int> &labGlobal,
			const std::vector<double> &derGlobal);
    
            /// @brief Add a special block to the current record. 
    /// @param nSpecial: Length of the special block 
    /// @param specialFloat: Floating point entries 
    /// @param specialInt: Integer entries 
    void addSpecial(int nSpecial, const std::vector<double> & specialFloat, const std::vector<int> specialInt);

    /// @brief Quickly copies encoded buffers for next record from a reader.
    ///        Used to quickly translate Mille formats. 
    /// @param source: A reader instance connected to a file 
    /// @return A flag indicating the success of the operation. 
    ///         False indicates either an error or EOF
    bool copyRecord(IMilleReader & source); 
    
    /// @brief Abort the current record, reset internal storage 
    void abort();

    /// @brief Interface for writing out the current entry 
    virtual void writeRecord() = 0;

    /// @brief Interface for flushing the current file to disk 
    virtual void flushOutputFile() = 0; 

    /// @brief Interface for closing and reopening the current file 
    virtual void resetOutputFile() = 0; 

    protected: 

        /// ==== Helper functions  === 
        /// @brief Add header for a new record if buffers are empty.
        ///        Will not do anything if buffers are populated. 
        void newSetIfNeeded(); 

        /// @brief reset the internal buffers, ready for a new entry
        void resetBuffers(); 

        /// @brief add one pair <label, value> to internal buffers. 
        /// Picks the floating point buffer corresponding to user-configured precision.
        /// Use of this interface ensures consistent buffer shape. 
        /// @param i: Label / value to fill into the integer array 
        /// @param k: Value to fill into the floating point array
        void addEntry(int i, double k);

        /// ==== Data members - accessible to derived classes === 

        std::vector<int> intBuffer_ = {}; ///< Integer buffer
        std::vector<float> floatBuffer_ = {}; ///< Float buffer
        std::vector<double> doubleBuffer_ = {}; ///< Double buffer
        bool doublePrecision_ = false;  ///< Flag for double precision storage
        bool globalDerKeepZeros_ = false;   ///< Flag to write global derivatives with value zero
        bool hasSpecialInRecord_ = false;   ///< Flag whether we already have a 'special' block in the current record
        std::size_t recordLengthHint_ = 2000; /// < expected record length 

};




#endif // MILLERECORD__H
