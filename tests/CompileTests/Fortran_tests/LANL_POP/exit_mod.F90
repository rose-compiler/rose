!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module exit_mod

!BOP
! !MODULE: exit_mod
!
! !DESCRIPTION:
!  This module provides a means for a graceful exit from POP when
!  encountering an error.  it contains only the routine exit\_POP
!
! !REVISION HISTORY:
!  CVS:$Id: exit_mod.F90,v 1.5 2002/06/22 13:36:15 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod
! DQ (8/4/2010): I can't find the communicate.F90 file.
   use communicate
   use constants

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: exit_POP

! !DEFINED PARAMETERS:

   integer (int_kind), parameter, public :: &
      sigExit  =  0,    &! signal for normal exit
      sigAbort = -1      ! signal for aborting (exit due to error)

!EOP
!BOC
!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: exit_POP
! !INTERFACE:

 subroutine exit_POP(exit_mode, exit_message)

! !DESCRIPTION:
!  This routine prints a message, exits any message environment
!  and cleans up before stopping

! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
     exit_mode    ! method for exiting (normal exit or abort)

   character (*), intent(in) :: &
     exit_message ! message to print before stopping

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: ierr  ! error flag

!-----------------------------------------------------------------------
!
!  print message - must use unit 6 in place of stdout to
!  prevent circular dependence with io module
!
!-----------------------------------------------------------------------

#if 1
! DQ (8/4/2010): I can't find the communicate.F90 file.
   if (my_task == master_task) then
      write (6,delim_fmt)
      write (6,blank_fmt)

      select case(exit_mode)
      case(sigExit)
         write (6,'(a14)') 'POP exiting...'
      case(sigAbort)
         write (6,'(a15)') 'POP aborting...'
      case default
         write (6,'(a37)') 'POP exiting with unknown exit mode...'
      end select

      write (6,*) exit_message
      write (6,blank_fmt)
      write (6,delim_fmt)
   endif
#endif

!-----------------------------------------------------------------------
!
!  exit or abort the message-passing environment if required
!
!-----------------------------------------------------------------------

   select case(exit_mode)
   case(sigExit)
      call exit_message_environment(ierr)
   case(sigAbort)
      call abort_message_environment(ierr)
   case default
   end select

!-----------------------------------------------------------------------
!
!  now we can stop
!
!-----------------------------------------------------------------------

   stop

!-----------------------------------------------------------------------
!EOC

 end subroutine exit_POP

!***********************************************************************

 end module exit_mod

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
