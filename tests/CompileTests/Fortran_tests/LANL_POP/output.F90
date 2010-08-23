!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module output

!BOP
! !MODULE: output
! !DESCRIPTION:
!  Contains necessary routines, variables for large model output
!  files - restart, history, movies, drifter, time average files.
!  This module is primarily a driver for the individual output
!  modules.
!
! !REVISION HISTORY:
!  CVS:$Id: output.F90,v 1.3 2003/12/23 22:24:12 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $
!
! !USES:

   use kinds_mod, only: char_len
!   use domain, only: 
!   use constants, only: 
!   use time_management, only: 
   use restart, only: write_restart, init_restart
   use history, only: write_history, init_history
   use movie, only: write_movie, init_movie
   use tavg, only: write_tavg, init_tavg

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: output_driver, &
             init_output

!EOP
!BOC
!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: output_driver
! !INTERFACE:

   subroutine output_driver

! !DESCRIPTION:
!  This is the main driver routine for all large model output routines.
!
! !REVISION HISTORY:
!  same as module

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   character (char_len) :: &
      restart_type          ! type of restart being written - used
                            ! to pass restart info to tavg routines

!-----------------------------------------------------------------------
!
!  write history, movie files - the decision when to write
!  is internal to each routine  
!  write these first so that if I/O fails, no restart is written
!
!-----------------------------------------------------------------------

   call write_history
   call write_movie

!-----------------------------------------------------------------------
!
!  check for restart and write restart if required
!
!-----------------------------------------------------------------------

   call write_restart(restart_type)

!-----------------------------------------------------------------------
!
!  write tavg - the decision when to write
!  is internal to routine except for notifying tavg that a 
!  restart must be written
!
!-----------------------------------------------------------------------

   call write_tavg(restart_type)

!-----------------------------------------------------------------------
!EOC

 end subroutine output_driver

!***********************************************************************
!BOP
! !IROUTINE: init_output
! !INTERFACE:

 subroutine init_output

! !DESCRIPTION:
!  Initializes frequency of output and filenames for
!  various files by calling individual initialization routines
!
! !REVISION HISTORY:
!  same as module

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  call individual init routines
!
!-----------------------------------------------------------------------

   call init_restart
   call init_history
   call init_movie
   call init_tavg

!-----------------------------------------------------------------------
!EOC

 end subroutine init_output

!***********************************************************************

 end module output

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
