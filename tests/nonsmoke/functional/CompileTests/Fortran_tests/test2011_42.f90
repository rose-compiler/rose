PROGRAM p
IMPLICIT NONE
INTEGER :: fileUnit
logical :: isOpen
fileUnit=11
OPEN (UNIT=fileUnit, FILE='t.tmp', POSITION='rewind') 
isOpen=.true.
if (isOpen) then ! test parse with rewind in branch
  REWIND (UNIT=fileUnit) 
else
  print *,'not open'
end if
CLOSE (UNIT=fileUnit) 
PRINT *, 'OK'
END PROGRAM 

