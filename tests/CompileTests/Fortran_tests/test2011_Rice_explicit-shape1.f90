program explicit_shape1
  integer a(10)
  integer, dimension(5, 4) :: a2, b(10:20, 30:40, 0:9), c
  integer e(10), f
  integer :: g, h(10), i(5)
  integer, dimension(2, 3) ::  j, k(10)
  integer x(5), y, z(10)
  
  contains
  
  subroutine subprog(c, d)
   integer, dimension(5, 4) :: c(10:20, 3:4, 0:*), d 
  end subroutine
end program
