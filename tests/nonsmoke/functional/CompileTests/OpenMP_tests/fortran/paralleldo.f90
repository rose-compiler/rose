subroutine a1(a,b,n)
    integer i,n
    real a(n),b(n)
!$omp parallel
!$omp do 
  do i=2,n
    b(i)= a(i)
  enddo
!$omp end do nowait
!$omp end parallel
  print *,'end'
end subroutine a1  
