C The output for the generated code is not correct:
C        DOUBLE PRECISION :: function
C        END


       REAL*8 FUNCTION DCABS1 (QVAR)
       REAL*8 QVAR(2)
       DCABS1 = DABS(QVAR(1)) + DABS(QVAR(2))
       RETURN
       END
