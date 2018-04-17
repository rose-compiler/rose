! Use of label on function is accepted by gfortran, but fails for OFP.
!  integer function alt_main()
10 integer function alt_main()
      integer :: a
      alt_main = a
      return
20 end function alt_main



