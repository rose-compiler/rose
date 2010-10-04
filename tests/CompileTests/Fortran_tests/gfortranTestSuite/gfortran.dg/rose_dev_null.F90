# 1 "/data/home/scott/test/gfortran/src/gfortran.dg/dev_null.F90"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "/data/home/scott/test/gfortran/src/gfortran.dg/dev_null.F90"
! { dg-do run }
! { dg-options "-std=legacy" }
!
! pr19478 read from /dev/null
! Thomas.Koenig@online.de
CHARACTER(len=20) :: foo
OPEN (UNIT=10, FILE="/dev/null") 
WRITE (UNIT=10, FMT='(A)') "Hello"
REWIND (UNIT=10) 
READ (UNIT=10, FMT='(A)', END=100) foo
CALL abort()
100 CONTINUE
END 
