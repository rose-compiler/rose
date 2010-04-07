       subroutine a1(n, a, b)
       integer i, n
       real b(n), a(n)
!$OMP PARALLEL DO schedule(auto)
       do i=2,n
       b(i) = (a(i) + a(i-1)) / 2.0
       enddo

!$OMP PARALLEL DO schedule(static)
       do i=2,n
       b(i) = (a(i) + a(i-1)) / 2.0
       enddo

!$OMP PARALLEL DO schedule(guided, N)
       do i=2,n
       b(i) = (a(i) + a(i-1)) / 2.0
       enddo

!$OMP PARALLEL DO schedule(dynamic, 5)
       do i=2,n
       b(i) = (a(i) + a(i-1)) / 2.0
       enddo
       end subroutine a1
