!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module prognostic

!BOP
! !MODULE: prognostic
! !DESCRIPTION:
!  This module contains all the prognostic variables used in POP.
!
! !REVISION HISTORY:
!  CVS:$Id: prognostic.F90,v 1.5 2003/12/23 22:11:40 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod
   use blocks
   use domain_size
   use domain
   use constants

   implicit none
   public
   save

! !PUBLIC DATA TYPES:

   type, public :: tracer_field
      character(char_len) :: short_name
      character(char_len) :: long_name
      character(char_len) :: units
   end type

! !PUBLIC DATA MEMBERS:

   real (r8), dimension(nx_block,ny_block,km,nt,3,max_blocks_clinic), &
      target :: &
      TRACER     ! 3d tracer fields for all blocks at 3 time levels

   type (tracer_field), dimension(nt) :: &
      tracer_d   ! descriptors for each tracer

   real (r8), dimension(nx_block,ny_block,km,3,max_blocks_clinic), &
      target :: &
      UVEL,     &! 3d horizontal velocity for all blocks at 3 time lvls
      VVEL,     &! 3d horizontal velocity for all blocks at 3 time lvls
      RHO        ! 3d density fields,     for all blocks at 3 time lvls

   real (r8), dimension(nx_block,ny_block,3,max_blocks_clinic), &
      target :: &
      PSURF,    &! surface pressure for all blocks at 3 time levels
      GRADPX,   &! surface-pressure gradient for all blocks at
      GRADPY,   &!   3 time levels
      UBTROP,   &! barotropic velocities for all blocks at
      VBTROP     !   3 time levels 

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), &
      target :: &
      PGUESS     ! next guess for surface pressure

   integer (int_kind) :: &! time indices for prognostic arrays
      curtime,           &! current time level  (n) 
      newtime,           &! next time level     (n+1)
      oldtime,           &! previous time level (n-1)
      mixtime             ! set to oldtime on leafrog steps
                          ! and to curtime on matsuno steps

!EOP
!BOC
!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_prognostic
! !INTERFACE:

 subroutine init_prognostic

! !DESCRIPTION:
!  This subroutine allocates prognostic arrays and intializes all
!  prognostic arrays to zero.
!
! !REVISION HISTORY:
!  same as module

!EOP
!BOC
!-----------------------------------------------------------------------
!
!     initialize prognostic arrays to zero - they will be filled
!     later with real values from restart or initialization.
!     initialize time indices.
!
!-----------------------------------------------------------------------

      oldtime = 1
      curtime = 2
      newtime = 3

      TRACER  = c0
      UVEL    = c0
      VVEL    = c0
      RHO     = c0
      PSURF   = c0
      GRADPX  = c0
      GRADPY  = c0
      UBTROP  = c0
      VBTROP  = c0
      PGUESS  = c0

      tracer_d(1)%short_name = 'TEMP'
      tracer_d(1)%long_name  = 'Potential temperature'
      tracer_d(1)%units      = 'deg C'

      tracer_d(2)%short_name = 'SALT'
      tracer_d(2)%long_name  = 'Salinity'
      tracer_d(2)%units      = 'msu (g/g)'

!-----------------------------------------------------------------------
!EOC

 end subroutine init_prognostic

!***********************************************************************

 end module prognostic

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
