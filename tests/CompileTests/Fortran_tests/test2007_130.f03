! Example program demonstrating the use of the F2003 "wait" statement

program test
integer :: N
open (unit=1,file='asynch.dat',asynchronous='YES',form='unformatted')
write (1) N (i,i=1,100)
rewind (1)
read (1,asynchronous='YES') N
wait(1)
write (*,*) N
end
