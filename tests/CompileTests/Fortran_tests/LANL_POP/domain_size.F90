!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module domain_size

!BOP
! !MODULE: domain_size
!
! !DESCRIPTION:
!  This module contains parameters for the global model domain size
!  decomposition block size.  It is used by the domain and block
!  modules for decomposing the model domain across processors.
!
! !REVISION HISTORY:
!  CVS:$Id: domain_size.F90.test,v 1.2 2003/03/25 13:41:48 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod

   implicit none
   private
   save

! !DEFINED PARAMETERS:

   integer (int_kind), parameter, public ::  &  ! model size parameters
      nx_global =  192 ,&! extent of horizontal axis in i direction
      ny_global =  128 ,&! extent of horizontal axis in j direction
      km = 20          ,&! number of vertical levels
      nt =  2            ! total number of tracers

   integer (int_kind), parameter, public :: &
      block_size_x = 16, &! size of block in first  horizontal dimension
      block_size_y = 16   ! size of block in second horizontal dimension

   !*** The model will inform the user of the correct
   !*** values for theparameters below.  A value higher than
   !*** necessary will not cause the code to fail, but will
   !*** allocate more memory than is necessary.  A value that
   !*** is too low will cause the code to exit.  
   !*** A good initial guess is found using
   !*** max=(nx_global/block_size_x)*(ny_global/block_size_y)/
   !***         num_procs
 
   integer (int_kind), parameter, public :: &
      max_blocks_clinic = 23,  &! max number of blocks per processor
      max_blocks_tropic = 24    !   in each distribution

!EOP
!BOC
!EOC
!***********************************************************************

 end module domain_size

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
