!> Millepede II main program \ref sssec-stalone "Pede".
PROGRAM TestMilleF
    USE MilleF 
    use iso_c_binding
    IMPLICIT NONE
    CHARACTER (LEN=100) :: fname 
    integer             :: lfn 
    integer(kind=c_int)             :: lun 
    integer(kind=c_int)             :: statusFlag 
    integer(kind=c_int)             :: bufSize 
    integer(kind=c_int)             :: iter 
    INTEGER(kind=c_int), dimension(5) :: localLabels
    REAL(kind=c_double), dimension(5) :: localDeriv
    INTEGER(kind=c_int), dimension(2) :: globalLabels
    REAL(kind=c_double), dimension(2) :: globalDeriv
    INTEGER(kind=c_int), dimension(2) :: specialInt
    REAL(kind=c_double), dimension(2) :: specialFloat
    INTEGER(kind=c_int), dimension(100) :: readBufferInt
    REAL(kind=c_float), dimension(100) :: readBufferFloat
    REAL(kind=c_double), dimension(100) :: readBufferDouble
    REAL(kind=c_double), dimension(5) :: check
    integer(kind=c_int)         :: nLocal = 5 
    integer(kind=c_int)            :: nGlobal = 2 
    logical (kind=c_bool)            :: flag
    real(kind=c_double)            :: measurement
    real(kind=c_double)            :: unc
    real(kind=c_double)            :: epsilon
    integer                        :: auxIndex

    SAVE
    fname = "testFromF90.dat"
    lfn = 15
    lun = 1
    flag = .false. 
    epsilon = 1.e-6
    measurement = 1.1234e-5
    unc       = 1.e-6 
    localDeriv = [1.0, 0.2, -0.2, 1.0, -1.0]
    localLabels = [1, 2, 4, 5, 6] 
    globalLabels = [10001, 20001]
    globalDeriv = [1., -0.5]
    specialInt = [1,2] 
    specialFloat = [99.,112.]
    CALL open_mille_out(fname, lfn, lun,  flag, flag, 2000)
    CALL add_mille_data(measurement, unc, nLocal, localLabels, localDeriv, nGlobal, globalLabels, globalDeriv, lun)
    CALL add_mille_special(2, specialInt, specialFloat, lun)
    CALL write_mille_out(lun)
    CALL close_mille_output(lun)

    ! read back
    CALL open_mille(fname, lfn, statusFlag, lun) 
    if (statusFlag /= 0) then 
        print *, "Failed to open file - status ",statusFlag
        call exit(1)
    endif 
    bufSize = 1 
    ! try a read with insufficient buffer, check for expected error code 
    call read_mille(readBufferDouble, readBufferFloat, readBufferInt, bufSize, statusFlag, lun) 
    if (statusFlag /= -4) then 
        print *, "Unexpected status flag from read_mille with invalid buffer - expected -4, got ",statusFlag
        call exit(1)
    endif 
    if (bufSize /= 5 + size(localDeriv) + size (globalDeriv) + size (specialFloat)) then 
        print *, "Unexpected corrected buffer size from read_mille with invalid buffer - expected " &
        , 5 + size(localDeriv) + size (globalDeriv) + size (specialFloat)," got ",bufSize
        call exit(1)
    endif 
    ! reset to start of entry for a new read
    call reset_mille_entry(lun) 
    
    ! now the read should work - bufSize was updated in the background
    call read_mille(readBufferDouble, readBufferFloat, readBufferInt, bufSize, statusFlag, lun) 

    ! Expect status 4 (floats read)
    if (statusFlag /= 4) then 
        print *, "Unexpected status flag from read_mille - expected +4, got ",statusFlag
        call exit(1)
    endif 
    ! expect the correct used buffer size to be reported
    if (bufSize /= 3 + size(localDeriv) + size(globalDeriv) + size(specialFloat) + 2) then 
        print *, "Unexpected nRead - expected ",3 + size(localDeriv) + size(globalDeriv) + size(specialFloat) + 2," got ",bufSize
        call exit(1)
    endif
    ! validate local derivative block 
    auxIndex = 2
    if (any(abs(readBufferDouble(auxIndex+1:auxIndex+size(localDeriv))-localDeriv) > epsilon)) then
        print *, "Local derivatives not ok!"
        do iter = auxIndex+1, auxIndex+size(localDeriv)
            print *, readBufferDouble(iter), localDeriv(iter-auxIndex)
        end do 
        call exit(1)
    end if 
    if (any(readBufferInt(auxIndex+1:auxIndex+size(localLabels)) /= localLabels)) then
        print *, "Local labels not ok!"
        do iter = auxIndex+1, auxIndex+size(localLabels)
            print *, readBufferInt(iter), localLabels(iter-auxIndex)
        end do 
        call exit(1)
    end if 
    ! validate global derivative block 
    auxIndex = 3 + size(localDeriv)
    if (any(abs(readBufferDouble(auxIndex + 1 : auxIndex + size(globalDeriv))-globalDeriv) > epsilon)) then
        print *, "Global derivatives not ok!"
        do iter = auxIndex+1, auxIndex + size(globalDeriv)
            print *, readBufferDouble(iter), localDeriv(iter- auxIndex)
        end do 
        call exit(1)
    end if 
    if (any(readBufferInt(auxIndex+1:auxIndex+size(globalLabels)) /= globalLabels)) then
        print *, "Global labels not ok!"
        do iter = auxIndex+1, auxIndex+size(globalLabels)
            print *, readBufferInt(iter), globalLabels(iter-auxIndex)
        end do 
        call exit(1)
    end if 

    ! validate special block 
    auxIndex = 5 + size(localDeriv) + size (globalDeriv)
    if (any(abs(readBufferDouble(auxIndex + 1 : auxIndex + size(specialFloat))-specialFloat) > epsilon)) then
        print *, "Special floats not ok!"
        do iter = auxIndex+1, auxIndex + size(specialFloat)
            print *, readBufferDouble(iter), specialFloat(iter- auxIndex)
        end do 
        call exit(1)
    end if 
    if (any(readBufferInt(auxIndex+1:auxIndex+size(specialInt)) /= specialInt)) then
        print *, "Global labels not ok!"
        do iter = auxIndex+1, auxIndex+size(specialInt)
            print *, readBufferInt(iter), specialInt(iter-auxIndex)
        end do 
        call exit(1)
    end if 



END PROGRAM


