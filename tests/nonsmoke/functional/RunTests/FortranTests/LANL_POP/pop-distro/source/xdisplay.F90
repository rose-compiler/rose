!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module xdisplay

!BOP
! !MODULE: xdisplay
!
! !DESCRIPTION:
!  This is a dummy version of an unsupported module containing 
!  routines for opening an xdisplay and viewing a field while 
!  the simulation is running.  The actual sort-of-working version
!  relies on an unsupported library called fix (fortran interface 
!  to X).
!
!  The purpose of this dummy version is to allow the makefiles 
!  to be portable to systems which may not have X windows.  If 
!  you have X windows on your system and you want to try this, 
!  then overwrite this file, xdisplay.F, with 
!
!    cp ../input_templates/xdisplay.F90.unsupported ./xdisplay.F90
!
!  and do 
!
!    cp ../input_templates/fix_64.C.unsupported ./fix_64.C
!
! !REVISION HISTORY:
!  CVS:$Id: xdisplay.F90,v 1.10 2003/03/07 17:46:45 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod, only: log_kind, int_kind, r8
   use blocks, only: nx_block, ny_block, block
   use domain_size, only:
   use domain, only:
   use constants, only: delim_fmt, blank_fmt, c1, c0
   use communicate, only: my_task, master_task
   use io_types, only: nml_in, nml_filename, stdin, stdout
   use broadcast, only: broadcast_scalar
   use prognostic, only: max_blocks_clinic
   use grid, only: 
   use exit_mod, only: sigAbort, exit_pop

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_xdisplay, &
             clear_display, &
             display

! !PUBLIC DATA MEMBERS:

   logical (log_kind), public :: &
      lxdisplay             ! logical flag to turn on xdisplay

   integer (int_kind), public :: &
      nstep_xdisplay        ! display image every nstep timesteps

!EOP
!BOC
!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_xdisplay
! !INTERFACE:

 subroutine init_xdisplay

! !DESCRIPTION:
!  This routine initializes an xwindow for displaying a 2-d field
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

   namelist /xdisplay_nml/lxdisplay, nstep_xdisplay

   integer (int_kind) :: & 
      nml_error            ! namelist i/o error flag

!-----------------------------------------------------------------------
!
!  read input namelist to see if xdisplay required
!
!-----------------------------------------------------------------------

   lxdisplay = .false.
   nstep_xdisplay = 1

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=xdisplay_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading xdisplay_nml')
   endif

   if (my_task == master_task) then
      write(stdout,delim_fmt)
      write(stdout,blank_fmt)
      write(stdout,'(a17)') ' Xdisplay options'
      write(stdout,blank_fmt)
      write(stdout,delim_fmt)

      if (lxdisplay) then
         write(stdout,'(a17)') ' Xdisplay enabled'
         write(stdout,'(a23,i6,a7)') ' Display changes every ', &
                                     nstep_xdisplay, ' steps.'
      else
         write(stdout,'(a18)') ' Xdisplay disabled'
      endif
   endif

   call broadcast_scalar(lxdisplay, master_task)

!-----------------------------------------------------------------------
!
!  if xdisplay requested, exit with error
!
!-----------------------------------------------------------------------

   if (lxdisplay) call exit_POP(sigAbort, &
        'X display requested when only dummy routines linked')

!-----------------------------------------------------------------------
!EOC

 end subroutine init_xdisplay

!***********************************************************************
!BOP
! !IROUTINE: clear_display
! !INTERFACE:

 subroutine clear_display

! !DESCRIPTION:
!  Clears the display and closes the X window
!
! !REVISION HISTORY:
!  same as module

!EOP
!BOC
!-----------------------------------------------------------------------

   call exit_POP(sigAbort, &
                 'X display requested when only dummy routines linked')

!-----------------------------------------------------------------------
!EOC

 end subroutine clear_display

!***********************************************************************
!BOP
! !IROUTINE: display
! !INTERFACE:

 subroutine display(FIELD1, field1_loc, FIELD2, field2_loc)

! !DESCRIPTION:
!  Computes image from two input fields and sends it to the
!  FIX routine for display. 
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), &
      intent(in) :: &
      FIELD1, FIELD2       ! two fields to stack and display

   integer (int_kind), intent(in) :: &
      field1_loc, field2_loc  ! grid locations for each field
!EOP
!BOC
!-----------------------------------------------------------------------

   call exit_POP(sigAbort, &
                 'X display requested when only dummy routines linked')

!-----------------------------------------------------------------------
!EOC

 end subroutine display

!***********************************************************************

 end module xdisplay

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
