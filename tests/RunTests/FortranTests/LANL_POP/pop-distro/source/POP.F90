!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
!BOP
! !ROUTINE: POP
! !INTERFACE:

 program POP

! !DESCRIPTION:
!  This is the main driver for the Parallel Ocean Program (POP).
!
! !REVISION HISTORY:
!  CVS:$Id: POP.F90,v 1.8 2003/01/28 23:21:19 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod, only: int_kind, r8
   use communicate, only: my_task, master_task
!   use constants, only: 
   use domain, only: distrb_clinic
   use timers, only: timer_print_all, get_timer, timer_start, timer_stop
   use time_management, only: init_time_flag, check_time_flag, sigAbort,    &
       nsteps_run, stdout, sigExit, exit_pop, set_time_flag
   use step_mod, only: step
   use initial, only: initialize_pop
   use xdisplay, only: lxdisplay, clear_display
   use diagnostics, only: check_KE
   use output, only: output_driver
!   use exit_mod, only: 
   use solvers, only: solv_sum_iters
!   use io, only: 

   implicit none

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      timer_total,       &! timer number for total time
      timer_step,        &! timer number for step
      ierr,              &! error flag
      fstop_now,         &! flag id for stop_now flag
      nscan

!SIGCATCH   integer (int_kind) :: &
!SIGCATCH      sigterm = 15,      &! system kill signal
!SIGCATCH      sigxcpu = 24,      &! system signal for little CPU time left
!SIGCATCH      flag    = -1        ! flag needed by system handling
!SIGCATCH
!SIGCATCH   integer, external :: set_term  ! routine which determines action
!SIGCATCH                                  ! when kill signal received
!SIGCATCH   integer, external :: signal    ! signal catching routine

!-----------------------------------------------------------------------
!
!     call system routine to catch kill signals and route them to the
!     routine set_term to be processed 
!     signal 15 is terminate
!     signal 24 is almost out of cpu time
!     this implementation is system dependent so not guaranteed to 
!      work on any machine
!
!-----------------------------------------------------------------------

!SIGCATCH   ierr = signal (sigterm, set_term, flag)
!SIGCATCH   ierr = signal (sigxcpu, set_term, flag)

!-----------------------------------------------------------------------
!
!  initialize the model run
!
!-----------------------------------------------------------------------

   call initialize_POP

   fstop_now = init_time_flag('stop_now')
   nscan = 0

!-----------------------------------------------------------------------
!
!  start up the main timer
!
!-----------------------------------------------------------------------

   call get_timer(timer_total,'TOTAL',1,distrb_clinic%nprocs)
   call timer_start(timer_total)

   call get_timer(timer_step,'STEP',1,distrb_clinic%nprocs)

!-----------------------------------------------------------------------
!
!  advance the model in time
!
!-----------------------------------------------------------------------

   do while (.not. check_time_flag(fstop_now))

      call timer_start(timer_step)
      call step
      call timer_stop(timer_step)

      nscan = nscan + solv_sum_iters

      !***
      !*** exit if energy is blowing
      !***

      if (check_KE(100.0_r8)) then
         call set_time_flag(fstop_now,.true.)
         call output_driver
         call exit_POP(sigAbort,'ERROR: k.e. > 100 ')
      endif

!-----------------------------------------------------------------------
!
!     write restart dumps and archiving
!
!-----------------------------------------------------------------------

      call output_driver

   enddo

!-----------------------------------------------------------------------
!
!  write an end restart if we are through the stepping loop 
!  without an error
!
!-----------------------------------------------------------------------

   nscan = nscan/nsteps_run
   if (my_task == master_task) & 
      write(stdout,*) ' average # scans =', nscan

!-----------------------------------------------------------------------
!
!  print timing information and clean up various environments if 
!  they have been used
!
!-----------------------------------------------------------------------

   call timer_stop(timer_total)
   call timer_print_all(stats=.true.)

   if (lxdisplay) call clear_display
   call exit_POP(sigExit,'Successful completion of POP run')

!-----------------------------------------------------------------------
!EOC

 end program POP

!***********************************************************************
!SIGCATCH!BOP
!SIGCATCH! !ROUTINE: set_term
!SIGCATCH! !INTERFACE:
!SIGCATCH
!SIGCATCH subroutine set_term (theSignal)
!SIGCATCH
!SIGCATCH! !DESCRIPTION:
!SIGCATCH!  This routine is used by the system signal catching routine to
!SIGCATCH!  set stop_now to true when SIGTERM (15) or (24) is received
!SIGCATCH!  from the operating system.  The main code then usually has
!SIGCATCH!  a few minutes to check this flag, write a restart dump and
!SIGCATCH!  terminate gracefully. This routine is system dependent so
!SIGCATCH!  can only be used by manually uncommenting code.
!SIGCATCH!
!SIGCATCH! !REVISION HISTORY:
!SIGCATCH!  unsupported
!SIGCATCH
!SIGCATCH! !USES:
!SIGCATCH
!SIGCATCH   use kinds_mod
!SIGCATCH   use domain
!SIGCATCH   use communicate
!SIGCATCH   use time_management
!SIGCATCH   use io
!SIGCATCH
!SIGCATCH   implicit none
!SIGCATCH 
!SIGCATCH! !INPUT PARAMETERS: 
!SIGCATCH 
!SIGCATCH   integer (int_kind), intent(in) :: & 
!SIGCATCH      theSignal   ! signal caught by system routine
!SIGCATCH
!SIGCATCH!EOP
!SIGCATCH!BOC
!SIGCATCH!----------------------------------------------------------------------
!SIGCATCH
!SIGCATCH   select case (theSignal)
!SIGCATCH   case(15)   ! kill signal (SIGTERM)
!SIGCATCH
!SIGCATCH      call set_time_flag(stop_now,.true.)
!SIGCATCH      write (stdout,*)  'CAUGHT SIGNAL 15 (KILL) FROM SYSTEM'
!SIGCATCH      write (stdout,*)  '  TASK = ',my_task
!SIGCATCH 
!SIGCATCH   case(24)   ! out of CPU time (system dependent)
!SIGCATCH
!SIGCATCH      call set_time_flag(stop_now,.true.)
!SIGCATCH      write (stdout,*)  'CAUGHT SIGNAL 24 (SIGXCPU) FROM SYSTEM'
!SIGCATCH      write (stdout,*)  '  TASK = ',my_task
!SIGCATCH 
!SIGCATCH   end select
!SIGCATCH
!SIGCATCH!----------------------------------------------------------------------
!SIGCATCH!EOC
!SIGCATCH
!SIGCATCH end subroutine set_term

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
