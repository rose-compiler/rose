! Liao, 5/27/2009
! Test line continuation handling 
!
! Two cases:  
!         & \n !$omp .... 
!         & \n !$omp & ....
subroutine a1(aa,b,n)
    integer i,n
    real aa(n),b(n)
!$omp parallel shared (a&  !first continuation with comments
!$omp &a,b,n) & !second continuation with a leading '&' and an ending '&'  
!$omp private(i)
!$omp do 
  do i=2,n
    b(i)= aa(i)
  enddo
!$omp end do &
!$omp nowait
!$omp end parallel
  print *,'end'
end subroutine a1  
