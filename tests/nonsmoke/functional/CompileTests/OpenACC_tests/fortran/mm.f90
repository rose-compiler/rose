program matrix_multiply
   use omp_lib
   use openacc
   implicit none
   integer :: i, j, k, myid, m, n, compiled_for, option
   integer, parameter :: fd = 11
   integer :: t1, t2, dt, count_rate, count_max
   real, allocatable, dimension(:,:) :: a, b, c
   real :: tmp, secs

   open(fd,file='wallclocktime',form='formatted')

   option = compiled_for(fd) ! 1-serial, 2-OpenMP, 3-OpenACC, 4-both

!$omp parallel
!$    myid = OMP_GET_THREAD_NUM()
!$    if (myid .eq. 0) then
!$      write(fd,"('Number of procs is ',i4)") OMP_GET_NUM_THREADS()
!$    endif
!$omp end parallel

   call system_clock(count_max=count_max, count_rate=count_rate)

   do m=1,4    ! compute for different size matrix multiplies

      call system_clock(t1)

      n = 1000*2**(m-1)    ! 1000, 2000, 4000, 8000
      allocate( a(n,n), b(n,n), c(n,n) )

! Initialize matrices
      do j=1,n
         do i=1,n
            a(i,j) = real(i + j)
            b(i,j) = real(i - j)
         enddo
      enddo

!$omp parallel do shared(a,b,c,n,tmp) reduction(+: tmp)
!$acc data copyin(a,b) copy(c)
!$acc kernels
! Compute matrix multiplication.
      do j=1,n
         do i=1,n
            tmp = 0.0  ! enables ACC parallelism for k-loop
            do k=1,n
               tmp = tmp + a(i,k) * b(k,j)
            enddo
            c(i,j) = tmp
         enddo
      enddo
!$acc end kernels
!$acc end data
!$omp end parallel do

      call system_clock(t2)
      dt = t2-t1
      secs = real(dt)/real(count_rate)
      write(fd,"('For n=',i4,', wall clock time is ',f12.2,' seconds')") &
              n, secs

      deallocate(a, b, c)

   enddo

  close(fd)
end program matrix_multiply

integer function compiled_for(fd)
implicit none
integer :: fd
#if defined _OPENMP && defined _OPENACC
  compiled_for = 4
  write(fd,"('This code is compiled with OpenMP & OpenACC')")
#elif defined _OPENACC
  compiled_for = 3
  write(fd,"('This code is compiled with OpenACC')")
#elif defined _OPENMP
  compiled_for = 2
  write(fd,"('This code is compiled with OpenMP')")
#else
  compiled_for = 1
  write(fd,"('This code is compiled for serial operations')")
#endif

end function compiled_for
