program test_mod
   implicit none

   integer, dimension(50) :: array

   array = 1

   print "(a, i8)", &
      "Line 1", &
       array(2), &
      "Line 2"
end program
