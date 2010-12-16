! Bug report from Rice: 06-forall-bug-2.f90
! A one-line forall construct with an assignment statement in its body
! causes the front end to fail an assertion.

program p
  integer :: x
  forall (i=1:2, .true.) x = 0    ! astExpressionStack not empty
end
