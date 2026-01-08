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


void openMilleOut(const char *fileName, int lengthFileName, int nFileOut=-1, bool doublePrec=false, bool keepZeros=false,
			    int recordLengthHint = 2000); 

void writeMilleOut(int nFileOut=-1);

void addMilleData( double aMeas, double aErr, unsigned int numLocal,
        unsigned int* indLocal, double* derLocal,
        int nGlobal, 
        int* labGlobal,
        double* derGlobal, int nFileOut=-1);

        
void addMilleSpecial(int nSpecial, const double* specialFloat, int* specialInt, int nFileOut=-1);


void abortMilleRecord(int nFileOut=-1);

void flushMilleOutput(int nFileOut=-1); 

void resetMilleOutput(int nFileOut=-1); 

void lastMilleOutFileIndex(int* index); 

void closeMilleOutput(int nFileOut = -1); 


#ifdef __cplusplus
}
#endif


#endif // WRITE_MILLE_C_BINDINGS__H
