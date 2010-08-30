
 module kinds_mod

!BOP
! !MODULE: kinds_mod
!
! !DESCRIPTION:
!  This module defines default numerical data types for all common data
!  types like integer, character, logical, real4 and real8.
!
! !REVISION HISTORY:
!  CVS:$Id: kinds_mod.F90,v 1.7 2002/12/11 17:28:16 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:
!  uses no other modules

   implicit none
!  private
   save

! !DEFINED PARAMETERS:

   integer, parameter, public ::          &
      char_len  = 100                    ,&
      log_kind  = kind(.true.)           ,&
      int_kind  = kind(1)                ,&
      i4        = selected_int_kind(6)   ,&
      i8        = selected_int_kind(13)  ,&
      r4        = selected_real_kind(6)  ,&
      r8        = selected_real_kind(13)

 end module kinds_mod


 module blocks

   use kinds_mod
!  use exit_mod
!  use domain_size

   implicit none
   private
   save

 ! PUBLIC TYPES:

   type, public :: block   ! block data type
      integer (int_kind) :: &
!        block_id          ,&! global block number
!        local_id          ,&! local address of block in current distrib
!        ib, ie, jb, je    ,&! begin,end indices for physical domain
         iblock, jblock      ! cartesian i,j position for bloc

!     integer (int_kind), dimension(:), pointer :: &
!        i_glob, j_glob     ! global domain location for each point
   end type

!contains

!***********************************************************************
!BOP
! !IROUTINE: create_blocks
! !INTERFACE:

! subroutine create_blocks(nx_global, ny_global, ew_boundary_type, ns_boundary_type)
! end subroutine create_blocks

 end module blocks
