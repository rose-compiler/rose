        I = 0 ; J = 0
        MY_DO: DO
          I = I + 1
          J = J + 1
          PRINT *, I
          IF (I.GT.4) EXIT MY_DO
          IF (J.GT.3) CYCLE MY_DO
          I = I + 2
        END DO MY_DO
        END
