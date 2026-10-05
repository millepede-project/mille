/**
 *  \file Unit tests for multi-treaded MilleRecord writing.
 *  \author Maximilian Goblirsch-Kolb, DESY, 2026
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



#include "Mille/MilleDataStructures.h"
#include "testUtils.h"
#include <Mille/MilleReaderC.h>
#include <Mille/MilleRecordC.h>
#include <Mille/MilleFactory.h>
#include <Mille/MilleDecoder.h>
#include <memory>
#include <test/milleReaderTests.h>
#include <future>
using namespace Mille;

int main(int, char**){
    using ::my_ssize;

    const std::string milleBinaryName = "milleTest.dat";

    const std::size_t nThreads = 40;
    const std::size_t recPerThread = 10000;

    auto mille = spawnMilleRecord(milleBinaryName);
    auto writeThread = [&](){
        MilleTestInputs inputs(recPerThread);
        auto localBuffer = mille->spawnLocalBuffer();
        inputs.writeTo(*localBuffer);
    };
    std::vector<std::future<void>> fillThreads;
    for (std::size_t nth = 0; nth < nThreads; ++nth){
        fillThreads.emplace_back(std::async(std::launch::async, writeThread));
    }
    for (auto & fut : fillThreads){
        fut.get();
    }
    mille.reset(); // close the output

    MilleReaderC readBack(milleBinaryName);
    // test single precision
    if (!readBack.open(milleBinaryName)){
        std::cout << " Failed to open file"<<std::endl;
        return 1;
    }
    MilleDecoder dec;
    std::size_t nrecs = 0;
    std::size_t nempty = 0;
    std::vector<MilleMeasurement> measurements;
    using ReadResult = enum Mille::MilleDecoder::ReadResult;
    while(true){
        measurements.clear();
        ReadResult r = dec.decode(readBack,measurements);
        TEST_CONDITION(r == ReadResult::error, false, "Readback without errors");
        if (r == ReadResult::atEof) break;
        nempty += measurements.empty();
        ++nrecs;
    }
    TEST_CONDITION(nempty, std::size_t(0), "No empty records");
    TEST_CONDITION(nrecs, nThreads * recPerThread,"Correct total recs");
}
