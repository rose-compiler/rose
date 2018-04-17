# 1 "/data/home/scott/test/gfortran/src/gfortran.dg/dev_null.F90"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "/data/home/scott/test/gfortran/src/gfortran.dg/dev_null.F90"
! { dg-do run }
! { dg-options "-std=legacy" }
!
! pr19478 read from /dev/null
! Thomas.Koenig@online.de





      character*20 foo
      open(10,file="/dev/null")
      write(10,'(A)') "Hello"
      rewind(10)
      read(10,'(A)',end=100) foo
      call abort
 100  continue
      end
