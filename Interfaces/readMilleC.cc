
/** \file
*  C bindings for reading Mille - definitions
*
* \author Max Goblirsch-Kolb, DESY (2026)
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

#include "Mille/readMilleC.h"
#include "Mille/IMilleReader.h"
#include "Mille/MilleFactory.h"
#include <cstddef>
#include <memory>
#include <mutex>
#include <vector>
#include <iostream>
#include <atomic>
#include <string>
#include <utility>

using Mille::spawnMilleReader; 

/// Helper struct to provide a thread-safe
/// way of managing in-flight file handles
/// in a way that hides the OOP logic 
/// from C-/Fortran client. 
///
/// Purely internal API not intended
/// for client use. 
/// 
/// All public members take "fortran-style"
/// indices starting with 1, as in the external API.
/// They are internally translated to array indices. 
struct fileHandleRegistry{

    /// @brief resize the registry with thread protection
    /// @param targetSize: Desired size. 
    /// Will not shrink the list. 
    void resizeRegistry(std::size_t targetSize){
        std::lock_guard g(mx_accessRegistry);
        if (targetSize > readFileHandles.size()){
            readFileHandles.resize(targetSize);
        }
    }

    
    /// @brief insert a new reader into the registry
    /// @param reader: Will be consumed and added to the registry
    /// @param userIndexedSlot: destination slot in user indexing
    /// 
    /// Will automatically adapt the array size as required and
    /// clean up existing readers in the slot. 
    void insertReader(std::unique_ptr<Mille::IMilleReader>&& reader, int userIndexedSlot){
        std::lock_guard g(mx_accessRegistry);

        // if the user passes <=0, they want to auto-add at the 
        // end of the current list
        if (userIndexedSlot <= 0) userIndexedSlot = readFileHandles.size()+1; 
        // translate to internal (C-style) indexing
        int internalIndex = userIndexedSlot-1; 
        // make space
        // safe direct cast as internalIndex >=0 is guaranteed
        if (std::size_t(internalIndex) >= readFileHandles.size()){
            readFileHandles.resize(internalIndex+1);
        }
        // close a potentially existing reader at this slot. 
        if (validFileIndex(internalIndex)){
            readFileHandles.at(internalIndex)->close(); 
        }
        /// move the reader into the registry (take ownership) 
        readFileHandles.at(internalIndex).swap(reader); 
        // remember the user-facing index of the slot, as 
        // this is now the most recently opened file. 
        lastOpenedFileIndex = userIndexedSlot;
    }

    /// @brief close an open handle at a given slot
    /// @param userIndexedSlot: The slot in user-indexing
    void closeHandle(int userIndexedSlot){
        std::lock_guard g(mx_accessRegistry);
        int internal = userIndexToInternal(userIndexedSlot);
        // don't close if there is nothing valid to close
        if (!validFileIndex(internal)) return;  
        readFileHandles.at(internal)->close(); 
        readFileHandles.at(internal).reset(nullptr);
    }

    /// @brief get the reader at a given slot as a non-owning pointer.
    /// @param userIndexedSlot: The slot in user-indexing.
    /// @return a pointer to the reader at a valid slot, or a null pointer
    /// if the slot is out of bounds or not populated with a valid reader. 
    Mille::IMilleReader* getHandleChecked(int userIndexedSlot){
        std::lock_guard g(mx_accessRegistry);
        int internalSlot = userIndexToInternal(userIndexedSlot);
        if (!validFileIndex(internalSlot)) return nullptr; 
        return readFileHandles.at(internalSlot).get();
    }

    int getIndexLastOpenedFile() const{
        return lastOpenedFileIndex; 
    }

    private:

    // mutex to protect MT access
    std::mutex mx_accessRegistry; 

    // list of open / active file handles 
    std::vector<std::unique_ptr<Mille::IMilleReader>> readFileHandles; 

    // the (1-indexed) number of the most recently opened file 
    std::atomic<int> lastOpenedFileIndex = 0;

    /// @brief translates the user-passed index 
    /// to the corresponding internal one. 
    /// permit passing a value <=0 
    /// to access the most recently opened
    /// file
    int userIndexToInternal (int userIndex){
        if (userIndex <= 0){
            return lastOpenedFileIndex - 1;
        }
        else return userIndex - 1; 
    }

    /// @brief check if a given internal file index
    /// is valid (legal index and pointing)
    /// to a properly initialized reader
    /// @param internalFileIndex file index (internal, 0-based indexing)
    bool validFileIndex(int internalFileIndex){
        if (internalFileIndex < 0) return false;
        if (internalFileIndex >= (int)readFileHandles.size()) return false;
        if (!readFileHandles.at(internalFileIndex)) return false;
        return true; 
    }
    

};

/// expose one global instance of our registry. 
struct fileHandleRegistry g_milleRegistry;


/// Initialises the 'global' variables used for file handling.
void initMille() {
    g_milleRegistry.resizeRegistry(100);
}

/*______________________________________________________________*/
/// Open file.
void open_mille(const char *fileName, int lengthFileName, int *errorFlag, int nFileIn)
{
    if (!errorFlag)
        return; /* 'printout' error? */ 

    // some sanitation of the input file name
    std::string fname (fileName, lengthFileName);

    // spawn the reader 
    auto reader = spawnMilleReader(fname); 
    if (!reader || !reader->open(fname)){
        *errorFlag = 2;     // failed to open file 
        return; 
    }
    
    // and then insert our reader to the registry. 
    // will automatically close existing readers in the slot 
    // and resize if required.
    g_milleRegistry.insertReader(std::move(reader), nFileIn);
    // all good!
    *errorFlag = 0; 
}

/*______________________________________________________________*/
/// Close file.
void close_mille(int nFileIn) {
    g_milleRegistry.closeHandle(nFileIn); 
}

/*______________________________________________________________*/
/// Rewind file.
void reset_mille(int nFileIn) {
    auto handle = g_milleRegistry.getHandleChecked(nFileIn);
    if (handle) handle->reset();
}

/*______________________________________________________________*/
/// Rewind file.
void reset_mille_entry(int nFileIn) {
    auto handle = g_milleRegistry.getHandleChecked(nFileIn);
    if (handle) handle->rewindEntry();
}

void skip_mille(int nFileIn){
    auto handle = g_milleRegistry.getHandleChecked(nFileIn);
    if (handle) handle->skipEntry();
}

/**______________________________________________________________*/
// Read record from file.

void read_mille(double *bufferDouble, float *bufferFloat, int *bufferInt,
    int *lengthBuffers, int *errorFlag, int nFileIn) {
        /* No return value since to be called as subroutine from fortran,
        negative *errorFlag are errors, otherwise fine.
        
        *nFileIn: number of the file the record is read from,
        starting from 1 (not 0)
        */
    if (!errorFlag)
        return;
    *errorFlag = 0;

    // access the requested handler 
    Mille::IMilleReader* reader = g_milleRegistry.getHandleChecked(nFileIn); 
    if (!reader){
        std::cerr <<" File index "<<nFileIn<<" is either invalid or points to invalid reader instance. Will not read anything."<<std::endl;
        *errorFlag = -3; 
        return; 
    }
    // read the next record via the IMilleReader
    std::pair<int,int> readResult = reader->readToBuffer(bufferDouble, bufferFloat, bufferInt, *lengthBuffers); 
    *errorFlag = readResult.second; 
    *lengthBuffers = readResult.first; 
}

void last_mille_in_file_index(int* index){
    if (!index) return; 
    *index = g_milleRegistry.getIndexLastOpenedFile(); 
}
