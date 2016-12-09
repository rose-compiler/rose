FUNCTION EOSHIFT_REAL (X, shift, boundary, dim)


  integer, parameter :: MX = 256
  integer, parameter :: MY = 256
  integer, parameter :: MZ = 256

  integer, parameter :: KR = KIND(1.0)
  integer, parameter :: KI = KIND(1)

  integer(ki) :: shift, dim
  real(kr) :: boundary
  real(kr) :: X(MX,MY,MZ)
  real(kr), dimension(MX,MY,MZ) :: eoshift_real

  eoshift_real = 1.0

end function eoshift_real


subroutine advect(dummy2, IL1, IL2, IR1, IR2, IR3, IR4, IR5, OR1, OR2, IO1, IO2)

   implicit none

!   include "advect.h"


integer, parameter :: MX = 256
integer, parameter :: MY = 256
integer, parameter :: MZ = 256

integer, parameter :: KR = KIND(1.0)
integer, parameter :: KI = KIND(1)
integer, parameter :: KL = KIND(.true.)

!   interface
!      subroutine start_clock() ! BIND(C)
!      end subroutine start_clock
!      subroutine stop_clock() ! BIND(C)
!      end subroutine stop_clock
!      FUNCTION EOSHIFT_REAL (X, shift, boundary, dim)
!      include "advect.h"
!      integer(ki) :: shift, dim
!      real(kr) :: boundary
!      real(kr) :: eoshift_real(0:mx,0:my,0:mz)
!      real(kr) :: X(0:mx,0:my,0:mz)
!      end function eoshift_real
!   end interface

   real(kr), external :: eoshift_real

   integer             :: id = 1

   real(KR), parameter :: ZEPS = 1.0d-9
   real(KR), parameter :: ZERO = 0.0d0

   !
   ! array declarations
   !

!   logical(KL), intent(in) :: IL1(0:MX,0:MY,0:MZ)
!   logical(KL), intent(in) :: IL2(0:MX,0:MY,0:MZ)
   logical :: dummy = .true.
   logical, intent(in) :: dummy2 
   logical, intent(in) :: IL1(MX, MY, MZ)
   logical, intent(in) :: IL2(MX,MY,MZ)

!   real(KR), intent(in)    :: IR1(0:MX,0:MY,0:MZ)
!   real(KR), intent(in)    :: IR2(0:MX,0:MY,0:MZ)
!   real(KR), intent(in)    :: IR3(0:MX,0:MY,0:MZ)
!   real(KR), intent(in)    :: IR4(0:MX,0:MY,0:MZ)
!   real(KR), intent(in)    :: IR5(0:MX,0:MY,0:MZ)
   real, intent(in)    :: IR1(MX,MY,MZ)
   real, intent(in)    :: IR2(MX,MY,MZ)
   real, intent(in)    :: IR3(MX,MY,MZ)
   real, intent(in)    :: IR4(MX,MY,MZ)
   real, intent(in)    :: IR5(MX,MY,MZ)

!   real(KR), intent(out)   :: OR1(0:MX,0:MY,0:MZ)
!   real(KR), intent(out)   :: OR2(0:MX,0:MY,0:MZ)
   real, intent(out)   :: OR1(MX,MY,MZ)
   real, intent(out)   :: OR2(MX,MY,MZ)

!   real(KR), intent(inout) :: IO1(0:MX,0:MY,0:MZ)
!   real(KR), intent(inout) :: IO2(0:MX,0:MY,0:MZ)
   real, intent(inout) :: IO1(MX,MY,MZ)
   real, intent(inout) :: IO2(MX,MY,MZ)

   !
   ! computation for advection
   !

   where(IL1)
      IO1 = IR5 - IO1
      where (IO1 < ZEPS) IO1 = ZERO
   else where
      IO1 = ZERO
   end where

   where(IL2)
      IO2 = IO2 - IR5
      where (IO2 < ZEPS) IO2 = ZERO
   else where 
      IO2 = ZERO
   end where

   OR1 = ZERO
   where(IO1*IO2 > ZERO) OR1 = IR1

   IO1  = sign(OR1,IO2)*min( abs(IO1), abs(IO2), IR2*abs(IO1) + IR3*abs(IO2) )

   OR1 = IR4*(IR5 + IO1)
   OR2 = eoshift_real(OR1, 1, ZERO, id)

!   call mpi_barrier(MPI_COMM_WORLD, ierr)
!   if (my_id .eq. 0) call stop_clock()

end subroutine advect
