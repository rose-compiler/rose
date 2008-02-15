! Example showing use of "external" statement with "block data"

! Here is the recommended approach to modifying a program 
! containing a program unit such as the following:

BLOCK DATA FOO
  COMMON /VARS/ X, Y, Z
  DATA X, Y, Z / 3., 4., 5. /
END

! If the above program unit might be placed in a library module, 
! then ensure that every program unit in every program that 
! references that particular COMMON area uses the EXTERNAL 
! statement to force the area to be initialized.

! For example, change a program unit that starts with

!INTEGER FUNCTION CURX()
!   COMMON /VARS/ X, Y, Z
!   CURX = X
!END

! so that it uses the EXTERNAL statement, as in:

INTEGER FUNCTION CURX()
   COMMON /VARS/ X, Y, Z
   EXTERNAL FOO
   CURX = X
END

