! I think this is using F03 syntax for procedure pointers.
! So this might really be a FFortran 2003 test code 
! (syntax fails for gfortran, but is part of the gfortran test suite).

module myMod34

  CONTAINS

  real function proc3( arg1 )
     integer :: arg1
     proc3 = arg1+7
  end function proc3

  subroutine proc4( arg1 )
     procedure(real), pointer :: arg1
     if (arg1(0)/=7) call abort()
  end subroutine proc4

end module myMod34

program myProg
  use myMod34
  PROCEDURE (real), POINTER :: p => NULL()
  p => proc3
  call proc4( p )
end program myProg
 
