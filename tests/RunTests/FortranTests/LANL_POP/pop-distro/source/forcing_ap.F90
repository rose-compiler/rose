!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module forcing_ap

!BOP
! !MODULE: forcing_ap
!
! !DESCRIPTION:
!  Contains routines and variables used for determining the
!  surface atmopheric pressure for using atmospheric pressure
!  forcing at the surface.
!
! !REVISION HISTORY:
!  CVS:$Id: forcing_ap.F90,v 1.12 2002/12/02 13:45:09 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $
!
! !USES:

   use kinds_mod
   use domain
   use boundary
   use constants
   use broadcast
   use io
   use forcing_tools
   use time_management
   use grid
   use exit_mod

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public:: init_ap,     &
            set_ap

! !PUBLIC DATA MEMBERS:

   real (r8), public :: & ! public for use in restart
      ap_interp_last      ! time when last interpolation was done

   character (char_len), public :: &! needed in barotropic
      ap_data_type     !  keyword for the period of the forcing data

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  module private variables
!
!-----------------------------------------------------------------------

   real (r8), dimension(:,:,:,:,:), allocatable :: &
      ATM_PRESS_DATA !  surface atm pressure at multiple times that
                     !  may be interpolated in time to get ATM_PRESS

   real (r8), dimension(12) :: &
      ap_data_time  !  time (in hours) corresponding to pressure
                    !  in ATM_PRESS_DATA

   real (r8), dimension(20) :: &
      ap_data_renorm   ! scale factors for changing input units
                       ! to model units

   real (r8) ::       &
      ap_data_inc,    &! time increment between values of forcing data
      ap_data_next,   &! time to be used for next forcing data
      ap_data_update, &! time when new forcing value added to interp set
      ap_interp_inc,  &! time increment between interpolation
      ap_interp_next   ! time when next interpolation will be done

   integer (int_kind) ::   &
      ap_interp_order,     &! order of temporal interpolation
      ap_data_time_min_loc  ! index of 3rd dimension of ATM_PRESS_DATA,
                            !   containing the minimum forcing time

   character (char_len) :: &
      ap_filename,    &!  name of file conainting forcing data
      ap_file_fmt,    &!  data format of forcing file (bin or nc)
      ap_interp_freq, &!  keyword for period of temporal interpolation
      ap_interp_type, &
      ap_data_label,  &
      ap_formulation

   character (char_len), dimension(:), allocatable :: &
      ap_data_names    ! field names for getting ap data

   integer (int_kind), dimension(:), allocatable :: &
      ap_bndy_loc,    &! location and field type for ghost cell
      ap_bndy_type     !   updates

   type (datafile) :: &
      ap_data_file    ! io file type for ap data file

   type (io_field_desc) :: &
      ap_data_in      ! io field type for reading ap data

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_ap
! !INTERFACE:

 subroutine init_ap(ATM_PRESS)

! !DESCRIPTION:
!  Initializes atm pressure forcing by either calculating or reading
!  in the atm pressure.  Also does initial book-keeping concerning
!  when new data is needed for the temporal interpolation and
!  when the forcing will need to be updated.
!
! !REVISION HISTORY:
!  same as module

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), &
      intent(inout) :: &
      ATM_PRESS        !  surface momentum fluxes at current timestep

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) ::     &
      n, iblock, nu,         &! dummy loop index and unit number
      nml_error               ! namelist i/o error flag

   character (char_len) ::   &
      forcing_filename        ! name of file containing forcing data

   type (block) :: &
      this_block    ! block information for current block

   type (io_dim) :: &
      i_dim, j_dim, &! dimension descriptor for horiz dims
      month_dim      ! dimension descriptor for monthly fields

   real (r8), dimension(nx_block,ny_block) :: &
      WORK

   real (r8), dimension(:,:,:,:), allocatable :: &
      TEMP_DATA   ! temp array for reading monthly data

   namelist /forcing_ap_nml/ ap_data_type,   ap_data_inc,    &
                             ap_interp_type, ap_interp_freq, &
                             ap_interp_inc,  ap_filename,    &
                             ap_data_renorm, ap_file_fmt

!-----------------------------------------------------------------------
!
!  read atmospheric pressure namelist input after setting
!  default values.
!
!-----------------------------------------------------------------------

   ap_data_type   = 'none'
   ap_data_inc    = 1.e20_r8
   ap_interp_type = 'nearest'
   ap_interp_freq = 'never'
   ap_interp_inc  = 1.e20_r8
   ap_filename    = 'unknown-ap'
   ap_file_fmt    = 'bin'
   ap_data_renorm = c1
!  ap_data_renorm = c10  ! convert from Pa to dynes/cm**2

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old', iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      !*** keep reading until find right namelist
      do while (nml_error > 0)
         read(nml_in, nml=forcing_ap_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   end if

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading forcing_ap namelist')
   endif

   call broadcast_scalar(ap_data_type,   master_task)
   call broadcast_scalar(ap_data_inc,    master_task)
   call broadcast_scalar(ap_interp_type, master_task)
   call broadcast_scalar(ap_interp_freq, master_task)
   call broadcast_scalar(ap_interp_inc,  master_task)
   call broadcast_scalar(ap_filename,    master_task)
   call broadcast_scalar(ap_file_fmt,    master_task)
   call broadcast_array (ap_data_renorm, master_task)

   ap_formulation = char_blank

!-----------------------------------------------------------------------
!
!  convert data_type to 'monthly-calendar' if input is 'monthly'
!
!-----------------------------------------------------------------------

   if (ap_data_type == 'monthly') ap_data_type = 'monthly-calendar'

!-----------------------------------------------------------------------
!
!  convert interp_type to corresponding integer value.
!
!-----------------------------------------------------------------------

   select case (ap_interp_type)

   case ('nearest')
      ap_interp_order = 1

   case ('linear')
      ap_interp_order = 2

   case ('4point')
      ap_interp_order = 4

   case default
      call exit_POP(sigAbort, &
                    'init_ap: Unknown value for ap_interp_type')

   end select

!-----------------------------------------------------------------------
!
!  set values of atm pressure arrays (ATM_PRESS or ATM_PRESS_DATA)
!  depending on the type of the atm pressure data.
!
!-----------------------------------------------------------------------

   select case (ap_data_type)

   case ('none')

      !***  no atm pressure, therefore no interpolation in time needed
      !***  (ap_interp_freq = 'none'), nor are there any new values
      !***  to be used (ap_data_next = ap_data_update = never).

      ATM_PRESS = c0
      ap_data_next = never
      ap_data_update = never
      ap_interp_freq = 'never'

   case ('analytic')

      !*** simple analytic atm pressure that is constant in time,
      !*** therefore no interpolation in time is needed
      !*** (ap_interp_freq = 'none'), nor are there any new values
      !*** to be used (ap_data_next = ap_data_update = never).

      !$OMP PARALLEL DO PRIVATE(iblock,this_block,WORK)

      do iblock=1,nblocks_clinic
         this_block = get_block(blocks_clinic(iblock),iblock)

         WORK = (ULAT(:,:,iblock)*radian -  25.0_r8)**2 + &
                (ULON(:,:,iblock)*radian - 180.0_r8)**2

         where (CALCT(:,:,iblock) .and. WORK < 100.0_r8**2)
            ATM_PRESS(:,:,iblock) = grav*exp(-WORK*p5/15.0_r8**2)
            ATM_PRESS(:,:,iblock) = WORK - &
                   sum(WORK*TAREA(:,:,iblock),mask=CALCT(:,:,iblock))/ &
                   sum(     TAREA(:,:,iblock),mask=CALCT(:,:,iblock))
         elsewhere
            ATM_PRESS(:,:,iblock) = c0
         end where

         if (ap_data_renorm(1) /= c1) ATM_PRESS(:,:,iblock) = &
                                      ATM_PRESS(:,:,iblock)*  &
                                      ap_data_renorm(1)

      end do
      !$OMP END PARALLEL DO

      ap_data_next = never
      ap_data_update = never
      ap_interp_freq = 'never'

   case ('annual')

      !*** annual mean climatological atm pressure (read in from a file)
      !*** constant in time, therefore no interpolation in time needed
      !*** (ap_interp_freq = 'none'), nor are there any new values
      !***  to be used (ap_data_next = ap_data_update = never).

      allocate(ap_data_names(1), &
               ap_bndy_loc  (1), &
               ap_bndy_type (1))
      ap_data_names(1) = 'ATMOSPHERIC PRESSURE'
      ap_bndy_loc  (1) = field_loc_center
      ap_bndy_type (1) = field_type_scalar

      ap_data_file = construct_file(ap_file_fmt,                 &
                                    full_name=trim(ap_filename), &
                                    record_length=rec_type_dbl,  &
                                    recl_words=nx_global*ny_global)

      call data_set(ap_data_file, 'open_read')

      i_dim = construct_io_dim('i',nx_global)
      j_dim = construct_io_dim('j',ny_global)

      ap_data_in = construct_io_field(trim(ap_data_names(1)),  &
                     i_dim, j_dim,                             &
                     field_loc = ap_bndy_loc(1),               &
                     field_type = ap_bndy_type(1),             &
                     d2d_array = ATM_PRESS)

      call data_set (ap_data_file, 'define', ap_data_in)
      call data_set (ap_data_file, 'read',   ap_data_in)
      call data_set (ap_data_file, 'close')
      call destroy_io_field(ap_data_in)
      call destroy_file(ap_data_file)

      if (ap_data_renorm(1) /= c1) ATM_PRESS = &
                                   ATM_PRESS*ap_data_renorm(1)

      ap_data_next = never
      ap_data_update = never
      ap_interp_freq = 'never'

      if (my_task == master_task) then
         write(stdout,blank_fmt)
         write(stdout,'(a22,a)') ' AP Annual file read: ', &
                                 trim(forcing_filename)
      endif

   case ('monthly-equal','monthly-calendar')

      !*** monthly mean climatological atm pressure. all 12 months
      !*** are read in from a file. interpolation order
      !*** (ap_interp_order) may be specified with namelist input.

      allocate(ATM_PRESS_DATA(nx_block,ny_block,max_blocks_clinic, &
                                                          1,0:12), &
               TEMP_DATA(nx_block,ny_block,12,max_blocks_clinic))

      allocate(ap_data_names(1), &
               ap_bndy_loc  (1), &
               ap_bndy_type (1))

      ATM_PRESS_DATA = c0
      ap_data_names(1) = 'ATMOSPHERIC PRESSURE'
      ap_bndy_loc  (1) = field_loc_center
      ap_bndy_type (1) = field_type_scalar

      call find_forcing_times(ap_data_time, ap_data_inc,            &
                              ap_interp_type, ap_data_next,         &
                              ap_data_time_min_loc, ap_data_update, &
                              ap_data_type)

      ap_data_file = construct_file(ap_file_fmt,                 &
                                    full_name=trim(ap_filename), &
                                    record_length=rec_type_dbl,  &
                                    recl_words=nx_global*ny_global)

      call data_set(ap_data_file, 'open_read')

      i_dim = construct_io_dim('i',nx_global)
      j_dim = construct_io_dim('j',ny_global)
      month_dim = construct_io_dim('month',12)

      ap_data_in = construct_io_field(trim(ap_data_names(1)),  &
                     i_dim, j_dim, dim3=month_dim,             &
                     field_loc = ap_bndy_loc(1),               &
                     field_type = ap_bndy_type(1),             &
                     d3d_array = TEMP_DATA)

      call data_set (ap_data_file, 'define', ap_data_in)
      call data_set (ap_data_file, 'read',   ap_data_in)
      call destroy_io_field(ap_data_in)

      call data_set (ap_data_file, 'close')
      call destroy_file(ap_data_file)

      !$OMP PARALLEL DO PRIVATE(iblock,n)
      do iblock=1,nblocks_clinic
      do n=1,12
         ATM_PRESS_DATA(:,:,iblock,1,n) = TEMP_DATA(:,:,n,iblock)

         if (ap_data_renorm(1) /= c1) &
            ATM_PRESS_DATA(:,:,iblock,1,n) = &
            ATM_PRESS_DATA(:,:,iblock,1,n)*ap_data_renorm(1)
      end do
      end do
      !$OMP END PARALLEL DO

      deallocate(TEMP_DATA)

      if (my_task == master_task) then
         write(stdout,blank_fmt)
         write(stdout,'(a23,a)') ' Monthly AP file read: ', &
                                 trim(forcing_filename)
      endif

   case ('n-hour')

      !*** atm pressure specified every n-hours, where the n-hour
      !*** increment should be specified with namelist input
      !*** (ap_data_inc).  Only as many times as are necessary based
      !*** on the order of the temporal interpolation scheme
      !*** (ap_interp_order) reside in memory at any given time.

      allocate (ATM_PRESS_DATA(nx_block,ny_block,max_blocks_clinic, &
                                              1,0:ap_interp_order))
      allocate(ap_data_names(1), &
               ap_bndy_loc  (1), &
               ap_bndy_type (1))

      ATM_PRESS_DATA = c0
      ap_data_names(1) = 'ATMOSPHERIC PRESSURE'
      ap_bndy_loc  (1) = field_loc_center
      ap_bndy_type (1) = field_type_scalar

      call find_forcing_times(ap_data_time,         ap_data_inc,    &
                              ap_interp_type,       ap_data_next,   &
                              ap_data_time_min_loc, ap_data_update, &
                              ap_data_type)

      do n = 1, ap_interp_order
         call get_forcing_filename(forcing_filename, ap_filename, &
                                   ap_data_time(n), ap_data_inc)

         ap_data_file = construct_file(ap_file_fmt,                 &
                                  full_name=trim(forcing_filename), &
                                  record_length=rec_type_dbl,       &
                                  recl_words=nx_global*ny_global)

         call data_set(ap_data_file, 'open_read')

         i_dim = construct_io_dim('i',nx_global)
         j_dim = construct_io_dim('j',ny_global)

         ap_data_in = construct_io_field(trim(ap_data_names(1)),  &
                        i_dim, j_dim,                             &
                        field_loc = ap_bndy_loc(1),               &
                        field_type = ap_bndy_type(1),             &
                        d2d_array = ATM_PRESS_DATA(:,:,:,1,n))

         call data_set (ap_data_file, 'define', ap_data_in)
         call data_set (ap_data_file, 'read',   ap_data_in)
         call data_set (ap_data_file, 'close')
         call destroy_io_field(ap_data_in)
         call destroy_file(ap_data_file)

         if (my_task == master_task) then
            write(stdout,blank_fmt)
            write(stdout,*) ' AP n-hour file read: ', forcing_filename
         endif
      enddo

      if (ap_data_renorm(1) /= c1) ATM_PRESS_DATA = &
                                   ATM_PRESS_DATA*ap_data_renorm(1)

   case default

      call exit_POP(sigAbort,'init_ap: Unknown value for ap_data_type')

   end select

!-----------------------------------------------------------------------
!
!  now check interpolation period (ap_interp_freq) to set the
!    time for the next temporal interpolation (ap_interp_next).
!
!  if no interpolation is to be done, set next interpolation time
!    to a large number so the atm pressure update test in routine
!    set_surface_forcing will always be false.
!
!  if interpolation is to be done every n-hours, find the first
!    interpolation time greater than the current time.
!
!  if interpolation is to be done every timestep, set next interpolation
!    time to a large negative number so the atm pressure update test in
!    routine set_surface_forcing will always be true.
!
!-----------------------------------------------------------------------

   select case (ap_interp_freq)

   case ('never')

      ap_interp_next = never
      ap_interp_last = never
      ap_interp_inc  = c0

   case ('n-hour')

      call find_interp_time(ap_interp_inc, ap_interp_next)

   case ('every-timestep')

      ap_interp_next = always
      ap_interp_inc  = c0

   case default

      call exit_POP(sigAbort, &
                    'init_ap: Unknown value for ap_interp_freq')

   end select

   if (nsteps_total == 0) ap_interp_last = thour00

!-----------------------------------------------------------------------
!
!  echo forcing options to stdout.
!
!-----------------------------------------------------------------------

   ap_data_label = 'Surface Atmospheric Pressure'
   call echo_forcing_options(ap_data_type,   ap_formulation,  &
                             ap_data_inc,    ap_interp_freq,  &
                             ap_interp_type, ap_interp_inc,   &
                             ap_data_label)

!-----------------------------------------------------------------------
!EOC

 end subroutine init_ap

!***********************************************************************
!BOP
! !IROUTINE: set_ap
! !INTERFACE:

 subroutine set_ap(ATM_PRESS)

! !DESCRIPTION:
!  Updates the current value of the atm pressure array (ATM\_PRESS) by
!  interpolating to the current time.  It may also be necessary to
!  use (read) new data values in the interpolation.
!
! !REVISION HISTORY:
!  same as module

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), &
      intent(inout) :: &
      ATM_PRESS        !  atmospheric pressure at current timestep

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

!-----------------------------------------------------------------------
!
!  check if new data is necessary for interpolation.  if yes, then
!  shuffle indices in ATM_PRESS_DATA and ap_data_time arrays.
!  and read in new data if necessary ('n-hour' case).  also
!  increment values of ap_data_time_min_loc, ap_data_next and
!  ap_data_update.  then do the temporal interpolation.
!
!-----------------------------------------------------------------------

   if (thour00 >= ap_interp_next .or. nsteps_run == 0) then

   select case(ap_data_type)

   case ('monthly-equal','monthly-calendar')

      ap_data_label = 'AP Monthly'
      if (thour00 >= ap_data_update)                                  &
         call update_forcing_data(ap_data_time, ap_data_time_min_loc, &
                                  ap_interp_type, ap_data_next,       &
                                  ap_data_update, ap_data_type,       &
                                  ap_data_inc,                        &
                                  ATM_PRESS_DATA(:,:,:,:,1:12),       &
                                  ap_data_renorm,                     &
                                  ap_data_label, ap_data_names,       &
                                  ap_bndy_loc, ap_bndy_type,          &
                                  ap_filename, ap_file_fmt)

      call interpolate_forcing(ATM_PRESS_DATA(:,:,:,:,0),          &
                               ATM_PRESS_DATA(:,:,:,:,1:12),       &
                               ap_data_time, ap_interp_type,       &
                               ap_data_time_min_loc,               &
                               ap_interp_freq, ap_interp_inc,      &
                               ap_interp_next, ap_interp_last,     &
                               nsteps_run)

      ATM_PRESS = ATM_PRESS_DATA(:,:,:,1,0)

   case ('n-hour')

      ap_data_label = 'AP n-hour'
      if (thour00 >= ap_data_update)                                  &
         call update_forcing_data(ap_data_time, ap_data_time_min_loc, &
                                  ap_interp_type, ap_data_next,       &
                                  ap_data_update, ap_data_type,       &
                                  ap_data_inc,                        &
                           ATM_PRESS_DATA(:,:,:,:,1:ap_interp_order), &
                                  ap_data_renorm,                     &
                                  ap_data_label, ap_data_names,       &
                                  ap_bndy_loc, ap_bndy_type,          &
                                  ap_filename, ap_file_fmt)


      call interpolate_forcing(ATM_PRESS_DATA(:,:,:,:,0),             &
                           ATM_PRESS_DATA(:,:,:,:,1:ap_interp_order), &
                               ap_data_time, ap_interp_type,          &
                               ap_data_time_min_loc,                  &
                               ap_interp_freq, ap_interp_inc,         &
                               ap_interp_next, ap_interp_last,        &
                               nsteps_run)

      ATM_PRESS = ATM_PRESS_DATA(:,:,:,1,0)

   end select

   if (nsteps_run /= 0) ap_interp_next = ap_interp_next + ap_interp_inc

   endif ! thour00 > ap_interp_next

!-----------------------------------------------------------------------
!EOC

 end subroutine set_ap

!***********************************************************************

 end module forcing_ap

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
