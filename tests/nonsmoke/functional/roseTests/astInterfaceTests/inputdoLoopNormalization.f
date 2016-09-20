! Liao 1/5/2010
! Several variants of Fortran Do loops
      program mymain
      integer i, n, sum
 
      sum = 0
      n = 10
      write (*,*) 'loop 1-----------------'
      do 10 i = 1, n
         sum = sum + i
         write(*,*) 'i =', i
         write(*,*) 'sum =', sum
  10  continue
      
! Decremental: -2, integer type
      write (*,*) 'loop 2-----------------'
      do 20 i = 10, 1, -2
         write(*,*) 'i =', i
  20  continue

! end do
      write (*,*) 'loop 3-----------------'
      do i = 1, n
         sum = sum + i
         write(*,*) 'i =', i
      end do 
      end
