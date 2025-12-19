/*
 * MilleDataStructures.h
 *
 *  Created on: Dec 18, 2025
 *      Author: goblirsc
 */

/** \file
 *  Data structures for working with Mille data in C++ code.  
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

#ifndef MILLE_DATA_STRUCTURES__H
#define MILLE_DATA_STRUCTURES__H

#include <vector>

/// @brief Helper to represent decoded Mille data. 
///        This stands for a single measurement (equation). 
///        A record (e.g. a particle track) consists of 
///        a list of measurements. 
struct MilleMeasurement{
    double measurement; 
    double uncertainty;
    std::vector<int> localLabels={};
    std::vector<double> localDerivatives={};
    std::vector<int> globalLabels={};
    std::vector<double> globalDerivatives={};
    std::vector<int> specialInts={};
    std::vector<double> specialFloat={};

    /// convenience functions 

    /// @brief Number of local derivatives 
    /// @return the number of local derivatives
    size_t nLocal() const {return localLabels.size();}

    /// @brief Number of global derivatives 
    /// @return the number of global derivatives
    size_t nGlobal() const {return globalLabels.size();}

    /// @brief Number of special data entries 
    /// @return the number of special data entries
    size_t nSpecial() const {return specialInts.size();}

    /// @brief get the corresponding label and value for a local derivative
    /// @param i index of the derivative 
    /// @return a pair - label and value 
    std::pair<int,double> localEntry(size_t i) const{
        return std::make_pair(localLabels.at(i), localDerivatives.at(i)); 
    }
    /// @brief get the corresponding label and value for a global derivative
    /// @param i index of the derivative 
    /// @return a pair - label and value 
    std::pair<int,double> globalEntry(size_t i) const{
        return std::make_pair(globalLabels.at(i), globalDerivatives.at(i)); 
    }
    /// @brief get a special data entry
    /// @param i index of the entry
    /// @return a pair - int and float values 
    std::pair<int,double> specialEntry(size_t i) const{
        return std::make_pair(specialInts.at(i), specialFloat.at(i)); 
    }
};

/// @brief Helper for the buffer structure used to encode Mille records. 
///        Closely linked to the binary representation. 
///        Useful when working in compiled C++. 
struct MilleBuffer{
    std::vector<int> intBuffer; ///< stores parameter labels
    std::vector<float> floatBuffer; ///< potentially used as auxiliary buffer
    std::vector<double> doubleBuffer;   ///< used to store derivatives, measurements and uncertainties 
    bool doublePrecision=false; ///< flag to store whether double-precision is selected for disk storage.  
    
    /// @brief Utility function - resizes all vectors 
    /// @param target: Desired size 
    /// @param allBuffers: If true, reset all vectors (also the ones not required by configured storage precision)
    void resize(size_t target, bool allBuffers=false){
        intBuffer.resize(target);
        if (allBuffers || !doublePrecision) floatBuffer.resize(target);
        if (allBuffers ||  doublePrecision) doubleBuffer.resize(target);
    }    
    
    /// @brief Utility function - reserves space in all vectors 
    /// @param target: Desired size 
    /// @param allBuffers: If true, reset all vectors (also the ones not required by configured storage precision)
    void reserve(size_t target, bool allBuffers=false){
        intBuffer.reserve(target);
        if(allBuffers || !doublePrecision) floatBuffer.reserve(target);
        if(allBuffers ||  doublePrecision) doubleBuffer.reserve(target);
    }
    
    /// @brief Utility function - clears all vectors
    void clear(){
        intBuffer.clear();
        floatBuffer.clear();
        doubleBuffer.clear();
    }
    
    /// Utility function - clears vectors not required by configured storage precision 
    void clearUnused(){
        if (doublePrecision) floatBuffer.clear();
        else doubleBuffer.clear(); 
    }
    
    /// @brief Utility function - add a label-value pair into
    ///        the vectors required by the user-selected precision
    void fill(int label, double value){
        intBuffer.push_back(label);
        if (doublePrecision) doubleBuffer.push_back(value);
        else floatBuffer.push_back(value); 
    }

    /// @brief Utility function - report if the buffer is empty 
    bool empty() const{
        return intBuffer.empty(); 
    }
    
    /// @brief Utility function - report current buffer size
    size_t size() const{
        return intBuffer.size(); 
    }
};

#endif // MILLE_DATA_STRUCTURES__H
