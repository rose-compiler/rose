function increment_value(a) result(b)
  integer :: a
  integer :: b

  b = a + 1
end function

program main
   integer :: d = 2

   print *, increment_value(d)
end program
