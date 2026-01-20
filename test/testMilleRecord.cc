/**
 *  \file Unit tests for MilleRecord base class. 
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

#include "testUtils.h"
#include "Mille/MilleRecord.h"
using namespace Mille; 
/// @brief Test class - expose internal storage for testing. 
class MilleRecordTester: public MilleRecord{
    public: 
        /// delegate any c-tor of the base class
        template <typename... args> MilleRecordTester(args && ...a): MilleRecord(a...){}

        /// read-only accessors to the internal storage 
        const std::vector<double> & doubleBuffer() const {return buffer_.doubleBuffer;}
        const std::vector<float> & floatBuffer() const {return buffer_.floatBuffer;}
        const std::vector<int> & intBuffer() const {return buffer_.intBuffer;}

        /// implement pure virtual functions with dummies to permit instantiation 
        virtual void writeRecord() override final{}; 
        virtual void flushOutputFile() override final{}; 
        virtual void resetOutputFile() override final{}; 
};


int main(int, char**){
    using ::my_ssize; 
    /// dummy data for testing 

    std::vector<unsigned int> localLabels{1,2,4}; 
    std::vector<double> localDeriv{1.0, -0.3, 1.2}; 
    std::vector<int> globalLabels{42,345}; 
    std::vector<double> globalDeriv{-1.,1.}; 
    double residual = -0.033;
    double deltaResidual = 1.e-3; 

    // default version of tester with single floating point precision
    MilleRecordTester tester; 

    // add an entry
    tester.addData(residual, deltaResidual, 
                   localLabels, localDeriv, 
                   globalLabels,globalDeriv); 

    // check for internal state after first data addition 
    TEST_CONDITION(
        my_ssize(tester.floatBuffer()), 
        3l + my_ssize(localDeriv) + my_ssize(globalDeriv),
        "Length of float buffer after first rcd"); 
    TEST_CONDITION(
        my_ssize(tester.intBuffer()), 
        3l + my_ssize(localDeriv) + my_ssize(globalDeriv),
        "Length of float buffer after first rcd"); 
    TEST_CONDITION(
        my_ssize(tester.doubleBuffer()),  
        0l,
        "Length of double buffer after first rcd"); 
    // also test the contents of the arrays
    size_t index = 1; 
    TEST_CONDITION (tester.floatBuffer().at(index++),
                    residual, 
                    "Written residual after first rcd");
    for (size_t i = 0; i < localDeriv.size(); ++i){
        TEST_CONDITION (
            tester.floatBuffer().at(index),
            localDeriv[i],
            "Written local derivative entry "<<i<<" after first rcd"
        );
        TEST_CONDITION (
            tester.intBuffer().at(index++),
            localLabels[i],
            "Written local label entry "<<i<<" after first rcd"
        );
    }
    TEST_CONDITION (tester.floatBuffer().at(index++),
                    deltaResidual, 
                    "Written residual error after first rcd"
    );
    for (size_t k = 0; k < globalDeriv.size(); ++k){
        TEST_CONDITION (
            tester.floatBuffer().at(index),
            globalDeriv[k],
            "Written global derivative entry "<<k<<" after first rcd"
        );
        TEST_CONDITION (
            tester.intBuffer().at(index++),
            globalLabels[k],
            "Written global label entry "<<k<<" after first rcd"
        );
    }

    // check effect of abort function 
    tester.abort(); 
    TEST_CONDITION(
        my_ssize(tester.floatBuffer()),
        1l,
        "Length of post-reset float buffer"
    ); 
    TEST_CONDITION(
        my_ssize(tester.intBuffer()),
        1l,
        "Length of post-reset float buffer"
    ); 
    TEST_CONDITION(
        my_ssize(tester.doubleBuffer()),
        0l,
        "Length of post-reset double buffer"
    ); 

    // check for internal state after special data addition 
    tester.addSpecial(globalLabels,globalDeriv); 

    TEST_CONDITION(
        my_ssize(tester.floatBuffer()), 
        3l + my_ssize(globalDeriv),
        "Length of special data float buffer"
    ); 
    TEST_CONDITION(
        my_ssize(tester.intBuffer()), 
        3l + my_ssize(globalLabels),
        "Length of special data float buffer"
    ); 
    TEST_CONDITION(
        my_ssize(tester.doubleBuffer()), 
        0l,
        "Length of special data double buffer"
    ); 

    /// also try double precision

    MilleRecordTester testerDouble( true);
    testerDouble.addData(residual, deltaResidual, 
                        localLabels, localDeriv, 
                        globalLabels, globalDeriv); 

    // check for internal state after first data addition 
    TEST_CONDITION(
        my_ssize(testerDouble.doubleBuffer()), 
        3l + my_ssize(localDeriv) + my_ssize(globalDeriv),
        "Double precision - Length of float buffer after first rcd"); 
    TEST_CONDITION(
        my_ssize(testerDouble.intBuffer()), 
        3l + my_ssize(localDeriv) + my_ssize(globalDeriv),
        "Double precision - Length of float buffer after first rcd"); 
    TEST_CONDITION(
        my_ssize(testerDouble.floatBuffer()),  
        0l,
        "Double precision - Length of double buffer after first rcd"); 
    // also test the contents of the arrays
    index = 1; 
    TEST_CONDITION (testerDouble.doubleBuffer().at(index++),
                    residual, 
                    "Double precision - Written residual after first rcd");
    for (size_t i = 0; i < localDeriv.size(); ++i){
        TEST_CONDITION (
            testerDouble.doubleBuffer().at(index),
            localDeriv[i],
            "Double precision - Written local derivative entry "<<i<<" after first rcd"
        );
        TEST_CONDITION (
            testerDouble.intBuffer().at(index++),
            localLabels[i],
            "Double precision - Written local label entry "<<i<<" after first rcd"
        );
    }
    TEST_CONDITION (testerDouble.doubleBuffer().at(index++),
                    deltaResidual, 
                    "Double precision - Written residual error after first rcd"
    );
    for (size_t k = 0; k < globalDeriv.size(); ++k){
        TEST_CONDITION (
            testerDouble.doubleBuffer().at(index),
            globalDeriv[k],
            "Double precision - Written global derivative entry "<<k<<" after first rcd"
        );
        TEST_CONDITION (
            testerDouble.intBuffer().at(index++),
            globalLabels[k],
            "Double precision - Written global label entry "<<k<<" after first rcd"
        );
    } 

    return 0; 
}
