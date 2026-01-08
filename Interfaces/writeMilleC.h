/** \file
 *  C-binding for Mille writing - Declarations
 *
 * \author Maximilian Goblirsch-Kolb, DESY (2026) 
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
 *
 */

#ifndef WRITE_MILLE_C_BINDINGS__H
#define WRITE_MILLE_C_BINDINGS__H

#ifdef __cplusplus
extern "C"
{
#endif

/// @brief Open a new output file for writing. The type will be determined based on the file extension. 
/// @param fileName: Desired file name 
/// @param lengthFileName: Length of the file name (needed for fortran use) 
/// @param nFileOut: Desired index of the file - start counting at 1 (fortran style). Passing -1 will auto-assign the next free index, writing it back to nFileOut 
/// @param doublePrec: Enable double-precision floating point storage 
/// @param keepZeros: Write zero-derivatives to the output. Not tested in pede 
/// @param recordLengthHint: Initial write buffer size - will grow if needed. 
void openMilleOut(const char *fileName, int lengthFileName, int nFileOut=-1, bool doublePrec=false, bool keepZeros=false,
			    int recordLengthHint = 2000); 

/// @brief Completes the current Mille record, writes it to the file, and then prepares the next one
/// @param nFileOut Index of the file to apply the operation to 
void writeMilleOut(int nFileOut=-1);

/// @brief Add a measurement to the current Mille record
/// @param aMeas The measurement
/// @param aErr The uncertainty of the measurement
/// @param numLocal Number of local derivatives
/// @param indLocal Labels of local parameters
/// @param derLocal Local derivatives
/// @param nGlobal Number of global derivatives
/// @param labGlobal Labels of global parameters
/// @param derGlobal Global derivatives
/// @param nFileOut Index of the file to apply the operation to  
void addMilleData( double aMeas, double aErr, unsigned int numLocal,
        unsigned int* indLocal, double* derLocal,
        int nGlobal, 
        int* labGlobal,
        double* derGlobal, int nFileOut=-1);

/// @brief Add a special data block to the current record. 
/// @param nSpecial: Size of the special data block 
/// @param specialFlot: Floating point entries of the block 
/// @param specialInt: Integer entries of the block 
/// @param nFileOut Index of the file to apply the operation to  
void addMilleSpecial(int nSpecial, const double* specialFloat, int* specialInt, int nFileOut=-1);

/// @brief Abort the current record, discarding measurements
/// @param nFileOut Index of the file to apply the operation to  
void abortMilleRecord(int nFileOut=-1);

/// @brief flush the file to disk 
/// @param nFileOut Index of the file to apply the operation to  
void flushMilleOutput(int nFileOut=-1); 

/// @brief Close and re-open the file. Discard previous content.  
/// @param nFileOut Index of the file to apply the operation to  
void resetMilleOutput(int nFileOut=-1); 

/// @brief retrieve the index of the last file added via openMilleOut. 
/// @param[out] index Will be populated with the index of the file   
void lastMilleOutFileIndex(int* index); 

/// @brief close the output file and write to disk. 
/// @param nFileOut Index of the file to apply the operation to  
void closeMilleOutput(int nFileOut = -1); 


#ifdef __cplusplus
}
#endif


#endif // WRITE_MILLE_C_BINDINGS__H
