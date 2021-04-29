integer function foo(i)
  integer, intent(in) :: i
  foo = i + 3
end function

program main
   integer :: foo
   integer :: d = 2
   integer :: e

   e = foo(d)

end program
