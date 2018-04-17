! S09-character-decl-asterisk-length.f90
! After tree building, tree consistency checking fails an assertion
! in 'TestExpressionTypes' when:
! (a) a character array is declared with length given after bounds
! (b) the array is referenced by indexing and substring selection.

program p
  character   c(100)*8  ! fails if we write this
! character*8 c(100)    ! works if we write this instead
  c(9)(1:1) = 'a'       ! 'assert false' in 'SgPntrArrRefExp::get_type()'
end program
