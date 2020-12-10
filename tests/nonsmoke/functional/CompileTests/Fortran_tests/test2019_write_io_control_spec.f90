!! This program can be run to compare unparsed version with original
!
program test_write
   integer :: i, j, k, ii
   namelist /nlist/ i, j, k
   character*4 :: ifmt = '(i7)'

   i = 1; j = 2; k = 3; ii = 13;

   write(fmt=10,unit=6) ii
   write(fmt='(i3)',unit=6) ii+1
   write(6, fmt='(i4)') ii+2
   write(6, '(i5)') ii+3
   write(*, '(i6)') ii+4
   write(*,ifmt) ii+5
   write(*,*) ii+6
   write(6, nlist)

10 format(i2)

end program
