module test_interface

interface
   subroutine foobar (i, x, y) bind(C, name = "fubar")
     integer, value :: i
     integer :: x
     integer :: y
   end subroutine foobar

   subroutine bar_f (ch, i) bind(C, name = "bar")
     integer, value :: ch
     real, value :: i
   end subroutine bar_f
end interface

end module
