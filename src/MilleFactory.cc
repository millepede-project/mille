/**
 *  \file Implementation of the MilleReaderFactory functions. 
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

#include "MilleFactory.h"
#include <iostream>

#include "MilleReaderC.h"
#include "MilleRecordC.h"
#include "MilleReaderCSV.h"
#include "MilleRecordCSV.h"
#ifdef SUPPORT_LZ
    #include "MilleReaderCcompressed.h"
#endif
#ifdef SUPPORT_ROOTIO
    #include "MilleReaderROOT.h"
    #include "MilleRecordROOT.h"
#endif 

MilleFormat deduceMilleFormat(const std::filesystem::path & fname){
    if (fname.extension() == ".dat" || fname.extension() == ".bin") return MilleFormat::C;
    if (fname.extension() == ".gz") return MilleFormat::Ccompressed;
    if (fname.extension() == ".root") return MilleFormat::ROOT;
    if (fname.extension() == ".txt" || fname.extension() == ".csv") return MilleFormat::CSV;
    std::cerr << "Could not deduce reader type for file "<<fname<<", extension "<<fname.extension()<<std::endl;
    return MilleFormat::Unknown; 
}


std::unique_ptr<IMilleReader> spawnMilleReader(MilleFormat fmt, const std::string & fname){
    switch (fmt){
        case MilleFormat::C:
            return std::make_unique<MilleReaderC>(fname);
            break; 
        case MilleFormat::CSV:
            return std::make_unique<MilleReaderCSV>(fname);
            break; 
        case MilleFormat::Ccompressed:
#ifdef SUPPORT_LZ
            return std::make_unique<MilleReaderCcompressed>(fname); 
#else 
            std::cerr << "Sorry! gzip-compressed binary reading not compiled into this build of Mille :-( Please rebuild with libz support to read file "<<fname<<std::endl;
            return nullptr;
#endif
        break; 
        case MilleFormat::ROOT: 
#ifdef SUPPORT_ROOTIO
            return std::make_unique<MilleReaderROOT>(fname); 
#else 
            std::cerr << "Sorry! ROOT binary reading not compiled into this build of Mille :-( Please rebuild with ROOT support to read file "<<fname<<std::endl;
            return nullptr;
#endif
        default:
            std::cerr << "Unknown format - can not deduce needed Mille reader type for file "<<fname<<std::endl;
            return nullptr;
    }
}

std::unique_ptr<IMilleReader> spawnMilleReader(const std::string & fname){
    return spawnMilleReader(deduceMilleFormat(fname),fname); 
}


std::unique_ptr<MilleRecord> spawnMilleRecord(MilleFormat fmt, const std::string & fname, 
                bool doublePrecision, 
                bool writeZero, 
                std::size_t recordLengthHint){
    switch (fmt){
        case MilleFormat::C:
            return std::make_unique<MilleRecordC>(fname, doublePrecision, writeZero, recordLengthHint);
            break; 
        case MilleFormat::CSV:
            return std::make_unique<MilleRecordCSV>(fname, doublePrecision, writeZero, recordLengthHint);
            break; 
        case MilleFormat::Ccompressed:
            std::cerr << "Sorry! gzip-compressed binary writing is not yet supported. Please write to C-binary and then manually compress using gzip."<<std::endl;
            return nullptr;
        break; 
        case MilleFormat::ROOT: 
#ifdef SUPPORT_ROOTIO
            return std::make_unique<MilleRecordROOT>(fname, doublePrecision, writeZero, recordLengthHint); 
#else 
            std::cerr << "Sorry! ROOT binary writing not compiled into this build of Mille :-( Please rebuild with ROOT support. "<<std::endl;
            return nullptr;
#endif
        default:
            std::cerr << "Unknown format - can not deduce needed Mille record type for file "<<fname<<std::endl;
            return nullptr;
    }
}

std::unique_ptr<MilleRecord> spawnMilleRecord(const std::string & fname, 
                bool doublePrecision, 
                bool writeZero, 
                std::size_t recordLengthHint){

    return spawnMilleRecord(deduceMilleFormat(fname),fname, doublePrecision, writeZero, recordLengthHint); 
}
