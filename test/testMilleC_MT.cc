/**
 *  \file Unit tests for multi-treaded MilleRecord writing 
 *        via the C-wrapper. 
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



#include "Mille/readMilleC.h"
#include "Mille/MilleFactory.h"
#include "Mille/MilleDataStructures.h"
#include "Mille/MilleDecoder.h"
#include "testUtils.h"
#include <memory>
#include <mutex>
#include <string>
#include <format>
#include <test/milleReaderTests.h>
#include <future>
#include <vector>

using namespace Mille;

// ad-hoc Mille parsing & decoding via the C API 
std::pair<bool, std::vector<std::vector<MilleMeasurement>>> readThread(const std::vector<std::string> & fin, int ithr){
    thread_local MilleBuffer buf;
    buf.doublePrecision = true; 
    buf.resize(200,true);
    thread_local int status = 0; 
    thread_local MilleDecoder dec;
    std::vector<std::vector<MilleMeasurement>> res; 
    for (const auto & file : fin){
        std::string threadID = std::format ("Thread {} on file {}",ithr, file);
        open_mille(file.c_str(),file.size(),&status,ithr+1);  
        if (status != 0 ){
            std::cerr <<" Failed to open: "+threadID<<std::endl;
            return {false, {}}; 
        }
        // process entries 
        while(true){
            int nRead = buf.size();
            read_mille(buf.doubleBuffer.data(), buf.floatBuffer.data(), buf.intBuffer.data(), &nRead, &status,ithr+1);

            if (status == 0){
                break; // EOF, good! 
            }
            if ( status == -4){
                buf.resize(nRead,true);
                reset_mille_entry(ithr+1); 
                read_mille(buf.doubleBuffer.data(), buf.floatBuffer.data(), buf.intBuffer.data(), &nRead, &status,ithr+1);
            }
            if (status < 0){
                std::cerr << "Failed to read the next entry: "+threadID <<std::endl;
                return {false, {}};  
            }
            res.push_back(std::vector<MilleMeasurement>{});
            auto decoded = dec.decode(buf,nRead,res.back());
            if (decoded == MilleDecoder::ReadResult::error){
                std::cerr << " Decoding error "+threadID<<std::endl;
                return {false, {}}; 
            }
        }
        close_mille(ithr+1);
    }
    
    return {true, res};
}

int main(int, char**){
    using ::my_ssize;

    // test config - many threads and files to maximise 
    // the chance to hit any race conditions
    const std::string milleBinaryName = "milleTest";
    const std::size_t nThreads = 16;
    const std::size_t nFilesPerThread = 4;
    const int recPerThread = 1000;

    // spawn test files with identical content
    initMille();
    std::vector<std::vector<std::vector<MilleTestInputs::dummyRecord>>> inputs; 
    std::vector<std::vector<std::string>> inputsForThreads;
    for (int ithr = 0; ithr < nThreads; ++ithr){
        inputsForThreads.push_back(std::vector<std::string>{});
        inputs.push_back(std::vector<std::vector<MilleTestInputs::dummyRecord>>{});
        for (int ifi = 0; ifi < nFilesPerThread; ++ifi){
            MilleTestInputs ti (recPerThread);
            inputs.back().push_back(ti.dummyData);
            std::string fname = std::format("{}_{}_{}.dat",milleBinaryName,ithr,ifi);
            auto Mille = spawnMilleRecord(fname, true);
            inputsForThreads.back().push_back(fname);
            ti.writeTo(*Mille);
        }
    };
    // now spawn threads to read back the information in a "closeandreload" fashion
    std::vector<std::future<std::pair<bool, std::vector<std::vector<MilleMeasurement>>>>> threads; 
    for (std::size_t nth = 0; nth < nThreads; ++nth){
        threads.emplace_back(std::async(std::launch::async, &readThread, inputsForThreads[nth], nth));
    }
    // wait for the threads to finish and validate their output
    
    for (std::size_t ithread = 0;  ithread < nThreads; ++ithread){
        auto & fut = threads.at(ithread); 
        const auto & [ok, res] = fut.get();
        TEST_CONDITION(ok, true, "Successful processing within thread"); 
        std::cout << "Validating Thread "<<ithread<< std::endl; 
        TEST_CONDITION(long(res.size()), long(nFilesPerThread * inputs.at(ithread).front().size()), "Number of total records read on thread"); 
        for (std::size_t ifile = 0; ifile < inputs[ithread].size(); ++ifile){
            const auto & inputPerFile = inputs[ithread].at(ifile);
            for (std::size_t itarget = 0; itarget < inputPerFile.size(); ++itarget){
                const MilleTestInputs::dummyRecord & dr = inputPerFile.at(itarget);
                const std::vector<MilleMeasurement> & readrec = res.at(itarget + ifile * recPerThread);
                // we expect 2: One measurement and one special block. 
                // This is defined in the MilleTestInputs constructor. 
                TEST_CONDITION(long(readrec.size()), long(2), "Size of the read record "<<ithread<<" "<<ifile); 
                const MilleMeasurement measurement = readrec.front();
                const MilleMeasurement specialBlock = readrec.back();
                // test data content
                TEST_CONDITION(measurement.measurement, dr.residual, "Residual on Measurement "<<ithread<<" "<<ifile); 
                TEST_CONDITION(measurement.uncertainty, dr.deltaResidual, "Uncertainty on Measurement "<<ithread<<" "<<ifile); 
                TEST_CONDITION(measurement.globalDerivatives, dr.globalDeriv, "global deriv on Measurement "<<ithread<<" "<<ifile); 
                TEST_CONDITION(measurement.localDerivatives, dr.localDeriv, "local deriv on Measurement "<<ithread<<" "<<ifile); 
                TEST_CONDITION(measurement.globalLabels, dr.globalLabels, "global labels on Measurement "<<ithread<<" "<<ifile); 
                TEST_CONDITION(measurement.localLabels, dr.localLabels, "local labels on Measurement "<<ithread<<" "<<ifile); 
                TEST_CONDITION(specialBlock.specialFloat, dr.specialFloat, "special floats "<<ithread<<" "<<ifile); 
                TEST_CONDITION(specialBlock.specialInts, dr.specialInt, "special ints "<<ithread<<" "<<ifile); 
            }
        }
        std::cout << " Thread "<<ithread<<" OK! "<< std::endl; 
    }
    return 0; 
}
