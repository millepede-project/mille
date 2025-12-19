/**
*  \file Unit test utilities for the Mille package . 
 *  \author Maximilian Goblirsch-Kolb, DESY, 2025 
 *  \copyright
 *  Copyright (c) 2011 - 2020 Deutsches Elektronen-Synchroton,
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
 * 
*/ 

#ifndef MILLE_TESTUTILS__H
#define MILLE_TESTUTILS__H

#include <vector>
#include <iostream>
#include <limits>

/// @brief Comparison method to check for equivalence of two different-typed values
/// Indented for double - float comparisons where they happen to be needed. 
/// @tparam T type of the left hand side argument
/// @tparam Q type of the right hand side argument
/// @param lhs value of the left hand side argument
/// @param rhs value of the right hand side argument
/// @return Bool indicating if the two arguments are compatible within the precision of the left hand side type 
template <typename T, typename Q>  typename std::enable_if<std::is_convertible_v<Q,T>, bool>::type testCompat(T lhs, Q rhs){
    T r_as_t = rhs; 
    return std::abs(lhs - r_as_t)<=std::numeric_limits<T>::epsilon(); 
}
/// @brief Template specialisation to check for exact equivalence of integral values. 
template <typename T> typename std::enable_if<std::is_integral_v<T>, bool>::type testCompat(T lhs, T rhs){
    return lhs == rhs; 
}

/// preprocessor macro to test for equivalence beween a test and a target value. 
/// @param test_val: Test result
/// @param target_val: Target value for test
/// @param message stream input for describing the comparison performed. 
#define TEST_CONDITION(test_val, target_val, msg){ \
    auto lhs = test_val; auto rhs = target_val; \
    if (!testCompat(lhs, rhs)) { \
        std::cerr << "Failure in test line "<<__LINE__<<": "    \
        << msg << " deviates from the expectation - found "\
        <<lhs<<", but expected "<<rhs<< std::endl; \
        return 1; \
    } \
}

// pre-c++20 signed size accessor
template <class container> 
long my_ssize (const container & c){return c.size();} 

#endif // MILLE_TESTUTILS__H
