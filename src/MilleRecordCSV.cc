    /**
 *  \file Implementation of the MilleRecordCSV class. 
 *  \author Maximilian Goblirsch-Kolb, DESY, 2026 
 *  \copyright
 *  Copyright (c) 2026 Deutsches Elektronen-Synchroton,
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

#include "Mille/MilleRecordCSV.h"
#include <iostream> 
#include <iomanip> 

MilleRecordCSV::MilleRecordCSV( const std::string & fname, 
                bool doublePrecision, 
                bool writeZero, 
                std::size_t recordLengthHint):
    MilleRecord(doublePrecision, writeZero, recordLengthHint),
    fname_(fname){
    openCSV(); 
}
/// @brief Destructor 
MilleRecordCSV::~MilleRecordCSV(){
    closeCSV();     
} 
void MilleRecordCSV::writeRecord(){
    // skip empty records 
    if (buffer_.empty()) return;
	const int recordLength =
			(buffer_.doublePrecision) ? -buffer_.size() *2 : buffer_.size() *2;
    // header word indicating record size and type 
    myOutFile_ << std::endl <<"#######  Start of record  #####" <<std::endl; 
    myOutFile_ << std::setw(10)<<recordLength << "  "<< std::setw(17)<<" "<< "     # record length (times 2 for backward compatibility)"<<std::endl; 
    int nextMeasurement = 0; // for extra printout - 0 = measurement, 1 = first local, 2 = inside local, 3 = uncertainty, 4 = first global, 5 = in global
    int iSpecial = 0; 
    for (size_t k = 0; k < buffer_.size(); ++k){
        const auto & [theInt, theFloat]  = buffer_.query(k);  
        myOutFile_<< std::setw(10)<< theInt<<"  "<<std::setw(17)<<std::setprecision(10)<<theFloat; 
        // add some extra verbosity, as the CSV format is mainly intended for pedagogical purposes
        annotateCurrentLine(k,nextMeasurement, iSpecial); 
        myOutFile_ << std::endl; 
    }
    myOutFile_ << "# End of record " <<std::endl<<std::endl; 

    // prepare for next record 
    resetBuffers();
    newSetIfNeeded();     
}

void MilleRecordCSV::flushOutputFile(){
    myOutFile_.flush(); 
} 

void MilleRecordCSV::resetOutputFile(){
    closeCSV();
    openCSV();
} 

void MilleRecordCSV::openCSV(){
    myOutFile_.open(fname_, std::ios::out); 
} 
void MilleRecordCSV::closeCSV(){
    myOutFile_.close(); 
} 

void MilleRecordCSV::annotateCurrentLine(size_t k, int & nextMeasurement, int & iSpecial){
    const auto & [theInt, theFloat]  = buffer_.query(k);
    bool last = (k == buffer_.size()-1);
    int nextInt = -999;
    double nextFloat = 999; 
    if (!last){
        const auto [i,f] = buffer_.query(k+1);
        nextInt=i;
        nextFloat=f;  
    }
    if (k == 0){
        myOutFile_ << "     # Header"; 
        return; 
    }
    // are we inside a special block? 
    else if (iSpecial == -1){
        myOutFile_ << "     # -1 x nSpecial (in float data)"; 
        iSpecial = (int)(-1 * theFloat); 
        nextMeasurement = 4;
    }
    else if (iSpecial > 0){
        if (nextMeasurement == 4){
            myOutFile_ << "     # special data";
            nextMeasurement = 5; 
        }
        --iSpecial;
    }
    else if (buffer_.intBuffer[k] ==0){
        // detect head of measurement or special block 
        if (k > 0 && nextMeasurement == 0 || nextMeasurement > 3){
            if (!last && nextInt == 0 && nextFloat < 0){
                myOutFile_ << "     # Special block"; 
                iSpecial = -1; 
            }
            else{ 
                myOutFile_ << "     # Measurement";
            }
            nextMeasurement = 1; 
        }
        else if (nextMeasurement > 0){
            myOutFile_ << "     # Uncertainty" ;
            nextMeasurement = 4; 
        }
    }
    else {
        if (nextMeasurement < 2){
            myOutFile_ << "     # local derivatives";
            nextMeasurement = 2; 
        }
        else if (nextMeasurement == 4) {
            myOutFile_ << "     # global derivatives";
            nextMeasurement = 5; 
        }
    }
    // add an empty line in front of next record 
    if (k > 0 && !last && nextInt == 0 && (nextMeasurement == 0 || nextMeasurement > 3)){
        myOutFile_ << std::endl;    
    }
}
