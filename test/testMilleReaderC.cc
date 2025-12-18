/**
 *  \file Unit tests for MilleReaderC class. 
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
#include "milleReaderTests.h"
#include <vector>
#include <iostream>
#include <limits>
#include <fstream>
#include <filesystem>


int main(int, char**){

    /// dummy data for testing 
    MilleTestInputs inputs; 

    {
        // single floating point precision
        MilleRecordC mille("milleTest.dat");
        inputs.writeTo(mille); 
        // double floating point precision
        MilleRecordC milleDouble("milleTestDbl.dat",true);
        inputs.writeTo(milleDouble); 
    }

    MilleReaderC reader; 
    // test single precision
    if (!reader.open("milleTest.dat")){
        std::cout << " Failed to open file"<<std::endl;
        return 1;    
    }

    int ret = MilleReaderTestSuite(reader,inputs); 
    if (ret !=0) return ret; 
    reader.close(); 

    // test double precision
    if (!reader.open("milleTestDbl.dat")){
        std::cout << " Failed to open file"<<std::endl;
        return 1;    
    }
    return MilleReaderTestSuite(reader,inputs,8); 
}
