! We can't have a global variable in Fortran (structurally) so a 
! call to a function can't be amgibuous with a variable reference.

!    integer :: foobar = 0
! end program

module communicate

     implicit none
     private
     save

     public :: foo

   ! This is an error in F90
   ! integer :: foobar = 0

   contains

     subroutine foo
        ! Call to function not yet declared (function declaration defined in initially in 
        ! global scope and converted to module function if it is found to be declared there).
          call foobar
     end subroutine foo

   ! If this is declared here then we have to fixup the scope of the declaration (from global scope to module scope)
     subroutine foobar
          character (3) :: cmodel   ! model name temporary
     end subroutine foobar

end module communicate


!subroutine foobar
!   character (3) :: cmodel   ! model name temporary
!end subroutine foobar
