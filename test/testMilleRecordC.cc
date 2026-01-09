/**
 *  \file Unit tests for MilleRecordC class. 
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
#include "MilleRecordC.h"
#include <vector>
#include <iostream>
#include <limits>
#include <fstream>
#include <filesystem>


int main(int, char**){

    /// dummy data for testing 
    std::vector<unsigned int> localLabels{1,2,4}; 
    std::vector<double> localDeriv{1.0, -0.3, 1.2}; 
    std::vector<int> globalLabels{42,345}; 
    std::vector<double> globalDeriv{-1.,1.}; 
    double residual = -0.033;
    double deltaResidual = 1.e-3; 

    // default version of tester with single floating point precision
    {
        MilleRecordC mille("milleTest.dat");
        // add an entry
        mille.addData(residual, deltaResidual, 
                    localLabels, localDeriv, 
                    globalLabels, globalDeriv); 

        mille.writeRecord();
        mille.flushOutputFile(); 
    }

    // check the file per hand 
    std::filesystem::path myFile("milleTest.dat"); 
    int fSize = std::filesystem::file_size(myFile);
    TEST_CONDITION(fSize, sizeof(int) + (sizeof(int) + sizeof(float)) * (localLabels.size() + globalLabels.size() + 3),"Size of written binary"); 

    // check the header 
    std::ifstream fs (myFile,std::ios::in | std::ios::binary);
    int recLength = -1; 
    fs.read(reinterpret_cast<char*>(&recLength), sizeof(int)); 
    if (fs.bad()){
        std::cout << " bad file handle reading header" <<std::endl;
        return 1; 
    }
    TEST_CONDITION(recLength, 2 * (localLabels.size() + globalLabels.size() + 3), "Record length in file header"); 

    // check the floats 
    recLength = std::abs(recLength)/2; 
    std::vector<float> floats(recLength, 0); 
    fs.read(reinterpret_cast<char*>(floats.data()), sizeof(float)*recLength); 
    if (fs.bad()){
        std::cout << " bad file handle reading floats" <<std::endl;
        return 1; 
    }
    size_t ix = 0; 
    TEST_CONDITION(floats[ix++],0, "Float header word"); 
    TEST_CONDITION(floats[ix++],residual, "Residual"); 
    for (size_t i =0; i < localDeriv.size(); ++i){
        TEST_CONDITION(floats[ix++],localDeriv[i], "Local derivative index "<<i); 
    }
    TEST_CONDITION(floats[ix++],deltaResidual, "Uncertainty"); 
    for (size_t i =0; i < globalDeriv.size(); ++i){
        TEST_CONDITION(floats[ix++],globalDeriv[i], "Global derivative index "<<i); 
    }


    // check the ints 
    std::vector<int> ints(recLength, 0); 
    fs.read(reinterpret_cast<char*>(ints.data()), sizeof(int)*recLength); 
    if (fs.bad()){
        std::cout << " bad file handle reading ints" <<std::endl;
        return 1; 
    }
    ix = 0; 
    TEST_CONDITION(ints[ix++],0, "Int header word"); 
    TEST_CONDITION(ints[ix++],0, "Residual label"); 
    for (size_t i =0; i < localLabels.size(); ++i){
        TEST_CONDITION(ints[ix++],localLabels[i], "Local label index "<<i); 
    }
    TEST_CONDITION(ints[ix++],0, "Uncertainty label"); 
    for (size_t i =0; i < globalLabels.size(); ++i){
        TEST_CONDITION(ints[ix++],globalLabels[i], "Global label index "<<i); 
    }


    return 0; 
}
