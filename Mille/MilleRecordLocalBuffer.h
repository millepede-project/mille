/*
 * \file
 *  MilleRecord specialisation for transient in-flight buffer
 *
 *  \author Maximilian Goblirsch-Kolb, DESY, 2026 
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

#ifndef TRANSIENT__MILLE__RECORD__H
#define TRANSIENT__MILLE__RECORD__H

 #include "Mille/MilleRecord.h"

namespace Mille{

   /// @brief MilleRecord implementation for a transient 
   /// in-memory record not connected to an output file.
   /// Spawned by a parent MilleRecord that is connected
   /// to an output. 
   /// When destroyed or upon invocation of integrateToParent,
   /// will copy buffer contents into the parent. 
   /// Can be used to multi-thread Mille writing.
   /// Will delegate calls to file I/O to the parent 
   /// object 
   class MilleRecordLocalBuffer: public MilleRecord{
      public:


      /// @brief  Constructor
      /// @param doublePrecision: Store derivatives with double precision. 
      /// @param writeZero If enabled, will write entries with global derivatives of 0 into the output array. Note that using these within pede has not been tested. 
      /// @param recordLengthHint: Hint for expected record length (will reserve buffer space). 
      MilleRecordLocalBuffer(MilleRecord* parent, bool doublePrecision = false, bool writeZero = false,
                    std::size_t recordLengthHint = 2000):
        MilleRecord(doublePrecision, writeZero, recordLengthHint),
        m_parent(parent){}
         
      /// @brief Destructor. If anything left to integrate to the parent,
      /// will do so. 
      virtual ~MilleRecordLocalBuffer(){
         if (buffer_.size() > 0) integrateToParent();
      }
      /// @brief Interface for writing out the current entry.
      /// Will forward the call to the parent, and copy buffer 
      /// content to parent prior to the writing.
      virtual void writeRecord() override final{
         integrateToParent();
         m_parent->writeRecord(); 
      }

      /// @brief Interface for flushing the current file to disk 
      /// Delegates to the parent instance 
      virtual void flushOutputFile() override final{
        m_parent->flushOutputFile(); 
      }

      /// @brief Interface for closing and reopening the current file 
      /// Delegates to the parent instance 
      virtual void resetOutputFile() override final{
        m_parent->resetOutputFile(); 
      }
      
      /// @brief copies the current buffer into a (persistifiable) other 
      /// MilleRecord instance. Clears our buffer. 
      void integrateToParent(){
        m_parent->consumeBuffer(this); 
        // prepare for next record 
        resetBuffers();
        newSetIfNeeded();     
      }

      private: 
        
      MilleRecord* m_parent; 
   }; 
}
#endif
