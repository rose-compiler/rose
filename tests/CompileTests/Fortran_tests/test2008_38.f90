!
! This program throws an assert in ROSE
! This is an example of something that is 
! currently unsupported and was spotted 
! by Craig. We need these examples before
! specific features of the language can be 
! addressed (supported in ROSE).
!
subroutine boo(A)
    real :: A(1:10)
end subroutine
