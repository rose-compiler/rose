subroutine print_incremented(x,y)
  integer, intent(in) :: x
  integer, intent(out) :: y
  
  print *, x + 1
  y = x * 2

end subroutine print_incremented

program main
   integer :: d = 2
   integer :: e

   call print_incremented(d, e)
   print *, e

end program
