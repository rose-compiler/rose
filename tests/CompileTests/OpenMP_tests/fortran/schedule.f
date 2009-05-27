       SUBROUTINE A1(N, A, B)
       INTEGER I, N
       REAL B(N), A(N)
!$OMP PARALLEL DO schedule(auto)
       DO I=2,N
       B(I) = (A(I) + A(I-1)) / 2.0
       ENDDO

!$OMP PARALLEL DO schedule(static)
       DO I=2,N
       B(I) = (A(I) + A(I-1)) / 2.0
       ENDDO

!$OMP PARALLEL DO schedule(guided, N)
       DO I=2,N
       B(I) = (A(I) + A(I-1)) / 2.0
       ENDDO

!$OMP PARALLEL DO schedule(dynamic, 5)
       DO I=2,N
       B(I) = (A(I) + A(I-1)) / 2.0
       ENDDO
       END SUBROUTINE A1
