! ROSE incorrectly asserts that the start of construct and the end of construct of 
! the IF construct to be different.
! NOTE: such assertion is incorrect for many other constructs.
PROGRAM ifConstructBug

! More than one statement can exist on the same line as long as they are separated by a semi-colon.
IF (.true.)  THEN ;  PRINT *, 'Hello World' ; ENDIF

END PROGRAM
 