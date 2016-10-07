      RECURSIVE INTEGER FUNCTION fib(n)
      INTEGER n, i, j
      IF ( n .LT. 2) THEN
      fib = n
      ELSE
!$OMP TASK SHARED(i) default(SHARED)
      i = fib( n-1 )
!$OMP END TASK

!$OMP TASK SHARED(j)
      j = fib( n-2 )
!$OMP END TASK
!$OMP TASKWAIT
!  !$OMP END TASKWAIT, wrong !!
      fib = i+j
      END IF
      END FUNCTION
