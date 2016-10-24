program main
  implicit none

! This fails with: 
! FortranParserActionROSE.C:6143: void c_action_common_block_object_list(int): Assertion `(*(FortranParserState::getCurrentExpressionStack())).size() == (size_t) count' failed.
 common / pointers /  p_x(10)

 integer*8 p_x

end program main
