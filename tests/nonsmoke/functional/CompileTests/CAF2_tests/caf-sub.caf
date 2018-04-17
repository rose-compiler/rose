subroutine mysub(A, B, C, myteam)
real :: A(:,:)[*]
double precision:: B(:)[*]
integer:: C(:,:,:)
team :: myteam

A = B[myteam]
C(1,2,:)[2@myteam] = B

end subroutine 


program cafsub
real :: A(:,:)[*]
double precision, allocatable:: B(:)[*]
integer:: C(:,:,:)
team :: ateam

call mysub(A, B, C, ateam)

end program
