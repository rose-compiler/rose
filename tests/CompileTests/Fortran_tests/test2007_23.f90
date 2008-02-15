subroutine foo()

INTEGER, DIMENSION(:,:), ALLOCATABLE :: a !rank 1
! INTEGER, ALLOCATABLE :: b(:,:) !rank 2
! REAL, DIMENSION(:), ALLOCATABLE :: c !rank 1

!ALLOCATE( a(100) )

! ALLOCATE( b(n,n), c(-10:89) ).

!DEALLOCATE ( a )

! DEALLOCATE ( a, b )
! DEALLOCATE ( c, STAT=test )

end subroutine

