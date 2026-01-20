/*
 * \file
 *  MilleRecord specialisation for the plain text format. 
 *
 *  \author Maximilian Goblirsch-Kolb, DESY, 2025 
 *
 *  \copyright
 *  Copyright (c) 2025 Deutsches Elektronen-Synchroton,
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

#ifndef MILLERECORD__CSV__H
#define MILLERECORD__CSV__H

 
 #include "Mille/MilleRecord.h"
 #include <string> 
 #include <fstream> 


namespace Mille{
   class MilleRecordCSV: public MilleRecord{
      public:
      /// @brief  Constructor
      /// @param fname Name of the file to write 
      /// @param writeZero If enabled, will write entries with global derivatives of 0 into the output array. Note that using these within pede has not been tested. 
      /// @param doublePrecision: ignored for this specialisation
      /// @param recordLengthHint: Hint for expected record length (will reserve buffer space). 
      MilleRecordCSV(   const std::string & fname="mille.csv", 
                     bool doublePrecision = false, 
                     bool writeZero = false, 
                     std::size_t recordLengthHint = 2000); 
      /// @brief Destructor 
      virtual ~MilleRecordCSV(); 
      /// @brief Interface for writing out the current entry 
      virtual void writeRecord() override final;

      /// @brief Interface for flushing the current file to disk 
      virtual void flushOutputFile() override final; 

      /// @brief Interface for closing and reopening the current file 
      virtual void resetOutputFile() override final; 

      private:
      
      /// Functions 
      void openCSV();  /// Open the text file in write mode 
      void closeCSV(); 

      void annotateCurrentLine(size_t k, int & nextMeasurement, int & ispecial);  

      /// Data members
      
      std::string fname_ = "mille.csv"; ///< File name for the output binary  
      std::ofstream myOutFile_; ///< text file for output
   }; 
}

#endif // MILLERECORD__CSV__H
