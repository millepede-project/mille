/**
 *  \file Implementation of the MilleReaderROOT class. 
 *  \author Maximilian Goblirsch-Kolb, DESY, 2025 
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

#include "MilleReaderROOT.h"
#include <iostream>

bool MilleReaderROOT::open(const std::string & fname){
    if (file_ && file_->IsOpen() && !fname_.empty()){
        std::cerr << " Careful, calling open() on an already active file handler!"<<std::endl;
        close(); 
    }
    fname_ = fname; 
    // Access the ROOT file. 
    file_.reset(TFile::Open(fname.c_str(),"READ")); 
    if (!file_ || !file_->IsOpen()){
        std::cerr << "could not open file "<<fname<<std::endl;
        return false; 
    }
    // Load the tree
    file_->GetObject<TTree>("MilleRecords",tree_);
    if (!tree_){
        std::cerr << "could not read the tree 'MilleRecords' from file "<<fname<<std::endl;
        return false; 
    }
    // connect our branches 
    tree_->SetBranchAddress("doubles",&doubles_); 
    tree_->SetBranchAddress("ints",	  &ints_); 
    tree_->SetBranchAddress("floats",	  &floats_); 
    tree_->SetBranchAddress("floatingPointFormat",	  &floatingPointFormat_); 

    // configure TTreeCache 
    tree_->SetCacheSize(100000000);	// 100 MB
    tree_->AddBranchToCache("*",true); 
    tree_->StopCacheLearningPhase();
    // check number of entries 
    nentries_ = tree_->GetEntries();
    // ensure valid status of internal pointers 
    ints_ = nullptr;
    doubles_ = nullptr; 
    floats_ = nullptr; 
    return true; 
}
void MilleReaderROOT::close(){
    // Release the unique_ptr to prevent a ROOT related double-delete 
    if (file_ && file_->IsOpen()) file_->Close(); 
    file_.release(); 
    tree_ = nullptr;
    doubles_ = nullptr;
    ints_ = nullptr;
    floats_ = nullptr;
    entryNumber_ = 0; 
    nentries_ = 0; 
}
void MilleReaderROOT::reset(){
    entryNumber_ = 0; 
}
void MilleReaderROOT::rewindEntry(){
    entryNumber_ = lastEntry_; 
}
void MilleReaderROOT::skipEntry(){
    lastEntry_ = entryNumber_; 
    ++entryNumber_; 
}
std::pair<int,int> MilleReaderROOT::readToBuffer(  double *bufferDouble, 
                            float * bufferFloat, 
                            int *bufferInt,
                            int bufferSize){
    if (!bufferDouble || !bufferFloat || !bufferInt || !bufferSize) {
        return {0,-1};
    }
    lastEntry_ = entryNumber_; 
    // the equivalent of EOF with a root tree 
    if (entryNumber_ >= nentries_){
        return {0,0}; 
    }
    // read the next entry, increment internal entry counter
    int got = tree_->GetEntry(entryNumber_++);
    // if we read nothing, we are at the end of the file - return EOF
    if (!got) return {0,0}; 
    // check the length of the label array for this record 
    int recordLength = ints_->size();
    bool doublePrecision = (floatingPointFormat_  == 8); 
    
    /// catch if we exceed the buffer size 
    if (recordLength > bufferSize){
        // skip forward
        return {recordLength, -4};
    }
    // copy label info into buffer
    std::memcpy(bufferInt, 	  ints_->data(),   recordLength*sizeof(ints_->front()   )); 
    if (doublePrecision){
        std::memcpy(bufferDouble, doubles_->data(),recordLength*sizeof(doubles_->front())); 
    }
    else{
        std::memcpy(bufferFloat, floats_->data(),recordLength*sizeof(floats_->front())); 
        // convert and populate double array 
        for (size_t k = 0; k < floats_->size(); ++k){
            bufferDouble[k] = bufferFloat[k]; 
        }
    }
    return {recordLength, floatingPointFormat_}; 
}
