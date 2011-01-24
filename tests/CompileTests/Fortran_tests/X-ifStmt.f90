! ROSE incorrectly parses the IF statement.
PROGRAM ifStmtBug

IF (.true.)  PRINT *, 'Hello World' 
! testTranslator translates the above into
! IF (.TRUE.) PRINT .TRUE., 'Hello World'

END PROGRAM
 