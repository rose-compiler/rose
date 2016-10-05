! MODULE can be reused
    MODULE stack
    IMPLICIT NONE
! data 
    INTEGER, PARAMETER :: stack_size = 100
    INTEGER, SAVE :: store(stack_size), pos=0

! can specify the visibility separately
! note no forward declaraion of member subprograms is needed
    PRIVATE :: pos, store, stack_size
    PUBLIC :: pop, push
!Alternatively, use statements or attributes;
!    PUBLIC                 ! set default visibility
!    INTEGER, PRIVATE, SAVE :: store(stack_size), pos
!    INTEGER, PRIVATE, PARAMETER :: stack_size = 100


! manipulations
! push
   CONTAINS
    SUBROUTINE push(i)
     INTEGER, INTENT(IN) :: i
      IF (pos < stack_size) THEN
       pos = pos + 1; store(pos) = i
      ELSE
!       STOP 'Stack Full error'
       STOP
      END IF
    END SUBROUTINE push
! pop 
    SUBROUTINE pop(i)
     INTEGER, INTENT(OUT) :: i
      IF (pos > 0) THEN
       i = store(pos); pos = pos - 1
      ELSE
       STOP 'Stack Empty error'
      END IF
     END SUBROUTINE pop
   END MODULE stack

! use of module
! the types, data structure, and subprograms are exposed 
     PROGRAM StackUser
      USE stack
      IMPLICIT NONE
      INTEGER :: i, j

      CALL Push(1); 
      CALL Push(2);
      CALL Pop(i); 
      CALL Pop(j)
      PRINT *, i,j

     END PROGRAM StackUser

