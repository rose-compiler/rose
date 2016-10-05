! Example program demonstrating the use of the F2003 "wait" statement

program test
integer, asynchronous, dimension(100) :: array
open (unit=1,file='asynch.dat',asynchronous='YES',form='unformatted')
write (1) (i,i=1,100)
rewind (1)
read (1,asynchronous='YES') array
wait(1)
write (*,*) array(1:10)
end
