subroutine a1(a,b,n)
    integer i,n
    real a(n),b(n)
!$omp parallel do
  do i=2,n
    b(i)= a(i)
  enddo
!$omp end parallel do nowait
  print *,'end'
end subroutine a1  
