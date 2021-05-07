integer function foo(i) result(j)
  integer, intent(in) :: i
  j = i + 2
end function

program main
   integer :: foo
   integer :: d = 2
   integer :: e

   e = foo(d)
end program
