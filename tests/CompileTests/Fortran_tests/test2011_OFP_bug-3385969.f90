!
! Unit test for bug 3385969.  Boz literals are handled by OFP correctly
! but not by ROSE.
!
   integer, parameter :: INT8  = SELECTED_INT_KIND(16)
   integer(INT8), parameter :: ieee64_two = Z'4000000000000000'
   integer, parameter :: binary = B'1101'
   integer, parameter :: octal  = O'15'
   integer, parameter :: hex  = Z'D'

   print *, ieee64_two
   print *, binary
   print *, octal
   print *, hex

   if (binary .ne. 13 .and. octal .ne. 13 .and. hex .ne. 13) stop 1

end
