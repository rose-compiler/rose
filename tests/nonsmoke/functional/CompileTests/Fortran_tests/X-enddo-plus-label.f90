! enddo-plus-label.f90 -- statement labels on 'end do's move to subsequent assignment statements

program prog

      integer :: x, y, z

100   do x = 1, 2
101     y = 1
102       do y = 3, 4
103         z = 2
104       end do
105     y = 3
106   end do

      z = 4
      z = 5

end program
