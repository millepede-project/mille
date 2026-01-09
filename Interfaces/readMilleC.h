/** \file
 *  C-binding for Mille reading - Declarations
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

#ifndef READ_MILLE_C_BINDINGS__H
#define READ_MILLE_C_BINDINGS__H


#ifdef __cplusplus
extern "C"
{
#endif


/// Reserve space for output files - no longer strictly needed 
void initMille();

/*______________________________________________________________*/
/// Open file.
/**
 * \param[in]  fileName  File name
 * \param[in]  lengthFileName  Length of file name
 * \param[in]  nFileIn  File number (1 .. maxNumFiles) or <=0 for next one
 * \param[out] errorFlag error flag:
 *      * 0: if file opened and OK,
 *      * 1: if too many files open,
 *      * 2: if file could not be opened
 *      * 3: if file opened, but with error (can that happen?)
 */
 void openMille(const char *fileName, int lengthFileName, int *errorFlag, int nFileIn=-1); 
/*______________________________________________________________*/
/// Close file.
/**
 * \param[in]  nFileIn  File number (1 .. maxNumFiles)
 */
void closeMille(int nFileIn=-1);
/*______________________________________________________________*/
/// Rewind file to first entry.
/**
 * \param[in]  nFileIn  File number (1 .. maxNumFiles)
 */
void resetMille(int nFileIn=-1);
/*______________________________________________________________*/
/// Rewind file to start of previous entry.
/**
 * \param[in]  nFileIn  File number (1 .. maxNumFiles)
 */
void resetMilleEntry(int nFileIn=-1);
/*______________________________________________________________*/
/// Read record from file.
/**
 * \param[out]    bufferDouble  read buffer for doubles
 * \param[out]    bufferFloat   read buffer for floats
 * \param[out]    bufferInt     read buffer for integers
 * \param[in,out] lengthBuffers in: buffer length, out: number of floats/ints in records
 *                              (> buffer size: record skipped)
 * \param[in]     nFileIn       File number (1 .. maxNumFiles)
 * \param[out]    errorFlag     error flag:
 *      *  -1: pointer to a buffer or lengthBuffers are null
 *      *  -2: problem reading record length
 *      *  -4: given buffers too short for record
 *      *  -8: problem with stream or EOF reading floats
 *      * -16: problem with stream or EOF reading ints
 *      * -32: problem with stream or EOF reading doubles
 *      *  =0: reached end of file (or read empty record?!)
 *      *  =4: found floats
 *      *  =8: found doubles
 */
/* No return value since to be called as subroutine from fortran,
    negative *errorFlag are errors, otherwise fine.

    *nFileIn: number of the file the record is read from,
    starting from 1 (not 0)
    */
void readMille(double *bufferDouble, float *bufferFloat, int *bufferInt,
		int *lengthBuffers, int *errorFlag, int nFileIn=-1);

/// skip the next record 
void skipMille(int nFileIn = -1); 

// get the index of the last file added (useful if passing "-1") to openMille
void lastMilleInFileIndex(int* index); 


#ifdef __cplusplus
}
#endif


#endif // READ_MILLE_C_BINDINGS__H
