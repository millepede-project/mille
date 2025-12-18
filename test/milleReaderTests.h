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

#include "IMilleReader.h"
#include "MilleRecord.h"
#include "testUtils.h"

// standardised test inputs
struct MilleTestInputs{

    /// dummy data for testing 
    std::vector<unsigned int> localLabels{1,2,4}; 
    std::vector<double> localDeriv{1.0, -0.3, 1.2}; 
    std::vector<int> globalLabels{42,345}; 
    std::vector<double> globalDeriv{-1.,1.}; 
    double residual = -0.033;
    double deltaResidual = 1.e-3; 
    /// helper function to write the test data into an 
    /// arbitraty type of Mille record. 
    void writeTo(MilleRecord & mille){
        mille.addData(residual, deltaResidual, 
                    localDeriv.size(), localLabels.data(),
                    localDeriv.data(), globalLabels,
                    globalDeriv); 

        mille.writeRecord();
        mille.flushOutputFile(); 
    }

};

/// @brief Run a set of tests for any mille reader class
/// @param reader: The reader instance to test
/// @param inputs: Set of test inputs to use
/// @param expectedPrecision: 4 (for float) or 8 (for double) internal precision
/// @return 0 if success, else 1
int MilleReaderTestSuite(IMilleReader & reader, MilleTestInputs & inputs, int expectedPrecision=4){

    // buffers 
    std::vector<double> doubles(1,0.);
    std::vector<float> floats(1,0.);
    std::vector<int> ints(1,0);

    // test catching invalid buffers 
    auto [nRead, status] = reader.readToBuffer(nullptr, floats.data(), ints.data(),1); 
    TEST_CONDITION(status, -1, "Invalid buffer: Exit code"); 

    // pass undersized buffers
    auto res = reader.readToBuffer(doubles.data(), floats.data(), ints.data(),1); 
    nRead=res.first; status=res.second; 
    TEST_CONDITION(status, -4, "Too short buffers: Exit code"); 
    TEST_CONDITION(nRead, 3 + inputs.localLabels.size() + inputs.globalLabels.size(), "Too short buffers: Length output");
    // resize our read buffer - should give successful read after next rewind
    doubles.resize(nRead,0);
    ints.resize(nRead,0);
    floats.resize(nRead,0);

    // should now be at EOF and yield exit code 0 - regardless of buffer size
    res = reader.readToBuffer(doubles.data(), floats.data(), ints.data(),1); 
    nRead=res.first; status=res.second; 
    TEST_CONDITION(status, 0, "EOF: Exit code"); 

    // rewind the file and start over 
    reader.reset();
    // should now return a status of 4 and the correct number of read bytes
    res = reader.readToBuffer(doubles.data(), floats.data(), ints.data(),ints.size()); 
    nRead=res.first; status=res.second; 
    TEST_CONDITION(status, expectedPrecision, "Status for successful read"); 
    TEST_CONDITION(nRead, ints.size(), "nRead for successful read"); 

    // this should allow us to read the same entry again, without hitting EoF
    reader.rewindEntry();
    // should return a status of 4 and the correct number of read bytes
    res = reader.readToBuffer(doubles.data(), floats.data(), ints.data(),ints.size()); 
    nRead=res.first; status=res.second; 
    TEST_CONDITION(status, expectedPrecision, "Status for successful read after rewind"); 
    TEST_CONDITION(nRead, ints.size(), "nRead for successful read after rewind"); 

    // validate the output against the input 

    // check the derivatives - note that the interface guarantees them to end up 
    // in the *double* buffer, regardless of the file-internal storage. 
    size_t ix = 0; 
    // use a float conversion to test at floating rather than double-precision
    // epsilion, as the information has been internally stored as float 
    // in the binary file. 
    TEST_CONDITION((float)doubles[ix++],0, "Float header word"); 
    TEST_CONDITION((float)doubles[ix++],inputs.residual, "Residual"); 
    for (size_t i =0; i < inputs.localDeriv.size(); ++i){
        TEST_CONDITION((float)doubles[ix++],inputs.localDeriv[i], "Local derivative index "<<i); 
    }
    TEST_CONDITION((float)doubles[ix++],inputs.deltaResidual, "Uncertainty"); 
    for (size_t i =0; i < inputs.globalDeriv.size(); ++i){
        TEST_CONDITION((float)doubles[ix++],inputs.globalDeriv[i], "Global derivative index "<<i); 
    }

    // check the ints 
    ix = 0; 
    TEST_CONDITION(ints[ix++],0, "Int header word"); 
    TEST_CONDITION(ints[ix++],0, "Residual label"); 
    for (size_t i =0; i < inputs.localLabels.size(); ++i){
        TEST_CONDITION(ints[ix++],inputs.localLabels[i], "Local label index "<<i); 
    }
    TEST_CONDITION(ints[ix++],0, "Uncertainty label"); 
    for (size_t i =0; i < inputs.globalLabels.size(); ++i){
        TEST_CONDITION(ints[ix++],inputs.globalLabels[i], "Global label index "<<i); 
    }
    return 0; 
}







#endif // MILLE__READER__TESTS__H
