!> Millepede II main program \ref sssec-stalone "Pede".
PROGRAM TestMilleF
    USE MilleF 
    use iso_c_binding
    IMPLICIT NONE
    CHARACTER (LEN=100) :: fname 
    integer             :: lfn 
    integer(kind=c_int)             :: lun 
    INTEGER(kind=c_int), dimension(5) :: localLabels
    REAL(kind=c_double), dimension(5) :: localDeriv
    INTEGER(kind=c_int), dimension(2) :: globalLabels
    REAL(kind=c_double), dimension(2) :: globalDeriv
    integer(kind=c_int)         :: nLocal = 5 
    integer(kind=c_int)            :: nGlobal = 2 
    logical (c_bool)            :: flag
    real(kind=c_double)            :: measurement
    real(kind=c_double)            :: unc

    SAVE
    fname = "testFromF90.csv"
    lfn = 15
    lun = 1
    flag = .false. 
    measurement = 1.1234e-5
    unc       = 1.e-6 
    localDeriv = [1.0, 0.2, -0.2, 1.0, -1.0]
    localLabels = [1, 2, 4, 5, 6] 
    globalLabels = [10001, 20001]
    globalDeriv = [1., -0.5]
    CALL open_mille_out(fname, lfn, lun,  flag, flag, 2000)
    CALL add_mille_data(measurement, unc, nLocal, localLabels, localDeriv, nGlobal, globalLabels, globalDeriv, lun)
    CALL write_mille_out(lun)
    CALL close_mille_output(lun)

END PROGRAM


