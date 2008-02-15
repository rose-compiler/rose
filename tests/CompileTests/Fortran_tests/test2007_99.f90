! This is a clearer example of test2007_19.f90 (which does not overload keywords)
! recursive pure integer function foo(n) result(i)
elemental pure integer function foo(n) result(i)
   integer, intent(in) :: n
   i = 1
end function foo

