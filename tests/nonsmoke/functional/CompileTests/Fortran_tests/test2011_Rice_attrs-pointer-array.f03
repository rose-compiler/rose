! attributes that can be used with ALLOCATABLE are
! DIMENSION (with deferred shape), PRIVATE, PUBLIC, SAVE, TARGET
! POINTER is often used in conjunction with ALLOCATABLE.
! Attributes compatible with POINTER are 
! DIMENSION (with deferred shape), OPTIONAL, SAVE, PUBLIC, PRIVATE
!
MODULE attrs_pointer_array
!
  REAL, ALLOCATABLE, PRIVATE, SAVE, TARGET :: a (:, :) 
  LOGICAL, SAVE, TARGET, DIMENSION (:), ALLOCATABLE, PUBLIC :: b
  REAL, SAVE, PRIVATE, POINTER, DIMENSION(:,:) :: gp => NULL()
!  
  TYPE :: Rec
    PRIVATE  
    LOGICAL, DIMENSION (:), ALLOCATABLE, PUBLIC :: a
    REAL, ALLOCATABLE :: b (:, :) 
  END TYPE
!  
  CONTAINS
!  
  SUBROUTINE MySub(n, m, d, c)
  !
    INTEGER n, m
    REAL, OPTIONAL, DIMENSION(:, :), POINTER :: c
    LOGICAL, POINTER :: d(:)
    ALLOCATE(a(n,n))
    ALLOCATE(b(m))
    IF (PRESENT(c)) THEN 
      c => a
    ELSE
      c => gp
    ENDIF
    d => b
  !
  END SUBROUTINE
!
END MODULE

PROGRAM prog
USE attrs_pointer_array
!
REAL, POINTER, DIMENSION(:,:) :: pa
LOGICAL, POINTER :: pb(:)
TYPE(Rec), SAVE, POINTER :: p
CALL MySub(2, 3, pb, pa)
CALL MySub(4, 5, pb)
ALLOCATE(p)
ALLOCATE(p%a(6))
pa(6,7) = 7.8
pb(9) = .true.
!
END
