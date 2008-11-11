module test2008_45_temp
 ! Unclear why the subroutine definition must be in the interface block
   interface assignment(=)
      subroutine real_sub(a,b)
         integer, intent(out) :: a
         logical, intent(in)  :: b
      end subroutine real_sub
   end interface

!   contains

end module

