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
#include <Mille/MilleReaderC.h>
#include <Mille/MilleRecordC.h>
#include <test/milleReaderTests.h>

using namespace Mille; 

int main(int, char**){
    using ::my_ssize; 

    const std::string milleBinaryName = "milleTest.dat"; 
    MilleTestInputs inputs; 
    {
        MilleRecordC mille(milleBinaryName); 

        auto localBuffer = mille.spawnLocalBuffer(); 
        localBuffer->addData(inputs.residual, inputs.deltaResidual, 
                inputs.localLabels,
                inputs.localDeriv, 
                inputs.globalLabels,
                inputs.globalDeriv); 
        localBuffer->addSpecial(inputs.specialInt, inputs.specialFloat);
        localBuffer->writeRecord();
    }

    MilleReaderC readBack(milleBinaryName); 
    // test single precision
    if (!readBack.open(milleBinaryName)){
        std::cout << " Failed to open file"<<std::endl;
        return 1;    
    }
    return MilleReaderTestSuite(readBack, inputs); 
}
