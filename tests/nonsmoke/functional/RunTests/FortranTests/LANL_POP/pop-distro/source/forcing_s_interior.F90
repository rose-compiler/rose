!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module forcing_s_interior

!BOP
! !MODULE: forcing_s_interior
!
! !DESCRIPTION:
!  Contains routines and variables used for determining the
!  interior salinity restoring.
!
! !REVISION HISTORY:
!  CVS:$Id: forcing_s_interior.F90,v 1.10 2003/12/23 22:16:42 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $
!
! !USES:

   use kinds_mod
   use blocks
   use domain
   use constants
   use boundary
   use io
   use forcing_tools
   use time_management
   use prognostic
   use grid
   use exit_mod

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_s_interior,      &
              get_s_interior_data, &
              set_s_interior

! !PUBLIC DATA MEMBERS:

   real (r8), public ::       &! public for use in restart
      s_interior_interp_last   ! time last interpolation done

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  internal module variables
!
!-----------------------------------------------------------------------

   real (r8), dimension(:,:,:,:,:), allocatable :: &
      S_INTERIOR_DATA  ! data to use for interior restoring

   real (r8), dimension(:,:,:), allocatable :: &
      S_RESTORE_RTAU   !  inverse restoring timescale for
                       !  spatially-varying restoring

   integer (int_kind), dimension(:,:,:), allocatable :: &
      S_RESTORE_MAX_LEVEL ! maximum level for applying variable
                          ! interior restoring

   real (r8), dimension(12) :: &
      s_interior_data_time

   real (r8), dimension(20) :: &
      s_interior_data_renorm   ! factors to convert data to model units

   real (r8) ::               &
      s_interior_data_inc,    &! time increment between values of forcing data
      s_interior_data_next,   &! time to be used for the next value of forcing data that is needed
      s_interior_data_update, &! time when new forcing value to be added to interpolation set
      s_interior_interp_inc,  &! time increment between interpolation
      s_interior_interp_next, &! time next interpolation to be done
      s_interior_restore_tau, &! restoring timescale
      s_interior_restore_rtau  ! reciprocal of restoring timescale

   integer (int_kind) ::            &
      s_interior_interp_order,      &! order of temporal interpolation
      s_interior_data_time_min_loc, &! index of x_data_time with the minimum forcing time
      s_interior_restore_max_level

   character (char_len) ::    &
      s_interior_data_type,   &! keyword for period of forcing data
      s_interior_filename,    &! name of file containing forcing data
      s_interior_file_fmt,    &! format (bin or nc) of forcing file
      s_interior_interp_freq, &! keyword for period of temporal interpolation
      s_interior_interp_type, &!
      s_interior_data_label,  &!
      s_interior_formulation, &!
      s_interior_restore_filename, &!
      s_interior_restore_file_fmt   !

   character (char_len), dimension(:), allocatable :: &
      s_interior_data_names    ! names for required input data fields

   integer (int_kind), dimension(:), allocatable :: &
      s_interior_bndy_loc,    &! location and field type for ghost
      s_interior_bndy_type     !    cell updates

   logical (log_kind) :: &
      s_interior_variable_restore

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_s_interior
! !INTERFACE:

 subroutine init_s_interior

! !DESCRIPTION:
!  Initializes salinity interior forcing by either calculating or
!  reading in the 3D salinity.  Also performs initial book-keeping
!  concerning when new data is needed for the temporal interpolation
!  and when the forcing will need to be updated.
!
! !REVISION HISTORY:
!  same as module

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      n,                 &!
      nml_error           ! namelist i/o error flag

   character (char_len) :: &
      forcing_filename,    &! final filename for forcing data
      long_name             ! long name for input data

   type (datafile) :: &
      s_int_data_file  ! data file descriptor for s interior data

   type (io_field_desc) :: &
      s_int_data_in    ! io field descriptor for input salinity data

   type (io_dim) :: &
      i_dim, j_dim, &! dimension descriptors for horiz dimensions
      k_dim          ! dimension descriptor  for vertical levels

   namelist /forcing_s_interior_nml/ s_interior_data_type,            &
        s_interior_data_inc,         s_interior_interp_type,          &
        s_interior_interp_freq,      s_interior_interp_inc,           &
        s_interior_restore_tau,      s_interior_filename,             &
        s_interior_file_fmt,         s_interior_restore_max_level,    &
        s_interior_data_renorm,      s_interior_formulation,          &
        s_interior_variable_restore, s_interior_restore_filename,     &
        s_interior_restore_file_fmt

!-----------------------------------------------------------------------
!
!  read interior S namelist input after setting default values.
!
!-----------------------------------------------------------------------

   s_interior_formulation    = 'restoring'
   s_interior_data_type      = 'none'
   s_interior_data_inc       = 1.e20_r8
   s_interior_interp_type    = 'nearest'
   s_interior_interp_freq    = 'never'
   s_interior_interp_inc     = 1.e20_r8
   s_interior_restore_tau    = 1.e20_r8
   s_interior_filename       = 'unknown-s_interior'
   s_interior_file_fmt       = 'bin'
   s_interior_restore_max_level = 0
   s_interior_data_renorm    = c1
!  s_interior_data_renorm    = 1.e-3_r8  !  convert from psu to g/g
   s_interior_variable_restore  = .false.
   s_interior_restore_filename  = 'unknown-s_interior_restore'
   s_interior_restore_file_fmt  = 'bin'

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old', iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      !*** keep reading until find right namelist
      do while (nml_error > 0)
         read(nml_in, nml=forcing_s_interior_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   end if

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
     call exit_POP(sigAbort,'ERROR: reading forcing_s_interior_nml')
   endif

   call broadcast_scalar(s_interior_formulation,       master_task)
   call broadcast_scalar(s_interior_data_type,         master_task)
   call broadcast_scalar(s_interior_data_inc,          master_task)
   call broadcast_scalar(s_interior_interp_type,       master_task)
   call broadcast_scalar(s_interior_interp_freq,       master_task)
   call broadcast_scalar(s_interior_interp_inc,        master_task)
   call broadcast_scalar(s_interior_restore_tau,       master_task)
   call broadcast_scalar(s_interior_filename,          master_task)
   call broadcast_scalar(s_interior_file_fmt,          master_task)
   call broadcast_scalar(s_interior_restore_max_level, master_task)
   call broadcast_scalar(s_interior_variable_restore,  master_task)
   call broadcast_scalar(s_interior_restore_filename,  master_task)
   call broadcast_scalar(s_interior_restore_file_fmt,  master_task)
   call broadcast_array (s_interior_data_renorm,       master_task)

!-----------------------------------------------------------------------
!
!  convert data_type to 'monthly-calendar' if input is 'monthly'
!
!-----------------------------------------------------------------------

   if (s_interior_data_type == 'monthly') &
       s_interior_data_type = 'monthly-calendar'

!-----------------------------------------------------------------------
!
!  calculate inverse of restoring time scale and convert to seconds.
!
!-----------------------------------------------------------------------

   s_interior_restore_rtau = c1/(seconds_in_day*s_interior_restore_tau)

!-----------------------------------------------------------------------
!
!  convert interp_type to corresponding integer value.
!
!-----------------------------------------------------------------------

   select case (s_interior_interp_type)
   case ('nearest')
      s_interior_interp_order = 1

   case ('linear')
      s_interior_interp_order = 2

   case ('4point')
      s_interior_interp_order = 4

   case default
      call exit_POP(sigAbort, &
         'init_s_interior: Unknown value for s_interior_interp_type')

   end select

!-----------------------------------------------------------------------
!
!  set values of the interior S array (S_INTERIOR_DATA)
!  depending on the type of S data.
!
!-----------------------------------------------------------------------

   select case (s_interior_data_type)

   case ('none')

      !*** no interior forcing, therefore no interpolation in time is
      !*** needed, nor are there any new values to be used.

      s_interior_data_next = never
      s_interior_data_update = never
      s_interior_interp_freq = 'never'

   case ('annual')

      !*** annual mean climatological salinity (read in from a file)
      !*** constant in time, therefore no new values will be needed

      allocate(S_INTERIOR_DATA(nx_block,ny_block,km, &
                               max_blocks_clinic,1))

      allocate(s_interior_data_names(1), &
               s_interior_bndy_loc  (1), &
               s_interior_bndy_type (1))

      S_INTERIOR_DATA = c0
      s_interior_data_names(1) = 'SALINITY'
      s_interior_bndy_loc  (1) = field_loc_center
      s_interior_bndy_type (1) = field_type_scalar

      forcing_filename = s_interior_filename

      s_int_data_file = construct_file(s_interior_file_fmt,            &
                                    full_name=trim(forcing_filename),  &
                                    record_length=rec_type_dbl,        &
                                    recl_words=nx_global*ny_global)

      call data_set(s_int_data_file, 'open_read')

      i_dim = construct_io_dim('i', nx_global)
      j_dim = construct_io_dim('j', ny_global)
      k_dim = construct_io_dim('k', km)

      s_int_data_in = construct_io_field(                      &
                         trim(s_interior_data_names(1)),       &
                         i_dim, j_dim, dim3=k_dim,             &
                         field_loc = s_interior_bndy_loc(1),   &
                         field_type = s_interior_bndy_type(1), &
                         d3d_array = S_INTERIOR_DATA(:,:,:,:,1))

      call data_set (s_int_data_file, 'define', s_int_data_in)
      call data_set (s_int_data_file, 'read',   s_int_data_in)
      call data_set (s_int_data_file, 'close')
      call destroy_io_field(s_int_data_in)
      call destroy_file(s_int_data_file)

      if (s_interior_data_renorm(1) /= c1) &
         S_INTERIOR_DATA = S_INTERIOR_DATA*s_interior_data_renorm(1)

      s_interior_data_next = never
      s_interior_data_update = never
      s_interior_interp_freq = 'never'

      if (my_task == master_task) then
         write(stdout,blank_fmt)
         write(stdout,'(a30,a)') ' Interior S Annual file read: ', &
                                 trim(forcing_filename)
      endif

   case ('monthly-equal','monthly-calendar')

      !*** monthly mean climatological interior salinity
      !*** all 12 months are read from a file. interpolation order
      !*** may be specified with namelist input.

      allocate(S_INTERIOR_DATA(nx_block,ny_block,km,    &
                               max_blocks_clinic,0:12))

      allocate(s_interior_data_names(12), &
               s_interior_bndy_loc  (12), &
               s_interior_bndy_type (12))

      S_INTERIOR_DATA = c0

      call find_forcing_times(           s_interior_data_time,         &
                 s_interior_data_inc,    s_interior_interp_type,       &
                 s_interior_data_next,   s_interior_data_time_min_loc, &
                 s_interior_data_update, s_interior_data_type)

      forcing_filename = s_interior_filename
      s_int_data_file = construct_file(s_interior_file_fmt,            &
                                    full_name=trim(forcing_filename),  &
                                    record_length=rec_type_dbl,        &
                                    recl_words=nx_global*ny_global)

      call data_set(s_int_data_file, 'open_read')

      i_dim = construct_io_dim('i', nx_global)
      j_dim = construct_io_dim('j', ny_global)
      k_dim = construct_io_dim('k', km)

      do n = 1, 12
         write(s_interior_data_names(n),'(a9,i2)') 'SALINITY ',n
         s_interior_bndy_loc (n) = field_loc_center
         s_interior_bndy_type(n) = field_type_scalar

         s_int_data_in = construct_io_field( &
                         trim(s_interior_data_names(n)),       &
                         i_dim, j_dim, dim3=k_dim,             &
                         field_loc = s_interior_bndy_loc(n),   &
                         field_type = s_interior_bndy_type(n), &
                         d3d_array = S_INTERIOR_DATA(:,:,:,:,n))

         call data_set (s_int_data_file, 'define', s_int_data_in)
         call data_set (s_int_data_file, 'read',   s_int_data_in)
         call destroy_io_field(s_int_data_in)
      enddo

      call data_set (s_int_data_file, 'close')
      call destroy_file(s_int_data_file)

      if (s_interior_data_renorm(1) /= c1) &
         S_INTERIOR_DATA = S_INTERIOR_DATA*s_interior_data_renorm(1)

      if (my_task == master_task) then
         write(stdout,blank_fmt)
         write(stdout,'(a31,a)') ' Interior S Monthly file read: ', &
                                 trim(forcing_filename)
      endif

   case ('n-hour')

      !*** interior salinity specified every n-hours, where
      !*** the n-hour increment (s_interior_data_inc) should be
      !*** specified with namelist input. Only as many times as are
      !*** necessary based on the order of the temporal interpolation
      !*** scheme reside in memory at any given time.

      allocate(S_INTERIOR_DATA(nx_block,ny_block,km,max_blocks_clinic, &
                                         0:s_interior_interp_order))

      allocate(s_interior_data_names(1), &
               s_interior_bndy_loc  (1), &
               s_interior_bndy_type (1))

      S_INTERIOR_DATA = c0
      s_interior_data_names(1) = 'SALINITY'
      s_interior_bndy_loc  (1) = field_loc_center
      s_interior_bndy_type (1) = field_type_scalar

      call find_forcing_times(           s_interior_data_time,         &
                 s_interior_data_inc,    s_interior_interp_type,       &
                 s_interior_data_next,   s_interior_data_time_min_loc, &
                 s_interior_data_update, s_interior_data_type)

      do n = 1, s_interior_interp_order
         call get_forcing_filename(forcing_filename,        &
                                   s_interior_filename,     &
                                   s_interior_data_time(n), &
                                   s_interior_data_inc)

         s_int_data_file = construct_file(s_interior_file_fmt,         &
                                    full_name=trim(forcing_filename),  &
                                    record_length=rec_type_dbl,        &
                                    recl_words=nx_global*ny_global)

         call data_set(s_int_data_file, 'open_read')

         i_dim = construct_io_dim('i', nx_global)
         j_dim = construct_io_dim('j', ny_global)
         k_dim = construct_io_dim('k', km)

         s_int_data_in = construct_io_field(                   &
                         trim(s_interior_data_names(1)),       &
                         i_dim, j_dim, dim3=k_dim,             &
                         field_loc = s_interior_bndy_loc(1),   &
                         field_type = s_interior_bndy_type(1), &
                         d3d_array = S_INTERIOR_DATA(:,:,:,:,n))

         call data_set (s_int_data_file, 'define', s_int_data_in)
         call data_set (s_int_data_file, 'read',   s_int_data_in)
         call data_set (s_int_data_file, 'close')
         call destroy_io_field(s_int_data_in)
         call destroy_file(s_int_data_file)

         if (my_task == master_task) then
            write(stdout,blank_fmt)
            write(stdout,'(a30,a)') ' Interior S n-hour file read: ', &
                                    trim(forcing_filename)
         endif
      enddo

      if (s_interior_data_renorm(1) /= c1) &
         S_INTERIOR_DATA = S_INTERIOR_DATA*s_interior_data_renorm(1)

   case default

     call exit_POP(sigAbort, &
              'init_s_interior: Unknown value for s_interior_data_type')

   end select

!-----------------------------------------------------------------------
!
!  now check interpolation period (s_interior_interp_freq) to set the
!    time for the next temporal interpolation (s_interior_interp_next).
!
!  if no interpolation is to be done, set next interpolation time
!    to a large number so the interior S update test in routine
!    get_forcing_data will always be false.
!
!  if interpolation is to be done every n-hours, find the first
!    interpolation time greater than the current time.
!
!  if interpolation is to be done every timestep, set next interpolation
!    time to a large negative number so the interior S update
!    test in routine set_surface_forcing will always be true.
!
!-----------------------------------------------------------------------

   select case (s_interior_interp_freq)

   case ('never')

     s_interior_interp_next = never
     s_interior_interp_last = never
     s_interior_interp_inc  = c0

   case ('n-hour')
     call find_interp_time(s_interior_interp_inc,s_interior_interp_next)

   case ('every-timestep')

     s_interior_interp_next = always
     s_interior_interp_inc  = c0

   case default

     call exit_POP(sigAbort, &
        'init_s_interior: Unknown value for s_interior_interp_freq')

   end select

   if(nsteps_total == 0) s_interior_interp_last = thour00

!-----------------------------------------------------------------------
!
!  allocate and read in arrays used for variable interior restoring
!    if necessary.
!
!-----------------------------------------------------------------------

   if (s_interior_variable_restore) then

      allocate(S_RESTORE_MAX_LEVEL(nx_block,ny_block,max_blocks_clinic), &
                    S_RESTORE_RTAU(nx_block,ny_block,max_blocks_clinic))

      forcing_filename = s_interior_restore_filename

      s_int_data_file = construct_file(s_interior_restore_file_fmt,    &
                                    full_name=trim(forcing_filename),  &
                                    record_length=rec_type_dbl,        &
                                    recl_words=nx_global*ny_global)

      call data_set(s_int_data_file, 'open_read')

      i_dim = construct_io_dim('i', nx_global)
      j_dim = construct_io_dim('j', ny_global)

      s_int_data_in = construct_io_field('S_RESTORE_MAX_LEVEL',  &
                       i_dim, j_dim,                             &
                       field_loc = field_loc_center,             &
                       field_type = field_type_scalar,           &
                       d2d_array = S_RESTORE_RTAU)

      call data_set (s_int_data_file, 'define', s_int_data_in)
      call data_set (s_int_data_file, 'read',   s_int_data_in)
      S_RESTORE_MAX_LEVEL = nint(S_RESTORE_RTAU)
      call destroy_io_field(s_int_data_in)

      s_int_data_in = construct_io_field('S_RESTORE_RTAU',       &
                       i_dim, j_dim,                             &
                       field_loc = field_loc_center,             &
                       field_type = field_type_scalar,           &
                       d2d_array = S_RESTORE_RTAU)

      call data_set (s_int_data_file, 'define', s_int_data_in)
      call data_set (s_int_data_file, 'read',   s_int_data_in)
      S_RESTORE_RTAU = S_RESTORE_RTAU/seconds_in_day ! convert days to secs
      call destroy_io_field(s_int_data_in)
      call data_set (s_int_data_file, 'close')
      call destroy_file(s_int_data_file)

   endif

!-----------------------------------------------------------------------
!
!  echo forcing options to stdout.
!
!-----------------------------------------------------------------------

   s_interior_data_label = 'Interior Salinity Forcing'
   if (s_interior_variable_restore .and. my_task == master_task) &
      write(stdout,"('Variable Interior Salinity Restoring enabled')")
   call echo_forcing_options(s_interior_data_type,                     &
                       s_interior_formulation, s_interior_data_inc,    &
                       s_interior_interp_freq, s_interior_interp_type, &
                       s_interior_interp_inc, s_interior_data_label)

!-----------------------------------------------------------------------
!EOC

 end subroutine init_s_interior

!***********************************************************************
!BOP
! !IROUTINE: get_s_interior_data
! !INTERFACE:

 subroutine get_s_interior_data

! !DESCRIPTION:
!  Determines whether new interior forcing data is required and
!  reads the data if necessary.  Also interpolates data to current
!  time if required.
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

!-----------------------------------------------------------------------
!
!  check if new data is necessary for interpolation.  if yes, then
!    shuffle indices in SSS_RESTORE and s_interior_data_time arrays
!    and read in new data if necessary ('n-hour' case).  also
!    increment values of s_interior_data_time_min_loc,
!    s_interior_data_next and s_interior_data_update. note that no new
!    data is necessary for 'analytic' and 'annual' cases.
!
!-----------------------------------------------------------------------

   select case(s_interior_data_type)

   case ('monthly-equal','monthly-calendar')

      s_interior_data_label = 'S_INTERIOR Monthly'
      if (thour00 >= s_interior_data_update) then
         call update_forcing_data(             s_interior_data_time,   &
                 s_interior_data_time_min_loc, s_interior_interp_type, &
                 s_interior_data_next,         s_interior_data_update, &
                 s_interior_data_type,         s_interior_data_inc,    &
                 S_INTERIOR_DATA(:,:,:,:,1:12),s_interior_data_renorm, &
                 s_interior_data_label,        s_interior_data_names,  &
                 s_interior_bndy_loc,          s_interior_bndy_type,   &
                 s_interior_filename,          s_interior_file_fmt)
      endif

      if (thour00 >= s_interior_interp_next .or. nsteps_run==0) then
         call interpolate_forcing(S_INTERIOR_DATA(:,:,:,:,0),          &
                                  S_INTERIOR_DATA(:,:,:,:,1:12),       &
                 s_interior_data_time,         s_interior_interp_type, &
                 s_interior_data_time_min_loc, s_interior_interp_freq, &
                 s_interior_interp_inc,        s_interior_interp_next, &
                 s_interior_interp_last,       nsteps_run)

         if (nsteps_run /= 0) s_interior_interp_next = &
                              s_interior_interp_next + &
                              s_interior_interp_inc
      endif

   case('n-hour')

      s_interior_data_label = 'S_INTERIOR n-hour'
      if (thour00 >= s_interior_data_update) then
         call update_forcing_data(             s_interior_data_time,   &
                 s_interior_data_time_min_loc, s_interior_interp_type, &
                 s_interior_data_next,         s_interior_data_update, &
                 s_interior_data_type,         s_interior_data_inc,    &
                 S_INTERIOR_DATA(:,:,:,:,1:s_interior_interp_order),   &
                 s_interior_data_renorm,                               &
                 s_interior_data_label,        s_interior_data_names,  &
                 s_interior_bndy_loc,          s_interior_bndy_type,   &
                 s_interior_filename,          s_interior_file_fmt)
      endif

      if (thour00 >= s_interior_interp_next .or. nsteps_run==0) then
         call interpolate_forcing(S_INTERIOR_DATA(:,:,:,:,0),          &
                 S_INTERIOR_DATA(:,:,:,:,1:s_interior_interp_order),   &
                 s_interior_data_time,         s_interior_interp_type, &
                 s_interior_data_time_min_loc, s_interior_interp_freq, &
                 s_interior_interp_inc,        s_interior_interp_next, &
                 s_interior_interp_last,       nsteps_run)

         if (nsteps_run /= 0) s_interior_interp_next = &
                              s_interior_interp_next + &
                              s_interior_interp_inc
      endif

   end select

!-----------------------------------------------------------------------
!EOC

 end subroutine get_s_interior_data

!***********************************************************************
!BOP
! !IROUTINE: set_s_interior
! !INTERFACE:

 subroutine set_s_interior(k,this_block,S_SOURCE)

! !DESCRIPTION:
!  Computes interior salinity forcing term (DS\_INTERIOR) using
!  updated and interpolated data.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
      k     ! vertical level index

   type (block), intent(in) :: &
      this_block   ! block information for this block

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(inout) :: &
      S_SOURCE    ! source terms for salinity forcing - add restoring
                  ! to any other source terms

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) ::  &
      bid,                &! local block address for this block
      now                  ! index for location of interpolated data

   real (r8), dimension(nx_block,ny_block) :: &
      DS_INTERIOR    ! interior salinity restoring term for this block
                     ! and level

!-----------------------------------------------------------------------
!
!  do interior restoring if required (no restoring at surface for any)
!
!-----------------------------------------------------------------------

   if (s_interior_data_type /= 'none' .and. k > 1) then

!-----------------------------------------------------------------------
!
!     set index for location of interpolated data.
!
!-----------------------------------------------------------------------

      select case(s_interior_data_type)

      case('annual')
         now = 1

      case ('monthly-equal','monthly-calendar')
         now = 0

      case('n-hour')
         now = 0

      end select

!-----------------------------------------------------------------------
!
!     now compute restoring
!
!-----------------------------------------------------------------------

      bid = this_block%local_id

      if (s_interior_variable_restore) then
         DS_INTERIOR = S_RESTORE_RTAU(:,:,bid)*                &
                       merge((S_INTERIOR_DATA(:,:,k,bid,now) - &
                              TRACER(:,:,k,2,curtime,bid)),    &
                             c0, k <= S_RESTORE_MAX_LEVEL(:,:,bid))
      else
         if (k <= s_interior_restore_max_level) then
            DS_INTERIOR = s_interior_restore_rtau*         &
                         (S_INTERIOR_DATA(:,:,k,bid,now) - &
                          TRACER(:,:,k,2,curtime,bid))
         else
            DS_INTERIOR = c0
         endif
      endif

      !*** add restoring term to other source terms

      S_SOURCE = S_SOURCE + DS_INTERIOR

   endif ! k=1

!-----------------------------------------------------------------------
!EOC

 end subroutine set_s_interior

!***********************************************************************

 end module forcing_s_interior

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
