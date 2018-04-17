! Example program demonstrating the use of the F2003 "wait" statement

program test

! Note that declaration of "i" is required if it is to not be implicitly built.
integer :: N,i
open (unit=1,file='asynch.dat',asynchronous='YES',form='unformatted')

! This is ilegal code
!write (1) N (i,i=1,100)
write (1) (N,i=1,100)

rewind (1)
read (1,asynchronous='YES') N
wait(1)
write (*,*) N
end
