! This example code demonstrates "Listed-Directed Output" for the write statment.
   integer mul
   integer multg
   integer, dimension(10) :: nfile
   write(nfile(1),'(a23,i3,a3,100a1)') ' ---- multigrid level (', mul, ' ) ',('-',i=1,40)
!  write(nfile(1),'(a23,i3,a3,100a1)') , mul, ' ) ',('-',i=1,40)
!  write(nfile(1),'(a23,i3,a3,100a1)') ' ---- multigrid level (', mul, ' ) ',('-',i=1,40)
!  write(*,*) mul, ('-',i=1,40)
   write(*,*) ('-',i=1,40)
end
