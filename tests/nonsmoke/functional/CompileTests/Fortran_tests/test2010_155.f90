! P06-module-scopes.f90
! When a module variable has the same name as a local variable in
! a subprogram contained in the module, the .rmod and unparsed output files
! produced by testTranslator are incorrect:
! (1) the module variable is declared with the subprogram variable's type
! (1) the subprogram variable gets a copy of the module variable's
!     initializer, if any.

module m155
   real  :: x = 0.0  ! type of this 'x' becomes 'character'
contains
  subroutine g
    character :: x   ! this 'x' gets an initializer '= 0.0'
  end subroutine
end module
