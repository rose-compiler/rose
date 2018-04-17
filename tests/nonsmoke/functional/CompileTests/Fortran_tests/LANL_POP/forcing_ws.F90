!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module forcing_ws

!BOP
! !MODULE: forcing_ws
!
! !DESCRIPTION:
!  Contains routines and variables used for determining the
!  surface wind stress.
!
! !REVISION HISTORY:
!  CVS:$Id: forcing_ws.F90,v 1.13 2003/12/23 22:21:17 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $
!
! !USES:

   use kinds_mod
   use constants
   use blocks
   use distribution
   use domain
   use boundary
   use io
   use forcing_tools
   use time_management
   use grid
   use exit_mod

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_ws, &
             set_ws

! !PUBLIC DATA MEMBERS:

   real (r8), public :: &! needed by restart
      ws_interp_last     ! time when last interpolation was done

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  module variables
!
!-----------------------------------------------------------------------

   real (r8), allocatable, dimension(:,:,:,:,:) :: &
      SMF_DATA !  surface momentum flux data at multiple times that
               !  may be interpolated in time to get SMF

   real (r8), dimension(12) :: &
      ws_data_time  ! time (hours) corresponding to surface fluxes
                    !  in SMF_DATA

   real (r8), dimension(20) :: &
      ws_data_renorm  ! factors for converting to model units

   real (r8) ::       &
      ws_data_inc,    &! time increment between values of forcing data
      ws_data_next,   &! time for next value of forcing data needed
      ws_data_update, &! time new forcing value needs to be added to interpolation set
      ws_interp_inc,  &! time increment between interpolation
      ws_interp_next   ! time when next interpolation will be done

   integer (int_kind) ::   &
      ws_interp_order,     &! order of temporal interpolation
      ws_data_time_min_loc  ! index of first time index of SMF_DATA
                            !   to use for interpolating forcing

   character (char_len) :: &
      ws_data_type,    &! keyword for period of the forcing data
      ws_filename,     &! name of file conainting forcing data
      ws_file_fmt,     &! format (bin or nc) for forcing file
      ws_interp_freq,  &! keyword for period of temporal interpolation
      ws_interp_type,  &!
      ws_data_label,   &! name of data to be read
      ws_formulation    ! formulation to use to compute wind stress

   character (char_len), dimension(:), allocatable :: &
      ws_data_names     ! names of each data field

   integer (int_kind), dimension(:), allocatable :: &
      ws_bndy_loc,     &! location and field type for ghost cell
      ws_bndy_type      !   updates

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_ws
! !INTERFACE:

 subroutine init_ws(SMF,SMFT,lsmft_avail)

! !DESCRIPTION:
!  Initializes wind stress forcing by either calculating or reading
!  in the wind stress.  Also performs initial book-keeping concerning
!  when new data is needed for the temporal interpolation and
!  when forcing will need to be updated.
!
! !REVISION HISTORY:
!  same as module

! !OUTPUT PARAMETERS:

   logical (log_kind), intent(out) :: &
      lsmft_avail      ! flag is true if momentum fluxes available
                       ! at T points (to prevent later averaging)

   real (r8), dimension(nx_block,ny_block,2,max_blocks_clinic), &
      intent(out) :: &
      SMF,           &!  surface momentum fluxes at current timestep
      SMFT            !  surface momentum fluxes at T points

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      n,                 &! loop index
      iblock,            &! block index
      nml_error           ! namelist i/o error flag

   character (char_len) :: &
      forcing_filename     ! full name of forcing input file

   namelist /forcing_ws_nml/ ws_data_type,   ws_data_inc,    &
                             ws_interp_type, ws_interp_freq, &
                             ws_interp_inc,  ws_filename,    &
                             ws_file_fmt,    ws_data_renorm

   real (r8), dimension(:,:,:,:), allocatable :: &
      TEMP_DATA        ! temp array for reading monthly data

   type (datafile) :: &
      forcing_file    ! io data file type for input forcing file

   type (io_field_desc) :: &
      io_tau,      &! io field descriptor for input ws (both components)
      io_taux,     &! io field descriptor for input ws in x direction
      io_tauy       ! io field descriptor for input ws in y direction

   type (io_dim) :: &
      i_dim, j_dim, &! dimension descriptors for horiz dims
      month_dim      ! dimension descriptor  for monthly data

!-----------------------------------------------------------------------
!
!  read wind stress namelist input after setting default values.
!
!-----------------------------------------------------------------------

   ws_data_type   = 'analytic'
   ws_data_inc    = 1.e20_r8
   ws_interp_type = 'nearest'
   ws_interp_freq = 'never'
   ws_interp_inc  = 1.e20_r8
   ws_filename    = 'unknown-ws'
   ws_file_fmt    = 'bin'
   ws_data_renorm = c1

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=forcing_ws_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
     call exit_POP(sigAbort,'ERROR reading forcing_ws_nml')
   endif

   call broadcast_scalar(ws_data_type,   master_task)
   call broadcast_scalar(ws_data_inc,    master_task)
   call broadcast_scalar(ws_interp_type, master_task)
   call broadcast_scalar(ws_interp_freq, master_task)
   call broadcast_scalar(ws_interp_inc,  master_task)
   call broadcast_scalar(ws_filename,    master_task)
   call broadcast_scalar(ws_file_fmt,    master_task)
   call broadcast_array (ws_data_renorm, master_task)

   ws_formulation = char_blank

!-----------------------------------------------------------------------
!
!  convert data_type to 'monthly-calendar' if input is 'monthly'
!
!-----------------------------------------------------------------------

   if (ws_data_type == 'monthly') ws_data_type = 'monthly-calendar'

!-----------------------------------------------------------------------
!
!  convert interp_type to corresponding integer value.
!
!-----------------------------------------------------------------------

   select case (ws_interp_type)
   case ('nearest')
      ws_interp_order = 1

   case ('linear')
      ws_interp_order = 2

   case ('4point')
      ws_interp_order = 4

   case default
      call exit_POP(sigAbort, &
                    'init_ws: Unknown value for ws_interp_type')

   end select

!-----------------------------------------------------------------------
!
!  set values of the wind stress arrays (SMF or SMF_DATA)
!  depending on the type of the wind stress data.
!
!-----------------------------------------------------------------------

   select case (ws_data_type)

!-----------------------------------------------------------------------
!
!  no wind stress, therefore no interpolation in time is needed,
!  nor are there any new values to be used.
!
!-----------------------------------------------------------------------

   case ('none')

      SMF = c0
      SMFT = c0
      lsmft_avail = .true.
      ws_data_next = never
      ws_data_update = never
      ws_interp_freq = 'never'

!-----------------------------------------------------------------------
!
!  simple analytic wind stress that is constant in time, therefore no
!  interpolation in time is needed, nor are there any new values
!  to be used.
!
!-----------------------------------------------------------------------

   case ('analytic')

      ws_data_next = never
      ws_data_update = never
      ws_interp_freq = 'never'

      !$OMP PARALLEL DO PRIVATE(iblock)
      do iblock = 1,nblocks_clinic

         ! simple zonal windstress field
         SMF (:,:,1,iblock) = c1 - c2*cos(ULAT(:,:,iblock))**2
         SMFT(:,:,1,iblock) = c1 - c2*cos(TLAT(:,:,iblock))**2

         ! Zero the zonal windstress at points within 1/100th degree 
         ! of the true North Pole.
         ! NOTE this is needed because in tripole grids the
         ! true North Pole lies exactly on a U-point, and at 
         ! that point ANGLE is not defined.
         ! In principle, this could also occur in a dipole grid
         ! if a U-point or T-point is near or very near the North Pole.
         ! This problem should also be dealt with in forcing_coupled.F
         ! and any other routines that use ANGLE and ANGLET at the
         ! North Pole point.
         where(abs(abs(radian*ULAT(:,:,iblock)) - 90.0_r8) < 0.01_r8) &
                   SMF(:,:,1,iblock) = c0
         where(abs(abs(radian*TLAT(:,:,iblock)) - 90.0_r8) < 0.01_r8) &
                   SMFT(:,:,1,iblock) = c0

         ! rotate vectors to grid
         SMF (:,:,2,iblock) = -sin(ANGLE (:,:,iblock))*SMF (:,:,1,iblock)
         SMFT(:,:,2,iblock) = -sin(ANGLET(:,:,iblock))*SMFT(:,:,1,iblock)
         SMF (:,:,1,iblock) =  cos(ANGLE (:,:,iblock))*SMF (:,:,1,iblock)
         SMFT(:,:,1,iblock) =  cos(ANGLET(:,:,iblock))*SMFT(:,:,1,iblock)

         if (ws_data_renorm(1) /= c1) then
            SMF (:,:,:,iblock) = SMF (:,:,:,iblock)*ws_data_renorm(1)
            SMFT(:,:,:,iblock) = SMFT(:,:,:,iblock)*ws_data_renorm(1)
         endif
      end do
      !$OMP END PARALLEL DO

      lsmft_avail = .true.

!-----------------------------------------------------------------------
!
!  annual mean climatological wind stress (read in from a file) that
!  is constant in time, therefore no interpolation in time is needed,
!  nor are there any new values to be used.
!
!-----------------------------------------------------------------------

   case ('annual')

      ws_data_next = never
      ws_data_update = never
      ws_interp_freq = 'never'
      lsmft_avail = .false.

      allocate(TEMP_DATA(nx_block,ny_block,max_blocks_clinic,2))

      allocate(ws_data_names(2), &
               ws_bndy_loc  (2), &
               ws_bndy_type (2))

      ws_data_names(1) = 'TAUX'
      ws_bndy_loc  (1) = field_loc_NEcorner
      ws_bndy_type (1) = field_type_vector
      ws_data_names(2) = 'TAUY'
      ws_bndy_loc  (2) = field_loc_NEcorner
      ws_bndy_type (2) = field_type_vector

      forcing_file = construct_file(ws_file_fmt,                  &
                                    full_name=trim(ws_filename),  &
                                    record_length = rec_type_dbl, &
                                    recl_words=nx_global*ny_global)

      call data_set(forcing_file,'open_read')

      i_dim = construct_io_dim('i',nx_global)
      j_dim = construct_io_dim('j',ny_global)

      io_taux = construct_io_field(trim(ws_data_names(1)), &
                 i_dim, j_dim,                             &
                 field_loc = ws_bndy_loc(1),               &
                 field_type = ws_bndy_type(1),             &
                 d2d_array=TEMP_DATA(:,:,:,1))
      io_tauy = construct_io_field(trim(ws_data_names(2)), &
                 i_dim, j_dim,                             &
                 field_loc = ws_bndy_loc(2),               &
                 field_type = ws_bndy_type(2),             &
                 d2d_array=TEMP_DATA(:,:,:,2))
      call data_set(forcing_file,'define',io_taux)
      call data_set(forcing_file,'define',io_tauy)
      call data_set(forcing_file,'read'  ,io_taux)
      call data_set(forcing_file,'read'  ,io_tauy)
      call destroy_io_field(io_taux)
      call destroy_io_field(io_tauy)
      call data_set(forcing_file,'close')
      call destroy_file(forcing_file)

      !$OMP PARALLEL DO
      do iblock=1,nblocks_clinic
         SMF(:,:,1,iblock) = TEMP_DATA(:,:,iblock,1)
         SMF(:,:,2,iblock) = TEMP_DATA(:,:,iblock,2)

         if (ws_data_renorm(1) /= c1) SMF(:,:,:,iblock) = &
                                      SMF(:,:,:,iblock)*ws_data_renorm(1)
      end do
      !$OMP END PARALLEL DO

      deallocate(TEMP_DATA)

      if (my_task == master_task) then
         write(stdout,blank_fmt)
         write(stdout,'(a22,a)') ' WS Annual file read: ', &
                                 trim(ws_filename)
      endif

!-----------------------------------------------------------------------
!
!  monthly mean climatological wind stress. all 12 months are read
!  in from a file. interpolation order (ws_interp_order) may be
!  specified with namelist input.
!
!-----------------------------------------------------------------------

   case ('monthly-equal','monthly-calendar')

      lsmft_avail = .false.
      allocate( SMF_DATA(nx_block,ny_block,max_blocks_clinic,2,0:12), &
               TEMP_DATA(nx_block,ny_block,12,max_blocks_clinic))

      allocate(ws_data_names(2), &
               ws_bndy_loc  (2), &
               ws_bndy_type (2))

      SMF_DATA = c0
      ws_data_names(1) = 'TAUX'
      ws_bndy_loc  (1) = field_loc_NEcorner
      ws_bndy_type (1) = field_type_vector
      ws_data_names(2) = 'TAUY'
      ws_bndy_loc  (2) = field_loc_NEcorner
      ws_bndy_type (2) = field_type_vector

      call find_forcing_times(ws_data_time,         ws_data_inc,    &
                              ws_interp_type,       ws_data_next,   &
                              ws_data_time_min_loc, ws_data_update, &
                              ws_data_type)

      forcing_file = construct_file(ws_file_fmt,                  &
                                    full_name=trim(ws_filename),  &
                                    record_length = rec_type_dbl, &
                                    recl_words=nx_global*ny_global)

      call data_set(forcing_file,'open_read')

      i_dim = construct_io_dim('i',nx_global)
      j_dim = construct_io_dim('j',ny_global)
      month_dim = construct_io_dim('month',12)

      io_taux = construct_io_field(trim(ws_data_names(1)), &
                 i_dim, j_dim, dim3=month_dim,             &
                 field_loc = ws_bndy_loc(1),               &
                 field_type = ws_bndy_type(1),             &
                 d3d_array=TEMP_DATA)
      io_tauy = construct_io_field(trim(ws_data_names(2)), &
                 i_dim, j_dim, dim3=month_dim,             &
                 field_loc = ws_bndy_loc(2),               &
                 field_type = ws_bndy_type(2),             &
                 d3d_array=TEMP_DATA)

      call data_set(forcing_file,'define',io_taux)
      call data_set(forcing_file,'define',io_tauy)

      call data_set(forcing_file,'read',io_taux)
      !$OMP PARALLEL DO PRIVATE(iblock, n)
      do iblock=1,nblocks_clinic
      do n=1,12
         SMF_DATA(:,:,iblock,1,n) = TEMP_DATA(:,:,n,iblock)
      end do
      end do
      !$OMP END PARALLEL DO
      call destroy_io_field(io_taux)

      call data_set(forcing_file,'read',io_tauy)
      !$OMP PARALLEL DO PRIVATE(iblock, n)
      do iblock=1,nblocks_clinic
      do n=1,12
         SMF_DATA(:,:,iblock,2,n) = TEMP_DATA(:,:,n,iblock)
      end do
      end do
      !$OMP END PARALLEL DO
      call destroy_io_field(io_tauy)

      call data_set(forcing_file,'close')
      call destroy_file(forcing_file)

      deallocate(TEMP_DATA)

      if (ws_data_renorm(1) /= c1) SMF_DATA = SMF_DATA*ws_data_renorm(1)

      if (my_task == master_task) then
         write(stdout,blank_fmt)
         write(stdout,'(a23,a)') ' WS Monthly file read: ', &
                                 trim(ws_filename)
      endif

!-----------------------------------------------------------------------
!
!  wind stress specified every n-hours, where the n-hour increment
!  should be specified with namelist input (ws_data_inc).
!  only as many times as are necessary based on the order
!  of the temporal interpolation scheme (ws_interp_order) reside
!  in memory at any given time.
!
!-----------------------------------------------------------------------

   case ('n-hour')

      lsmft_avail = .false.

      allocate( SMF_DATA(nx_block,ny_block,max_blocks_clinic,2, &
                         0:ws_interp_order))

      allocate(ws_data_names(2), &
               ws_bndy_loc  (2), &
               ws_bndy_type (2))

      SMF_DATA = c0
      ws_data_names(1) = 'TAUX'
      ws_bndy_loc  (1) = field_loc_NEcorner
      ws_bndy_type (1) = field_type_vector
      ws_data_names(2) = 'TAUY'
      ws_bndy_loc  (2) = field_loc_NEcorner
      ws_bndy_type (2) = field_type_vector

      call find_forcing_times(ws_data_time,         ws_data_inc,    &
                              ws_interp_type,       ws_data_next,   &
                              ws_data_time_min_loc, ws_data_update, &
                              ws_data_type)

      do n = 1, ws_interp_order
         call get_forcing_filename(forcing_filename, ws_filename, &
                                   ws_data_time(n), ws_data_inc)

         forcing_file = construct_file(ws_file_fmt,                    &
                                     full_name=trim(forcing_filename), &
                                     record_length = rec_type_dbl,     &
                                     recl_words=nx_global*ny_global)
         call data_set(forcing_file,'open_read')

         i_dim = construct_io_dim('i',nx_global)
         j_dim = construct_io_dim('j',ny_global)

         io_taux = construct_io_field(trim(ws_data_names(1)), &
                    i_dim, j_dim,                             &
                    field_loc = ws_bndy_loc(1),               &
                    field_type = ws_bndy_type(1),             &
                    d2d_array=SMF_DATA(:,:,:,1,n))
         io_tauy = construct_io_field(trim(ws_data_names(2)), &
                    i_dim, j_dim,                             &
                    field_loc = ws_bndy_loc(2),               &
                    field_type = ws_bndy_type(2),             &
                    d2d_array=SMF_DATA(:,:,:,2,n))

         call data_set(forcing_file,'define',io_taux)
         call data_set(forcing_file,'define',io_tauy)
         call data_set(forcing_file,'read'  ,io_taux)
         call data_set(forcing_file,'read'  ,io_tauy)
         call destroy_io_field(io_taux)
         call destroy_io_field(io_tauy)

         call data_set(forcing_file,'close')
         call destroy_file(forcing_file)

         if (my_task == master_task) then
            write(stdout,blank_fmt)
            write(stdout,'(a22,a)') ' WS n-hour file read: ', &
                                    trim(forcing_filename)
         endif
      enddo

      if (ws_data_renorm(1) /= c1) SMF_DATA = SMF_DATA*ws_data_renorm(1)

   case default

     call exit_POP(sigAbort,'init_ws: Unknown value for ws_data_type')

   end select

!-----------------------------------------------------------------------
!
!  now check interpolation period (ws_interp_freq) to set the
!    time for the next temporal interpolation (ws_interp_next).
!
!  if no interpolation is to be done, set next interpolation time
!    to a large number so the wind stress update test in routine
!    set_surface_forcing will always be false.
!
!  if interpolation is to be done every n-hours, find the first
!    interpolation time greater than the current time.
!
!  if interpolation is to be done every timestep, set next interpolation
!    time to a large negative number so the wind stress update test in
!    routine set_surface_forcing will always be true.
!
!-----------------------------------------------------------------------

   select case (ws_interp_freq)

   case ('never')

      ws_interp_next = never
      ws_interp_last = never
      ws_interp_inc  = c0

   case ('n-hour')

      call find_interp_time(ws_interp_inc, ws_interp_next)

   case ('every-timestep')

      ws_interp_next = always
      ws_interp_inc  = c0

   case default

      call exit_POP(sigAbort, &
                    'init_ws: Unknown value for ws_interp_freq')

   end select

   if (nsteps_total == 0) ws_interp_last = thour00

!-----------------------------------------------------------------------
!
!  echo forcing options to stdout.
!
!-----------------------------------------------------------------------

   ws_data_label = 'Surface Wind Stress'
   call echo_forcing_options(ws_data_type,                   &
                             ws_formulation, ws_data_inc,    &
                             ws_interp_freq, ws_interp_type, &
                             ws_interp_inc,  ws_data_label)

!-----------------------------------------------------------------------
!EOC

 end subroutine init_ws

!***********************************************************************
!BOP
! !IROUTINE: set_ws
! !INTERFACE:

 subroutine set_ws(SMF,SMFT)

! !DESCRIPTION:
!  Update the current value of the wind stress arrays (SMF) by
!  interpolating to the current time.  It may also be necessary to
!  use new data values than were used in the previous interpolation.
!
! !REVISION HISTORY:
!  same as module

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,2,max_blocks_clinic), &
      intent(inout) :: &
      SMF    !  surface momentum fluxes at current timestep

   real (r8), dimension(nx_block,ny_block,2,max_blocks_clinic), &
      intent(inout), optional :: &
      SMFT   !  surface momentum fluxes at T points if available

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      iblock,            &
      first, second, third, fourth, &
      ws_data_time_max_loc

!-----------------------------------------------------------------------
!
!  check if new data is necessary for interpolation.  if yes, then
!  shuffle indices in SMF_DATA, and ws_data_time arrays
!  and read in new data if necessary ('n-hour' case).  also
!  increment values of ws_data_time_min_loc, ws_data_next and
!  ws_data_update.then do the temporal interpolation.
!
!-----------------------------------------------------------------------

   if (thour00 >= ws_interp_next .or. nsteps_run == 0) then

   select case(ws_data_type)

   case ('monthly-equal','monthly-calendar')

      ws_data_label = 'WS Monthly'
      if (thour00 >= ws_data_update) &
         call update_forcing_data(ws_data_time,   ws_data_time_min_loc,&
                                  ws_interp_type, ws_data_next,        &
                                  ws_data_update, ws_data_type,        &
                                  ws_data_inc, SMF_DATA(:,:,:,:,1:12), &
                                  ws_data_renorm,                      &
                                  ws_data_label,  ws_data_names,       &
                                  ws_bndy_loc,    ws_bndy_type,        &
                                  ws_filename,    ws_file_fmt)

      call interpolate_forcing(SMF_DATA(:,:,:,:,0),                  &
                               SMF_DATA(:,:,:,:,1:12),               &
                               ws_data_time, ws_interp_type,         &
                               ws_data_time_min_loc, ws_interp_freq, &
                               ws_interp_inc, ws_interp_next,        &
                               ws_interp_last, nsteps_run)

      !$OMP PARALLEL DO
      do iblock=1,nblocks_clinic
         SMF(:,:,1,iblock) = SMF_DATA(:,:,iblock,1,0)
         SMF(:,:,2,iblock) = SMF_DATA(:,:,iblock,2,0)
      end do
      !$OMP END PARALLEL DO

   case ('n-hour')

      ws_data_label = 'WS n-hour'
      if (thour00 >= ws_data_update) &
         call update_forcing_data(ws_data_time,   ws_data_time_min_loc,&
                                  ws_interp_type, ws_data_next,        &
                                  ws_data_update, ws_data_type,        &
                                  ws_data_inc,                         &
                                  SMF_DATA(:,:,:,:,1:ws_interp_order), &
                                  ws_data_renorm,                      &
                                  ws_data_label,  ws_data_names,       &
                                  ws_bndy_loc,    ws_bndy_type,        &
                                  ws_filename,    ws_file_fmt)

      call interpolate_forcing(SMF_DATA(:,:,:,:,0),                  &
                               SMF_DATA(:,:,:,:,1:ws_interp_order),  &
                               ws_data_time, ws_interp_type,         &
                               ws_data_time_min_loc, ws_interp_freq, &
                               ws_interp_inc, ws_interp_next,        &
                               ws_interp_last, nsteps_run)

      !$OMP PARALLEL DO
      do iblock=1,nblocks_clinic
         SMF(:,:,1,iblock) = SMF_DATA(:,:,iblock,1,0)
         SMF(:,:,2,iblock) = SMF_DATA(:,:,iblock,2,0)
      end do
      !$OMP END PARALLEL DO

   end select

   if (nsteps_run /= 0) ws_interp_next = ws_interp_next + &
                                         ws_interp_inc

   endif ! thour00 > interp_next

!-----------------------------------------------------------------------
!EOC

 end subroutine set_ws

!***********************************************************************

 end module forcing_ws

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
