! Example program demonstrating the use of the F2003 "wait" statement

program test
implicit none
integer :: N,i
!open (unit=1,file='asynch.dat',asynchronous='YES',form='unformatted')

! This works fine and does not cause a function to be generated
!write (1) N

! Error: This causes a function "N" to be generated, but it is not legal
!        code. But when this was a f03 test code this was not so clear!
! write (1) N (i,i=1,100) ! Illegal code!
write (1) (N,i=1,100)

! The variable "i" is in the function scope not the scope of the write statement
! i = 0

!rewind (1)
!read (1,asynchronous='YES') N
!wait(1)
!write (*,*) N
end
