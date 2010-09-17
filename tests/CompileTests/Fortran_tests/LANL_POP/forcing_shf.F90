!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module forcing_shf

!BOP
! !MODULE: forcing_shf
! !DESCRIPTION:
!  Contains routines and variables used for determining the surface
!  heat flux.
!
! !REVISION HISTORY:
!  CVS:$Id: forcing_shf.F90,v 1.17 2003/01/28 23:21:19 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $
!
! !USES:

   use kinds_mod
   use blocks
   use distribution
   use domain
   use constants
   use boundary
   use io
   use grid
   use forcing_tools
   use time_management
   use prognostic
   use exit_mod

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_shf,      &
             set_shf,       &
             add_sw_absorb, &
             sw_absorb_frac

! !PUBLIC DATA MEMBERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), &
      public, target :: &
      SHF_QSW

   logical (log_kind), public :: &
      lsw_absorb    ! true if short wave available as separate flux
                    !  (use penetrative short wave)

   real (r8), dimension(:), allocatable, public :: &
      sw_absorb     ! shortwave absorption factor at each level

   !*** the following must be shared with sfwf forcing in
   !*** bulk-NCEP option

   real (r8), allocatable, dimension(:,:,:,:), public :: &
      SHF_COMP

   real (r8), allocatable, dimension(:,:,:), public :: &
      OCN_WGT

   integer (int_kind), allocatable, dimension(:,:,:), public :: &
      MASK_SR    ! strong restoring mask for marginal seas

   integer (int_kind), public :: &
      shf_data_tair,     &
      shf_data_qair,     &
      shf_data_cldfrac,  &
      shf_data_windspd,  &
      shf_comp_qsw,      &
      shf_comp_qlw,      &
      shf_comp_qlat,     &
      shf_comp_qsens,    &
      shf_comp_wrest,    &
      shf_comp_srest

   !*** the following are needed by restart

   real (r8), public :: &
      shf_interp_last   ! time when last interpolation was done

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  module variables
!
!-----------------------------------------------------------------------

   real (r8), allocatable, dimension(:,:,:,:,:) :: &
      SHF_DATA        ! forcing data to use for computing SHF

   real (r8), dimension(12) :: &
      shf_data_time  !  time (hours) corresponding to surface heat fluxes

   real (r8), dimension(20) :: &
      shf_data_renorm          ! factors for converting to model units

   real (r8) ::          &
      shf_data_inc,      &! time increment between values of forcing data
      shf_data_next,     &! time that will be used for the next value of forcing data that is needed
      shf_data_update,   &! time when the a new forcing value needs to be added to interpolation set
      shf_interp_inc,    &! time increment between interpolation
      shf_interp_next,   &! time when next interpolation will be done
      shf_restore_tau,   &
      shf_restore_rtau,  &
      shf_weak_restore,  &! heat flux weak restoring max time scale
      shf_strong_restore  ! heat flux strong restoring max time scale

   integer (int_kind) ::     &
      shf_interp_order,      &!  order of temporal interpolation
      shf_data_time_min_loc, &!  time index for first shf_data point
      shf_data_num_fields,   &!
      shf_num_comps

   character (char_len), dimension(:), allocatable :: &
      shf_data_names          ! short names for input data fields

   integer (int_kind), dimension(:), allocatable :: &
      shf_bndy_loc,          &! location and field type for ghost
      shf_bndy_type           !    cell updates

!  the following is necessary for sst restoring
   integer (int_kind) :: &
      shf_data_sst

!  the following are necessary for Barnier-restoring
   integer (int_kind) :: &
      shf_data_tstar,    &
      shf_data_tau,      &
      shf_data_ice,      &
      shf_data_qsw

   character (char_len) :: &
      shf_data_type,   &! keyword for period of forcing data
      shf_interp_freq, &! keyword for period of temporal interpolation
      shf_filename,    &! file containing forcing data
      shf_file_fmt,    &! format (bin or netcdf) of shf file
      shf_interp_type, &
      shf_data_label,  &
      shf_formulation

!-----------------------------------------------------------------------
!
!  the following is necessary for penetrative shortwave used with
!  Barnier-restoring, bulk-NCEP and KPP vertical mixing
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      jerlov_water_type

!-----------------------------------------------------------------------
!
!  the following are needed for long-wave heat flux
!  with bulk-NCEP forcing
!
!-----------------------------------------------------------------------

   real (r8), allocatable, dimension (:,:,:) :: &
      CCINT

   real (r8), dimension(21) :: &
      cc = (/ 0.88_r8, 0.84_r8, 0.80_r8, &
              0.76_r8, 0.72_r8, 0.68_r8, &
              0.63_r8, 0.59_r8, 0.52_r8, &
              0.50_r8, 0.50_r8, 0.50_r8, &
              0.52_r8, 0.59_r8, 0.63_r8, &
              0.68_r8, 0.72_r8, 0.76_r8, &
              0.80_r8, 0.84_r8, 0.88_r8 /)

   real (r8), dimension(21) :: &
     clat = (/ -90.0_r8, -80.0_r8, -70.0_r8, &
               -60.0_r8, -50.0_r8, -40.0_r8, &
               -30.0_r8, -20.0_r8, -10.0_r8, &
                -5.0_r8,   0.0_r8,   5.0_r8, &
                10.0_r8,  20.0_r8,  30.0_r8, &
                40.0_r8,  50.0_r8,  60.0_r8, &
                70.0_r8,  80.0_r8,  90.0_r8 /)

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_shf
! !INTERFACE:

   subroutine init_shf(STF)

! !DESCRIPTION:
!  Initializes surface heat flux forcing by either calculating
!  or reading in the surface heat flux.  Also do initial
!  book-keeping concerning when new data is needed for the temporal
!  interpolation and when the forcing will need to be updated.
!
! !REVISION HISTORY:
!  same as module

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,nt,max_blocks_clinic), &
      intent(out) :: &
      STF    ! surface tracer flux - this routine only modifies
             ! the slice corresponding to temperature (tracer 1)

!EOP
!BOC
!----------------------------------------------------------------------
!
!  local variables
!
!----------------------------------------------------------------------

   integer (int_kind) ::     &
      i,j, k, n, iblock,     &! loop indices
      nml_error               ! namelist error flag

   character (char_len) :: &
      forcing_filename     ! temp for full filename of forcing file

   logical (log_kind) :: &
      no_region_mask     ! flag for existence of region mask

   real (r8), dimension(:,:,:,:,:), allocatable :: &
      TEMP_DATA     ! temporary data array for monthly forcing

   type (datafile) :: &
      forcing_file     ! file containing forcing fields

   type (io_field_desc) :: &  ! io descriptors for various input fields
      io_sst,      &
      io_tstar,    &
      io_tau,      &
      io_ice,      &
      io_qsw,      &
      io_tair,     &
      io_qair,     &
      io_cldfrac,  &
      io_windspd

   type (io_dim) :: &
      i_dim, j_dim, &! dimension descriptors for horiz dims
      month_dim      ! dimension descriptor  for monthly data

   namelist /forcing_shf_nml/ shf_data_type, shf_data_inc,         &
                              shf_interp_type, shf_interp_freq,    &
                              shf_interp_inc, shf_restore_tau,     &
                              shf_filename, shf_file_fmt,          &
                              shf_data_renorm,                     &
                              shf_formulation, jerlov_water_type,  &
                              shf_weak_restore, shf_strong_restore

!-----------------------------------------------------------------------
!
!  read surface heat flux namelist input after setting default values.
!
!-----------------------------------------------------------------------

   shf_formulation      = 'restoring'
   shf_data_type        = 'analytic'
   shf_data_inc         = 1.e20_r8
   shf_interp_type      = 'nearest'
   shf_interp_freq      = 'never'
   shf_interp_inc       = 1.e20_r8
   shf_restore_tau      = 1.e20_r8
   shf_filename         = 'unknown-shf'
   shf_file_fmt         = 'bin'
   shf_data_renorm      = c1
   jerlov_water_type    = 3
   shf_weak_restore     = c0
   shf_strong_restore   = 92.64_r8

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=forcing_shf_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading forcing_shf_nml')
   endif

   call broadcast_scalar(shf_formulation,       master_task)
   call broadcast_scalar(shf_data_type,         master_task)
   call broadcast_scalar(shf_data_inc,          master_task)
   call broadcast_scalar(shf_interp_type,       master_task)
   call broadcast_scalar(shf_interp_freq,       master_task)
   call broadcast_scalar(shf_interp_inc,        master_task)
   call broadcast_scalar(shf_restore_tau,       master_task)
   call broadcast_scalar(shf_filename,          master_task)
   call broadcast_scalar(shf_file_fmt,          master_task)
   call broadcast_scalar(jerlov_water_type,     master_task)
   call broadcast_array (shf_data_renorm,       master_task)
   call broadcast_scalar(shf_weak_restore,      master_task)
   call broadcast_scalar(shf_strong_restore,    master_task)

!-----------------------------------------------------------------------
!
!  convert data_type to 'monthly-calendar' if input is 'monthly'
!
!-----------------------------------------------------------------------

   if (shf_data_type == 'monthly') shf_data_type = 'monthly-calendar'

!-----------------------------------------------------------------------
!
!  set values based on shf_formulation
!
!-----------------------------------------------------------------------

   select case (shf_formulation)
   case ('restoring')
      lsw_absorb = .false.
      shf_data_num_fields = 1
      allocate(shf_data_names(shf_data_num_fields), &
               shf_bndy_loc  (shf_data_num_fields), &
               shf_bndy_type (shf_data_num_fields))
      shf_data_sst        = 1
      shf_data_names(shf_data_sst) = 'SST'
      shf_bndy_loc  (shf_data_sst) = field_loc_center
      shf_bndy_type (shf_data_sst) = field_type_scalar

   case ('Barnier-restoring')
      lsw_absorb = .true.
      shf_data_num_fields = 4
      allocate(shf_data_names(shf_data_num_fields), &
               shf_bndy_loc  (shf_data_num_fields), &
               shf_bndy_type (shf_data_num_fields))
      shf_data_tstar      = 1
      shf_data_tau        = 2
      shf_data_ice        = 3
      shf_data_qsw        = 4
      shf_data_names(shf_data_tstar) = 'TSTAR'
      shf_bndy_loc  (shf_data_tstar) = field_loc_center
      shf_bndy_type (shf_data_tstar) = field_type_scalar
      shf_data_names(shf_data_tau) = 'TAU'
      shf_bndy_loc  (shf_data_tau) = field_loc_center
      shf_bndy_type (shf_data_tau) = field_type_scalar
      shf_data_names(shf_data_ice) = 'ICE'
      shf_bndy_loc  (shf_data_ice) = field_loc_center
      shf_bndy_type (shf_data_ice) = field_type_scalar
      shf_data_names(shf_data_qsw) = 'QSW'
      shf_bndy_loc  (shf_data_qsw) = field_loc_center
      shf_bndy_type (shf_data_qsw) = field_type_scalar

   case ('bulk-NCEP')
      lsw_absorb = .true.
      shf_data_num_fields = 6
      allocate(shf_data_names(shf_data_num_fields), &
               shf_bndy_loc  (shf_data_num_fields), &
               shf_bndy_type (shf_data_num_fields))
      shf_data_sst        = 1
      shf_data_tair       = 2
      shf_data_qair       = 3
      shf_data_qsw        = 4
      shf_data_cldfrac    = 5
      shf_data_windspd    = 6
      shf_data_names(shf_data_sst)     = 'SST'
      shf_bndy_loc  (shf_data_sst)     = field_loc_center
      shf_bndy_type (shf_data_sst)     = field_type_scalar
      shf_data_names(shf_data_tair)    = 'TAIR'
      shf_bndy_loc  (shf_data_tair)    = field_loc_center
      shf_bndy_type (shf_data_tair)    = field_type_scalar
      shf_data_names(shf_data_qair)    = 'QAIR'
      shf_bndy_loc  (shf_data_qair)    = field_loc_center
      shf_bndy_type (shf_data_qair)    = field_type_scalar
      shf_data_names(shf_data_qsw)     = 'QSW'
      shf_bndy_loc  (shf_data_qsw)     = field_loc_center
      shf_bndy_type (shf_data_qsw)     = field_type_scalar
      shf_data_names(shf_data_cldfrac) = 'CLDFRAC'
      shf_bndy_loc  (shf_data_cldfrac) = field_loc_center
      shf_bndy_type (shf_data_cldfrac) = field_type_scalar
      shf_data_names(shf_data_windspd) = 'WINDSPD'
      shf_bndy_loc  (shf_data_windspd) = field_loc_center
      shf_bndy_type (shf_data_windspd) = field_type_scalar

      shf_num_comps   = 6
      shf_comp_qsw    = 1
      shf_comp_qlw    = 2
      shf_comp_qlat   = 3
      shf_comp_qsens  = 4
      shf_comp_wrest  = 5
      shf_comp_srest  = 6

      !***  initialize CCINT (cloud factor used in long-wave heat flux
      !***  with bulk-NCEP forcing).

      allocate(CCINT(nx_block,ny_block,max_blocks_clinic))

      !$OMP PARALLEL DO PRIVATE(iblock,i,j)
      do iblock=1,nblocks_clinic
         do j=1,ny_block
            do i=1,20
               where ((TLAT(:,j,iblock)*radian >  clat(i  )) .and. &
                      (TLAT(:,j,iblock)*radian <= clat(i+1)))
                  CCINT(:,j,iblock) = cc(i) + (cc(i+1)-cc(i))* &
                      (TLAT(:,j,iblock)*radian - clat(i))/ &
                                      (clat(i+1)-clat(i))
               endwhere
            end do ! i
         end do ! j
      end do ! block loop
      !$OMP END PARALLEL DO

   case default
      call exit_POP(sigAbort, &
                    'init_shf: Unknown value for shf_formulation')

   end select

!-----------------------------------------------------------------------
!
!  calculate inverse of restoring time scale and convert to seconds.
!
!-----------------------------------------------------------------------

   shf_restore_tau  = seconds_in_day*shf_restore_tau
   shf_restore_rtau = c1/shf_restore_tau

!-----------------------------------------------------------------------
!
!  define shortwave solar absorption model if necessary.
!  also, initialize SHF_QSW in case a value is needed but not
!  supplied by data:  for example, with KPP and restoring.
!
!-----------------------------------------------------------------------

   allocate(sw_absorb(0:km))
   sw_absorb(0) = c1
   sw_absorb(km) = c0
   do k = 1, km-1
! DQ (9/12/2010): Commented out because this is an error in ROSE.
!      call sw_absorb_frac(zw(k),sw_absorb(k))
   enddo

   SHF_QSW = c0

!-----------------------------------------------------------------------
!
!  set strong restoring mask to 0 only at ocean points that are
!  marginal seas.
!
!-----------------------------------------------------------------------

   if (allocated(REGION_MASK)) then
      allocate( MASK_SR(nx_block,ny_block,max_blocks_clinic))
      no_region_mask = .false.
      !$OMP PARALLEL DO PRIVATE(iblock)
      do iblock=1,nblocks_clinic
         MASK_SR(:,:,iblock) = merge(0, 1, &
                               REGION_MASK(:,:,iblock) < 0)
      end do
      !$OMP END PARALLEL DO
   
   else
      no_region_mask = .true.
   endif
  
!-----------------------------------------------------------------------
!
!  convert interp_type to corresponding integer value.
!
!-----------------------------------------------------------------------

   select case (shf_interp_type)
   case ('nearest')
      shf_interp_order = 1

   case ('linear')
      shf_interp_order = 2

   case ('4point')
      shf_interp_order = 4

   case default
      call exit_POP(sigAbort, &
                    'init_shf: Unknown value for shf_interp_type')

   end select

!-----------------------------------------------------------------------
!
!  set values of the surface heat flux arrays (STF or SHF_DATA)
!  depending on the type of the surface heat flux data.
!
!-----------------------------------------------------------------------

   select case (shf_data_type)

!-----------------------------------------------------------------------
!
!  no surface heat flux, therefore no interpolation in time
!  needed, nor are there any new values to be used.
!
!-----------------------------------------------------------------------

   case ('none')

      STF(:,:,1,:) = c0
      shf_data_next   = never
      shf_data_update = never
      shf_interp_freq = 'never'

!-----------------------------------------------------------------------
!
!  simple analytic surface temperature that is constant in
!  time, therefore no new values will be needed.
!
!-----------------------------------------------------------------------

   case ('analytic')

      allocate( SHF_DATA(nx_block,ny_block,max_blocks_clinic, &
                         shf_data_num_fields,1))

      !$OMP PARALLEL DO PRIVATE(iblock)
      do iblock=1,nblocks_clinic
         select case (shf_formulation)
         case ('restoring')
            SHF_DATA(:,:,iblock,shf_data_sst,1) = &
                28.0_r8*(c1 - sin(ULAT(:,:,iblock)))

         end select
      end do ! block loop
      !$OMP END PARALLEL DO

      shf_data_next = never
      shf_data_update = never
      shf_interp_freq = 'never'

!-----------------------------------------------------------------------
!
!  annual mean climatological surface temperature (read in from file)
!  that is constant in time, therefore no new values will be needed
!  (shf_data_next = shf_data_update = never).
!
!-----------------------------------------------------------------------

   case ('annual')

      allocate( SHF_DATA(nx_block,ny_block,max_blocks_clinic,  &
                         shf_data_num_fields,1))

      SHF_DATA = c0

      forcing_file = construct_file(shf_file_fmt,                 &
                                    full_name=trim(shf_filename), &
                                    record_length = rec_type_dbl, &
                                    recl_words=nx_global*ny_global)

      call data_set(forcing_file,'open_read')

      i_dim = construct_io_dim('i',nx_global)
      j_dim = construct_io_dim('j',ny_global)

      select case (shf_formulation)
      case ('restoring')

         io_sst = construct_io_field( &
                      trim(shf_data_names(shf_data_sst)),       &
                      i_dim, j_dim,                             &
                      field_loc = shf_bndy_loc(shf_data_sst),   &
                      field_type = shf_bndy_type(shf_data_sst), &
                      d2d_array=SHF_DATA(:,:,:,shf_data_sst,1))
         call data_set(forcing_file,'define',io_sst)
         call data_set(forcing_file,'read'  ,io_sst)
         call destroy_io_field(io_sst)

      case ('Barnier-restoring')

         io_tstar = construct_io_field( &
                    trim(shf_data_names(shf_data_tstar)),       &
                    i_dim, j_dim,                               &
                    field_loc = shf_bndy_loc(shf_data_tstar),   &
                    field_type = shf_bndy_type(shf_data_tstar), &
                    d2d_array=SHF_DATA(:,:,:,shf_data_tstar,1))
         io_tau   = construct_io_field( &
                    trim(shf_data_names(shf_data_tau)),       &
                    i_dim, j_dim,                             &
                    field_loc = shf_bndy_loc(shf_data_tau),   &
                    field_type = shf_bndy_type(shf_data_tau), &
                    d2d_array=SHF_DATA(:,:,:,shf_data_tau,1))
         io_ice   = construct_io_field( &
                    trim(shf_data_names(shf_data_ice)),       &
                    i_dim, j_dim,                             &
                    field_loc = shf_bndy_loc(shf_data_ice),   &
                    field_type = shf_bndy_type(shf_data_ice), &
                    d2d_array=SHF_DATA(:,:,:,shf_data_ice,1))
         io_qsw   = construct_io_field( &
                    trim(shf_data_names(shf_data_qsw)),       &
                    i_dim, j_dim,                             &
                    field_loc = shf_bndy_loc(shf_data_qsw),   &
                    field_type = shf_bndy_type(shf_data_qsw), &
                    d2d_array=SHF_DATA(:,:,:,shf_data_qsw,1))

         call data_set(forcing_file,'define',io_tstar)
         call data_set(forcing_file,'define',io_tau)
         call data_set(forcing_file,'define',io_ice)
         call data_set(forcing_file,'define',io_qsw)

         call data_set(forcing_file,'read',io_tstar)
         call data_set(forcing_file,'read',io_tau)
         call data_set(forcing_file,'read',io_ice)
         call data_set(forcing_file,'read',io_qsw)

         call destroy_io_field(io_tstar)
         call destroy_io_field(io_tau)
         call destroy_io_field(io_ice)
         call destroy_io_field(io_qsw)

         SHF_DATA(:,:,:,shf_data_tau,1) = seconds_in_day* &
         SHF_DATA(:,:,:,shf_data_tau,1)

      case ('bulk-NCEP')

         io_sst     = construct_io_field(                       &
                      trim(shf_data_names(shf_data_sst)),       &
                      i_dim, j_dim,                             &
                      field_loc = shf_bndy_loc(shf_data_sst),   &
                      field_type = shf_bndy_type(shf_data_sst), &
                      d2d_array=SHF_DATA(:,:,:,shf_data_sst,1))
         io_tair    = construct_io_field(                        &
                      trim(shf_data_names(shf_data_tair)),       &
                      i_dim, j_dim,                              &
                      field_loc = shf_bndy_loc(shf_data_tair),   &
                      field_type = shf_bndy_type(shf_data_tair), &
                      d2d_array=SHF_DATA(:,:,:,shf_data_tair,1))
         io_qair    = construct_io_field(                        &
                      trim(shf_data_names(shf_data_qair)),       &
                      i_dim, j_dim,                              &
                      field_loc = shf_bndy_loc(shf_data_qair),   &
                      field_type = shf_bndy_type(shf_data_qair), &
                      d2d_array=SHF_DATA(:,:,:,shf_data_qair,1))
         io_qsw     = construct_io_field(                        &
                      trim(shf_data_names(shf_data_qsw)),        &
                      i_dim, j_dim,                              &
                      field_loc = shf_bndy_loc(shf_data_qsw),    &
                      field_type = shf_bndy_type(shf_data_qsw),  &
                      d2d_array=SHF_DATA(:,:,:,shf_data_qsw,1))
         io_cldfrac = construct_io_field(                           &
                      trim(shf_data_names(shf_data_cldfrac)),       &
                      i_dim, j_dim,                                 &
                      field_loc = shf_bndy_loc(shf_data_cldfrac),   &
                      field_type = shf_bndy_type(shf_data_cldfrac), &
                      d2d_array=SHF_DATA(:,:,:,shf_data_cldfrac,1))
         io_windspd = construct_io_field(                           &
                      trim(shf_data_names(shf_data_windspd)),       &
                      i_dim, j_dim,                                 &
                      field_loc = shf_bndy_loc(shf_data_windspd),   &
                      field_type = shf_bndy_type(shf_data_windspd), &
                      d2d_array=SHF_DATA(:,:,:,shf_data_windspd,1))

         call data_set(forcing_file, 'define', io_sst)
         call data_set(forcing_file, 'define', io_tair)
         call data_set(forcing_file, 'define', io_qair)
         call data_set(forcing_file, 'define', io_qsw)
         call data_set(forcing_file, 'define', io_cldfrac)
         call data_set(forcing_file, 'define', io_windspd)

         call data_set(forcing_file, 'read', io_sst)
         call data_set(forcing_file, 'read', io_tair)
         call data_set(forcing_file, 'read', io_qair)
         call data_set(forcing_file, 'read', io_qsw)
         call data_set(forcing_file, 'read', io_cldfrac)
         call data_set(forcing_file, 'read', io_windspd)

         call destroy_io_field(io_sst)
         call destroy_io_field(io_tair)
         call destroy_io_field(io_qair)
         call destroy_io_field(io_qsw)
         call destroy_io_field(io_cldfrac)
         call destroy_io_field(io_windspd)

         allocate(SHF_COMP(nx_block,ny_block,max_blocks_clinic,  &
                                                 shf_num_comps), &
                  OCN_WGT (nx_block,ny_block,max_blocks_clinic))

         SHF_COMP = c0   ! initialize

      end select

      call data_set(forcing_file,'close')

      !*** renormalize values if necessary to compensate for different
      !*** units

      do n = 1,shf_data_num_fields
         if (shf_data_renorm(n) /= c1) SHF_DATA(:,:,:,n,:) = &
                    shf_data_renorm(n)*SHF_DATA(:,:,:,n,:)
      enddo

      shf_data_next = never
      shf_data_update = never
      shf_interp_freq = 'never'

      if (my_task == master_task) then
         write(stdout,blank_fmt)
         write(stdout,'(a23,a)') ' SHF Annual file read: ', &
                                 trim(forcing_file%full_name)
      endif

      call destroy_file(forcing_file)

!-----------------------------------------------------------------------
!  monthly mean climatological surface heat flux. all
!  12 months are read in from a file. interpolation order
!  (shf_interp_order) may be specified with namelist input.
!-----------------------------------------------------------------------

   case ('monthly-equal','monthly-calendar')

      allocate(SHF_DATA(nx_block,ny_block,max_blocks_clinic, &
                        shf_data_num_fields,0:12),           &
               TEMP_DATA(nx_block,ny_block,12,max_blocks_clinic, &
                         shf_data_num_fields))

      SHF_DATA = c0

      call find_forcing_times(shf_data_time, shf_data_inc,            &
                              shf_interp_type, shf_data_next,         &
                              shf_data_time_min_loc, shf_data_update, &
                              shf_data_type)

      forcing_file = construct_file(shf_file_fmt, &
                                    full_name = trim(shf_filename), &
                                    record_length = rec_type_dbl,   &
                                    recl_words=nx_global*ny_global)

      call data_set(forcing_file,'open_read')

      i_dim = construct_io_dim('i',nx_global)
      j_dim = construct_io_dim('j',ny_global)
      month_dim = construct_io_dim('month',12)

      select case (shf_formulation)
      case ('restoring')
         io_sst = construct_io_field( &
                      trim(shf_data_names(shf_data_sst)),       &
                      i_dim, j_dim, month_dim,                  &
                      field_loc = shf_bndy_loc(shf_data_sst),   &
                      field_type = shf_bndy_type(shf_data_sst), &
                      d3d_array=TEMP_DATA(:,:,:,:,shf_data_sst))
         call data_set(forcing_file,'define',io_sst)
         call data_set(forcing_file,'read'  ,io_sst)
         call destroy_io_field(io_sst)

         !$OMP PARALLEL DO PRIVATE(iblock, n)
         do iblock=1,nblocks_clinic
         do n=1,12
            SHF_DATA (:,:,iblock,shf_data_sst,n) = &
            TEMP_DATA(:,:,n,iblock,shf_data_sst)
         end do
         end do
         !$OMP END PARALLEL DO

      case ('Barnier-restoring')
         io_tstar = construct_io_field( &
                    trim(shf_data_names(shf_data_tstar)),       &
                    i_dim, j_dim, month_dim,                    &
                    field_loc = shf_bndy_loc(shf_data_tstar),   &
                    field_type = shf_bndy_type(shf_data_tstar), &
                    d3d_array=TEMP_DATA(:,:,:,:,shf_data_tstar))
         io_tau   = construct_io_field( &
                    trim(shf_data_names(shf_data_tau)),       &
                    i_dim, j_dim, month_dim,                  &
                    field_loc = shf_bndy_loc(shf_data_tau),   &
                    field_type = shf_bndy_type(shf_data_tau), &
                    d3d_array=TEMP_DATA(:,:,:,:,shf_data_tau))
         io_ice   = construct_io_field( &
                    trim(shf_data_names(shf_data_ice)),       &
                    i_dim, j_dim, month_dim,                  &
                    field_loc = shf_bndy_loc(shf_data_ice),   &
                    field_type = shf_bndy_type(shf_data_ice), &
                    d3d_array=TEMP_DATA(:,:,:,:,shf_data_ice))
         io_qsw   = construct_io_field( &
                    trim(shf_data_names(shf_data_qsw)),       &
                    i_dim, j_dim, month_dim,                  &
                    field_loc = shf_bndy_loc(shf_data_qsw),   &
                    field_type = shf_bndy_type(shf_data_qsw), &
                    d3d_array=TEMP_DATA(:,:,:,:,shf_data_qsw))

         call data_set(forcing_file,'define',io_tstar)
         call data_set(forcing_file,'define',io_tau)
         call data_set(forcing_file,'define',io_ice)
         call data_set(forcing_file,'define',io_qsw)

         call data_set(forcing_file,'read',io_tstar)
         call data_set(forcing_file,'read',io_tau)
         call data_set(forcing_file,'read',io_ice)
         call data_set(forcing_file,'read',io_qsw)

         !$OMP PARALLEL DO PRIVATE(iblock,n)
         do iblock=1,nblocks_clinic
         do n=1,12
            SHF_DATA (:,:,iblock,shf_data_tstar,n) = &
            TEMP_DATA(:,:,n,iblock,shf_data_tstar)
            SHF_DATA (:,:,iblock,shf_data_tau,n) = &
            TEMP_DATA(:,:,n,iblock,shf_data_tau)*seconds_in_day
            SHF_DATA (:,:,iblock,shf_data_ice,n) = &
            TEMP_DATA(:,:,n,iblock,shf_data_ice)
            SHF_DATA (:,:,iblock,shf_data_qsw,n) = &
            TEMP_DATA(:,:,n,iblock,shf_data_qsw)
         end do
         end do
         !$OMP END PARALLEL DO

         call destroy_io_field(io_tstar)
         call destroy_io_field(io_tau)
         call destroy_io_field(io_ice)
         call destroy_io_field(io_qsw)

      case ('bulk-NCEP')
         io_sst     = construct_io_field(                        &
                      trim(shf_data_names(shf_data_sst)),        &
                      i_dim, j_dim, month_dim,                   &
                      field_loc = shf_bndy_loc(shf_data_sst),    &
                      field_type = shf_bndy_type(shf_data_sst),  &
                      d3d_array=TEMP_DATA(:,:,:,:,shf_data_sst))
         io_tair    = construct_io_field(                        &
                      trim(shf_data_names(shf_data_tair)),       &
                      i_dim, j_dim, month_dim,                   &
                      field_loc = shf_bndy_loc(shf_data_tair),   &
                      field_type = shf_bndy_type(shf_data_tair), &
                      d3d_array=TEMP_DATA(:,:,:,:,shf_data_tair))
         io_qair    = construct_io_field(                        &
                      trim(shf_data_names(shf_data_qair)),       &
                      i_dim, j_dim, month_dim,                   &
                      field_loc = shf_bndy_loc(shf_data_qair),   &
                      field_type = shf_bndy_type(shf_data_qair), &
                      d3d_array=TEMP_DATA(:,:,:,:,shf_data_qair))
         io_qsw     = construct_io_field(                        &
                      trim(shf_data_names(shf_data_qsw)),        &
                      i_dim, j_dim, month_dim,                   &
                      field_loc = shf_bndy_loc(shf_data_qsw),    &
                      field_type = shf_bndy_type(shf_data_qsw),  &
                      d3d_array=TEMP_DATA(:,:,:,:,shf_data_qsw))
         io_cldfrac = construct_io_field(                           &
                      trim(shf_data_names(shf_data_cldfrac)),       &
                      i_dim, j_dim, month_dim,                      &
                      field_loc = shf_bndy_loc(shf_data_cldfrac),   &
                      field_type = shf_bndy_type(shf_data_cldfrac), &
                      d3d_array=TEMP_DATA(:,:,:,:,shf_data_cldfrac))
         io_windspd = construct_io_field(                           &
                      trim(shf_data_names(shf_data_windspd)),       &
                      i_dim, j_dim, month_dim,                      &
                      field_loc = shf_bndy_loc(shf_data_windspd),   &
                      field_type = shf_bndy_type(shf_data_windspd), &
                      d3d_array=TEMP_DATA(:,:,:,:,shf_data_windspd))

         call data_set(forcing_file, 'define', io_sst)
         call data_set(forcing_file, 'define', io_tair)
         call data_set(forcing_file, 'define', io_qair)
         call data_set(forcing_file, 'define', io_qsw)
         call data_set(forcing_file, 'define', io_cldfrac)
         call data_set(forcing_file, 'define', io_windspd)

         call data_set(forcing_file, 'read', io_sst)
         call data_set(forcing_file, 'read', io_tair)
         call data_set(forcing_file, 'read', io_qair)
         call data_set(forcing_file, 'read', io_qsw)
         call data_set(forcing_file, 'read', io_cldfrac)
         call data_set(forcing_file, 'read', io_windspd)

         !$OMP PARALLEL DO PRIVATE(iblock, n)
         do iblock=1,nblocks_clinic
         do n=1,12
            SHF_DATA (:,:,iblock,shf_data_sst,n)     = &
            TEMP_DATA(:,:,n,iblock,shf_data_sst)
            SHF_DATA (:,:,iblock,shf_data_tair,n)    = &
            TEMP_DATA(:,:,n,iblock,shf_data_tair)
            SHF_DATA (:,:,iblock,shf_data_qair,n)    = &
            TEMP_DATA(:,:,n,iblock,shf_data_qair)
            SHF_DATA (:,:,iblock,shf_data_qsw,n)     = &
            TEMP_DATA(:,:,n,iblock,shf_data_qsw)
            SHF_DATA (:,:,iblock,shf_data_cldfrac,n) = &
            TEMP_DATA(:,:,n,iblock,shf_data_cldfrac)
            SHF_DATA (:,:,iblock,shf_data_windspd,n) = &
            TEMP_DATA(:,:,n,iblock,shf_data_windspd)
         end do
         end do
         !$OMP END PARALLEL DO

         call destroy_io_field(io_sst)
         call destroy_io_field(io_tair)
         call destroy_io_field(io_qair)
         call destroy_io_field(io_qsw)
         call destroy_io_field(io_cldfrac)
         call destroy_io_field(io_windspd)

         allocate( SHF_COMP(nx_block,ny_block,max_blocks_clinic, &
                                                  shf_num_comps), &
                    OCN_WGT(nx_block,ny_block,max_blocks_clinic))
         SHF_COMP = c0   ! initialize

      end select

      deallocate(TEMP_DATA)
      call data_set(forcing_file,'close')
      call destroy_file(forcing_file)

      !*** renormalize values if necessary to compensate for different
      !*** units.

      do n = 1,shf_data_num_fields
         if (shf_data_renorm(n) /= c1) SHF_DATA(:,:,:,n,:) = &
                    shf_data_renorm(n)*SHF_DATA(:,:,:,n,:)
      enddo

      if (my_task == master_task) then
         write(stdout,blank_fmt)
         write(stdout,'(a24,a)') ' SHF Monthly file read: ', &
                                 trim(shf_filename)
      endif

!-----------------------------------------------------------------------
!
!  surface temperature specified every n-hours, where the n-hour
!    increment should be specified with namelist input
!    (shf_data_inc). only as many times as are necessary based on
!    the order of the temporal interpolation scheme
!    (shf_interp_order) reside in memory at any given time.
!
!-----------------------------------------------------------------------

   case ('n-hour')

      allocate(SHF_DATA(nx_block,ny_block,max_blocks_clinic, &
                        shf_data_num_fields,0:shf_interp_order))

      SHF_DATA = c0

      call find_forcing_times(shf_data_time,         shf_data_inc,    &
                              shf_interp_type,       shf_data_next,   &
                              shf_data_time_min_loc, shf_data_update, &
                              shf_data_type)

      do n = 1, shf_interp_order

         call get_forcing_filename(forcing_filename, shf_filename, &
                                   shf_data_time(n), shf_data_inc)

         forcing_file = construct_file(shf_file_fmt,                 &
                                   full_name=trim(forcing_filename), &
                                   record_length = rec_type_dbl,     &
                                   recl_words = nx_global*ny_global)

         call data_set(forcing_file,'open_read')

         i_dim = construct_io_dim('i',nx_global)
         j_dim = construct_io_dim('j',ny_global)

         select case (shf_formulation)
         case ('restoring')

            io_sst = construct_io_field(                       &
                     trim(shf_data_names(shf_data_sst)),       &
                     i_dim, j_dim,                             &
                     field_loc = shf_bndy_loc(shf_data_sst),   &
                     field_type = shf_bndy_type(shf_data_sst), &
                     d2d_array=SHF_DATA(:,:,:,shf_data_sst,n))
            call data_set(forcing_file,'define',io_sst)
            call data_set(forcing_file,'read'  ,io_sst)
            call destroy_io_field(io_sst)

         case ('Barnier-restoring')
            io_tstar = construct_io_field(                         &
                       trim(shf_data_names(shf_data_tstar)),       &
                       i_dim, j_dim,                               &
                       field_loc = shf_bndy_loc(shf_data_tstar),   &
                       field_type = shf_bndy_type(shf_data_tstar), &
                       d2d_array=SHF_DATA(:,:,:,shf_data_tstar,n))
            io_tau   = construct_io_field(                       &
                       trim(shf_data_names(shf_data_tau)),       &
                       i_dim, j_dim,                             &
                       field_loc = shf_bndy_loc(shf_data_tau),   &
                       field_type = shf_bndy_type(shf_data_tau), &
                       d2d_array=SHF_DATA(:,:,:,shf_data_tau  ,n))
            io_ice   = construct_io_field(                       &
                       trim(shf_data_names(shf_data_ice)),       &
                       i_dim, j_dim,                             &
                       field_loc = shf_bndy_loc(shf_data_ice),   &
                       field_type = shf_bndy_type(shf_data_ice), &
                       d2d_array=SHF_DATA(:,:,:,shf_data_ice  ,n))
            io_qsw   = construct_io_field(                       &
                       trim(shf_data_names(shf_data_qsw)),       &
                       i_dim, j_dim,                             &
                       field_loc = shf_bndy_loc(shf_data_qsw),   &
                       field_type = shf_bndy_type(shf_data_qsw), &
                       d2d_array=SHF_DATA(:,:,:,shf_data_qsw  ,n))

            call data_set(forcing_file,'define',io_tstar)
            call data_set(forcing_file,'define',io_tau)
            call data_set(forcing_file,'define',io_ice)
            call data_set(forcing_file,'define',io_qsw)

            call data_set(forcing_file,'read',io_tstar)
            call data_set(forcing_file,'read',io_tau)
            call data_set(forcing_file,'read',io_ice)
            call data_set(forcing_file,'read',io_qsw)

            call destroy_io_field(io_tstar)
            call destroy_io_field(io_tau)
            call destroy_io_field(io_ice)
            call destroy_io_field(io_qsw)

            SHF_DATA(:,:,:,shf_data_tau  ,n) = &
            SHF_DATA(:,:,:,shf_data_tau  ,n)*seconds_in_day

         case ('bulk-NCEP')
            io_sst     = construct_io_field(                        &
                         trim(shf_data_names(shf_data_sst)),        &
                         i_dim, j_dim,                              &
                         field_loc = shf_bndy_loc(shf_data_sst),    &
                         field_type = shf_bndy_type(shf_data_sst),  &
                         d2d_array=SHF_DATA(:,:,:,shf_data_sst,n))
            io_tair    = construct_io_field(                        &
                         trim(shf_data_names(shf_data_tair)),       &
                         i_dim, j_dim,                              &
                         field_loc = shf_bndy_loc(shf_data_tair),   &
                         field_type = shf_bndy_type(shf_data_tair), &
                         d2d_array=SHF_DATA(:,:,:,shf_data_tair,n))
            io_qair    = construct_io_field(                        &
                         trim(shf_data_names(shf_data_qair)),       &
                         i_dim, j_dim,                              &
                         field_loc = shf_bndy_loc(shf_data_qair),   &
                         field_type = shf_bndy_type(shf_data_qair), &
                         d2d_array=SHF_DATA(:,:,:,shf_data_qair,n))
            io_qsw     = construct_io_field(                        &
                         trim(shf_data_names(shf_data_qsw)),        &
                         i_dim, j_dim,                              &
                         field_loc = shf_bndy_loc(shf_data_qsw),    &
                         field_type = shf_bndy_type(shf_data_qsw),  &
                         d2d_array=SHF_DATA(:,:,:,shf_data_qsw,n))
            io_cldfrac = construct_io_field(                           &
                         trim(shf_data_names(shf_data_cldfrac)),       &
                         i_dim, j_dim,                                 &
                         field_loc = shf_bndy_loc(shf_data_cldfrac),   &
                         field_type = shf_bndy_type(shf_data_cldfrac), &
                         d2d_array=SHF_DATA(:,:,:,shf_data_cldfrac,n))
            io_windspd = construct_io_field(                           &
                         trim(shf_data_names(shf_data_windspd)),       &
                         i_dim, j_dim,                                 &
                         field_loc = shf_bndy_loc(shf_data_windspd),   &
                         field_type = shf_bndy_type(shf_data_windspd), &
                         d2d_array=SHF_DATA(:,:,:,shf_data_windspd,n))

            call data_set(forcing_file, 'define', io_sst)
            call data_set(forcing_file, 'define', io_tair)
            call data_set(forcing_file, 'define', io_qair)
            call data_set(forcing_file, 'define', io_qsw)
            call data_set(forcing_file, 'define', io_cldfrac)
            call data_set(forcing_file, 'define', io_windspd)

            call data_set(forcing_file, 'read', io_sst)
            call data_set(forcing_file, 'read', io_tair)
            call data_set(forcing_file, 'read', io_qair)
            call data_set(forcing_file, 'read', io_qsw)
            call data_set(forcing_file, 'read', io_cldfrac)
            call data_set(forcing_file, 'read', io_windspd)


            call destroy_io_field(io_sst)
            call destroy_io_field(io_tair)
            call destroy_io_field(io_qair)
            call destroy_io_field(io_qsw)
            call destroy_io_field(io_cldfrac)
            call destroy_io_field(io_windspd)

         end select

         call data_set(forcing_file,'close')
         call destroy_file(forcing_file)

         if (my_task == master_task) then
            write(stdout,blank_fmt)
            write(stdout,'(a23,a)') ' SHF n-hour file read: ', &
                                    trim(forcing_filename)
         endif
      enddo

      if (shf_formulation == 'bulk-NCEP') then
         allocate(SHF_COMP(nx_block,ny_block,max_blocks_clinic,  &
                                                 shf_num_comps), &
                   OCN_WGT(nx_block,ny_block,max_blocks_clinic))
         SHF_COMP = c0   ! initialize
      endif

      !*** renormalize values if necessary to compensate for different
      !*** units.

      do n = 1,shf_data_num_fields
         if (shf_data_renorm(n) /= c1) SHF_DATA(:,:,:,n,:) = &
                    shf_data_renorm(n)*SHF_DATA(:,:,:,n,:)
      enddo

   case default

     call exit_POP(sigAbort,'init_shf: Unknown value for shf_data_type')

   end select

!-----------------------------------------------------------------------
!
!  now check interpolation period (shf_interp_freq) to set the
!    time for the next temporal interpolation (shf_interp_next).
!
!  if no interpolation is to be done, set next interpolation time
!    to a large number so the surface heat flux update test
!    in routine set_surface_forcing will always be false.
!
!  if interpolation is to be done every n-hours, find the first
!    interpolation time greater than the current time.
!
!  if interpolation is to be done every timestep, set next interpolation
!    time to a large negative number so the surface heat flux
!    update test in routine set_surface_forcing will always be true.
!
!-----------------------------------------------------------------------

   select case (shf_interp_freq)

   case ('never')
      shf_interp_next = never
      shf_interp_last = never
      shf_interp_inc  = c0

   case ('n-hour')
      call find_interp_time(shf_interp_inc, shf_interp_next)

   case ('every-timestep')
      shf_interp_next = always
      shf_interp_inc  = c0

   case default
      call exit_POP(sigAbort, &
                    'init_shf: Unknown value for shf_interp_freq')

   end select

   if (nsteps_total == 0) shf_interp_last = thour00

!-----------------------------------------------------------------------
!
!  echo forcing options to stdout.
!
!-----------------------------------------------------------------------

   shf_data_label = 'Surface Heat Flux'
   call echo_forcing_options(shf_data_type,   shf_formulation, &
                             shf_data_inc,    shf_interp_freq, &
                             shf_interp_type, shf_interp_inc,  &
                             shf_data_label)

!-----------------------------------------------------------------------
!EOC

 end subroutine init_shf

!***********************************************************************
!BOP
! !IROUTINE: set_shf
! !INTERFACE:

 subroutine set_shf(STF)

! !DESCRIPTION:
!  Updates the current value of the surface heat flux array
!  (shf) by interpolating to the current time or calculating
!  fluxes based on states at current time.  If new data are
!  required for interpolation, new data are read.
!
! !REVISION HISTORY:
!  same as module

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,nt,max_blocks_clinic), &
      intent(inout) :: &
      STF    !  surface tracer fluxes at current timestep

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      iblock

!-----------------------------------------------------------------------
!
!  check if new data is necessary for interpolation.  if yes, then
!     shuffle indices in SHF_DATA and shf_data_time arrays
!     and read in new data if necessary ('n-hour' case).  note
!     that no new data is necessary for 'analytic' and 'annual' cases.
!  then perform interpolation using updated shf data or compute fluxes
!     based on current or interpolated state data.
!
!-----------------------------------------------------------------------

   select case(shf_data_type)

   case ('analytic')

      select case (shf_formulation)
      case ('restoring')

         !$OMP PARALLEL DO PRIVATE(iblock)
         do iblock=1,nblocks_clinic
            STF(:,:,1,iblock) = (SHF_DATA(:,:,iblock,shf_data_sst,1) - &
                                 TRACER(:,:,1,1,curtime,iblock))*      &
                                shf_restore_rtau*dz(1)
         end do
         !$OMP END PARALLEL DO

      end select

   case ('annual')

      select case (shf_formulation)
      case ('restoring')
         !$OMP PARALLEL DO PRIVATE(iblock)
         do iblock=1,nblocks_clinic
            STF(:,:,1,iblock) = (SHF_DATA(:,:,iblock,shf_data_sst,1) - &
                                 TRACER(:,:,1,1,curtime,iblock))*      &
                                shf_restore_rtau*dz(1)
         end do
         !$OMP END PARALLEL DO

      case ('Barnier-restoring')
         call calc_shf_barnier_restoring(STF,1)

      case ('bulk-NCEP')
         call calc_shf_bulk_ncep(STF,1)

      end select

   case ('monthly-equal','monthly-calendar')

      shf_data_label = 'SHF Monthly'
      if (thour00 >= shf_data_update) then
         call update_forcing_data(shf_data_time, shf_data_time_min_loc,&
                                  shf_interp_type, shf_data_next,      &
                                  shf_data_update, shf_data_type,      &
                                  shf_data_inc, SHF_DATA(:,:,:,:,1:12),&
                                  shf_data_renorm,                     &
                                  shf_data_label, shf_data_names,      &
                                  shf_bndy_loc,   shf_bndy_type,       &
                                  shf_filename, shf_file_fmt)
      endif

      if (thour00 >= shf_interp_next .or. nsteps_run == 0) then
         call interpolate_forcing(SHF_DATA(:,:,:,:,0),                 &
                               SHF_DATA(:,:,:,:,1:12),                 &
                               shf_data_time,         shf_interp_type, &
                               shf_data_time_min_loc, shf_interp_freq, &
                               shf_interp_inc,        shf_interp_next, &
                               shf_interp_last,       nsteps_run)

         if (nsteps_run /= 0) shf_interp_next = &
                              shf_interp_next + shf_interp_inc
      endif

      select case (shf_formulation)
      case ('restoring')

         !$OMP PARALLEL DO PRIVATE(iblock)
         do iblock=1,nblocks_clinic
            STF(:,:,1,iblock) = (SHF_DATA(:,:,iblock,shf_data_sst,0) - &
                                 TRACER(:,:,1,1,curtime,iblock))*      &
                                shf_restore_rtau*dz(1)
         end do
         !$OMP END PARALLEL DO

      case ('Barnier-restoring')
         call calc_shf_barnier_restoring(STF,12)

      case ('bulk-NCEP')
         call calc_shf_bulk_ncep(STF,12)

      end select

   case('n-hour')

      shf_data_label = 'SHF n-hour'
      if (thour00 >= shf_data_update) then
         call update_forcing_data(shf_data_time, shf_data_time_min_loc,&
                                  shf_interp_type, shf_data_next,      &
                                  shf_data_update, shf_data_type,      &
                                  shf_data_inc,                        &
                                  SHF_DATA(:,:,:,:,1:shf_interp_order),&
                                  shf_data_renorm,                     &
                                  shf_data_label, shf_data_names,      &
                                  shf_bndy_loc,   shf_bndy_type,       &
                                  shf_filename, shf_file_fmt)
      endif

      if (thour00 >= shf_interp_next .or. nsteps_run == 0) then
         call interpolate_forcing(SHF_DATA(:,:,:,:,0),                &
                             SHF_DATA(:,:,:,:,1:shf_interp_order),    &
                             shf_data_time,          shf_interp_type, &
                             shf_data_time_min_loc,  shf_interp_freq, &
                             shf_interp_inc,         shf_interp_next, &
                             shf_interp_last,        nsteps_run)

        if (nsteps_run /= 0) shf_interp_next = &
                             shf_interp_next + shf_interp_inc
      endif

      select case (shf_formulation)
      case ('restoring')


         !$OMP PARALLEL DO PRIVATE(iblock)
         do iblock=1,nblocks_clinic
            STF(:,:,1,iblock) = (SHF_DATA(:,:,iblock,shf_data_sst,0) - &
                                 TRACER(:,:,1,1,curtime,iblock))*      &
                                shf_restore_rtau*dz(1)
         end do
         !$OMP END PARALLEL DO

      case ('Barnier-restoring')
         call calc_shf_barnier_restoring(STF, shf_interp_order)

      case ('bulk-NCEP')
         call calc_shf_bulk_ncep(STF, shf_interp_order)

      end select

   end select    ! shf_data_type

!-----------------------------------------------------------------------
!EOC

 end subroutine set_shf

!***********************************************************************
!BOP
! !IROUTINE: calc_shf_barnier_restoring
! !INTERFACE:

   subroutine calc_shf_barnier_restoring(STF, time_dim)

! !DESCRIPTION:
!  calculates surface heat fluxes
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
      time_dim      ! number of time points for interpolation

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,nt,max_blocks_clinic), &
      intent(inout) :: &
      STF    !  surface heat flux at current timestep

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      nearest_data, now, &! indices for nearest,interpolated time slices
      iblock              ! local address of current block

   real (r8) :: &
      tcheck, ice_cutoff, ice_restore_temp

!-----------------------------------------------------------------------
!
!  local parameters
!
!-----------------------------------------------------------------------

   ice_cutoff = 0.9_r8
   ice_restore_temp = -2.0_r8

!-----------------------------------------------------------------------
!
!  if annual forcing, no interpolation to current time is necessary.
!  otherwise, interpolated fields in index=0 slice of data array
!
!-----------------------------------------------------------------------

   if (shf_data_type == 'annual') then
      now = 1
      nearest_data = 1

   else
      now = 0

      !*** find nearest data time and use it for determining the ice
      !*** mask in place of interpolated field.
      !*** NOTE:  this is for backward compatibility.  perhaps
      !*** interpolating and using a cut-off of .45 would be acceptable.

      tcheck = (shf_data_update - thour00)/shf_data_inc

      select case(shf_interp_type)
      case ('nearest')
         nearest_data = shf_data_time_min_loc

      case ('linear')
         if (tcheck > 0.5)  then
            nearest_data = shf_data_time_min_loc
         else
            nearest_data = shf_data_time_min_loc + 1
         endif

      case ('4point')
         if (tcheck > 0.5)  then
            nearest_data = shf_data_time_min_loc + 1
         else
            nearest_data = shf_data_time_min_loc + 2
         endif

      end select

      if ((nearest_data - time_dim) > 0 ) nearest_data = &
                                          nearest_data - time_dim

   endif

!-----------------------------------------------------------------------
!
!  calculate forcing for each block
!
!-----------------------------------------------------------------------

   !$OMP PARALLEL DO PRIVATE(iblock)
   do iblock=1,nblocks_clinic

!-----------------------------------------------------------------------
!
!     check for ice concentration >= ice_cutoff in the nearest month.
!     if there is ice, set TAU to be constant and set TSTAR to
!     ice_restore_temp.
!
!-----------------------------------------------------------------------

      where (SHF_DATA(:,:,iblock,shf_data_ice,nearest_data) >= &
             ice_cutoff)
         SHF_DATA(:,:,iblock,shf_data_tau,now)   = shf_restore_tau
         SHF_DATA(:,:,iblock,shf_data_tstar,now) = ice_restore_temp
      endwhere

!-----------------------------------------------------------------------
!
!     apply restoring only where TAU is defined.
!
!-----------------------------------------------------------------------

      where (SHF_DATA(:,:,iblock,shf_data_tau,now) > c0)
         STF(:,:,1,iblock) =(SHF_DATA(:,:,iblock,shf_data_tstar,now) - &
                             TRACER(:,:,1,1,curtime,iblock))*          &
                             dz(1)/SHF_DATA(:,:,iblock,shf_data_tau,now)
      elsewhere
         STF(:,:,1,iblock) = c0
      end where

!-----------------------------------------------------------------------
!
!     copy penetrative shortwave into its own array (SHF_QSW) and
!     convert to T flux from W/m^2.
!
!-----------------------------------------------------------------------

      SHF_QSW(:,:,iblock) = SHF_DATA(:,:,iblock,shf_data_qsw,now)* &
                            hflux_factor

   end do
   !$OMP END PARALLEL DO

!-----------------------------------------------------------------------
!EOC

 end subroutine calc_shf_barnier_restoring

!***********************************************************************
!BOP
! !IROUTINE: calc_shf_bulk_ncep
! !INTERFACE:

   subroutine calc_shf_bulk_ncep(STF, time_dim)

! !DESCRIPTION:
!  Calculates surface heat flux from a combination of
!  air-sea fluxes (based on air temperature, specific humidity,
!  solar short wave flux, cloud fraction, and windspeed)
!  and restoring terms (due to restoring fields of SST).
!
!  Notes:
!       the forcing data (on t-grid)
!       are computed as SHF\_DATA(:,:,shf\_comp\_*,now) where:
!
!       shf\_data\_sst,       restoring SST                     (C)
!       shf\_data\_tair,      surface air temp. at tair\_height  (K)
!       shf\_data\_qair,      specific humidity at qair\_height  (kg/kg)
!       shf\_data\_qsw,       surface short wave flux   ($W/m^2$)
!       shf\_data\_cldfrac,   cloud fraction            (0.-1.)
!       shf\_data\_windspd ,  windspeed at height windspd\_height (m/s)
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
      time_dim

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,nt,max_blocks_clinic), &
      intent(inout) :: &
      STF    !  surface tracer fluxes at current timestep

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      n, now, k, &
      iblock              ! local address of current block

   real (r8), dimension(nx_block,ny_block) :: &
      RTEA,             &!  work array
      FRAC_CLOUD_COVER   !  fractional cloud cover

   real (r8), parameter ::              &
      T_strong_restore_limit = -1.8_r8, &
      T_weak_restore_limit   = -0.8_r8, &
      dT_restore_limit = T_weak_restore_limit - T_strong_restore_limit,&
      windspd_height = 10.0_r8,      &
      tair_height    =  2.0_r8,      &
      qair_height    =  2.0_r8,      &
      qair_mod_fact  =  0.94_r8,     &! factor to modify humidity
      sw_mod_fact    =  0.875_r8,    &! factor to modify short-wave flux
      sw_mod_albedo  =  0.93_r8       ! factor to modify albedo

!-----------------------------------------------------------------------
!
!  shf_weak_restore= weak(non-ice) restoring heatflux per degree (W/m2/C)
!  shf_strong_restore= strong  (ice) ..        ..  ..   ..  ..      ..
!
!  to calculate restoring factors, use mixed layer of 50m,
!  and restoring time constant tau (days):
!
!                Q (W/m2/C)
!  tau =   6  :  386.0
!  tau =  30  :   77.2
!  tau = 182.5:   12.0
!  tau = 365  :    6.0
!  tau = 730  :    3.0
!  tau = Inf  :    0.0
!
!---------------------------------------------------------------------

!-----------------------------------------------------------------------
!
!  set location of interpolated data
!
!-----------------------------------------------------------------------

   if (shf_data_type == 'annual') then
      now = 1
   else
      now = 0
   endif

!----------------------------------------------------------------------
!
!  do the rest of the computation for each block
!
!----------------------------------------------------------------------

   !$OMP PARALLEL DO PRIVATE(iblock)
   do iblock=1,nblocks_clinic

!----------------------------------------------------------------------
!
!     compute ocean weights (fraction of ocean vs. ice) every timestep
!
!----------------------------------------------------------------------

      where (SHF_DATA(:,:,iblock,shf_data_sst,now) <= &
             T_strong_restore_limit)
         OCN_WGT(:,:,iblock) = c0
      elsewhere
         OCN_WGT(:,:,iblock) =(SHF_DATA(:,:,iblock,shf_data_sst,now) - &
                              T_strong_restore_limit)/dT_restore_limit
      endwhere

      where (SHF_DATA(:,:,iblock,shf_data_sst,now) >= &
             T_weak_restore_limit) OCN_WGT(:,:,iblock) = c1

      !*** zero OCN_WGT at land pts
      where (KMT(:,:,iblock) == 0) OCN_WGT(:,:,iblock) = c0

      !***  set ocean weights to zero in all strong-restoring regions
      !***  (MASK_SR is 0 in marginal seas, 1 elsewhere)

      OCN_WGT(:,:,iblock) = OCN_WGT(:,:,iblock)*MASK_SR(:,:,iblock)

!----------------------------------------------------------------------
!
!     compute sensible and latent heat fluxes
!
!----------------------------------------------------------------------

      call sen_lat_flux(                                            &
         SHF_DATA(:,:,iblock,shf_data_windspd,now), windspd_height, &
         TRACER(:,:,1,1,curtime,iblock),                            &
         SHF_DATA(:,:,iblock,shf_data_tair,now),    tair_height,    &
         SHF_DATA(:,:,iblock,shf_data_qair,now),    qair_height,    &
         T0_Kelvin, SHF_COMP(:,:,iblock,shf_comp_qsens),            &
                    SHF_COMP(:,:,iblock,shf_comp_qlat))

!----------------------------------------------------------------------
!
!     compute short wave and long wave fluxes
!
!----------------------------------------------------------------------

      SHF_COMP(:,:,iblock,shf_comp_qsw) = sw_mod_albedo*sw_mod_fact* &
                                 SHF_DATA(:,:,iblock,shf_data_qsw,now)

      FRAC_CLOUD_COVER = c1 - CCINT(:,:,iblock)* &
                         SHF_DATA(:,:,iblock,shf_data_cldfrac,now)**2

      RTEA = sqrt( c1000*SHF_DATA(:,:,iblock,shf_data_qair,now)    &
                  /(0.622_r8 + 0.378_r8                            &
                 *SHF_DATA(:,:,iblock,shf_data_qair,now)) + eps2 )

      SHF_COMP(:,:,iblock,shf_comp_qlw) = -emissivity*stefan_boltzmann*&
                            SHF_DATA(:,:,iblock,shf_data_tair,now)**3* &
                           (SHF_DATA(:,:,iblock,shf_data_tair,now)*    &
                            (0.39_r8-0.05_r8*RTEA)*FRAC_CLOUD_COVER +  &
                            c4*(TRACER(:,:,1,1,curtime,iblock) +       &
                            T0_Kelvin -                                &
                            SHF_DATA(:,:,iblock,shf_data_tair,now)) )

!----------------------------------------------------------------------
!
!     weak temperature restoring term (note: OCN_WGT = 0 at land pts)
!
!----------------------------------------------------------------------

      SHF_COMP(:,:,iblock,shf_comp_wrest) = shf_weak_restore*         &
                              OCN_WGT(:,:,iblock)*                    &
                             (SHF_DATA(:,:,iblock,shf_data_sst,now) - &
                              TRACER(:,:,1,1,curtime,iblock))

!----------------------------------------------------------------------
!
!     strong temperature restoring term
!
!----------------------------------------------------------------------

      where (KMT(:,:,iblock) > 0)
         SHF_COMP(:,:,iblock,shf_comp_srest) = shf_strong_restore*    &
                             (c1-OCN_WGT(:,:,iblock))*                &
                             (SHF_DATA(:,:,iblock,shf_data_sst,now) - &
                              TRACER(:,:,1,1,curtime,iblock))
      elsewhere
         SHF_COMP(:,:,iblock,shf_comp_srest) = c0
      endwhere

!----------------------------------------------------------------------
!
!     net surface heat flux (W/m^2) (except penetrative shortwave flux)
!     convert to model units
!
!----------------------------------------------------------------------

      STF(:,:,1,iblock) =  hflux_factor*(OCN_WGT(:,:,iblock)*    &
                         (SHF_COMP(:,:,iblock,shf_comp_qsens)  + &
                          SHF_COMP(:,:,iblock,shf_comp_qlat )  + &
                          SHF_COMP(:,:,iblock,shf_comp_qlw  )) + &
                          SHF_COMP(:,:,iblock,shf_comp_wrest)  + &
                          SHF_COMP(:,:,iblock,shf_comp_srest))

!----------------------------------------------------------------------
!
!     copy penetrative shortwave flux into its own array (SHF_QSW) and
!     convert it and SHF to model units.
!
!----------------------------------------------------------------------

      SHF_QSW(:,:,iblock) = SHF_COMP(:,:,iblock,shf_comp_qsw)* &
                             OCN_WGT(:,:,iblock)*hflux_factor

   end do
   !$OMP END PARALLEL DO

!----------------------------------------------------------------------
!EOC

 end subroutine calc_shf_bulk_ncep

!***********************************************************************
!BOP
! !IROUTINE: sw_absorb_frac
! !INTERFACE:

 subroutine sw_absorb_frac( depth, sw_absorb_fraction )

! !DESCRIPTION:
!  Computes fraction of solar short-wave flux penetrating to
!  specified depth due to exponential decay in Jerlov water type.
!  Reference : two band solar absorption model of Simpson and
!     Paulson (1977)
!  Note: below 200m the solar penetration gets set to zero,
!     otherwise the limit for the exponent ($+/- 5678$) needs to be 
!     taken care of.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8) :: &
      depth     ! vertical depth (cm, >0.) for desired sw fraction

! !OUTPUT PARAMETERS:

   real (r8) :: &
     sw_absorb_fraction     ! short wave (radiation) fractional decay

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind), parameter :: &
      num_water_types = 5  ! max number of different water types

   real (r8), parameter :: &
      depth_cutoff = -200.0_r8

   real (r8) :: &
      depth_neg_meters

!-----------------------------------------------------------------------
!
!   define Jerlov water properties with rfac, depth1, depth2
!     Jerlov water type :  I       IA      IB      II      III
!     jerlov_water_type :  1       2       3       4       5
!
!-----------------------------------------------------------------------

   real (r8), dimension(num_water_types) ::                       &
      rfac   = (/ 0.58_r8, 0.62_r8, 0.67_r8, 0.77_r8, 0.78_r8 /), &
      depth1 = (/ 0.35_r8, 0.60_r8, 1.00_r8, 1.50_r8, 1.40_r8 /), &
      depth2 = (/ 23.0_r8, 20.0_r8, 17.0_r8, 14.0_r8, 7.90_r8 /)

!-----------------------------------------------------------------------
!
!  compute absorption fraction
!
!-----------------------------------------------------------------------

   depth_neg_meters =  -depth*mpercm  ! convert from cm to m and
                                      ! change sign

   if (depth_neg_meters < depth_cutoff) then
      sw_absorb_fraction = c0
   else
      sw_absorb_fraction =     rfac(jerlov_water_type)*            &
                 exp(depth_neg_meters/depth1(jerlov_water_type)) +  &
                     (c1 - rfac(jerlov_water_type))*                &
                 exp(depth_neg_meters/depth2(jerlov_water_type))
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine sw_absorb_frac

!***********************************************************************
!BOP
! !IROUTINE: add_sw_absorb
! !INTERFACE:

 subroutine add_sw_absorb(T_SOURCE, k, this_block)

! !DESCRIPTION:
!  If surface short wave heat flux is available, this routine caculates
!  the flux which passes through the top layer and enters lower vertical
!  depths in the ocean.  This flux is added as a source term in the
!  baroclinic equations.
!
! !REVISION HISTORY:
!  same as module

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,nt), intent(inout) :: &
      T_SOURCE     ! source terms for all tracers (to avoid copies)
                   ! sw absorption added only to other potential
                   ! temperature tracers

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
      k            ! vertical level index

   type (block), intent(in) :: &
      this_block   ! block info for this block

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      bid                ! local block index

   real (r8), dimension(nx_block,ny_block) :: &
      WORK    ! temporary work space

!-----------------------------------------------------------------------
!
!  calculate short wave absorption if available
!  absorption profile pre-calculated in init_shf routine
!
!-----------------------------------------------------------------------

   if (lsw_absorb) then  ! short wave flux is available

      bid = this_block%local_id

      !*** insure no neg QSW - store in work

      WORK = max(SHF_QSW(:,:,bid),c0)

      if (partial_bottom_cells) then
         where (k < KMT(:,:,bid))
            T_SOURCE(:,:,1) = T_SOURCE(:,:,1) +                      &
                              WORK*(sw_absorb(k-1) - sw_absorb(k))*  &
                              dzr(k)
         elsewhere  !  do not allow energy absorption by the ground
            T_SOURCE(:,:,1) = T_SOURCE(:,:,1) + WORK*                &
                              (sw_absorb(k-1))/DZT(:,:,k,bid)
         endwhere
      else
         where (k < KMT(:,:,bid))
            T_SOURCE(:,:,1) = T_SOURCE(:,:,1) +                      &
                              WORK*(sw_absorb(k-1) - sw_absorb(k))*  &
                              dzr(k)
         elsewhere  !  do not allow energy absorption by the ground
            T_SOURCE(:,:,1) = T_SOURCE(:,:,1) +                      &
                              WORK *(sw_absorb(k-1))*dzr(k)
         endwhere
      endif

   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine add_sw_absorb

!***********************************************************************
!BOP
! !IROUTINE: sen_lat_flux
! !INTERFACE:

 subroutine sen_lat_flux(US,hu,SST,TH,ht,QH,hq,tk0,HS,HL)

! !DESCRIPTION:
!  Computes latent and sensible heat fluxes following bulk formulae and
!  coefficients in Large and Pond (1981; 1982)
!
!  Assume 1) a neutral 10m drag coefficient = cdn =
!                   .0027/u10 + .000142 + .0000764 u10
!         2) a neutral 10m stanton number ctn= .0327 sqrt(cdn), unstable
!                                         ctn= .0180 sqrt(cdn), stable
!         3) a neutral 10m dalton  number  cen= .0346 sqrt(cdn)
!         4) the saturation humidity of air at t(k)  = qsat(t)  ($kg/m^3$)
!
!   note  1) here, tstar = <wt>/u*, and qstar = <wq>/u*.
!         2) wind speedx should all be above a minimum speed say 0.5 m/s
!         3) with optional interation loop, niter=3, should suffice
!
! ***  this version is for analyses inputs with hu = 10m and ht = hq **
! ***  also, SST enters in Celsius  ***************************
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), dimension (nx_block,ny_block), intent(in) :: &
      US,     &! mean wind speed (m/s)        at height hu (m)
      TH,     &! mean air temperature (k)     at height ht (m)
      QH,     &! mean air humidity (kg/kg)    at height hq (m)
      SST      ! sea surface temperature (K)

   real (r8), intent(in) :: &
      hu,     &! height (m) for mean wind speed
      ht,     &! height (m) for mean air temperature
      hq,     &! height (m) for mean air humidity
      tk0      ! Celcius zero point

! !OUTPUT PARAMETERS:

   real (r8), dimension (nx_block,ny_block), intent(out) :: &
      HS, &! sensible heat flux  (w/m^2), into ocean
      HL   ! latent   heat flux  (w/m^2), into ocean

!EOP
!BOC
!--------------------------------------------------------------------------
!
!  local variables
!
!--------------------------------------------------------------------------

   real (r8), dimension (nx_block,ny_block) ::                    &
      SH,T0,DELP,DELQ,STABLETMP,RDN,RHN,USTARR,TSTARR,QSTARR,TAU, &
      HUOL,HTOL,HQOL,SSHUM,PSIMH,PSIXH,RD,UZN,RH,RE,QSAT

   real (r8) ::         &
      ren,umin,zolmin,vonk,lapse_rate,gravity_mks,f1,refhgt,aln,czol

!-----------------------------------------------------------------------
!
!  constants
!
!-----------------------------------------------------------------------

   umin         = 0.5_r8          ! minimum wind speed
   zolmin       = -100._r8        ! minimum stability parameter
   vonk         = 0.4_r8          ! Von Karman''s constant
   lapse_rate   = 0.01_r8         ! abiabatic lapse rate deg/m
   gravity_mks  = grav/100.0_r8   ! gravity m/s/s
   f1           = 0.606_r8
   refhgt       = 10.0_r8         ! reference height
   aln          = log(ht/refhgt)
   czol         = hu*vonk*gravity_mks

   SH = max(US,umin)

!-----------------------------------------------------------------------
!
!  initial guess z/l=0.0; hu=ht=hq=z
!
!-----------------------------------------------------------------------

   T0  = TH * (c1 + f1 * QH)        ! virtual temperature (k)
   QSAT = 640380._r8 / exp(5107.4_r8/(SST+tk0))
   SSHUM = 0.98_r8 * QSAT/rho_air
                                       ! sea surface humidity (kg/kg)
   DELP = TH + lapse_rate*ht - SST - tk0 ! pot temperature diff (k)
   DELQ = QH - SSHUM
   STABLETMP = 0.5_r8 + sign(0.5_r8 , DELP)
   RDN  = sqrt(CDN(SH))
   RHN  = (c1-STABLETMP)* 0.0327_r8 + STABLETMP * 0.0180_r8
   ren  = 0.0346_r8
   USTARR = RDN * SH
   TSTARR = RHN * DELP
   QSTARR = REN * DELQ

!-----------------------------------------------------------------------
!
!  first iteration loop
!
!-----------------------------------------------------------------------

   HUOL = czol * (TSTARR/T0 + QSTARR/(c1/f1+QH)) / USTARR**2
   HUOL = max(HUOL,zolmin)
   STABLETMP = 0.5_r8 + sign(0.5_r8 , HUOL)
   HTOL = HUOL * ht / hu
   HQOL = HUOL * hq / hu

!-----------------------------------------------------------------------
!
!  evaluate all stability functions assuming hq = ht
!
!-----------------------------------------------------------------------

   SSHUM   = max(sqrt(abs(c1 - 16._r8*HUOL)),c1)
   SSHUM   = sqrt(SSHUM)
   PSIMH = -5._r8 * HUOL * STABLETMP + (c1-STABLETMP)              &
             * log((c1+SSHUM*(c2+SSHUM))*(c1+SSHUM*SSHUM)/8._r8)   &
             - c2*atan(SSHUM)+1.571_r8
   SSHUM   = max(sqrt(abs(c1 - 16._r8*HTOL)),c1)
   PSIXH = -5._r8*HTOL*STABLETMP + (c1-STABLETMP)*c2*log((c1+SSHUM)/c2)

!-----------------------------------------------------------------------
!
!  shift wind speed using old coefficient
!
!-----------------------------------------------------------------------

   RD   = RDN / (c1-RDN/vonk*PSIMH )
   UZN  = max(SH * RD / RDN , umin)

!-----------------------------------------------------------------------
!
!  update the transfer coefficients at 10 meters and neutral stability
!
!-----------------------------------------------------------------------

   RDN = sqrt(CDN(UZN))
   ren = 0.0346_r8
   RHN = (c1-STABLETMP)*0.0327_r8 + STABLETMP *0.0180_r8

!-----------------------------------------------------------------------
!
!  shift all coefficients to the measurement height and stability
!
!-----------------------------------------------------------------------

   RD   = RDN / (c1-RDN/vonk*PSIMH )
   RH   = RHN / (c1+RHN/vonk*(  aln     -PSIXH) )
   RE   = ren / (c1+ren/vonk*(  aln     -PSIXH) )

!-----------------------------------------------------------------------
!
!  update USTARR, TSTARR, QSTARR using updated, shifted  coefficients
!
!-----------------------------------------------------------------------

   USTARR = RD * SH
   QSTARR = RE * DELQ
   TSTARR = RH * DELP

!-----------------------------------------------------------------------
!
!  second iteration to converge on z/l and hence the fluxes
!
!-----------------------------------------------------------------------

   HUOL= czol * (TSTARR/T0+QSTARR/(c1/f1+QH)) / USTARR**2
   HUOL= max(HUOL,zolmin)
   STABLETMP = 0.5_r8 + sign(0.5_r8 , HUOL)
   HTOL = HUOL * ht / hu
   HQOL = HUOL * hq / hu

!-----------------------------------------------------------------------
!
!  evaluate all stability functions assuming hq = ht
!
!-----------------------------------------------------------------------

   SSHUM   = max(sqrt(abs(c1 - 16.*HUOL)),c1)
   SSHUM   = sqrt(SSHUM)
   PSIMH = -5._r8 * HUOL * STABLETMP + (c1-STABLETMP)              &
              * log((c1+SSHUM*(c2+SSHUM))*(c1+SSHUM*SSHUM)/8._r8)  &
              - c2*atan(SSHUM)+1.571_r8
   SSHUM   = max(sqrt(abs(c1 - 16._r8*HTOL)),c1)
   PSIXH = -5._r8*HTOL*STABLETMP + (c1-STABLETMP)*c2*log((c1+SSHUM)/c2)

!-----------------------------------------------------------------------
!
!  shift wind speed using old coefficient
!
!-----------------------------------------------------------------------

   RD   = RDN / (c1-RDN/vonk*PSIMH )
   UZN  = max(SH * RD / RDN , umin)

!-----------------------------------------------------------------------
!
!  update the transfer coefficients at 10 meters and neutral stability
!
!-----------------------------------------------------------------------

   RDN = sqrt(CDN(UZN))
   ren = 0.0346_r8
   RHN = (c1-STABLETMP)*0.0327_r8 + STABLETMP*0.0180_r8

!-----------------------------------------------------------------------
!
!  shift all coefficients to the measurement height and stability
!
!-----------------------------------------------------------------------

   RD   = RDN / (c1-RDN/vonk*PSIMH )
   RH   = RHN / (c1+RHN/vonk*(  aln     -PSIXH) )
   RE   = ren / (c1+ren/vonk*(  aln     -PSIXH) )

!-----------------------------------------------------------------------
!
!  update USTARR, TSTARR, QSTARR using updated, shifted  coefficients
!
!-----------------------------------------------------------------------

   USTARR = RD * SH
   QSTARR = RE * DELQ
   TSTARR = RH * DELP

!-----------------------------------------------------------------------
!
!  done >>>>  compute the fluxes
!
!-----------------------------------------------------------------------

   TAU = rho_air * USTARR**2
   TAU = TAU  * US / SH
   HS  = cp_air* TAU * TSTARR / USTARR
   HL  = latent_heat_vapor * TAU * QSTARR / USTARR

!-----------------------------------------------------------------------
!EOC

 end subroutine sen_lat_flux

!***********************************************************************
!BOP
! !IROUTINE: CDN
! !INTERFACE:

 function CDN(UMPS)

! !DESCRIPTION:
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(in) :: &
      UMPS

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block) :: &
      CDN

!EOP
!BOC
!-----------------------------------------------------------------------
!-----------------------------------------------------------------------

   CDN = 0.0027_r8/UMPS + .000142_r8 + .0000764_r8*UMPS

!-----------------------------------------------------------------------
!EOC

 end function CDN

!***********************************************************************

 end module forcing_shf

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
