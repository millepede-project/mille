/**
 *  \file Unit tests for MilleDecoder class. 
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
#include "MilleReaderC.h"
#include "MilleRecordC.h"
#include "MilleDecoder.h"
#include "milleReaderTests.h"
#include <vector>
#include <iostream>
#include <limits>
#include <fstream>
#include <filesystem>


int compareExpectMeasurement(const MilleDecoder::Measurement & meas, const MilleTestInputs & inputs){

    TEST_CONDITION((float)meas.measurement, inputs.residual, "Extracted residual");
    TEST_CONDITION((float)meas.uncertainty, inputs.deltaResidual, "Extracted uncertainty");
    TEST_CONDITION(ssize(meas.localDerivatives),ssize(inputs.localDeriv), "number of extracted local derivatives");
    TEST_CONDITION(ssize(meas.globalDerivatives),ssize(inputs.globalDeriv), "number of extracted global derivatives");
    for (size_t k = 0; k < meas.localDerivatives.size(); ++k){
        TEST_CONDITION(meas.localDerivatives[k].first, inputs.localLabels[k], "Local derivative #"<<k<<": label");
        TEST_CONDITION((float)meas.localDerivatives[k].second, inputs.localDeriv[k], "Local derivative #"<<k<<": value");
    }
    for (size_t k = 0; k < meas.globalDerivatives.size(); ++k){
        TEST_CONDITION(meas.globalDerivatives[k].first, inputs.globalLabels[k], "Global derivative #"<<k<<": label");
        TEST_CONDITION((float)meas.globalDerivatives[k].second, inputs.globalDeriv[k], "Global derivative #"<<k<<": value");
    }

    return 0; 
};

int compareExpectSpecial(const MilleDecoder::Measurement & meas, const std::vector<int> & specialInt, const std::vector<double> & specialDouble){

    TEST_CONDITION(ssize(meas.localDerivatives),0, "no local derivatives on special data");
    TEST_CONDITION(ssize(meas.globalDerivatives),0, "no global derivatives on special data");
    TEST_CONDITION(ssize(meas.specialData),specialInt.size(), "number of special data entries");
    for (size_t k = 0; k < meas.specialData.size(); ++k){
        TEST_CONDITION(meas.specialData[k].first, specialInt[k], "Special int data #"<<k<<": label");
        TEST_CONDITION((float)meas.specialData[k].second, specialDouble[k], "Special double data #"<<k<<": value");
    }
    return 0; 
};

int main(int, char**){

    /// dummy data for testing 
    MilleTestInputs inputs; 
    std::vector<int> specialInt{1,2};   // guaranteed to be very special
    std::vector<double> specialDouble{3,4}; 

    {
        // single floating point precision
        MilleRecordC mille("milleTest.dat");
        
        mille.addData(inputs.residual, inputs.deltaResidual, inputs.localDeriv.size(), 
        inputs.localLabels.data(), inputs.localDeriv.data(),
        inputs.globalLabels, inputs.globalDeriv);

        mille.addData(inputs.residual, inputs.deltaResidual, inputs.localDeriv.size(), 
        inputs.localLabels.data(), inputs.localDeriv.data(),
        inputs.globalLabels, inputs.globalDeriv);

        mille.addSpecial(specialInt.size(), specialDouble, specialInt); 

        mille.writeRecord();

        mille.addData(inputs.residual, inputs.deltaResidual, inputs.localDeriv.size(), 
        inputs.localLabels.data(), inputs.localDeriv.data(),
        inputs.globalLabels, inputs.globalDeriv);

        mille.writeRecord();
         
    }

    MilleReaderC reader; 
    // test single precision
    if (!reader.open("milleTest.dat")){
        std::cout << " Failed to open file"<<std::endl;
        return 1;    
    }
    MilleDecoder decoder;
    auto measurementsFirst = decoder.decode(reader); 
    TEST_CONDITION(ssize(measurementsFirst), 3, "Number of measurements in first record"); 

    auto measurementsSecond = decoder.decode(reader); 
    TEST_CONDITION(ssize(measurementsSecond), 1, "Number of measurements in second record"); 

    // the file only has two record. Should now return an empty record, indicating EOF
    auto measurementsEOF = decoder.decode(reader); 
    TEST_CONDITION(ssize(measurementsEOF), 0, "Number of measurements when reading past EOF"); 

    /// validate content 
    int ret = 0; 
    ret = compareExpectMeasurement(measurementsFirst[0], inputs);
    if (ret != 0) return ret; 
    ret = compareExpectMeasurement(measurementsFirst[1], inputs);
    if (ret != 0) return ret;
    ret = compareExpectSpecial(measurementsFirst[2], specialInt, specialDouble);
    if (ret != 0) return ret; 
    ret = compareExpectMeasurement(measurementsSecond[0], inputs);
    return ret; 
}
