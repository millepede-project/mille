/**
 *  \file Unit tests for MilleRecordROOT class. 
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
#include "MilleRecordROOT.h"
#include <vector>
#include <iostream>
#include <limits>
#include <fstream>
#include <filesystem>
#include "TFile.h"
#include "TTree.h"


int main(int, char**){
    using ::ssize; 
    /// dummy data for testing 
    std::vector<unsigned int> localLabels{1,2,4}; 
    std::vector<double> localDeriv{1.0, -0.3, 1.2}; 
    std::vector<int> globalLabels{42,345}; 
    std::vector<double> globalDeriv{-1.,1.}; 
    double residual = -0.033;
    double deltaResidual = 1.e-3; 

    // default version of tester with single floating point precision
    {
        MilleRecordROOT mille("milleTest.root");
        // add an entry
        mille.addData(residual, deltaResidual, 
                    localDeriv.size(), localLabels.data(),
                    localDeriv.data(), globalLabels,
                    globalDeriv); 

        mille.writeRecord();
        mille.flushOutputFile(); 
    }

    TFile* readBack = TFile::Open("milleTest.root","READ");
    if (!readBack || !readBack->IsOpen()){
        std::cerr << "File was not written correctly"<<std::endl;
        return 1; 
    }
    TTree* tree = nullptr;  
    readBack->GetObject<TTree>("MilleRecords",tree); 
    if(!tree){
        std::cerr << "Failed to read back the tree"<<std::endl;
        return 1; 
    }
    TEST_CONDITION(tree->GetEntries(), 1, "Tree entries"); 
    std::vector<double>* doubles = nullptr;
    std::vector<float>* floats = nullptr;
    std::vector<int>* ints = nullptr;
    int floatingPointFormat; 
    tree->SetBranchAddress("floats",&floats); 
    tree->SetBranchAddress("ints",&ints); 
    tree->SetBranchAddress("doubles",&doubles); 
    tree->SetBranchAddress("floatingPointFormat",&floatingPointFormat); 
    tree->GetEntry(0); 
    TEST_CONDITION(floatingPointFormat,4,"Recorded floating point format"); 
    TEST_CONDITION(ssize(*floats),3 + ssize(globalDeriv) + ssize(localDeriv),"Number of float entries"); 
    TEST_CONDITION(ssize(*ints),3 + ssize(globalDeriv) + ssize(localDeriv),"Number of int entries"); 

    size_t ix = 0; 
    TEST_CONDITION(floats->at(ix++),0, "Float header word"); 
    TEST_CONDITION(floats->at(ix++),residual, "Residual"); 
    for (size_t i =0; i < localDeriv.size(); ++i){
        TEST_CONDITION(floats->at(ix++),localDeriv[i], "Local derivative index "<<i); 
    }
    TEST_CONDITION(floats->at(ix++),deltaResidual, "Uncertainty"); 
    for (size_t i =0; i < globalDeriv.size(); ++i){
        TEST_CONDITION(floats->at(ix++),globalDeriv[i], "Global derivative index "<<i); 
    }

    // check the ints 
    ix = 0; 
    TEST_CONDITION(ints->at(ix++),0, "Int header word"); 
    TEST_CONDITION(ints->at(ix++),0, "Residual label"); 
    for (size_t i =0; i < localLabels.size(); ++i){
        TEST_CONDITION(ints->at(ix++),localLabels[i], "Local label index "<<i); 
    }
    TEST_CONDITION(ints->at(ix++),0, "Uncertainty label"); 
    for (size_t i =0; i < globalLabels.size(); ++i){
        TEST_CONDITION(ints->at(ix++),globalLabels[i], "Global label index "<<i); 
    }
    
    return 0; 
}
