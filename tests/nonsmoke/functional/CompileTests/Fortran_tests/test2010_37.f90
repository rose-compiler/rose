! This is a copy of test2007_172.f03 (but I think this is not F03 specific).
SUBROUTINE SUB(N)
  CHARACTER(N) A(5)
  INQUIRE (IOLENGTH=IOL) A(1)  ! Inquire by output list
! OPEN (7,RECL=IOL)

END SUBROUTINE
