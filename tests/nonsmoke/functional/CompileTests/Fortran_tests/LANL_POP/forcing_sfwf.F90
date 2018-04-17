!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module forcing_sfwf

!BOP
! !MODULE: forcing_sfwf
! !DESCRIPTION:
!  Contains routines and variables used for determining the
!  surface fresh water flux.
!
! !REVISION HISTORY:
!  CVS:$Id: forcing_sfwf.F90,v 1.13 2003/01/28 23:21:19 pwjones Exp $
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
   use global_reductions
   use forcing_tools
   use forcing_shf
   use time_management
   use prognostic
   use exit_mod

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_sfwf,     &
             set_sfwf

! !PUBLIC DATA MEMBERS:

   real (r8), public :: &! public for use in restart
      sfwf_interp_last   ! time when last interpolation was done

   !*** water balance factors for bulk-NCEP forcing

   real (r8), public :: &! public for use in restart
      sum_precip,       &! global precip for water balance
      precip_fact = c1, &! factor for adjusting precip for water balance
      ssh_initial        ! initial ssh

   real (r8), dimension(km), public :: &
      sal_initial

   logical (log_kind), public :: &
      lfw_as_salt_flx          ! treat fw flux as virtual salt flux
                               ! even with var.thickness sfc layer

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  module variables
!
!-----------------------------------------------------------------------

   real (r8), allocatable, dimension(:,:,:,:,:) :: &
      SFWF_DATA    ! forcing data used to get SFWF

   real (r8), allocatable, dimension(:,:,:,:) :: &
      SFWF_COMP

   real (r8), dimension(12) :: &
      sfwf_data_time  ! time (hours) corresponding to surface fresh
                      ! water fluxes

   real (r8), dimension(20) :: &
      sfwf_data_renorm         ! factors for converting to model units

   real (r8) ::          &
      sfwf_data_inc,     &! time increment between values of forcing data
      sfwf_data_next,    &! time to be used for next value of forcing data
      sfwf_data_update,  &! time new forcing data needs to be added to interpolation set
      sfwf_interp_inc,   &! time increment between interpolation
      sfwf_interp_next,  &! time when next interpolation will be done
      sfwf_restore_tau,  &! restoring time scale
      sfwf_restore_rtau, &! reciprocal of restoring time scale
      sfwf_weak_restore, &!
      sfwf_strong_restore !

   integer (int_kind) :: &
      sfwf_interp_order,      &! order of temporal interpolation
      sfwf_data_time_min_loc, &! time index for first SFWF_DATA point
      sfwf_data_num_fields,   &!
      sfwf_num_comps           !

   character (char_len), dimension(:), allocatable :: &
      sfwf_data_names          ! short names for input data fields

   integer (int_kind), dimension(:), allocatable :: &
      sfwf_bndy_loc,          &! location and field types for ghost
      sfwf_bndy_type           !    cell update routines

   !*** integer addresses for various forcing data fields

   integer (int_kind) :: & ! restoring
      sfwf_data_sss

   integer (int_kind) :: &! bulk-NCEP
      sfwf_data_precip, &
      sfwf_comp_precip, &
      sfwf_comp_evap,   &
      sfwf_comp_wrest,  &
      sfwf_comp_srest

   real (r8) ::                &
      ann_avg_precip,          &!
      !sum_fw,                  &!
      !ann_avg_fw,              &!
      ssh_final

   real (r8), dimension (km) :: &
      sal_final

   logical (log_kind) :: &
      ladjust_precip

   character (char_len) :: &
      sfwf_data_type,      &! keyword for period of forcing data
      sfwf_filename,       &! name of file conainting forcing data
      sfwf_file_fmt,       &! format (bin or netcdf) of forcing file
      sfwf_interp_freq,    &! keyword for period of temporal interpolation
      sfwf_interp_type,    &!
      sfwf_formulation,    &!
      sfwf_data_label

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_sfwf
! !INTERFACE:

 subroutine init_sfwf(STF)

! !DESCRIPTION:
!  Initializes surface fresh water flux forcing by either calculating
!  or reading in the surface fresh water flux.  Also does initial
!  book-keeping concerning when new data is needed for the temporal
!  interpolation and when the forcing will need to be updated.
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

   integer(int_kind) :: &
      k, n,             &! dummy loop indices
      iblock,           &! block loop index
      nml_error          ! namelist error flag

   character (char_len) :: &
      forcing_filename     ! full filename for forcing input

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic) :: &
      WORK               ! temporary work space

   real (r8), dimension(:,:,:,:,:), allocatable :: &
      TEMP_DATA          ! temporary array for reading monthly data

   type (block) :: &
      this_block   ! block info for local block

   type (datafile) ::  &
      forcing_file     ! data file structure for input forcing file

   type (io_field_desc) :: &
      io_sss,         &! io field descriptor for input sss field
      io_precip        ! io field descriptor for input precip field

   type (io_dim) :: &
      i_dim, j_dim, &! dimension descriptors for horiz dimensions
      month_dim      ! dimension descriptor  for monthly data

   namelist /forcing_sfwf_nml/ sfwf_data_type,      sfwf_data_inc,    &
                               sfwf_interp_type,    sfwf_interp_freq, &
                               sfwf_interp_inc,     sfwf_restore_tau, &
                               sfwf_filename,       sfwf_file_fmt,    &
                               sfwf_data_renorm,    sfwf_formulation, &
                               ladjust_precip,      sfwf_weak_restore,&
                               sfwf_strong_restore, lfw_as_salt_flx

!-----------------------------------------------------------------------
!
!  read surface fresh water flux namelist input after setting
!  default values.
!
!-----------------------------------------------------------------------

   sfwf_formulation = 'restoring'
   sfwf_data_type   = 'analytic'
   sfwf_data_inc    = 1.e20_r8
   sfwf_interp_type = 'nearest'
   sfwf_interp_freq = 'never'
   sfwf_interp_inc  = 1.e20_r8
   sfwf_restore_tau = 1.e20_r8
   sfwf_filename    = 'unknown-sfwf'
   sfwf_file_fmt    = 'bin'
   sfwf_data_renorm = c1
   !sfwf_data_renorm = 1.e-3_r8  !  convert from psu to msu
   ladjust_precip   = .false.
   sfwf_weak_restore   = 0.092_r8
   sfwf_strong_restore = 0.6648_r8
   lfw_as_salt_flx  = .false.

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=forcing_sfwf_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif


   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
     call exit_POP(sigAbort,'ERROR reading forcing_sfwf_nml')
   endif

   call broadcast_scalar(sfwf_data_type,      master_task)
   call broadcast_scalar(sfwf_data_inc,       master_task)
   call broadcast_scalar(sfwf_interp_type,    master_task)
   call broadcast_scalar(sfwf_interp_freq,    master_task)
   call broadcast_scalar(sfwf_interp_inc,     master_task)
   call broadcast_scalar(sfwf_restore_tau,    master_task)
   call broadcast_scalar(sfwf_filename,       master_task)
   call broadcast_scalar(sfwf_file_fmt,       master_task)
   call broadcast_scalar(sfwf_formulation,    master_task)
   call broadcast_array (sfwf_data_renorm,    master_task)
   call broadcast_scalar(ladjust_precip,      master_task)
   call broadcast_scalar(sfwf_weak_restore,   master_task)
   call broadcast_scalar(sfwf_strong_restore, master_task)
   call broadcast_scalar(lfw_as_salt_flx,     master_task)

!-----------------------------------------------------------------------
!
!  convert data_type to 'monthly-calendar' if input is 'monthly'
!
!-----------------------------------------------------------------------

   if (sfwf_data_type == 'monthly') sfwf_data_type = 'monthly-calendar'

!-----------------------------------------------------------------------
!
!  set values based on sfwf_formulation
!
!-----------------------------------------------------------------------

   select case (sfwf_formulation)
   case ('restoring')
      allocate(sfwf_data_names(1), &
               sfwf_bndy_loc  (1), &
               sfwf_bndy_type (1))
      sfwf_data_num_fields = 1
      sfwf_data_sss = 1
      sfwf_data_names(sfwf_data_sss) = 'SSS'
      sfwf_bndy_loc  (sfwf_data_sss) = field_loc_center
      sfwf_bndy_type (sfwf_data_sss) = field_type_scalar

   case ('bulk-NCEP')
      sfwf_data_num_fields = 2
      sfwf_data_sss = 1
      sfwf_data_precip = 2

      allocate(sfwf_data_names(sfwf_data_num_fields), &
               sfwf_bndy_loc  (sfwf_data_num_fields), &
               sfwf_bndy_type (sfwf_data_num_fields))
      sfwf_data_names(sfwf_data_sss) = 'SSS'
      sfwf_bndy_loc  (sfwf_data_sss) = field_loc_center
      sfwf_bndy_type (sfwf_data_sss) = field_type_scalar
      sfwf_data_names(sfwf_data_precip) = 'PRECIPITATION'
      sfwf_bndy_loc  (sfwf_data_precip) = field_loc_center
      sfwf_bndy_type (sfwf_data_precip) = field_type_scalar

      sfwf_num_comps = 4
      sfwf_comp_precip = 1
      sfwf_comp_evap = 2
      sfwf_comp_wrest = 3
      sfwf_comp_srest = 4

      !*** calculate initial salinity profile for ocean points that are
      !*** not marginal seas.

      !*** very first step of run
      if (ladjust_precip .and. nsteps_total == 0) then

         sum_precip = c0
         ssh_initial = c0
         !sum_fw = c0

         do k = 1,km
            !$OMP PARALLEL DO PRIVATE(iblock, this_block)
            do iblock=1,nblocks_clinic
               this_block = get_block(blocks_clinic(iblock),iblock)

               if (partial_bottom_cells) then
                  WORK(:,:,iblock) = &
                         merge(TRACER(:,:,k,2,curtime,iblock)*      &
                               TAREA(:,:,iblock)*DZT(:,:,k,iblock), &
                               c0, k <= KMT(:,:,iblock) .and.       &
                                   MASK_SR(:,:,iblock) > 0)
               else
                  WORK(:,:,iblock) = &
                         merge(TRACER(:,:,k,2,curtime,iblock)*  &
                               TAREA(:,:,iblock)*dz(k),         &
                               c0, k <= KMT(:,:,iblock) .and.   &
                                   MASK_SR(:,:,iblock) > 0)
               endif
            end do
            !$OMP END PARALLEL DO

            sal_initial(k) = global_sum(WORK,distrb_clinic,field_loc_center)/ &
                             (volume_t_k(k) - volume_t_marg_k(k))
         enddo
      endif

   case default
      call exit_POP(sigAbort, &
                    'init_sfwf: Unknown value for sfwf_formulation')

   end select

!-----------------------------------------------------------------------
!
!  calculate inverse of restoring time scale and convert to seconds.
!
!-----------------------------------------------------------------------

   sfwf_restore_rtau = c1/(seconds_in_day*sfwf_restore_tau)

!-----------------------------------------------------------------------
!
!  convert interp_type to corresponding integer value.
!
!-----------------------------------------------------------------------

   select case (sfwf_interp_type)
   case ('nearest')
      sfwf_interp_order = 1

   case ('linear')
      sfwf_interp_order = 2

   case ('4point')
      sfwf_interp_order = 4

   case default
      call exit_POP(sigAbort, &
                    'init_sfwf: Unknown value for sfwf_interp_type')

   end select

!-----------------------------------------------------------------------
!
!  set values of the surface fresh water flux arrays (SFWF or
!  SFWF_DATA) depending on type of the surface fresh water flux
!  data.
!
!-----------------------------------------------------------------------

   select case (sfwf_data_type)

!-----------------------------------------------------------------------
!
!  no surface fresh water flux, therefore no interpolation in time
!  is needed (sfwf_interp_freq = 'none'), nor are there any new
!  values to be used (sfwf_data_next = sfwf_data_update = never).
!
!-----------------------------------------------------------------------

   case ('none')

     STF(:,:,2,:) = c0
     sfwf_data_next = never
     sfwf_data_update = never
     sfwf_interp_freq = 'never'

!-----------------------------------------------------------------------
!
!  simple analytic surface salinity that is constant in time,
!  therefore no new values will be needed.
!
!-----------------------------------------------------------------------

   case ('analytic')

      allocate(SFWF_DATA(nx_block,ny_block,max_blocks_clinic, &
                         sfwf_data_num_fields,1))
      SFWF_DATA = c0

      select case (sfwf_formulation)
      case ('restoring')
         SFWF_DATA(:,:,:,sfwf_data_sss,1) = 0.035_r8
      end select

      sfwf_data_next = never
      sfwf_data_update = never
      sfwf_interp_freq = 'never'

!-----------------------------------------------------------------------
!
!  annual mean climatological surface salinity (read in from a file)
!  that is constant in time, therefore no new values will be needed.
!
!-----------------------------------------------------------------------

   case ('annual')

      allocate(SFWF_DATA(nx_block,ny_block,max_blocks_clinic, &
                         sfwf_data_num_fields,1))
      SFWF_DATA = c0

      forcing_file = construct_file(sfwf_file_fmt,                 &
                                    full_name=trim(sfwf_filename), &
                                    record_length = rec_type_dbl,  &
                                    recl_words=nx_global*ny_global)

      call data_set(forcing_file,'open_read')

      i_dim = construct_io_dim('i',nx_global)
      j_dim = construct_io_dim('j',nx_global)

      select case (sfwf_formulation)
      case ('restoring')

         io_sss = construct_io_field( &
                       trim(sfwf_data_names(sfwf_data_sss)),       &
                       i_dim, j_dim,                               &
                       field_loc = sfwf_bndy_loc(sfwf_data_sss),   &
                       field_type = sfwf_bndy_type(sfwf_data_sss), &
                       d2d_array=SFWF_DATA(:,:,:,sfwf_data_sss,1))
         call data_set(forcing_file,'define',io_sss)
         call data_set(forcing_file,'read'  ,io_sss)
         call destroy_io_field(io_sss)

      case ('bulk-NCEP')

         io_sss = construct_io_field( &
                       trim(sfwf_data_names(sfwf_data_sss)),       &
                       i_dim, j_dim,                               &
                       field_loc = sfwf_bndy_loc(sfwf_data_sss),   &
                       field_type = sfwf_bndy_type(sfwf_data_sss), &
                       d2d_array=SFWF_DATA(:,:,:,sfwf_data_sss,1))
         io_precip = construct_io_field( &
                       trim(sfwf_data_names(sfwf_data_precip)),       &
                       i_dim, j_dim,                                  &
                       field_loc = sfwf_bndy_loc(sfwf_data_precip),   &
                       field_type = sfwf_bndy_type(sfwf_data_precip), &
                       d2d_array=SFWF_DATA(:,:,:,sfwf_data_precip,1))
         call data_set(forcing_file,'define',io_sss)
         call data_set(forcing_file,'define',io_precip)
         call data_set(forcing_file,'read'  ,io_sss)
         call data_set(forcing_file,'read'  ,io_precip)
         call destroy_io_field(io_sss)
         call destroy_io_field(io_precip)

         allocate( SFWF_COMP(nx_block,ny_block,max_blocks_clinic, &
                             sfwf_num_comps))
         SFWF_COMP = c0  ! initialize

      end select

      call data_set(forcing_file,'close')
      call destroy_file(forcing_file)

      !*** renormalize values if necessary to compensate for
      !*** different units.

      do n = 1,sfwf_data_num_fields
         if (sfwf_data_renorm(n) /= c1) SFWF_DATA(:,:,:,n,:) = &
                    sfwf_data_renorm(n)*SFWF_DATA(:,:,:,n,:)
      enddo

      sfwf_data_next = never
      sfwf_data_update = never
      sfwf_interp_freq = 'never'

      if (my_task == master_task) then
         write(stdout,blank_fmt)
         write(stdout,'(a25,a)') ' SFWF Annual file read: ', &
                                 trim(sfwf_filename)
      endif

!-----------------------------------------------------------------------
!
!  monthly mean climatological surface fresh water flux. all
!  12 months are read in from a file. interpolation order
!  (sfwf_interp_order) may be specified with namelist input.
!
!-----------------------------------------------------------------------

   case ('monthly-equal','monthly-calendar')

      allocate(SFWF_DATA(nx_block,ny_block,max_blocks_clinic,    &
                                     sfwf_data_num_fields,0:12), &
               TEMP_DATA(nx_block,ny_block,12,max_blocks_clinic, &
                                           sfwf_data_num_fields) )
      SFWF_DATA = c0

      call find_forcing_times(sfwf_data_time,   sfwf_data_inc,  &
                              sfwf_interp_type, sfwf_data_next, &
                              sfwf_data_time_min_loc,           &
                              sfwf_data_update, sfwf_data_type)

      forcing_file = construct_file(sfwf_file_fmt,                 &
                                    full_name=trim(sfwf_filename), &
                                    record_length = rec_type_dbl,  &
                                    recl_words=nx_global*ny_global)
      call data_set(forcing_file,'open_read')

      i_dim = construct_io_dim('i',nx_global)
      j_dim = construct_io_dim('j',nx_global)
      month_dim = construct_io_dim('month',12)

      select case (sfwf_formulation)
      case ('restoring')
         io_sss = construct_io_field( &
                       trim(sfwf_data_names(sfwf_data_sss)),       &
                       i_dim, j_dim, dim3=month_dim,               &
                       field_loc = sfwf_bndy_loc(sfwf_data_sss),   &
                       field_type = sfwf_bndy_type(sfwf_data_sss), &
                       d3d_array=TEMP_DATA(:,:,:,:,sfwf_data_sss))
         call data_set(forcing_file,'define',io_sss)
         call data_set(forcing_file,'read'  ,io_sss)
         call destroy_io_field(io_sss)

      case ('bulk-NCEP')
         io_sss = construct_io_field( &
                       trim(sfwf_data_names(sfwf_data_sss)),       &
                       i_dim, j_dim, dim3=month_dim,               &
                       field_loc = sfwf_bndy_loc(sfwf_data_sss),   &
                       field_type = sfwf_bndy_type(sfwf_data_sss), &
                       d3d_array=TEMP_DATA(:,:,:,:,sfwf_data_sss))
         io_precip = construct_io_field( &
                       trim(sfwf_data_names(sfwf_data_precip)),       &
                       i_dim, j_dim, dim3=month_dim,                  &
                       field_loc = sfwf_bndy_loc(sfwf_data_precip),   &
                       field_type = sfwf_bndy_type(sfwf_data_precip), &
                       d3d_array=TEMP_DATA(:,:,:,:,sfwf_data_precip))
         call data_set(forcing_file,'define',io_sss)
         call data_set(forcing_file,'define',io_precip)
         call data_set(forcing_file,'read'  ,io_sss)
         call data_set(forcing_file,'read'  ,io_precip)
         call destroy_io_field(io_sss)
         call destroy_io_field(io_precip)

         allocate(SFWF_COMP(nx_block,ny_block,max_blocks_clinic, &
                                                 sfwf_num_comps))
         SFWF_COMP = c0  ! initialize

      end select

      call data_set(forcing_file,'close')
      call destroy_file(forcing_file)

      !*** re-order data and renormalize values if necessary to
      !*** compensate for different units.

      !$OMP PARALLEL DO PRIVATE(iblock, k, n)
      do iblock=1,nblocks_clinic
         do k=1,sfwf_data_num_fields
            if (sfwf_data_renorm(k) /= c1) then
               do n=1,12
                  SFWF_DATA(:,:,iblock,k,n) = &
                  TEMP_DATA(:,:,n,iblock,k)*sfwf_data_renorm(k)
               end do
            else
               do n=1,12
                  SFWF_DATA(:,:,iblock,k,n) = TEMP_DATA(:,:,n,iblock,k)
               end do
            endif
         end do
      end do
      !$OMP END PARALLEL DO

      deallocate(TEMP_DATA)

      if (my_task == master_task) then
         write(stdout,blank_fmt)
         write(stdout,'(a25,a)') ' SFWF Monthly file read: ', &
                                 trim(sfwf_filename)
      endif

!-----------------------------------------------------------------------
!
!  surface salinity specified every n-hours, where the n-hour
!  increment should be specified with namelist input
!  (sfwf_data_inc). only as many times as are necessary based on
!  the order of the temporal interpolation scheme
!  (sfwf_interp_order) reside in memory at any given time.
!
!-----------------------------------------------------------------------

   case ('n-hour')

      allocate( SFWF_DATA(nx_block,ny_block,max_blocks_clinic, &
                          sfwf_data_num_fields,0:sfwf_interp_order))
      SFWF_DATA = c0

      call find_forcing_times(sfwf_data_time,   sfwf_data_inc,  &
                              sfwf_interp_type, sfwf_data_next, &
                              sfwf_data_time_min_loc,           &
                              sfwf_data_update, sfwf_data_type)

      do n=1,sfwf_interp_order
         call get_forcing_filename(forcing_filename,  sfwf_filename, &
                                   sfwf_data_time(n), sfwf_data_inc)

         forcing_file = construct_file(sfwf_file_fmt,                 &
                                       full_name=trim(sfwf_filename), &
                                       record_length = rec_type_dbl,  &
                                       recl_words=nx_global*ny_global)
         call data_set(forcing_file,'open_read')

         i_dim = construct_io_dim('i',nx_global)
         j_dim = construct_io_dim('j',nx_global)

         select case (sfwf_formulation)
         case ('restoring')
            io_sss = construct_io_field( &
                       trim(sfwf_data_names(sfwf_data_sss)),       &
                       i_dim, j_dim,                               &
                       field_loc = sfwf_bndy_loc(sfwf_data_sss),   &
                       field_type = sfwf_bndy_type(sfwf_data_sss), &
                       d2d_array=SFWF_DATA(:,:,:,sfwf_data_sss,n))
            call data_set(forcing_file,'define',io_sss)
            call data_set(forcing_file,'read'  ,io_sss)
            call destroy_io_field(io_sss)

         case ('bulk-NCEP')
            io_sss = construct_io_field( &
                       trim(sfwf_data_names(sfwf_data_sss)),       &
                       i_dim, j_dim,                               &
                       field_loc = sfwf_bndy_loc(sfwf_data_sss),   &
                       field_type = sfwf_bndy_type(sfwf_data_sss), &
                       d2d_array=SFWF_DATA(:,:,:,sfwf_data_sss,n))
            io_precip = construct_io_field( &
                       trim(sfwf_data_names(sfwf_data_precip)),       &
                       i_dim, j_dim,                                  &
                       field_loc = sfwf_bndy_loc(sfwf_data_precip),   &
                       field_type = sfwf_bndy_type(sfwf_data_precip), &
                       d2d_array=SFWF_DATA(:,:,:,sfwf_data_precip,n))
            call data_set(forcing_file,'define',io_sss)
            call data_set(forcing_file,'define',io_precip)
            call data_set(forcing_file,'read'  ,io_sss)
            call data_set(forcing_file,'read'  ,io_precip)
            call destroy_io_field(io_sss)
            call destroy_io_field(io_precip)

         end select

         call data_set(forcing_file,'close')
         call destroy_file(forcing_file)

         if (my_task == master_task) then
            write(stdout,blank_fmt)
            write(stdout,'(a24,a)') ' SFWF n-hour file read: ', &
                                    trim(forcing_filename)
         endif
      enddo

      if (sfwf_formulation == 'bulk-NCEP') then
         allocate(SFWF_COMP(nx_block,ny_block,max_blocks_clinic, &
                                                 sfwf_num_comps))
         SFWF_COMP = c0  ! initialize
      endif

      !*** renormalize values if necessary to compensate for different
      !*** units.

      do n = 1,sfwf_data_num_fields
         if (sfwf_data_renorm(n) /= c1) SFWF_DATA(:,:,:,n,:) = &
                    sfwf_data_renorm(n)*SFWF_DATA(:,:,:,n,:)
      enddo

   case default

      call exit_POP(sigAbort, &
                    'init_sfwf: Unknown value for sfwf_data_type')

   end select

!-----------------------------------------------------------------------
!
!  now check interpolation period (sfwf_interp_freq) to set the
!    time for the next temporal interpolation (sfwf_interp_next).
!
!  if no interpolation is to be done, set next interpolation time
!    to a large number so the surface fresh water flux update test
!    in routine set_surface_forcing will always be false.
!
!  if interpolation is to be done every n-hours, find the first
!    interpolation time greater than the current time.
!
!  if interpolation is to be done every timestep, set next interpolation
!    time to a large negative number so the surface fresh water flux
!    update test in routine set_surface_forcing will always be true.
!
!-----------------------------------------------------------------------

   select case (sfwf_interp_freq)

   case ('never')

      sfwf_interp_next = never
      sfwf_interp_last = never
      sfwf_interp_inc  = c0

   case ('n-hour')

      call find_interp_time(sfwf_interp_inc, sfwf_interp_next)

   case ('every-timestep')

      sfwf_interp_next = always
      sfwf_interp_inc  = c0

   case default

      call exit_POP(sigAbort, &
                    'init_sfwf: Unknown value for sfwf_interp_freq')

   end select

   if(nsteps_total == 0) sfwf_interp_last = thour00

!-----------------------------------------------------------------------
!
!  echo forcing options to stdout.
!
!-----------------------------------------------------------------------

   sfwf_data_label = 'Surface Fresh Water Flux'
   call echo_forcing_options(sfwf_data_type,                     &
                             sfwf_formulation, sfwf_data_inc,    &
                             sfwf_interp_freq, sfwf_interp_type, &
                             sfwf_interp_inc,  sfwf_data_label)

   if (my_task == master_task) then
      if (lfw_as_salt_flx .and. sfc_layer_type == sfc_layer_varthick) &
         write(stdout,'(a47)') &
         '    Fresh water flux input as virtual salt flux'
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine init_sfwf

!***********************************************************************
!BOP
! !IROUTINE: set_sfwf
! !INTERFACE:

 subroutine set_sfwf(STF,FW,TFW)

! !DESCRIPTION:
!  Updates the current value of the surface fresh water flux arrays
!  by interpolating fields or computing fields at the current time.
!  If new data are necessary for the interpolation, the new data are
!  read from a file.
!
! !REVISION HISTORY:
!  same as module

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,nt,max_blocks_clinic), &
      intent(inout) :: &
      STF,    &! surface tracer fluxes at current timestep
      TFW      ! tracer concentration in fresh water flux

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), &
      intent(inout) :: &
      FW       ! fresh water flux if using varthick sfc layer

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      iblock             ! local address for current block

   type (block) :: &
      this_block   ! block info for current block

!-----------------------------------------------------------------------
!
!  check if new data is necessary for interpolation.  if yes, then
!    shuffle indices in SFWF_DATA and sfwf_data_time arrays
!    and read in new data if necessary ('n-hour' case).  note
!    that no new data is necessary for 'analytic' and 'annual' cases.
!  then perform interpolation or computation of fluxes at current time
!    using updated forcing data.
!
!-----------------------------------------------------------------------

   select case(sfwf_data_type)

   case ('analytic')

      select case (sfwf_formulation)
      case ('restoring')
         !$OMP PARALLEL DO PRIVATE(iblock, this_block)
         do iblock=1,nblocks_clinic
            this_block = get_block(blocks_clinic(iblock),iblock)

            STF(:,:,2,iblock) = &
                              (SFWF_DATA(:,:,iblock,sfwf_data_sss,1) - &
                               TRACER(:,:,1,2,curtime,iblock))*        &
                              sfwf_restore_rtau*dz(1)
         end do
         !$OMP END PARALLEL DO

      end select

   case('annual')

      select case (sfwf_formulation)
      case ('restoring')
         !$OMP PARALLEL DO PRIVATE(iblock, this_block)
         do iblock=1,nblocks_clinic
            this_block = get_block(blocks_clinic(iblock),iblock)

            STF(:,:,2,iblock) = &
                              (SFWF_DATA(:,:,iblock,sfwf_data_sss,1) - &
                               TRACER(:,:,1,2,curtime,iblock))*        &
                              sfwf_restore_rtau*dz(1)
         end do
         !$OMP END PARALLEL DO

      case ('bulk-NCEP')
         call calc_sfwf_bulk_ncep(STF,FW,TFW,1)

      end select

   case ('monthly-equal','monthly-calendar')

      sfwf_data_label = 'SFWF Monthly'
      if (thour00 >= sfwf_data_update) then
         call update_forcing_data(                   sfwf_data_time,   &
                             sfwf_data_time_min_loc, sfwf_interp_type, &
                             sfwf_data_next,         sfwf_data_update, &
                             sfwf_data_type,         sfwf_data_inc,    &
                             SFWF_DATA(:,:,:,:,1:12),sfwf_data_renorm, &
                             sfwf_data_label,        sfwf_data_names,  &
                             sfwf_bndy_loc,          sfwf_bndy_type,   &
                             sfwf_filename,          sfwf_file_fmt)
      endif

      if (thour00 >= sfwf_interp_next .or. nsteps_run == 0) then
         call interpolate_forcing(SFWF_DATA(:,:,:,:,0),               &
                                  SFWF_DATA(:,:,:,:,1:12),            &
                            sfwf_data_time,         sfwf_interp_type, &
                            sfwf_data_time_min_loc, sfwf_interp_freq, &
                            sfwf_interp_inc,        sfwf_interp_next, &
                            sfwf_interp_last,       nsteps_run)
         if (nsteps_run /= 0) sfwf_interp_next = &
                              sfwf_interp_next + sfwf_interp_inc
      endif

      select case (sfwf_formulation)
      case ('restoring')

         !$OMP PARALLEL DO PRIVATE(iblock, this_block)
         do iblock=1,nblocks_clinic
            this_block = get_block(blocks_clinic(iblock),iblock)

            STF(:,:,2,iblock) = &
                              (SFWF_DATA(:,:,iblock,sfwf_data_sss,0) - &
                               TRACER(:,:,1,2,curtime,iblock))*        &
                              sfwf_restore_rtau*dz(1)
         end do
         !$OMP END PARALLEL DO

      case ('bulk-NCEP')

         call calc_sfwf_bulk_ncep(STF,FW,TFW,12)

      end select

   case('n-hour')

      sfwf_data_label = 'SFWF n-hour'
      if (thour00 >= sfwf_data_update) then
         call update_forcing_data(                   sfwf_data_time,   &
                             sfwf_data_time_min_loc, sfwf_interp_type, &
                             sfwf_data_next,         sfwf_data_update, &
                             sfwf_data_type,         sfwf_data_inc,    &
                             SFWF_DATA(:,:,:,:,1:sfwf_interp_order),   &
                             sfwf_data_renorm,                         &
                             sfwf_data_label, sfwf_data_names,         &
                             sfwf_bndy_loc,   sfwf_bndy_type,          &
                             sfwf_filename,   sfwf_file_fmt)
      endif

      if (thour00 >= sfwf_interp_next .or. nsteps_run == 0) then
         call interpolate_forcing(SFWF_DATA(:,:,:,:,0),               &
                            SFWF_DATA(:,:,:,:,1:sfwf_interp_order),   &
                            sfwf_data_time,         sfwf_interp_type, &
                            sfwf_data_time_min_loc, sfwf_interp_freq, &
                            sfwf_interp_inc,        sfwf_interp_next, &
                            sfwf_interp_last,       nsteps_run)
         if (nsteps_run /= 0) sfwf_interp_next = &
                              sfwf_interp_next + sfwf_interp_inc
      endif

      select case (sfwf_formulation)
      case ('restoring')

         !$OMP PARALLEL DO PRIVATE(iblock, this_block)
         do iblock=1,nblocks_clinic
            this_block = get_block(blocks_clinic(iblock),iblock)

            STF(:,:,2,iblock) = &
                              (SFWF_DATA(:,:,iblock,sfwf_data_sss,0) - &
                               TRACER(:,:,1,2,curtime,iblock))*        &
                              sfwf_restore_rtau*dz(1)
         end do
         !$OMP END PARALLEL DO

      case ('bulk-NCEP')

         call calc_sfwf_bulk_ncep(STF, FW, TFW, sfwf_interp_order)

      end select

   end select

!-----------------------------------------------------------------------
!EOC

 end subroutine set_sfwf

!***********************************************************************
!BOP
! !IROUTINE: calc_sfwf_bulk_ncep
! !INTERFACE:

 subroutine calc_sfwf_bulk_ncep(STF, FW, TFW, time_dim)

! !DESCRIPTION:
!  Calculates surface freshwater flux from a combination of
!  air-sea fluxes (precipitation,  evaporation based on
!  latent heat flux computed in calc\_shf\_bulk\_ncep),
!  and restoring terms (due to restoring fields of SSS).
!
!  Notes:
!    the forcing data (on t-grid) are computed and
!    stored in SFWF\_DATA(:,:,sfwf\_comp\_*,now) where:
!       sfwf\_data\_sss    is   restoring SSS       (psu)
!       sfwf\_data\_precip is   precipitation       (m/y)
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) ::  &
      time_dim                ! number of time points for interpolation

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,nt,max_blocks_clinic), &
      intent(inout) :: &
      STF,     &! surface tracer fluxes for all tracers
      TFW       ! tracer concentration in fresh water flux

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), &
      intent(out) :: &
      FW        !  fresh water flux if using varthick sfc layer

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      now,               &! index for location of interpolated data
      k, n,              &! dummy loop indices
      iblock              ! block loop index

   real (r8) ::           &
      dttmp,              &! temporary time step variable
      fres_hor_ave,       &! area-weighted mean of weak restoring
      fres_hor_area,      &! total area of weak restoring
      area_glob_m_marg,   &! total ocean area - marginal sea area
      vol_glob_m_marg,    &! total ocean volume - marginal sea volume
      weak_mean            ! mean weak restoring

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic) :: &
      WORK1, WORK2   ! temporary work space

   type(block) :: &
      this_block  ! block info for current block

!-----------------------------------------------------------------------
!
!  sfwf_weak_restore= weak(non-ice) restoring h2o flux per msu  (kg/s/m^2/msu)
!  sfwf_strong_restore= strong  (ice) ..   ..  ..   ..  ..   ..
!
!  to calculate restoring factors, use mixed layer of 50m,
!  and restoring time constant tau (days):
!
!           F (kg/s/m^2/msu)
!   tau =   6  :     2.77
!   tau =  30  :     0.55
!   tau = 182.5:     0.092
!   tau = 365  :     0.046
!   tau = 730  :     0.023
!   tau = Inf  :     0.0
!
!-----------------------------------------------------------------------

!-----------------------------------------------------------------------
!
!  set location where interpolated data exists.
!
!-----------------------------------------------------------------------

   if (sfwf_data_type == 'annual') then
      now = 1
   else
      now = 0
   endif

!-----------------------------------------------------------------------
!
!  compute forcing terms for each block
!
!-----------------------------------------------------------------------

   !$OMP PARALLEL DO PRIVATE(iblock, this_block)
   do iblock=1,nblocks_clinic
      this_block = get_block(blocks_clinic(iblock),iblock)

      !***  compute evaporation from latent heat computed in shf forcing

      SFWF_COMP(:,:,iblock,sfwf_comp_evap) = &
       SHF_COMP(:,:,iblock,shf_comp_qlat)/latent_heat_vapor

      !*** precipitation (kg/m^2/s)

      SFWF_COMP(:,:,iblock,sfwf_comp_precip) = &
      SFWF_DATA(:,:,iblock,sfwf_data_precip,now)*precip_fact

      ! *c1000/seconds_in_year ! convert m/y to Kg/m^2/s if needed

      !*** weak salinity restoring term
      !*** (note: OCN_WGT = 0. at land points)
      !*** will be subtracting global mean later, so compute
      !*** necessary terms for global mean

      SFWF_COMP(:,:,iblock,sfwf_comp_wrest) = &
                         -sfwf_weak_restore*OCN_WGT(:,:,iblock)*    &
                         (SFWF_DATA(:,:,iblock,sfwf_data_sss,now) - &
                          TRACER(:,:,1,2,curtime,iblock))

      WORK1(:,:,iblock) = TAREA(:,:,iblock)* &
                          SFWF_COMP(:,:,iblock,sfwf_comp_wrest)
      WORK2(:,:,iblock) = TAREA(:,:,iblock)*OCN_WGT(:,:,iblock)

      !*** strong salinity restoring term

      where (KMT(:,:,iblock) > 0)
         SFWF_COMP(:,:,iblock,sfwf_comp_srest) = &
                     -sfwf_strong_restore*(c1 - OCN_WGT(:,:,iblock))* &
                      (SFWF_DATA(:,:,iblock,sfwf_data_sss,now) -      &
                       TRACER(:,:,1,2,curtime,iblock))
      elsewhere
         SFWF_COMP(:,:,iblock,sfwf_comp_srest) = c0
      endwhere

   end do ! block loop
   !$OMP END PARALLEL DO

!----------------------------------------------------------------------
!
!  compute global mean of weak restoring term
!
!----------------------------------------------------------------------

   fres_hor_ave  = global_sum(WORK1, distrb_clinic, field_loc_center)
   fres_hor_area = global_sum(WORK2, distrb_clinic, field_loc_center)
   weak_mean = fres_hor_ave/fres_hor_area

!-----------------------------------------------------------------------
!
!  finish computing forcing terms for each block
!
!-----------------------------------------------------------------------

   !$OMP PARALLEL DO PRIVATE(iblock, this_block)
   do iblock=1,nblocks_clinic
      this_block = get_block(blocks_clinic(iblock),iblock)

      !*** subtract mean from weak restoring term

      SFWF_COMP(:,:,iblock,sfwf_comp_wrest) = &
      SFWF_COMP(:,:,iblock,sfwf_comp_wrest) - OCN_WGT(:,:,iblock)* &
                                              weak_mean

      !  if variable thickness surface layer, compute net surface
      !  freshwater flux (kg/m^2/s) due to restoring terms only
      !  then compute freshwater flux due to P-E and convert to (m/s)
      !  then set the tracer content in the freshwater flux
      !  defaults are FW*SST for tracer 1 (temperature)
      !             0 for salinity (really fresh water)
      !             0 for all other tracers
      !
      !  IF DATA IS AVAILABLE...
      !  IMPLEMENT SUM OVER FRESHWATER TYPES (E,P,R,F,M) HERE:
      !
      !  TFW(:,:,n) = FW_EVAP*TW_EVAP(:,:,n) + FW_PRECIP*TW_PRECIP(:,:,n)
      !             + FW_ROFF*TW_ROFF(:,:,n) + FW_MELT*TW_MELT(:,:,n)
      !             + FW_FREEZE*TW_FREEZE(:,:,n)
      !
      !     where, for example  FW_ROFF is the water flux from rivers
      !     and TW_ROFF(:,:,n) is the concentration of the nth tracer
      !     in the river water;  similarly for water fluxes due to
      !     evaporation, precipitation, ice freezing, and ice melting.

      if (sfc_layer_type == sfc_layer_varthick .and. &
          .not. lfw_as_salt_flx) then

         STF(:,:,2,iblock) = SFWF_COMP(:,:,iblock,sfwf_comp_wrest) + &
                             SFWF_COMP(:,:,iblock,sfwf_comp_srest)

         FW(:,:,iblock) = OCN_WGT(:,:,iblock)*                      &
                          (SFWF_COMP(:,:,iblock,sfwf_comp_evap) +   &
                           SFWF_COMP(:,:,iblock,sfwf_comp_precip))* &
                          fwmass_to_fwflux

         !*** fw same temp as ocean and no tracers in FW input
         TFW(:,:,1,iblock) = FW(:,:,iblock)* &
                             TRACER(:,:,1,1,curtime,iblock)
         TFW(:,:,2:nt,iblock) = c0

      !*** if rigid lid or old free surface form, compute
      !*** net surface freshwater flux (kg/m^2/s)

      else

         STF(:,:,2,iblock) = OCN_WGT(:,:,iblock)*                    &
                          (SFWF_COMP(:,:,iblock,sfwf_comp_evap) +    &
                           SFWF_COMP(:,:,iblock,sfwf_comp_precip)) + &
                           SFWF_COMP(:,:,iblock,sfwf_comp_wrest)+    &
                           SFWF_COMP(:,:,iblock,sfwf_comp_srest)

      endif

      !*** convert surface freshwater flux (kg/m^2/s) to
      !*** salinity flux (msu*cm/s)

      STF(:,:,2,iblock) = STF(:,:,2,iblock)*salinity_factor

      !*** compute fields for accumulating annual-mean precipitation
      !*** over ocean points that are not marginal seas.

      WORK1(:,:,iblock) = merge(SFWF_COMP(:,:,iblock,sfwf_comp_precip)*&
                                TAREA(:,:,iblock)*OCN_WGT(:,:,iblock), &
                                c0, MASK_SR(:,:,iblock) > 0)

      !WORK2 = merge(FW_OLD(:,:,iblock)*TAREA(:,:,iblock), &
      !              c0, MASK_SR(:,:,iblock) > 0)

   end do ! block loop
   !$OMP END PARALLEL DO

!-----------------------------------------------------------------------
!
!  accumulate annual-mean precipitation over ocean points that are
!  not marginal seas.
!
!-----------------------------------------------------------------------

   if (avg_ts .or. back_to_back) then
      dttmp = p5*dtt
   else
      dttmp = dtt
   endif

   area_glob_m_marg = area_t - area_t_marg

   sum_precip = sum_precip + &
                dttmp*global_sum(WORK1,distrb_clinic,field_loc_center)/ &
                      area_glob_m_marg

   !sum_fw    = sum_fw     + &
   !            dttmp*global_sum(WORK2,distrb_clinic,field_loc_center)/ &
   !                  area_glob_m_marg

!-----------------------------------------------------------------------
!
!  Perform end of year adjustment calculations
!
!-----------------------------------------------------------------------

   if (eoy) then

      !*** Compute the surface volume-averaged salinity and
      !*** average surface height (for variable thickness sfc layer)
      !*** note that it is evaluated at the current time level.

      !$OMP PARALLEL DO PRIVATE(iblock, this_block)
      do iblock=1,nblocks_clinic
         this_block = get_block(blocks_clinic(iblock),iblock)

         if (sfc_layer_type == sfc_layer_varthick) then
            WORK1(:,:,iblock) = merge( &
                  TRACER(:,:,1,2,curtime,iblock)*TAREA(:,:,iblock)* &
                  (dz(1) + PSURF(:,:,curtime,iblock)/grav),         &
                 c0, KMT(:,:,iblock) > 0 .and. MASK_SR(:,:,iblock) > 0)
            WORK2(:,:,iblock) = merge(PSURF(:,:,curtime,iblock)*  &
                                      TAREA(:,:,iblock)/grav, c0, &
                                      KMT(:,:,iblock) > 0 .and.   &
                                      MASK_SR(:,:,iblock) > 0)
         else
            WORK1(:,:,iblock) = merge(TRACER(:,:,1,2,curtime,iblock)* &
                                      TAREA(:,:,iblock)*dz(1),        &
                                      c0, KMT(:,:,iblock) > 0 .and.   &
                                      MASK_SR(:,:,iblock) > 0)
         endif
      end do
      !$OMP END PARALLEL DO

      vol_glob_m_marg = volume_t_k(1) - volume_t_marg_k(1)
      sal_final(1) = global_sum(WORK1, distrb_clinic, field_loc_center)/ &
                     vol_glob_m_marg

      if (sfc_layer_type == sfc_layer_varthick) then
         ssh_final = global_sum(WORK2, distrb_clinic, field_loc_center)/ &
                     area_glob_m_marg/seconds_in_year
         if (my_task == master_task) then
            write(stdout,blank_fmt)
            write(stdout,'(a22,1pe23.15)') &
               'annual change in SSH: ', ssh_final
         endif
         ssh_final = ssh_final*10.0_r8 ! convert (cm/s) -> kg/m^2/s
                                       ! (cm/s)x0.01(m/cm)x1000kg/m^3
      else
         ssh_final = c0
      endif

      !*** Compute the volume-averaged salinity for each level.

      do k=2,km

         !$OMP PARALLEL DO PRIVATE(iblock, this_block)
         do iblock=1,nblocks_clinic
            this_block = get_block(blocks_clinic(iblock),iblock)

            if (partial_bottom_cells) then
               WORK1(:,:,iblock) =                               &
                  merge(TRACER(:,:,k,2,curtime,iblock)*          &
                        TAREA(:,:,iblock)*DZT(:,:,k,iblock), c0, &
                        k <= KMT(:,:,iblock) .and.               &
                        MASK_SR(:,:,iblock) > 0)
            else
               WORK1(:,:,iblock) =                               & 
                  merge(TRACER(:,:,k,2,curtime,iblock)*          &
                        TAREA(:,:,iblock)*dz(k), c0,             &
                        k <= KMT(:,:,iblock) .and.               &
                        MASK_SR(:,:,iblock) > 0)
            endif
         end do
         !$OMP END PARALLEL DO

         vol_glob_m_marg = volume_t_k(k) - volume_t_marg_k(k)
         if (vol_glob_m_marg == 0) vol_glob_m_marg = 1.e+20_r8
         sal_final(k) = global_sum(WORK1, distrb_clinic, field_loc_center)/ &
                        vol_glob_m_marg
      enddo

      !*** find annual mean precip and reset annual counters

      !ann_avg_fw = sum_fw / seconds_in_year
      !if (my_task == master_task) then
      !   write(stdout,blank_fmt)
      !   write(stdout,'(a32,1pe22.15)') &
      !     'annual average freshwater flux: ', ann_avg_fw
      !endif
      !sum_fw = c0

      ann_avg_precip = sum_precip / seconds_in_year
      sum_precip = c0

      if (ladjust_precip) call precip_adjustment

      sal_initial = sal_final
      ssh_initial = ssh_final

   endif ! end of year calculations

!-----------------------------------------------------------------------
!EOC

 end subroutine calc_sfwf_bulk_ncep

!***********************************************************************
!BOP
! !IROUTINE: precip_adjustment
! !INTERFACE:

 subroutine precip_adjustment

! !DESCRIPTION:
!  Computes a precipitation factor to multiply the fresh water flux
!  due to precipitation uniformly to insure a balance of fresh water
!  at the ocean surface.
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

   real (r8) ::  &
      sal_tendency, fw_tendency, precip_tav, &
      area_glob_m_marg, &! global ocean area - marginal sea area (cm^2)
      vol_glob_m_marg    ! global ocean vol  - marginal sea vol  (cm^3)

  integer (int_kind) :: k

!-----------------------------------------------------------------------
!
!  compute tendency of salinity for eack "k" layer, considering the
!  effects of depth acceleration
!
!-----------------------------------------------------------------------

   do k=1,km
      sal_initial(k) = (sal_final(k) - sal_initial(k))/ &
                       (dttxcel(k)*seconds_in_year)
   enddo

!-----------------------------------------------------------------------
!
!  form the global volume-averaged tendency to be used in "precip_fact"
!  computation
!
!-----------------------------------------------------------------------

   sal_tendency = c0
   do k=1,km
      vol_glob_m_marg = volume_t_k(k) - volume_t_marg_k(k)
      sal_tendency = sal_tendency + vol_glob_m_marg*sal_initial(k)
   enddo

   vol_glob_m_marg = volume_t - volume_t_marg
   sal_tendency = sal_tendency/vol_glob_m_marg

   if (my_task == master_task) then
      write (stdout,'(a58,1pe22.15)') &
         ' calc_sfwf_bulk_ncep: volume-averaged salinity tendency = ', &
         sal_tendency
   endif

!-----------------------------------------------------------------------
!
!  convert "sal_tendency" from (msu/s) to -(kg/m^2/s). note that
!  areag in cm^2 and volgt in cm^3
!  assumes density of fresh water = 1000 kg/m**3
!
!-----------------------------------------------------------------------

   area_glob_m_marg = area_t - area_t_marg
   sal_tendency = - sal_tendency*vol_glob_m_marg/area_glob_m_marg/ &
                    ocn_ref_salinity*1.0e4_r8

!-----------------------------------------------------------------------
!
!  compute annual change in mass due to freshwater flux (kg/m^2/s)
!
!-----------------------------------------------------------------------

   fw_tendency = ssh_final - ssh_initial

   if (my_task == master_task) then
      write (stdout,'(a22)') ' calc_sfwf_bulk_ncep: '
      write (stdout,'(a28,1pe22.15)') '   sal_tendency (kg/m^2/s): ', &
                                      sal_tendency
      write (stdout,'(a28,1pe22.15)') '    fw_tendency (kg/m^2/s): ', &
                                      fw_tendency
   endif

!-----------------------------------------------------------------------
!
!  change "precip_fact" based on tendency of freshwater and previous
!  amount of precipitation for stand-alone version only
!
!-----------------------------------------------------------------------

   precip_tav = ann_avg_precip/precip_fact

!  precip_fact = precip_fact - sal_tendency/precip_tav
   precip_fact = precip_fact - &
                 (sal_tendency + fw_tendency)/precip_tav

   if (my_task == master_task) then
      write (stdout,'(a33,e14.8)') ' Changed precipitation factor to ',&
                                  precip_fact
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine precip_adjustment

!***********************************************************************


 end module forcing_sfwf

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
