/*
 * MilleReaderFactory.h
 *
 *  Created on: Jan 06, 2026
 *      Author: goblirsc
 */

/** \file
 *  MilleReaderFactory declaration. Factory for MilleReader instances. 
 *
 *  \author Maximilian Goblirsch-Kolb, DESY, 2026 
 *
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

#ifndef MILLE_MILLEREADERFACTORY__H
#define MILLE_MILLEREADERFACTORY__H

#include "IMilleReader.h" 
#include "MilleRecord.h" 
#include <memory> 
#include <filesystem>

/// @brief Enum describing the supported Mille formats.  
enum class MilleFormat{
    C,
    CSV,
    Ccompressed,
    ROOT, 
    Unknown 
};

/// @brief Deduce the format of a given Mille binary based on the extension. 
/// @param fname: File name
/// @return a MilleFormat describing the identified format.
///         Supported formats: 
///         .root --> ROOT 
///         .dat or .bin  --> C-binary 
///         .gz   --> compressed C-binary  
///         .txt or .csv --> plain text
///         MilleFormat::Unknown will be return if the deduction failed.
MilleFormat deduceMilleFormat(const std::filesystem::path & fname);

/// @brief Try to create a reader for a given Mille format. 
/// @param fmt The format
/// @param fname: File name to read
/// @return A unique_ptr to a MilleReader instance. 
///         Will be a nullptr in case of an unknown format. 
std::unique_ptr<IMilleReader> spawnMilleReader(MilleFormat fmt, const std::string & fname);


/// @brief Try to create a reader for a given file, auto-detecting the type based on its extension. 
/// @param fmt A file extension string 
/// @param fname: File name to read
/// @return A unique_ptr to a MilleReader instance. 
///         Supported formats: 
///         .root --> ROOT 
///         .dat or .bin  --> C-binary 
///         .gz   --> compressed C-binary  
///         .txt or .csv --> plain text
///         Will return a nullptr in case of an unknown format.
std::unique_ptr<IMilleReader> spawnMilleReader(const std::string & fname);

/// @brief Try to create a record writer for a given Mille format. 
/// @param fmt The format
/// @param fname: File name to write
/// @param writeZero If enabled, will write entries with global derivatives of 0 into the output array. Note that using these within pede has not been tested. 
/// @param doublePrecision: Store derivatives with double precision. 
/// @param recordLengthHint: Hint for expected record length (will reserve buffer space). 
/// @return A unique_ptr to a MilleRecord instance. 
///         Will be a nullptr in case of an unsupported format. 
std::unique_ptr<MilleRecord> spawnMilleRecord(MilleFormat fmt, 
                    const std::string & fname, 
                    bool doublePrecision = false, 
                    bool writeZero = false, 
                    std::size_t recordLengthHint = 2000);


/// @brief Try to create a record writer for a given file, auto-detecting the type based on its extension. 
/// @param fmt A file extension string 
/// @param fname: File name to write
/// @param writeZero If enabled, will write entries with global derivatives of 0 into the output array. Note that using these within pede has not been tested. 
/// @param doublePrecision: Store derivatives with double precision. 
/// @param recordLengthHint: Hint for expected record length (will reserve buffer space). 
/// @return A unique_ptr to a MilleRecord instance. 
///         Supported formats: 
///         .root --> ROOT 
///         .dat or .bin  --> C-binary 
///         .txt or .csv --> plain text
///         Compressed C-binary is not supported for direct writing - please write to C binary and then manually run gzip. 
///         Will return a nullptr in case of an unsupported format.
std::unique_ptr<MilleRecord> spawnMilleRecord(
                    const std::string & fname,
                    bool doublePrecision = false, 
                    bool writeZero = false, 
                    std::size_t recordLengthHint = 2000);




#endif // MILLE_MILLEREADERFACTORY__H
