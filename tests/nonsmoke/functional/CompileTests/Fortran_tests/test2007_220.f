      subroutine foo()
    ! Commonblocks build variables much like variable declarations, 
    ! only a later variable declaration of the same variable can 
    ! change the type (or base type if the common block declared an array)
      COMMON /my_common_block/ A
      CHARACTER*16 A
!     integer A
      end
