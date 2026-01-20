    /**
 *  \file Implementation of the MilleRecordROOT class. 
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


#include "Mille/MilleRecordROOT.h"
#include <iostream>

namespace Mille{
    MilleRecordROOT::MilleRecordROOT( const std::string & fname, 
                    bool doublePrecision, 
                    bool writeZero, 
                    std::size_t recordLengthHint):
        MilleRecord(doublePrecision, writeZero, recordLengthHint),
        fname_(fname){
        open(); 
    }
    /// @brief Destructor 
    MilleRecordROOT::~MilleRecordROOT(){
        close();     
    } 
    void MilleRecordROOT::writeRecord(){
        floatingPointFormat_ = 4; 
        if (buffer_.doublePrecision) floatingPointFormat_ = 8;
        // skip empty records 
        if (buffer_.empty()) return;
        outTree_->Fill(); 

        // prepare for next record 
        resetBuffers();
        newSetIfNeeded();     
    }

    void MilleRecordROOT::flushOutputFile(){
        file_->Flush(); 
    } 

    void MilleRecordROOT::resetOutputFile(){
        close();
        open();
    } 

    void MilleRecordROOT::open(){
    file_.reset(TFile::Open(fname_.c_str(),"RECREATE")); 
    if (!file_ || !file_->IsOpen()){
        std::cerr << "Mille::openRoot: Could not open " << fname_ 
            << " as output file." << std::endl;
    }
    outTree_ = new TTree("MilleRecords","MilleRecords");
    outTree_->SetDirectory(file_.get()); 
    outTree_->Branch("floats",&(buffer_.floatBuffer)); 
    outTree_->Branch("ints",&(buffer_.intBuffer)); 
    outTree_->Branch("doubles",&(buffer_.doubleBuffer)); 
    outTree_->Branch("floatingPointFormat",&floatingPointFormat_); 
    outTree_->SetAutoFlush(100); 

    } 
    void MilleRecordROOT::close(){
    file_->Write(); 
    file_->Close();
    } 
}
