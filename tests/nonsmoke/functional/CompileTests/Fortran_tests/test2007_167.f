! Assigned Goto
      INTEGER RETURN_LABEL

!  Simulate a call to a local procedure
      ASSIGN 100 TO RETURN_LABEL
      GOTO 9000
100   CONTINUE

9000  CONTINUE
!  A "local" procedure

      GOTO RETURN_LABEL

      END

