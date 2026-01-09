! Fortran bindings for Mille I/O
!! \author Maximilian Goblirsch-Kolb, DESY 2026 
!! \copyright
!! Copyright (c) 2026 Deutsches Elektronen-Synchroton,
!! Member of the Helmholtz Association, (DESY), HAMBURG, GERMANY \n\n
!! This library is free software; you can redistribute it and/or modify
!! it under the terms of the GNU Library General Public License as
!! published by the Free Software Foundation; either version 2 of the
!! License, or (at your option) any later version. \n\n
!! This library is distributed in the hope that it will be useful,
!! but WITHOUT ANY WARRANTY; without even the implied warranty of
!! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
!! GNU Library General Public License for more details. \n\n
!! You should have received a copy of the GNU Library General Public
!! License along with this program (see the file COPYING.LIB for more
!! details); if not, write to the Free Software Foundation, Inc.,
!! 675 Mass Ave, Cambridge, MA 02139, USA.

MODULE MilleF
    IMPLICIT NONE
    SAVE
    interface
        subroutine open_mille(filename, lfn, ios, lun) BIND(c)
            use iso_c_binding
            CHARACTER(kind=c_char), DIMENSION(*), INTENT(IN) :: filename
            INTEGER(c_int), INTENT(IN), VALUE :: lfn
            INTEGER(c_int), INTENT(INOUT) :: ios
            INTEGER(c_int), INTENT(IN), VALUE :: lun
        end subroutine open_mille
        
        subroutine close_mille( lun) BIND(c)
            use iso_c_binding
            INTEGER(c_int), INTENT(IN), VALUE :: lun
        end subroutine close_mille
        
        subroutine reset_mille( lun) BIND(c)
            use iso_c_binding
            INTEGER(c_int), INTENT(IN), VALUE :: lun
        end subroutine reset_mille
        
        subroutine reset_mille_entry( lun) BIND(c)
            use iso_c_binding
            INTEGER(c_int), INTENT(IN), VALUE :: lun
        end subroutine reset_mille_entry

        subroutine read_mille(bufferD, bufferF, bufferI, bufferLength, err, lun ) BIND(c)
            use iso_c_binding
            REAL(c_double), DIMENSION(*), INTENT(OUT) :: bufferD
            REAL(c_float), DIMENSION(*), INTENT(OUT) :: bufferF
            INTEGER(c_int), DIMENSION(*), INTENT(OUT) :: bufferI
            INTEGER(c_int), INTENT(INOUT) :: bufferLength
            INTEGER(c_int), INTENT(OUT) :: err
            INTEGER(c_int), INTENT(IN), VALUE :: lun
        end subroutine read_mille
        
        subroutine skip_mille( lun) BIND(c)
            use iso_c_binding
            INTEGER(c_int), INTENT(IN), VALUE :: lun
        end subroutine skip_mille
        
        subroutine last_mille_in_file_index( lun) BIND(c)
            use iso_c_binding
            INTEGER(c_int), INTENT(OUT) :: lun
        end subroutine last_mille_in_file_index

        subroutine open_mille_out(filename, lfn, lun, doublePrec, keepZeros, recordLengthHint) BIND(c)
            use iso_c_binding
            CHARACTER(kind=c_char), DIMENSION(*), INTENT(IN) :: filename
            INTEGER(c_int), INTENT(IN), VALUE :: lfn
            INTEGER(c_int), INTENT(IN), VALUE :: lun
            LOGICAL(C_bool), INTENT(IN), VALUE :: doublePrec
            LOGICAL(C_bool), INTENT(IN), VALUE :: keepZeros
            INTEGER(c_int), INTENT(IN), VALUE :: recordLengthHint
        end subroutine open_mille_out
        
        subroutine write_mille_out( lun) BIND(c)
            use iso_c_binding
            INTEGER(c_int), INTENT(IN), VALUE:: lun
        end subroutine write_mille_out
        
        subroutine add_mille_data( measured, error, nLocal, indLocal, derLocal, nGlobal, indGlobal, derGlobal, lun) BIND(c)
            use iso_c_binding
            REAL(c_double), INTENT(IN), VALUE :: measured 
            REAL(c_double), INTENT(IN), VALUE :: error 
            INTEGER(c_int), INTENT(IN), VALUE :: nLocal 
            INTEGER(c_int), DIMENSION(*), INTENT(IN) :: indLocal
            REAL(c_double), DIMENSION(*), INTENT(IN) :: derLocal
            INTEGER(c_int), INTENT(IN), VALUE :: nGlobal 
            INTEGER(c_int), DIMENSION(*), INTENT(IN) :: indGlobal
            REAL(c_double), DIMENSION(*), INTENT(IN) :: derGlobal
            INTEGER(c_int), INTENT(IN), VALUE :: lun
        end subroutine add_mille_data
        
        subroutine copy_to_mille_buffer( nEntries, indices, derivatives, lun) BIND(c)
            use iso_c_binding
            INTEGER(c_int), INTENT(IN), VALUE :: nEntries 
            INTEGER(c_int), DIMENSION(*), INTENT(IN) :: indices
            REAL(c_double), DIMENSION(*), INTENT(IN) :: derivatives
            INTEGER(c_int), INTENT(IN), VALUE :: lun
        end subroutine copy_to_mille_buffer
        
        subroutine add_mille_special( nSpecial, indSpecial, derSpecial, lun) BIND(c)
            use iso_c_binding
            INTEGER(c_int), INTENT(IN), VALUE :: nSpecial 
            INTEGER(c_int), DIMENSION(*), INTENT(IN) :: indSpecial
            REAL(c_double), DIMENSION(*), INTENT(IN) :: derSpecial
            INTEGER(c_int), INTENT(IN), VALUE :: lun
        end subroutine add_mille_special
        
        subroutine abort_mille_record( lun) BIND(c)
            use iso_c_binding
            INTEGER(c_int), INTENT(IN), VALUE :: lun
        end subroutine abort_mille_record
        
        subroutine flush_mille_output( lun) BIND(c)
            use iso_c_binding
            INTEGER(c_int), INTENT(IN), VALUE :: lun
        end subroutine flush_mille_output
        
        subroutine reset_mille_output( lun) BIND(c)
            use iso_c_binding
            INTEGER(c_int), INTENT(IN), VALUE :: lun
        end subroutine reset_mille_output
        
        subroutine last_mille_out_file_index( lun) BIND(c)
            use iso_c_binding
            INTEGER(c_int), INTENT(OUT) :: lun
        end subroutine last_mille_out_file_index
        
        subroutine close_mille_output( lun) BIND(c)
            use iso_c_binding
            INTEGER(c_int), INTENT(IN), VALUE :: lun
        end subroutine close_mille_output
    end interface
end module
