! Another example program from Liao (required for Fortran outliner work)
      SUBROUTINE OUT_1
      WRITE(UNIT=*, FMT=*) 'Hello, world'
      END

      PROGRAM HELLO
      CALL OUT_1()

! This was treated as a variable instead of a function!
! This is now correctly treated as a function but output with the "()" syntax.
      CALL OUT_1
      END
