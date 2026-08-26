/**
 *  \file Unit tests for Mille C-bindings. 
 *  \author Maximilian Goblirsch-Kolb, DESY, 2026 
 *  \copyright
 *  Copyright (c) 2011 - 2026 Deutsches Elektronen-Synchroton,
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

#include "Mille/readMilleC.h"
#include "testUtils.h"
#include "milleReaderTests.h"
#include "Mille/MilleFactory.h"

using namespace Mille; 

int main(int, char**){

    /// dummy data for testing 
    MilleTestInputs inputs(1); 
    {
        auto record = spawnMilleRecord("milleTest.dat"); 
        inputs.writeTo(*record); 
    }
    auto dummy = inputs.dummyData.front();

    // open using C-bindings
    int errFlag = 0; 
    open_mille("milleTest.dat", std::string("milleTest.dat").size(),&errFlag,1);
    TEST_CONDITION(errFlag, 0, "Successful file open"); 


    std::vector<double> doubles(1,0.);
    std::vector<float> floats(1,0.);
    std::vector<int> ints(1,0);

    // test catching invalid buffers 
    errFlag = 0; 
    int bufSize = ints.size(); 
    read_mille(nullptr, floats.data(), ints.data(),&bufSize,&errFlag,1); 
    TEST_CONDITION(errFlag, -1, "Invalid buffer: Exit code"); 

    // pass undersized buffers
    bufSize = ints.size();
    errFlag = 0; 
    read_mille(doubles.data(), floats.data(), ints.data(),&bufSize,&errFlag,1); 
    TEST_CONDITION(errFlag, int(-4), "Too short buffers: Exit code"); 
    TEST_CONDITION(bufSize, int(3 + dummy.localLabels.size() + dummy.globalLabels.size() + 2 + dummy.specialFloat.size()), "Too short buffers: Length output");
    // resize our read buffer - should give successful read after next rewind
    doubles.resize(bufSize,0);
    ints.resize(bufSize,0);
    floats.resize(bufSize,0);

    // should now be at EOF and yield exit code 0 - regardless of buffer size
    bufSize = ints.size();
    errFlag = 0; 
    read_mille(doubles.data(), floats.data(), ints.data(),&bufSize,&errFlag,1); 
    TEST_CONDITION(errFlag, 0, "EOF: Exit code"); 

    // rewind the file and start over 
    reset_mille(1);
    // should now return an errFlag of 4 and the correct number of read bytes
    bufSize = ints.size();
    errFlag = 0; 
    read_mille(doubles.data(), floats.data(), ints.data(),&bufSize,&errFlag,1); 
    TEST_CONDITION(errFlag, 4, "Status for successful read"); 
    TEST_CONDITION(bufSize, ints.size(), "bufSize for successful read"); 

    // this should allow us to read the same entry again, without hitting EoF
    reset_mille_entry(1);
    // should return an errFlag of 4 and the correct number of read bytes
    bufSize = ints.size();
    errFlag = 0; 
    read_mille(doubles.data(), floats.data(), ints.data(),&bufSize,&errFlag,1); 
    TEST_CONDITION(errFlag, 4, "Status for successful read after rewind"); 
    TEST_CONDITION(bufSize, ints.size(), "bufSize for successful read after rewind"); 

    // validate the output against the input 

    // check the derivatives - note that the interface guarantees them to end up 
    // in the *double* buffer, regardless of the file-internal storage. 
    size_t ix = 0; 
    // use a float conversion to test at floating rather than double-precision
    // epsilion, as the information has been internally stored as float 
    // in the binary file. 
    TEST_CONDITION((float)doubles[ix++],0, "Float header word"); 
    TEST_CONDITION((float)doubles[ix++],dummy.residual, "Residual"); 
    for (size_t i =0; i < dummy.localDeriv.size(); ++i){
        TEST_CONDITION((float)doubles[ix++],dummy.localDeriv[i], "Local derivative index "<<i); 
    }
    TEST_CONDITION((float)doubles[ix++],dummy.deltaResidual, "Uncertainty"); 
    for (size_t i =0; i < dummy.globalDeriv.size(); ++i){
        TEST_CONDITION((float)doubles[ix++],dummy.globalDeriv[i], "Global derivative index "<<i); 
    }
    TEST_CONDITION((float)doubles[ix++],0, "Special header"); 
    TEST_CONDITION((float)doubles[ix++],-1. * dummy.specialFloat.size(), "nSpecial"); 
    for (size_t i =0; i < dummy.specialFloat.size(); ++i){
        TEST_CONDITION((float)doubles[ix++],dummy.specialFloat[i], "Special float "<<i); 
    }

    // check the ints 
    ix = 0; 
    TEST_CONDITION(ints[ix++],0, "Int header word"); 
    TEST_CONDITION(ints[ix++],0, "Residual label"); 
    for (size_t i =0; i < dummy.localLabels.size(); ++i){
        TEST_CONDITION(ints[ix++],dummy.localLabels[i], "Local label index "<<i); 
    }
    TEST_CONDITION(ints[ix++],0, "Uncertainty label"); 
    for (size_t i =0; i < dummy.globalLabels.size(); ++i){
        TEST_CONDITION(ints[ix++],dummy.globalLabels[i], "Global label index "<<i); 
    }
    TEST_CONDITION(ints[ix++],0, "Special header"); 
    TEST_CONDITION(ints[ix++],0, "nSpecial (int entry)"); 
    for (size_t i =0; i < dummy.specialInt.size(); ++i){
        TEST_CONDITION(ints[ix++],dummy.specialInt[i], "Special integer "<<i); 
    }

    return 0; 
}
