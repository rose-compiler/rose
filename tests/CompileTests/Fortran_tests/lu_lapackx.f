      INTEGER          LDA, INFO, N, I, J
      DOUBLE PRECISION A(2048,2048), L(2048,2048), U(2048,2048)
      INTEGER          IPIV(2048)

      DOUBLE PRECISION tstart, tend, ttotal, gflops

*     .. External Subroutines ..

      LDA=2048
      N=2048
        
      DO I = 1,N
        Do J = 1,N
          L(I,J)=0.0
          U(I,J)=0.0
        END DO
      END DO

      DO I = 1,N
        Do J = 1,I+1
          L(I,J)=I+J+1
          U(I,J)=I+J+1
        END DO
      END DO

      DO I = 1,N
        Do J = 1,N
          Do K = 1,N
            A(I,J) = A(I,J) + L(I,K) * U(K,J)
          END DO
        END DO
      END DO



      write (*,*) 'Hello world'
      gflops = ((2*(N/1000)*(N/1000)*(N/1000))/3)/(ttotal)

      END

      subroutine timestamp ( )

      implicit none

      character ( len = 8 ) date
      character ( len = 10 ) time
      integer I

      
      DO I = 1,4
      write (*,*) 'Hello world'
      END DO

      return
      end
