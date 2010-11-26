! Bug report from Rice: 05-forall-bug-1.f90
! A forall construct with an assignment statement in its body
! causes the front end to fail an assertion.

program p 
  integer :: x
  forall (i = 1:2, .true.) 
     x = 0    ! astExpressionStack not empty
  end forall
end
