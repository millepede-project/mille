/**
*  \file Standardised tests for a Mille reader instance. 
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

#ifndef MILLE__READER__TESTS__H
#define MILLE__READER__TESTS__H

#include "Mille/IMilleReader.h"
#include "Mille/MilleRecord.h"
#include "testUtils.h"
#include <random>
#include <string>


// standardised test inputs
struct MilleTestInputs{

    struct dummyRecord{
        std::vector<unsigned int> localLabels;
        std::vector<double> localDeriv;
        std::vector<int> globalLabels;
        std::vector<double> globalDeriv;
        std::vector<int> specialInt;
        std::vector<double> specialFloat;
        double residual;
        double deltaResidual; 
    };

    dummyRecord generateDummy(){
        std::uniform_real_distribution<double> f;
        dummyRecord r;
        r.localLabels = {1,2,3}; 
        r.localDeriv.push_back(f(m_rnd)); 
        r.localDeriv.push_back(f(m_rnd)); 
        r.localDeriv.push_back(f(m_rnd)); 
        r.globalLabels = {42,45,96}; 
        r.globalDeriv.push_back(f(m_rnd)); 
        r.globalDeriv.push_back(f(m_rnd)); 
        r.globalDeriv.push_back(f(m_rnd)); 
        r.specialInt = {99};
        r.specialFloat = {f(m_rnd)};
        r.residual = f(m_rnd);
        r.deltaResidual = std::abs(f(m_rnd));
        return r; 
    }

    MilleTestInputs (int nEvents=1){
        for (int k = 0; k < nEvents; ++k){
            dummyData.push_back(generateDummy());
        }
    }
    /// dummy data for testing 
    std::vector<dummyRecord> dummyData; 
    /// helper function to write the test data into an 
    /// arbitraty type of Mille record. 
    void writeTo(Mille::MilleRecord & mille){
        for (const dummyRecord & dummy : dummyData ){
            mille.addData(dummy.residual, dummy.deltaResidual, 
                        dummy.localLabels,
                        dummy.localDeriv, 
                        dummy.globalLabels,
                        dummy.globalDeriv); 
            mille.addSpecial(dummy.specialInt, dummy.specialFloat); 

            mille.writeRecord();
        }
        mille.flushOutputFile(); 
    }
    std::random_device m_rnd; 
};


/// @brief Run a set of tests for any mille reader class
/// @param reader: The reader instance to test
/// @param inputs: Set of test inputs to use
/// @param expectedPrecision: 4 (for float) or 8 (for double) internal precision
/// @return 0 if success, else 1
int MilleReaderTestSuite(Mille::IMilleReader & reader, MilleTestInputs & inputs, int expectedPrecision=4){

    // buffers 
    std::vector<double> doubles(1,0.);
    std::vector<float> floats(1,0.);
    std::vector<int> ints(1,0);

    // test catching invalid buffers 
    auto [nRead, status] = reader.readToBuffer(nullptr, floats.data(), ints.data(),1); 
    TEST_CONDITION(status, -1, "Invalid buffer: Exit code"); 

    // pass undersized buffers
    const MilleTestInputs::dummyRecord & dummy = inputs.dummyData.front();
    auto res = reader.readToBuffer(doubles.data(), floats.data(), ints.data(),1); 
    nRead=res.first; status=res.second; 
    TEST_CONDITION(status, -4, "Too short buffers: Exit code"); 
    TEST_CONDITION(nRead, 3 + dummy.localLabels.size() + dummy.globalLabels.size() + 2 + dummy.specialFloat.size(), "Too short buffers: Length output");
    // resize our read buffer - should give successful read after next rewind, bc. all reads are designed to give the same length 
    doubles.resize(nRead,0);
    ints.resize(nRead,0);
    floats.resize(nRead,0);

    reader.reset();

    // validate the output against the input 
    std::size_t iEntry = 0; 
    for (const auto & thisDummy : inputs.dummyData){
        ++iEntry; 
        std::string theEntry = ", Entry "+std::to_string(iEntry)+ " of "+std::to_string(inputs.dummyData.size());
        // should always return a status of 4/8 and the correct number of read bytes 
        res = reader.readToBuffer(doubles.data(), floats.data(), ints.data(),ints.size()); 
        nRead=res.first; status=res.second; 
        TEST_CONDITION(status, expectedPrecision, "Status for successful read"+theEntry); 
        TEST_CONDITION(nRead, ints.size(), "nRead for successful read"+theEntry); 


        // check the derivatives - note that the interface guarantees them to end up 
        // in the *double* buffer, regardless of the file-internal storage. 
        size_t ix = 0; 
        // use a float conversion to test at floating rather than double-precision
        // epsilion, as the information has been internally stored as float 
        // in the binary file. 
        TEST_CONDITION((float)doubles[ix++],0, "Float header word"+theEntry); 
        TEST_CONDITION((float)doubles[ix++],thisDummy.residual, "Residual"+theEntry); 
        for (size_t i =0; i < thisDummy.localDeriv.size(); ++i){
            TEST_CONDITION((float)doubles[ix++],thisDummy.localDeriv[i], "Local derivative index "<<i<<theEntry); 
        }
        TEST_CONDITION((float)doubles[ix++],thisDummy.deltaResidual, "Uncertainty"<<theEntry); 
        for (size_t i =0; i < thisDummy.globalDeriv.size(); ++i){
            TEST_CONDITION((float)doubles[ix++],thisDummy.globalDeriv[i], "Global derivative index "<<i<<theEntry); 
        }
        TEST_CONDITION((float)doubles[ix++],0, "Special header word"<<theEntry); 
        TEST_CONDITION((int)(-1 * doubles[ix++]),thisDummy.specialInt.size(), "nSpecial"<<theEntry); 
        for (size_t i =0; i < thisDummy.specialFloat.size(); ++i){
            TEST_CONDITION((float)doubles[ix++],thisDummy.specialFloat[i], "Special float  index "<<i<<theEntry); 
        }

        // check the ints 
        ix = 0; 
        TEST_CONDITION(ints[ix++],0, "Int header word"<<theEntry); 
        TEST_CONDITION(ints[ix++],0, "Residual label"<<theEntry); 
        for (size_t i =0; i < thisDummy.localLabels.size(); ++i){
            TEST_CONDITION(ints[ix++],thisDummy.localLabels[i], "Local label index "<<i<<theEntry); 
        }
        TEST_CONDITION(ints[ix++],0, "Uncertainty label"<<theEntry); 
        for (size_t i =0; i < thisDummy.globalLabels.size(); ++i){
            TEST_CONDITION(ints[ix++],thisDummy.globalLabels[i], "Global label index "<<i<<theEntry); 
        }
        TEST_CONDITION(ints[ix++], 0, "Special header word"<<theEntry); 
        TEST_CONDITION(ints[ix++], 0, "nSpecial int"<<theEntry); 
        for (size_t i =0; i < thisDummy.specialInt.size(); ++i){
            TEST_CONDITION(ints[ix++],thisDummy.specialInt[i], "Special int  index "<<i<<theEntry); 
        }
    }
    // should now be at EOF and yield exit code 0 - regardless of buffer size
    res = reader.readToBuffer(doubles.data(), floats.data(), ints.data(),nRead); 
    nRead=res.first; status=res.second; 
    TEST_CONDITION(status, 0, "EOF: Exit code"); 

    // rewind the file and start over 
    reader.reset();

    for (size_t k = 0; k < inputs.dummyData.size(); ++k){
        reader.skipEntry();
    }
    reader.rewindEntry();
    res = reader.readToBuffer(doubles.data(), floats.data(), ints.data(),ints.size()); 
    nRead=res.first; status=res.second; 
    TEST_CONDITION(status, expectedPrecision, "Status for successful read after rewind"); 
    TEST_CONDITION(nRead, ints.size(), "nRead for successful read after rewind"); 

    return 0; 
}







#endif // MILLE__READER__TESTS__H
