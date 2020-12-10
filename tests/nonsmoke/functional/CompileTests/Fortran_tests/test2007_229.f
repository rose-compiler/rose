      SUBROUTINE foo()

! This is an OFP error (reported 12/20/2007)
! 10   GOTO 11
      GOTO 11
! Need to have write statement process it's label
 12   WRITE (*,*)
 11   PRINT *, "hello"

      RETURN
C
      END
C                                                                    C
