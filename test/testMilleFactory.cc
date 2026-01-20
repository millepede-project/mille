/**
 *  \file Unit tests for MilleReaderFactory tools. 
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

#include "Mille/MilleFactory.h"
#include "Mille/MilleReaderC.h"
#include "Mille/MilleRecordC.h"
#include "Mille/MilleReaderCSV.h"
#include "Mille/MilleRecordCSV.h"
#include "Mille/MilleReaderCcompressed.h"
#ifdef SUPPORT_ROOTIO
    #include "Mille/MilleReaderROOT.h"
    #include "Mille/MilleRecordROOT.h"
#endif
#ifdef SUPPORT_LZ
    #include "Mille/MilleReaderROOT.h"
#endif
#include <iostream>
#include <memory>

#include "testUtils.h"

using namespace Mille; 

int main(int, char**){
    TEST_CONDITION(dynamic_cast<MilleReaderC*>(spawnMilleReader("MyCoolFile.dat").get()) != nullptr, true, "Detection of C binary as .dat - reader"); 
    TEST_CONDITION(dynamic_cast<MilleReaderC*>(spawnMilleReader("MyCoolFile.bin").get()) != nullptr, true, "Detection of C binary as .bin - reader"); 
    TEST_CONDITION(dynamic_cast<MilleRecordC*>(spawnMilleRecord("MyCoolFile.dat").get()) != nullptr, true, "Detection of C binary as .dat - writer"); 
    TEST_CONDITION(dynamic_cast<MilleRecordC*>(spawnMilleRecord("MyCoolFile.bin").get()) != nullptr, true, "Detection of C binary as .bin - writer"); 
    TEST_CONDITION(dynamic_cast<MilleReaderCSV*>(spawnMilleReader("MyCoolFile.csv").get()) != nullptr, true, "Detection of plain text as .csv - reader"); 
    TEST_CONDITION(dynamic_cast<MilleRecordCSV*>(spawnMilleRecord("MyCoolFile.csv").get()) != nullptr, true, "Detection of plain text as .csv - writer"); 
    TEST_CONDITION(dynamic_cast<MilleReaderCSV*>(spawnMilleReader("MyCoolFile.txt").get()) != nullptr, true, "Detection of plain text as .txt - reader"); 
    TEST_CONDITION(dynamic_cast<MilleRecordCSV*>(spawnMilleRecord("MyCoolFile.txt").get()) != nullptr, true, "Detection of plain text as .txt - writer"); 
#ifdef SUPPORT_LZ
    TEST_CONDITION(dynamic_cast<MilleReaderCcompressed*>(spawnMilleReader("MyCoolFile.dat.gz").get()) != nullptr, true, "Detection of compressed C binary as .dat.gz"); 
    TEST_CONDITION(dynamic_cast<MilleReaderCcompressed*>(spawnMilleReader("MyCoolFile.bin.gz").get()) != nullptr, true, "Detection of compressed C binary as .dat.gz"); 
#endif
#ifdef SUPPORT_ROOTIO
    TEST_CONDITION(dynamic_cast<MilleReaderROOT*>(spawnMilleReader("MyCoolFile.root").get()) != nullptr, true, "Detection of ROOT binary as .root - reader"); 
    TEST_CONDITION(dynamic_cast<MilleRecordROOT*>(spawnMilleRecord("MyCoolFile.root").get()) != nullptr, true, "Detection of ROOT binary as .root - writer"); 
#endif 
    TEST_CONDITION(spawnMilleReader("MyCoolFile.asdf").get() != nullptr, false, "Non-detection of unknown file type - reader"); 
    TEST_CONDITION(spawnMilleRecord("MyCoolFile.asdf").get() != nullptr, false, "Non-detection of unknown file type - writer"); 
    return 0; 
}
