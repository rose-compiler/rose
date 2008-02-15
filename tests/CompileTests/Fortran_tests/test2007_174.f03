      SUBROUTINE SUB (X,Y)
        INTERFACE
          SUBROUTINE SUB2 (A,B)
            OPTIONAL :: B
          END SUBROUTINE
        END INTERFACE
        OPTIONAL :: Y
        IF (PRESENT(Y)) THEN          ! Reference to Y conditional
          X = X + Y                   ! on its presence
        ENDIF
        CALL SUB2(X,Y)
      END SUBROUTINE

      SUBROUTINE SUB2 (A,B)
        OPTIONAL :: B                 ! B and Y are argument associated,
        IF (PRESENT(B)) THEN          ! even if Y is not present, in
          B = B * A                   ! which case, B is also not present
          PRINT*, B
        ELSE
          A = A**2
          PRINT*, A
        ENDIF
      END SUBROUTINE

