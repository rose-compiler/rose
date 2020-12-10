        subroutine dgemm(a,b,c,n)
          double precision a
          dimension a(*)
          double precision b
          dimension b(*)
          double precision c
          dimension c(*)
          integer n

          integer i,j,k
          do k=0,n-1,1
             do j=0,n-1,1
                do i=0,n-1,1
                   c(j*n+i) = c(j*n+i)+a(k*n+i)*b(j*n+k)
                enddo
             enddo
          enddo
        end

