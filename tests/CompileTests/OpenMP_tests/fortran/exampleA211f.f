      REAL FUNCTION FN1(I)
        INTEGER I
        FN1 = I * 2.0
        RETURN
      END FUNCTION FN1

      REAL FUNCTION FN2(A, B)
        REAL A, B
        FN2 = A + B
        RETURN
      END FUNCTION FN2

      PROGRAM A21
        INCLUDE "omp_lib.h" ! or USE OMP_LIB
        INTEGER ISYNC(256)
        REAL WORK(256)
        REAL RESULT(256)
        INTEGER IAM, NEIGHBOR
!$OMP PARALLEL PRIVATE(IAM, NEIGHBOR) SHARED(WORK, ISYNC)
        IAM = OMP_GET_THREAD_NUM() + 1
        ISYNC(IAM) = 0
!$OMP BARRIER
C Do computation into my portion of work array
        WORK(IAM) = FN1(IAM)
C Announce that I am done with my work.
C The first flush ensures that my work is made visible before
C synch. The second flush ensures that synch is made visible.
!$OMP FLUSH(WORK,ISYNC)
        ISYNC(IAM) = 1
!$OMP FLUSH(ISYNC)
C Wait until neighbor is done. The first flush ensures that
C synch is read from memory, rather than from the temporary
C view of memory. The second flush ensures that work is read
C from memory, and is done so after the while loop exits.
        IF (IAM .EQ. 1) THEN
        NEIGHBOR = OMP_GET_NUM_THREADS()
        ELSE
        NEIGHBOR = IAM - 1
        ENDIF
        DO WHILE (ISYNC(NEIGHBOR) .EQ. 0)
!$OMP FLUSH(ISYNC)
        END DO
!$OMP FLUSH(WORK, ISYNC)
        RESULT(IAM) = FN2(WORK(NEIGHBOR), WORK(IAM))
!$OMP END PARALLEL
      END PROGRAM A21

