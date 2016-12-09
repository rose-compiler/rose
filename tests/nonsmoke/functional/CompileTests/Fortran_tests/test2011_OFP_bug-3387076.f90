!
! Regression test for ROSE bug 3387076
!
! Title: ROSE: Array initialization unparses incorrectly
!
! The bug occurs when the array inializer is unparsed as (/(/1,2,3/)/), 
! rather than (/1,2,3/)
!
   integer :: A(3) = [1,2,3]

end
