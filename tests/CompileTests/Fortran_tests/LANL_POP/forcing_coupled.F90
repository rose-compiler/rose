!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module forcing_coupled

!MODULE: forcing_coupled
! !DESCRIPTION:
!  This module contains all the routines necessary for coupling POP to
!  atmosphere and sea ice models using the NCAR CCSM flux coupler.  To
!  enable the routines in this module, the coupled ifdef option must
!  be specified during the make process.
!
! !REVISION HISTORY:
!  CVS:$Id: forcing_coupled.F90,v 1.9 2003/02/24 16:18:43 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $
!
! !USES:

   use kinds_mod
   use domain_size
   use domain
   use communicate
   use constants
   use broadcast
   use io
   use time_management
   use grid
   use prognostic
   use exit_mod
   use ice, only: tfreez, get_ice_flux
   use forcing_shf
   use forcing_sfwf
   !NCAR use qflux_mod
   !NCAR use ms_balance
   use timers
   use gather_scatter, only: scatter_global, gather_global
   use global_reductions, only: global_sum_prod
   !NCAR use shr_sys_mod

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_coupled,        &
             set_coupled_forcing

! !PUBLIC DATA MEMBERS:

   logical (log_kind), public ::   &
      lcoupled,            &! flag for coupled forcing
      ldiag_cpl             ! flag for computing coupled diagnostics

   integer (int_kind), public ::   &
      coupled_freq_iopt,   &! coupler frequency option
      coupled_freq          ! frequency of coupling

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  module variables
!
!-----------------------------------------------------------------------

#if coupled
   integer (int_kind) ::   &
      timer_send_to_cpl,   &
      timer_recv_from_cpl, &
      timer_recv_to_send,  &
      timer_send_to_recv
 
   integer (int_kind) ::   &
      cpl_stop_now,        &! flag id for stop_now flag
      cpl_ts,              &! flag id for coupled_ts flag
      cpl_write_restart,   &! flag id for write restart
      cpl_write_history,   &! flag id for write history
      cpl_write_tavg,      &! flag id for write tavg      
      cpl_diag_global,     &! flag id for computing diagnostics
      cpl_diag_transp       ! flag id for computing diagnostics

   integer (int_kind), parameter :: &
      nibuff       = 100,  &! size of integer control buffer
      nsfields     = 7,    &! number of fields sent  to   coupler
      nrfields     = 14,   &! number of fields recvd from coupler
      msgtype_d2oi = 51,   &! message type for cpl->ocn init
      msgtype_o2di = 60,   &! message type for ocn->cpl init
      msgtype_d2o  = 50,   &! message type for cpl->ocn
      msgtype_o2d  = 61     ! message type for ocn->cpl

   integer (int_kind), parameter :: &
      krbuff_ifrac =  1,   &! loc in recv buff of ice fraction
      krbuff_mslp  =  2,   &! loc in recv buff of mean sea-level press
      krbuff_taux  =  3,   &! loc in recv buff of taux field
      krbuff_tauy  =  4,   &! loc in recv buff of tauy field
      krbuff_netsw =  5,   &! loc in recv buff of shortwave heat flux
      krbuff_senhf =  6,   &! loc in recv buff of sensible  heat flux
      krbuff_lwup  =  7,   &! loc in recv buff of longwave  heat flux (up)
      krbuff_lwdwn =  8,   &! loc in recv buff of longwave  heat flux (dwn)
      krbuff_melthf=  9,   &! loc in recv buff of heat flux from snow&ice melt
      krbuff_salt  = 10,   &! loc in recv buff of salt flux (kg of salt)
      krbuff_prec  = 11,   &! loc in recv buff of precipitation flux
      krbuff_evap  = 12,   &! loc in recv buff of evaporation   flux
      krbuff_melt  = 13,   &! loc in recv buff of snow melt     flux
      krbuff_roff  = 14     ! loc in recv buff of river runoff  flux

   integer (int_kind), parameter :: &
      ksbuff_sst   = 1,    &! loc in send buff of sst
      ksbuff_sss   = 2,    &! loc in send buff of sst
      ksbuff_uvel  = 3,    &! loc in send buff of u vel
      ksbuff_vvel  = 4,    &! loc in send buff of v vel
      ksbuff_dhdx  = 5,    &! loc in send buff of e-w surface slope
      ksbuff_dhdy  = 6,    &! loc in send buff of n-s surface slope
      ksbuff_q     = 7      ! loc in send buff of freeze/melt heat flx

   real (r8) :: &
      tlast_coupled

   real (r8), &
      dimension(nx_block,ny_block,nsfields,max_blocks_clinic) :: &
      SBUFF_SUM           ! accumulated sum of send buffer quantities
                          ! for averaging before being sent

#endif

!EOC
!***********************************************************************

      contains

!***********************************************************************
!BOP
! !IROUTINE: init_coupled
! !INTERFACE:

 subroutine init_coupled(SMF, SMFT, STF, SHF_QSW, lsmft_avail)

! !DESCRIPTION:
!  This routine sets up everything necessary for coupling with
!  the NCAR flux coupler.
!
! !REVISION HISTORY:
!  same as coupled

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,2,max_blocks_clinic), &
      intent(inout) :: &
      SMF,             &!  surface momentum fluxes (wind stress)
      SMFT              !  surface momentum fluxes at T points

   real (r8), dimension(nx_block,ny_block,nt,max_blocks_clinic), &
      intent(inout) :: &
      STF                !  surface tracer fluxes

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), &
      intent(inout) :: &
      SHF_QSW          !  penetrative solar heat flux

   logical (log_kind), intent(inout) :: &
      lsmft_avail      ! true if SMFT is an available field

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   character (char_len) :: &
      coupled_freq_opt

   namelist /coupled_nml/ coupled_freq_opt, coupled_freq

   integer (int_kind) :: &
      i,j,k,             &! dummy loop index
      ncouple_per_day,   &! num of coupler comms per day
      nml_error           ! namelist i/o error flag

#if coupled

   include 'mpif.h'

   integer (int_kind) :: &
      iblock,            &! dummy block index
      ier                 ! error flag for messages

   integer (int_kind), dimension(nibuff) :: &
      ibuff               ! integer control buffer

   integer (int_kind), dimension(MPI_STATUS_SIZE) :: &
      mpistatus

   integer (int_kind), dimension(:,:), allocatable :: &
      IMASK_G             ! global ocean mask

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic) :: &
      LONSW,             &! longitude of SW corner
      LONSE,             &! longitude of SE corner
      LONNW,             &! longitude of NW corner
      LONNE               ! longitude of NE corner

   real (r8), dimension(:,:), allocatable :: &
      TLAT_G,            &! latitude  of cell center
      TLON_G              ! longitude of cell center

   real (r8), dimension(:,:,:), allocatable :: &
      TLON_V              ! coord of cell vertices

   type (block) :: this_block ! local block info
#endif
!-----------------------------------------------------------------------
!
!  read coupled_nml namelist to start coupling and determine
!  coupling frequency
!
!-----------------------------------------------------------------------

   lcoupled          = .false.
   coupled_freq_opt  = 'never'
   coupled_freq_iopt = freq_opt_never
   coupled_freq      = 100000

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=coupled_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR: reading coupled_nml')
   endif

   if (my_task == master_task) then

      write(stdout, delim_fmt)
      write(stdout,blank_fmt)
      write(stdout,'(a22)') 'Model coupling options'
      write(stdout,blank_fmt)
      write(stdout,delim_fmt)

      select case (coupled_freq_opt)

      case ('nyear')
         coupled_freq_iopt = -1000

      case ('nmonth')
         coupled_freq_iopt = -1000

      case ('nday')
         if (coupled_freq == 1) then
            lcoupled = .true.
            coupled_freq_iopt = freq_opt_nday
            ncouple_per_day = 1
         else
            coupled_freq_iopt = -1000
         endif

      case ('nhour')
         if (coupled_freq <= 24) then
            lcoupled = .true.
            coupled_freq_iopt = freq_opt_nhour
            ncouple_per_day = 24/coupled_freq
         else
            coupled_freq_iopt = -1000
         endif

      case ('nsecond')
         if (coupled_freq <= seconds_in_day) then
            lcoupled = .true.
            coupled_freq_iopt = freq_opt_nsecond
            ncouple_per_day = seconds_in_day/coupled_freq
         else
            coupled_freq_iopt = -1000
         endif

      case ('nstep')
         if (coupled_freq <= nsteps_per_day) then
            lcoupled = .true.
            coupled_freq_iopt = freq_opt_nstep
            ncouple_per_day = nsteps_per_day/coupled_freq
         else
            coupled_freq_iopt = -1000
         endif

      case ('never')
         lcoupled = .false.

      case default
         coupled_freq_iopt = -2000
      end select

   endif

   call broadcast_scalar(lcoupled,          master_task)
   call broadcast_scalar(coupled_freq_iopt, master_task)
   call broadcast_scalar(coupled_freq     , master_task)

   if (coupled_freq_iopt == -1000) then
      call exit_POP(sigAbort, &
             'ERROR: Coupling frequency must be at least once per day')
   else if (coupled_freq_iopt == -2000) then
      call exit_POP(sigAbort, &
                    'ERROR: Unknown option for coupling frequency')
   endif

#if coupled
   if (.not. lcoupled) then
      call exit_POP(sigAbort, &
            'ERROR: Coupled ifdef option enabled but lcoupled=false')
   endif

!-----------------------------------------------------------------------
!
!  Initialize flags and shortwave absorption profile
!
!-----------------------------------------------------------------------

   cpl_stop_now    = init_time_flag('stop_now', default=.false.)
   cpl_ts          = init_time_flag('coupled_ts',default=.false.,  &
                                     freq_opt = coupled_freq_iopt, &
                                     freq     = coupled_freq)
   cpl_write_restart = init_time_flag('restart')
   cpl_write_history = init_time_flag('history')
   cpl_write_tavg    = init_time_flag('tavg')
   cpl_diag_global   = init_time_flag('diag_global')
   cpl_diag_transp   = init_time_flag('diag_transp')

   lsmft_avail = .true.
   tlast_coupled = c0

   lsw_absorb = .true.
   if (.not. allocated(sw_absorb)) allocate(sw_absorb(0:km))
   sw_absorb(0) = c1
   sw_absorb(km) = c0
   do k = 1, km-1
      call sw_absorb_frac(zw(k),sw_absorb(k))
   enddo

!-----------------------------------------------------------------------
!
!  receive initial buffer from coupler and ignore content
!
!-----------------------------------------------------------------------

   if (my_task == master_task) then
      call MPI_RECV(ibuff, nibuff, MPI_INTEGER, cpl_task, &
                    msgtype_d2oi, MPI_COMM_WORLD, mpistatus, ier)
   endif

   call broadcast_scalar(ier, master_task)
   if (ier < MPI_SUCCESS ) then
      call exit_POP(sigAbort,'ERROR in coupler after initial recv')
   endif

!-----------------------------------------------------------------------
!
!  initialize and send buffer
!
!-----------------------------------------------------------------------

   if (my_task == master_task) then
      ibuff = 0

      ibuff(4) = iyear*10000 + imonth*100 + iday
      ibuff(5) = ihour*seconds_in_hour + iminute*seconds_in_minute &
               + isecond
      ibuff(7) = nx_global
      ibuff(8) = ny_global
      ibuff(9) = ncouple_per_day

      call MPI_SEND(ibuff, nibuff, MPI_INTEGER, cpl_task,   &
                           msgtype_o2di, MPI_COMM_WORLD, ier)

   endif

   call broadcast_scalar(ier, master_task)
   if (ier < MPI_SUCCESS ) then
      call exit_POP(sigAbort, &
                    'ERROR in coupler after initial ibuff send')
   endif

!-----------------------------------------------------------------------
!
!  initialize and send grid center coordinates in degrees
!
!-----------------------------------------------------------------------

   !***
   !*** use LONSE, LONSW as temps to hold TLAT, TLON
   !***

   !$OMP PARALLEL DO
   do iblock=1,nblocks_clinic
      LONSW(:,:,iblock) = radian*TLON(:,:,iblock)
      LONSE(:,:,iblock) = radian*TLAT(:,:,iblock)
   end do
   !$OMP END PARALLEL DO

   allocate(TLON_G(nx_global,ny_global), &
            TLAT_G(nx_global,ny_global))

   call gather_global(TLON_G, LONSW, master_task, distrb_clinic)
   call gather_global(TLAT_G, LONSE, master_task, distrb_clinic)

   if (my_task == master_task) then
       call MPI_SEND(TLON_G, nx_global*ny_global,  &
                             MPI_DOUBLE_PRECISION, &
                     cpl_task, msgtype_o2di, MPI_COMM_WORLD, ier)

       call MPI_SEND(TLAT_G, nx_global*ny_global,  &
                             MPI_DOUBLE_PRECISION, &
                     cpl_task, msgtype_o2di, MPI_COMM_WORLD, ier)

   endif

   call broadcast_scalar(ier, master_task)
   if (ier < MPI_SUCCESS ) then
      call exit_POP(sigAbort, &
            'ERROR in coupler after initial send of center coords')
   endif

   deallocate(TLON_G, TLAT_G)

!-----------------------------------------------------------------------
!
!  initialize and send grid vertex coordinates in degrees
!
!-----------------------------------------------------------------------

   !$OMP PARALLEL DO PRIVATE(iblock, i, j, this_block)
   do iblock=1,nblocks_clinic

      this_block = get_block(blocks_clinic(iblock),iblock)

      do j=this_block%jb,this_block%je
      do i=this_block%ib,this_block%ie
      
         LONSE(i,j,iblock) = ULON(i  ,j-1,iblock)
         LONNW(i,j,iblock) = ULON(i-1,j  ,iblock)
         LONSW(i,j,iblock) = ULON(i-1,j-1,iblock)

         if (LONSE(i,j,iblock) - ULON (i,j,iblock) >  c4) &
             LONSE(i,j,iblock) = LONSE(i,j,iblock) - pi2
         if (LONSE(i,j,iblock) - ULON (i,j,iblock) < -c4) &
             LONSE(i,j,iblock) = LONSE(i,j,iblock) + pi2
         if (LONNW(i,j,iblock) - ULON (i,j,iblock) >  c4) &
             LONNW(i,j,iblock) = LONNW(i,j,iblock) - pi2
         if (LONNW(i,j,iblock) - ULON (i,j,iblock) < -c4) &
             LONNW(i,j,iblock) = LONNW(i,j,iblock) + pi2
         if (LONSW(i,j,iblock) - ULON (i,j,iblock) >  c4) &
             LONSW(i,j,iblock) = LONSW(i,j,iblock) - pi2
         if (LONSW(i,j,iblock) - ULON (i,j,iblock) < -c4) &
             LONSW(i,j,iblock) = LONSW(i,j,iblock) + pi2

         LONNE(i,j,iblock) = radian*ULON (i,j,iblock)
         LONNW(i,j,iblock) = radian*LONNW(i,j,iblock)
         LONSE(i,j,iblock) = radian*LONSE(i,j,iblock)
         LONSW(i,j,iblock) = radian*LONSW(i,j,iblock)
      end do
      end do
   end do
   !$OMP END PARALLEL DO

   allocate(TLON_V(4,nx_global,ny_global))

   call gather_global(TLON_V(1,:,:),LONSW,master_task,distrb_clinic)
   call gather_global(TLON_V(2,:,:),LONSE,master_task,distrb_clinic)
   call gather_global(TLON_V(3,:,:),LONNE,master_task,distrb_clinic)
   call gather_global(TLON_V(4,:,:),LONNW,master_task,distrb_clinic)

   if (my_task == master_task) then
      call MPI_SEND(TLON_V, 4*nx_global*ny_global,  &
                              MPI_DOUBLE_PRECISION, &
                    cpl_task, msgtype_o2di, MPI_COMM_WORLD, ier)
   endif

!-----------------------------------------------------------------------
!
!  do latitudes now using LON arrays as temps
!
!-----------------------------------------------------------------------

   !$OMP PARALLEL DO PRIVATE(iblock, i, j, this_block)
   do iblock=1,nblocks_clinic

      this_block = get_block(blocks_clinic(iblock),iblock)

      do j=this_block%jb,this_block%je
      do i=this_block%ib,this_block%ie
      
         LONNE(i,j,iblock) = radian*ULAT(i  ,j  ,iblock)
         LONSE(i,j,iblock) = radian*ULAT(i  ,j-1,iblock)
         LONNW(i,j,iblock) = radian*ULAT(i-1,j  ,iblock)
         LONSW(i,j,iblock) = radian*ULAT(i-1,j-1,iblock)

      end do
      end do
   end do
   !$OMP END PARALLEL DO

   call gather_global(TLON_V(1,:,:),LONSW,master_task,distrb_clinic)
   call gather_global(TLON_V(2,:,:),LONSE,master_task,distrb_clinic)
   call gather_global(TLON_V(3,:,:),LONNE,master_task,distrb_clinic)
   call gather_global(TLON_V(4,:,:),LONNW,master_task,distrb_clinic)

   if (my_task == master_task) then
      call MPI_SEND(TLON_V, 4*nx_global*ny_global, &
                            MPI_DOUBLE_PRECISION,  &
                  cpl_task, msgtype_o2di, MPI_COMM_WORLD, ier)
   endif

   call broadcast_scalar(ier, master_task)
   if (ier < MPI_SUCCESS ) then
      call exit_POP(sigAbort, &
             'ERROR in coupler after initial send of vertex coords')
   endif

   deallocate(TLON_V)

!-----------------------------------------------------------------------
!
!  initialize and send T-grid areas (TAREA) in square radians
!  create and send imask
!
!-----------------------------------------------------------------------

   !***
   !*** use LONSW and TLON_G as temporaries
   !***

   LONSW = TAREA/(radius*radius)

   allocate( TLON_G(nx_global,ny_global), &
            IMASK_G(nx_global,ny_global))

   call gather_global( TLON_G, LONSW,       master_task, distrb_clinic)
   call gather_global(IMASK_G, REGION_MASK, master_task, distrb_clinic)

   if (my_task == master_task) then
      call MPI_SEND(TLON_G, nx_global*ny_global,  &
                            MPI_DOUBLE_PRECISION, &
                    cpl_task, msgtype_o2di, MPI_COMM_WORLD, ier)
   endif

   call broadcast_scalar(ier, master_task)
   if (ier < MPI_SUCCESS ) then
      call exit_POP(sigAbort, &
             'ERROR in coupler after initial send of T-grid areas')
   endif

   if (my_task == master_task) then

      call MPI_SEND(IMASK_G, nx_global*ny_global, MPI_INTEGER, &
                    cpl_task, msgtype_o2di, MPI_COMM_WORLD, ier)

      if (ier == MPI_SUCCESS ) then
         write(stdout,*) '(ocn) Sent grid info'
      endif

   endif

   call broadcast_scalar(ier, master_task)
   if (ier < MPI_SUCCESS ) then
      call exit_POP(sigAbort, &
             'ERROR in coupler after initial send of land mask ')
   endif

   deallocate(TLON_G, IMASK_G)

!-----------------------------------------------------------------------
!
!  send initial state info to coupler
!
!-----------------------------------------------------------------------

   call sum_buffer
   call send_to_coupler

!-----------------------------------------------------------------------
!
!  initialize timers for coupled model
!
!-----------------------------------------------------------------------
      
   call get_timer (timer_send_to_cpl  , 'SEND'        , 1, &
                                         distrb_clinic%nprocs)
   call get_timer (timer_recv_from_cpl, 'RECV'        , 1, &
                                         distrb_clinic%nprocs)
   call get_timer (timer_recv_to_send , 'RECV to SEND', 1, &
                                         distrb_clinic%nprocs)
   call get_timer (timer_send_to_recv , 'SEND to RECV', 1, &
                                         distrb_clinic%nprocs)
#endif
!-----------------------------------------------------------------------
!EOC

 end subroutine init_coupled

!***********************************************************************
!BOP
! !IROUTINE: set_coupled_forcing
! !INTERFACE:

 subroutine set_coupled_forcing(SMF,SMFT,STF,SHF_QSW,FW,TFW,IFRAC)

! !DESCRIPTION:
!  This routine calls communicates with NCAR flux coupler to set
!  surface forcing data
!
! !REVISION HISTORY:
!  same as module

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,2,max_blocks_clinic), &
      intent(inout) :: &
      SMF,             &!  surface momentum fluxes (wind stress)
      SMFT              !  surface momentum fluxes at T points

   real (r8), dimension(nx_block,ny_block,nt,max_blocks_clinic), &
      intent(inout) :: &
      STF,             &!  surface tracer fluxes
      TFW               !  tracer concentration in water flux

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), &
      intent(inout) :: &
      SHF_QSW,         &!  penetrative solar heat flux
      FW,              &!  fresh water flux
      IFRAC             !  fractional ice coverage

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  exit if coupling not enabled
!
!-----------------------------------------------------------------------

   if (.not. lcoupled) return

#if coupled
!-----------------------------------------------------------------------
!
!  if it is time to couple, exchange data with flux coupler
!  be sure to trigger communication on very first time step
!
!-----------------------------------------------------------------------

   call sum_buffer

   if (check_time_flag(cpl_ts) .or. nsteps_run == 0) then

      !*** send state variables at end of coupling interval

      if (nsteps_run /= 0) then
         call timer_stop  (timer_recv_to_send)

         call timer_start (timer_send_to_cpl)
         call send_to_coupler
         call timer_stop  (timer_send_to_cpl)
      endif

      !*** recv data to advance next time step

      call timer_start (timer_recv_from_cpl)
      call recv_from_coupler(SMF,SMFT,STF,SHF_QSW,FW,TFW,IFRAC)
      call timer_stop  (timer_recv_from_cpl)

      call timer_start (timer_recv_to_send)

      !NCARif ( shf_formulation == 'partially-coupled' ) then
      !NCAR   SHF_COMP(:,:,shf_comp_cpl) = STF(:,:,1) 
      !NCAR   if ( .not. lms_balance ) then
      !NCAR      SHF_COMP(:,:,shf_comp_cpl) = & 
      !NCAR      SHF_COMP(:,:,shf_comp_cpl) * MASK_SR
      !NCAR      SHF_QSW = SHF_QSW * MASK_SR
      !NCAR   endif
      !NCARendif
 
      !NCARif ( sfwf_formulation == 'partially-coupled' ) then

      !NCAR   if (sfc_layer_type == sfc_layer_varthick .and. &
      !NCAR       .not. lfw_as_salt_flx) then
      !NCAR      SFWF_COMP(:,:,sfwf_comp_cpl) =  FW * MASK_SR
      !NCAR      do n=1,nt
      !NCAR        TFW_COMP(:,:,n,tfw_comp_cpl) = TFW(:,:,n) * MASK_SR
      !NCAR      enddo
      !NCAR   else
      !NCAR      SFWF_COMP(:,:,sfwf_comp_cpl) = STF(:,:,2) * MASK_SR
      !NCAR   endif

      !NCARelse

      !NCAR   if ( sfc_layer_type == sfc_layer_varthick .and. &
      !NCAR       .not. lfw_as_salt_flx .and. liceform ) then
      !NCAR      SFWF_COMP(:,:,sfwf_comp_cpl) = FW
      !NCAR      TFW_COMP(:,:,:,tfw_comp_cpl) = TFW
      !NCAR   endif

      !NCARendif
 
      !NCARif ( luse_cpl_ifrac ) then
      !NCAR   OCN_WGT = (c1-IFRAC) * RCALCT
      !NCARendif
 
   endif

#endif
!-----------------------------------------------------------------------
!EOC

 end subroutine set_coupled_forcing

#if coupled
!***********************************************************************
!BOP
! !IROUTINE: recv_from_coupler
! !INTERFACE:

 subroutine recv_from_coupler(SMF,SMFT,STF,SHF_QSW,FW,TFW,IFRAC)

! !DESCRIPTION:
!  This routine receives message from NCAR flux coupler with surface
!  flux data.
!
! !REVISION HISTORY:
!  same as module

! !INCLUDES:

   include 'mpif.h'

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(:,:,:,:), intent(inout) :: &
      SMF,              &!  surface momentum fluxes (wind stress)
      SMFT,             &!  surface momentum fluxes at T points
      STF,              &!  surface tracer fluxes
      TFW                !  tracer concentration in water flux

   real (r8), dimension(:,:,:), intent(inout) :: &
      SHF_QSW,          &!  penetrative solar heat flux
      FW,               &!  fresh water flux
      IFRAC              !  fractional ice coverage

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!  contents of ibuff values received from cpl5:
!
!     ibuff( 1) = ierr         error code, 0 => no error
!     ibuff( 2) = stop_eod     flag: stop at end of day
!     ibuff( 3) = stop_now     flag: stop now
!     ibuff( 4) = cdate        date (encoded: yyyymmdd)
!     ibuff( 5) = sec          seconds
!     ibuff(11) = info_time    flag: mp timing
!     ibuff(12) = info_dbug    dbug diagnostic level
!     ibuff(21) = rest_eod     flag: make restart data at eod
!     ibuff(22) = rest_now     flag: make restart data now
!     ibuff(23) = hist_eod     flag: make history data at eod
!     ibuff(24) = hist_now     flag: make history data now
!     ibuff(25) = hist_tavg    flag: make monthly average data
!     ibuff(26) = diag_eod     flag: make diagnostic at eod
!     ibuff(27) = diag_now     flag: make diagnostic data now
!     ibuff(32) = ashift       albedo time shift
!
!-----------------------------------------------------------------------

   integer (int_kind), dimension(nibuff) :: &
      ibuff                  ! integer control buffer

   integer (int_kind), parameter ::  &! words in received msg
      nrwords = nrfields*nx_global*ny_global

   integer (int_kind), dimension(MPI_STATUS_SIZE) :: &
      mpistatus

   integer (int_kind) ::  &
      iblock,             &! local block index
      ier,                &! error flag for message calls
      n

   real (r8), dimension(nx_global,ny_global,nrfields) :: &
      RBUFF               ! received message buffer

   real (r8), dimension(nx_block,ny_block) :: &
      WORK,              &! local work space
      TFRZ                ! space to hold local freezing temp

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic) :: &
      TAUX_F,          &! wind stress   flux -- from cpl   (N/m2/s)
      TAUY_F,          &! wind stress   flux -- from cpl   (N/m2/s)
      EVAP_F,          &! evaporation   flux -- from cpl   (kg/m2/s)
      PREC_F,          &! precipitation flux -- from cpl   (kg/m2/s)
      MELT_F,          &! melt          flux -- from cpl   (kg/m2/s)
      ROFF_F,          &! river runoff  flux -- from cpl   (kg/m2/s)
      SALT_F,          &! salt          flux -- from cpl   (kg/m2/s)
      QSW_F ,          &! shortwave     flux -- from cpl   (W/m2/s)
      SENH_F,          &! sensible heat flux    -- from cpl (W/m2   )
      LWUP_F,          &! longwave heat flux up -- from cpl (W/m2   )
      LWDN_F,          &! longwave heat flux dn -- from cpl (W/m2   )
      MELTH_F,         &! melt     heat flux    -- from cpl (W/m2   )
      MSLP              ! sea level pressure (ignored currently)

!-----------------------------------------------------------------------
!
!  receive message from coupler and check for terminate signal
!
!-----------------------------------------------------------------------

   if (my_task == master_task) then
      call MPI_RECV(ibuff, nibuff, MPI_INTEGER, cpl_task, &
                    msgtype_d2o, MPI_COMM_WORLD, mpistatus, ier)

      call MPI_RECV(RBUFF, nrwords, MPI_DOUBLE_PRECISION, cpl_task, &
                    msgtype_d2o, MPI_COMM_WORLD, mpistatus, ier)

   endif

   call broadcast_scalar(ier, master_task)
   if (ier /= MPI_SUCCESS) then
      call exit_POP(sigAbort,'ERROR recieving message from coupler')
   end if

!-----------------------------------------------------------------------
!
!  broadcast all coupler flags and respond appropriately
!
!-----------------------------------------------------------------------

   call broadcast_array(ibuff, master_task)

   !***
   !*** received terminate signal, set stop_now flag and exit
   !***

   if (ibuff(3) == 1) then
      call set_time_flag(cpl_stop_now,.true.)
      if (my_task == master_task) then
         !*** these defined already in time manager?
         !call int_to_char(4, iyear   ,cyear )
         !call int_to_char(2, imonth  ,cmonth )
         !call int_to_char(2, iday    ,cday   )
         !call int_to_char(2, ihour   ,chour  )
         !call int_to_char(2, iminute ,cminute)
         !call int_to_char(2, isecond ,csecond)
         write(stdout,'(a30,1x,a4,a1,a2,a1,a2,1x,a2,a1,a2,a1,a2)') &
               ' cpl requests termination now:', &
               cyear,'/',cmonth,'/',cday,        &
               chour,':',cminute,':',csecond
      endif
      !call exit_POP(sigAbort,'Received terminate signal from coupler')
      return
   endif

   if (ibuff(12) >= 2) then
      ldiag_cpl = .true. 
   else
      ldiag_cpl = .false.
   endif

   if (ibuff(21) == 1) then
      if (eod) call set_time_flag(cpl_write_restart,.true.)
      if (my_task == master_task) then
         write(stdout,'(a34,1x,a4,a1,a2,a1,a2)') &
               ' cpl requests restart file at eod ', &
               cyear,'/',cmonth,'/',cday
      endif
   endif

   !*** ignore these two flags for now
   !
   !if (ibuff(23) == 1) then
   !   if (eod) call set_time_flag(cpl_write_history,.true.)
   !   !*** these defined already in time manager?
   !   !call int_to_char(4, iyear   , cyear )
   !   !call int_to_char(2, imonth  ,cmonth )
   !   !call int_to_char(2, iday    ,cday   )
   !   !call int_to_char(2, ihour   ,chour  )
   !   !call int_to_char(2, iminute ,cminute)
   !   !call int_to_char(2, isecond ,csecond)
   !   if (my_task == master_task) then
   !      write(stdout,'(a34,1x,a4,a1,a2,a1,a2)') &
   !            ' cpl requests history file at eod ', &
   !            cyear,'/',cmonth,'/',cday
   !   endif
   !endif
   !
   !if (ibuff(25) == 1) then
   !   if (eod) call set_time_flag(cpl_write_tavg,.true.)
   !   !*** these defined already in time manager?
   !   !call int_to_char(4, iyear   , cyear )
   !   !call int_to_char(2, imonth  ,cmonth )
   !   !call int_to_char(2, iday    ,cday   )
   !   !call int_to_char(2, ihour   ,chour  )
   !   !call int_to_char(2, iminute ,cminute)
   !   !call int_to_char(2, isecond ,csecond)
   !   if (my_task == master_task) then
   !      write(stdout,'(a34,1x,a4,a1,a2,a1,a2)') &
   !            ' cpl requests tavg    file at eod ', &
   !            cyear,'/',cmonth,'/',cday
   !   endif
   !endif
   !

   if (ibuff(26) == 1) then
      call set_time_flag(cpl_diag_global,.true.)
      call set_time_flag(cpl_diag_transp,.true.)
      !*** these defined already in time manager?
      !call int_to_char(4, iyear   ,cyear  )
      !call int_to_char(2, imonth  ,cmonth )
      !call int_to_char(2, iday    ,cday   )
      !call int_to_char(2, ihour   ,chour  )
      !call int_to_char(2, iminute ,cminute)
      !call int_to_char(2, isecond ,csecond)
      if (my_task == master_task) then
         write(stdout,'(a38,1x,a4,a1,a2,a1,a2)') &
               ' cpl requests diagnostics file at eod ', &
               cyear,'/',cmonth,'/',cday
      endif
   endif

!-----------------------------------------------------------------------
!
!  unpack a variety of fields from the message buffer
!
!-----------------------------------------------------------------------

   call scatter_global(TAUX_F, RBUFF(:,:,krbuff_taux  ), master_task, &
                     distrb_clinic, field_loc_center, field_type_vector)
   call scatter_global(TAUY_F, RBUFF(:,:,krbuff_tauy  ), master_task, &
                     distrb_clinic, field_loc_center, field_type_vector)
   call scatter_global(PREC_F, RBUFF(:,:,krbuff_prec  ), master_task, &
                     distrb_clinic, field_loc_center, field_type_scalar)
   call scatter_global(EVAP_F, RBUFF(:,:,krbuff_evap  ), master_task, &
                     distrb_clinic, field_loc_center, field_type_scalar)
   call scatter_global(MELT_F, RBUFF(:,:,krbuff_melt  ), master_task, &
                     distrb_clinic, field_loc_center, field_type_scalar)
   call scatter_global(ROFF_F, RBUFF(:,:,krbuff_roff  ), master_task, &
                     distrb_clinic, field_loc_center, field_type_scalar)
   call scatter_global(SALT_F, RBUFF(:,:,krbuff_salt  ), master_task, &
                     distrb_clinic, field_loc_center, field_type_scalar)
   call scatter_global( QSW_F, RBUFF(:,:,krbuff_netsw ), master_task, &
                     distrb_clinic, field_loc_center, field_type_scalar)
   call scatter_global(SENH_F, RBUFF(:,:,krbuff_senhf ), master_task, &
                     distrb_clinic, field_loc_center, field_type_scalar)
   call scatter_global(LWUP_F, RBUFF(:,:,krbuff_lwup  ), master_task, &
                     distrb_clinic, field_loc_center, field_type_scalar)
   call scatter_global(LWDN_F, RBUFF(:,:,krbuff_lwdwn ), master_task, &
                     distrb_clinic, field_loc_center, field_type_scalar)
   call scatter_global(MELTH_F,RBUFF(:,:,krbuff_melthf), master_task, &
                     distrb_clinic, field_loc_center, field_type_scalar)
   call scatter_global(IFRAC,  RBUFF(:,:,krbuff_ifrac ), master_task, &
                     distrb_clinic, field_loc_center, field_type_scalar)
   call scatter_global(MSLP ,  RBUFF(:,:,krbuff_mslp  ), master_task, &
                     distrb_clinic, field_loc_center, field_type_scalar)

!-----------------------------------------------------------------------
!
!  convert to correct units
!  if necessary, rotate components to logical coordinates
!
!-----------------------------------------------------------------------


   !$OMP PARALLEL DO PRIVATE(iblock,n,WORK)

   do iblock=1,nblocks_clinic

      IFRAC(:,:,iblock) = IFRAC(:,:,iblock)*RCALCT(:,:,iblock)

      !***
      !*** convert units for net shortwave from W/m2
      !***

      SHF_QSW(:,:,iblock) = QSW_F(:,:,iblock)*RCALCT(:,:,iblock)* &
                            hflux_factor

      !***
      !*** Rotate true zonal/meridional wind stress into local
      !*** coordinates and convert to dyne/cm**2
      !***

      SMFT(:,:,1,iblock)=(TAUX_F(:,:,iblock)*cos(ANGLET(:,:,iblock)) +&
                          TAUY_F(:,:,iblock)*sin(ANGLET(:,:,iblock)))*&
                          RCALCT(:,:,iblock)*momentum_factor
      SMFT(:,:,2,iblock)=(TAUY_F(:,:,iblock)*cos(ANGLET(:,:,iblock)) -&
                          TAUX_F(:,:,iblock)*sin(ANGLET(:,:,iblock)))*&
                          RCALCT(:,:,iblock)*momentum_factor
 
      !***
      !*** Shift SMFT to U grid
      !***

      call tgrid_to_ugrid(SMF(:,:,1,iblock),SMFT(:,:,1,iblock),iblock)
      call tgrid_to_ugrid(SMF(:,:,2,iblock),SMFT(:,:,2,iblock),iblock)

      !***
      !***  for variable thickness surface layer, compute fresh water
      !***  and salt fluxes
      !***

      if (sfc_layer_type == sfc_layer_varthick .and. & 
          .not. lfw_as_salt_flx) then

         !*** compute fresh water flux (cm/s)

         FW(:,:,iblock) = RCALCT(:,:,iblock)* &
                          (PREC_F(:,:,iblock) + EVAP_F(:,:,iblock) + &
                           ROFF_F(:,:,iblock))*fwmass_to_fwflux

         WORK = RCALCT(:,:,iblock)*MELT_F(:,:,iblock)*fwmass_to_fwflux

         !*** compute tracer concentration in fresh water
         !*** in principle, temperature of each water flux
         !*** could be different. e.g.
         !TFW(:,:,1) = RCALCT*(PREC_F*TEMP_PREC +
         !                     EVAP_F*TEMP_EVAP +
         !                     MELT_F*TEMP_MELT +
         !                     ROFF_F*TEMP_ROFF)*fwmass_to_fwflux

         !*** currently atm/land water comes in at sea surface temp
         !*** meltwater comes in at melting temp

         call tfreez(TFRZ,TRACER(:,:,1,2,curtime,iblock))

         TFW(:,:,1,iblock) = FW(:,:,iblock)* &
                             TRACER(:,:,1,1,curtime,iblock) + &
                             WORK*TFRZ

         FW(:,:,iblock) = FW(:,:,iblock) + WORK


         !*** compute salt flux
         !*** again, salinity could be different for each
         !***   component of water flux
         !TFW(:,:,1) = RCALCT*(PREC_F*SALT_PREC +
         !                     EVAP_F*SALT_EVAP +
         !                     MELT_F*SALT_MELT +
         !                     ROFF_F*SALT_ROFF)*fwmass_to_fwflux
         !*** currently assume prec, evap and roff are fresh
         !*** and all salt come from ice melt

         where (MELT_F(:,:,iblock) /= c0)
            !*** salinity (msu) of melt water
            WORK = SALT_F(:,:,iblock)/MELT_F(:,:,iblock)
         elsewhere
            WORK = c0
         end where

         TFW(:,:,2,iblock) =  RCALCT(:,:,iblock)*MELT_F(:,:,iblock)* &
                              fwmass_to_fwflux*WORK
                              ! + PREC_F*c0 + EVAP_F*c0 + ROFF_F*c0

         do n=3,nt
            TFW(:,:,n,iblock) = c0  ! no other tracers in fresh water
         end do

      !***
      !***  if not a variable thickness surface layer or if 
      !***  fw_as_salt_flx flag is on, convert fresh and salt inputs
      !***  to a virtual salinity flux
      !***

      else  ! convert fresh water to virtual salinity flux

         STF(:,:,2,iblock) = RCALCT(:,:,iblock)*(  &
                            (PREC_F(:,:,iblock) + EVAP_F(:,:,iblock) + &
                             MELT_F(:,:,iblock) + ROFF_F(:,:,iblock))* &
                             salinity_factor                           &
                           + SALT_F(:,:,iblock)*sflux_factor )
      endif

      !***
      !*** combine and convert heat fluxes from W/m**2
      !***    (note: latent heat flux = evaporation*latent_heat_vapor)
      !***

      STF(:,:,1,iblock) = (EVAP_F(:,:,iblock)*latent_heat_vapor + &
                           SENH_F(:,:,iblock) + LWUP_F (:,:,iblock) + &
                           LWDN_F(:,:,iblock) + MELTH_F(:,:,iblock))* &
                           RCALCT(:,:,iblock)*hflux_factor

   end do
   !$OMP END PARALLEL DO

!-----------------------------------------------------------------------
!
!     balance salt/freshwater in marginal seas
!
!-----------------------------------------------------------------------
!NCAR 
!NCAR      if  (lms_balance .and. sfwf_formulation /= 'partially-coupled' )
!NCAR     & call ms_balancing (STF(:,:,2),EVAP_F, PREC_F, MELT_F,ROFF_F
!NCAR     &,                   SALT_F, 'salt')
!NCAR 
!NCAR      endif
!NCAR
!-----------------------------------------------------------------------
!
!  compute coupled diagnostics
!
!-----------------------------------------------------------------------

   if (ldiag_cpl) &
      call coupled_diagnostics_recv(IFRAC  , MSLP  , TAUX_F, TAUY_F, &
                                    QSW_F  , SENH_F, LWUP_F, LWDN_F, &
                                    MELTH_F, SALT_F, PREC_F, EVAP_F, &
                                    MELT_F , ROFF_F)

!-----------------------------------------------------------------------
!EOC

 end subroutine recv_from_coupler

!***********************************************************************
!BOP
! !IROUTINE: send_to_coupler
! !INTERFACE:

 subroutine send_to_coupler

! !DESCRIPTION:
!  This routine packs fields into a message buffer and sends the
!  message to the NCAR flux coupler.
!
! !REVISION HISTORY:
!  same as module

! !INCLUDES:

   include 'mpif.h'

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind), dimension(nibuff) :: &
      ibuff                  ! integer control buffer

   integer (int_kind), parameter ::  &! words in sent msg
      nswords = nsfields*nx_global*ny_global

   integer (int_kind), dimension(MPI_STATUS_SIZE) :: &
      mpistatus

   integer (int_kind) :: &
      iblock,            &! local block index
      ier                 ! error flag for message calls

   real (r8), dimension(nx_global,ny_global,nsfields) :: &
      SBUFF               ! sent message buffer

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic) :: &
      UUU,               &! U velocity at T points
      VVV,               &! U velocity at T points
      SST,               &! sea surface temperature
      SSS,               &! sea surface salinity
      DHDX,              &! e-w sea surface slope
      DHDY,              &! n-s sea surface slope
      QICE                ! ice freeze/melt heat flux

   real (r8), dimension(nx_block,ny_block) :: &
      WORK1, WORK2        ! local work space

!-----------------------------------------------------------------------
!
!  initialize control buffer
!
!-----------------------------------------------------------------------

   ibuff = 0

   if (check_time_flag(cpl_stop_now)) then
      ibuff(3) = 1
   endif

   ibuff(4) = iyear*10000 + imonth*100 + iday
   ibuff(5) = ihour*seconds_in_hour + &
              iminute*seconds_in_minute + isecond

!NCAR   if ( lsend_precip_fact ) &
!NCAR      ibuff(31) = precip_fact * 1.0e6_dbl_kind   ! send real as integer

!-----------------------------------------------------------------------
!
!  convert units, move to T points and rotate vectors for all fields
!
!-----------------------------------------------------------------------

   !$OMP PARALLEL DO 
   do iblock=1,nblocks_clinic

      !*** Coupler assumes all variables are on T points

      call ugrid_to_tgrid(WORK1, &
                          SBUFF_SUM(:,:,ksbuff_uvel,iblock), iblock)
      call ugrid_to_tgrid(WORK2, &
                          SBUFF_SUM(:,:,ksbuff_vvel,iblock), iblock)

      !*** Need to rotate the local velocity vector to
      !*** true zonal/meridional components and convert to m/s

      UUU(:,:,iblock) = (WORK1*cos( ANGLET(:,:,iblock)) + &
                         WORK2*sin(-ANGLET(:,:,iblock)))* &
                         mpercm/tlast_coupled
      VVV(:,:,iblock) = (WORK2*cos( ANGLET(:,:,iblock)) - &
                         WORK1*sin(-ANGLET(:,:,iblock)))* &
                         mpercm/tlast_coupled

      !*** If you want to trick the coupler and pass through
      !***  stuff at U points, comment the lines above and un-comment
      !***  the the following lines
      !***  make sure the ice model has made the same assumptions.
      !UUU(:,:,iblock) = SBUFF_SUM(:,:,ksbuff_uvel,iblock)
      !VVV(:,:,iblock) = SBUFF_SUM(:,:,ksbuff_vvel,iblock)


      !***
      !*** convert and pack surface temperature and salinity
      !***

      SST(:,:,iblock) = SBUFF_SUM(:,:,ksbuff_sst,iblock)/tlast_coupled &
                      + T0_Kelvin
      SSS(:,:,iblock) = SBUFF_SUM(:,:,ksbuff_sss,iblock)* &
                        salt_to_ppt/tlast_coupled

      !***
      !*** convert and rotate surface slope to zonal/merid components
      !*** Coupler assumes all variables are on T points
      !***

      call ugrid_to_tgrid(WORK1, &
                          SBUFF_SUM(:,:,ksbuff_dhdx,iblock), iblock)
      call ugrid_to_tgrid(WORK2, &
                          SBUFF_SUM(:,:,ksbuff_dhdy,iblock), iblock)

      !***
      !*** Need to rotate the local velocity vector to
      !*** true zonal/meridional components and convert to m/s
      !***

      DHDX(:,:,iblock) = (WORK1*cos( ANGLET(:,:,iblock)) + &
                          WORK2*sin(-ANGLET(:,:,iblock)))/ &
                          grav/tlast_coupled
      DHDY(:,:,iblock) = (WORK2*cos( ANGLET(:,:,iblock)) - &
                          WORK1*sin(-ANGLET(:,:,iblock)))/ &
                          grav/tlast_coupled

      !*** If you want to trick the coupler and pass through
      !***  stuff at U points and not rotated, comment the above calls
      !***  and un-comment the lines below
      !***  make sure the ice model has made the same assumptions.
      !DHDX(:,:,iblock) = SBUFF_SUM(:,:,ksbuff_dhdx,iblock)
      !DHDY(:,:,iblock) = SBUFF_SUM(:,:,ksbuff_dhdy,iblock)

   end do ! block loop
   !$OMP END PARALLEL DO

!-----------------------------------------------------------------------
!
!  gather fields into global buffer for sending to coupler
!
!-----------------------------------------------------------------------

   call gather_global(SBUFF(:,:,ksbuff_uvel), UUU , master_task, &
                                                    distrb_clinic)
   call gather_global(SBUFF(:,:,ksbuff_vvel), VVV , master_task, &
                                                    distrb_clinic)
   call gather_global(SBUFF(:,:,ksbuff_sst ), SST , master_task, &
                                                    distrb_clinic)
   call gather_global(SBUFF(:,:,ksbuff_sss ), SSS , master_task, &
                                                    distrb_clinic)
   call gather_global(SBUFF(:,:,ksbuff_dhdx), DHDX, master_task, &
                                                    distrb_clinic)
   call gather_global(SBUFF(:,:,ksbuff_dhdy), DHDY, master_task, &
                                                    distrb_clinic)
   call get_ice_flux(QICE)
   call gather_global(SBUFF(:,:,ksbuff_q), QICE, master_task, &
                                                 distrb_clinic)

!-----------------------------------------------------------------------
!
!  send fields to coupler
!
!-----------------------------------------------------------------------

   if (my_task == master_task) then
      call MPI_SEND(ibuff, nibuff, MPI_INTEGER, cpl_task, &
                           msgtype_o2d, MPI_COMM_WORLD, ier)

      call MPI_SEND(SBUFF, nswords, MPI_DOUBLE_PRECISION, cpl_task, &
                           msgtype_o2d, MPI_COMM_WORLD, ier)

   endif

   call broadcast_scalar(ier, master_task)
   if (ier /= MPI_SUCCESS) then
      call exit_POP(sigAbort,'ERROR after send to coupler')
   end if

   tlast_coupled = c0

   if (ldiag_cpl) &
      call coupled_diagnostics_send (SST,SSS,UUU,VVV,DHDX,DHDY,QICE)

!-----------------------------------------------------------------------
!EOC

 end subroutine send_to_coupler

!***********************************************************************
!BOP
! !IROUTINE: sum_buffer
! !INTERFACE:

 subroutine sum_buffer

! !DESCRIPTION:
!  This routine accumulates sums for (time) averaging fields to
!  be sent to the coupler.
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

   integer (int_kind) :: &
      iblock

   real (r8) :: &
      delt                ! time interval since last step

!-----------------------------------------------------------------------
!
!  update time since last coupling
!
!-----------------------------------------------------------------------

   if (avg_ts .or. back_to_back) then
      delt = p5*dtt
   else
      delt =    dtt
   endif

!-----------------------------------------------------------------------
!
!  accumulated time integral sums
!
!-----------------------------------------------------------------------

   !$OMP PARALLEL DO
   do iblock=1,nblocks_clinic

!-----------------------------------------------------------------------
!
!     zero buffer if this is the first time after a coupling interval
!
!-----------------------------------------------------------------------

      if (tlast_coupled == c0) SBUFF_SUM(:,:,:,iblock) = c0

!-----------------------------------------------------------------------
!
!     accumulate sums of U,V,T,S and GRADP
!     ice formation flux is handled separately in ice routine
!
!-----------------------------------------------------------------------

      SBUFF_SUM(:,:,ksbuff_uvel,iblock) =       &
      SBUFF_SUM(:,:,ksbuff_uvel,iblock) + delt* &
                                          UVEL(:,:,1,curtime,iblock)

      SBUFF_SUM(:,:,ksbuff_vvel,iblock) =       &
      SBUFF_SUM(:,:,ksbuff_vvel,iblock) + delt* &
                                          VVEL(:,:,1,curtime,iblock)

      SBUFF_SUM(:,:,ksbuff_sst ,iblock) =       &
      SBUFF_SUM(:,:,ksbuff_sst ,iblock) + delt* &
                                          TRACER(:,:,1,1,curtime,iblock)

      SBUFF_SUM(:,:,ksbuff_sss ,iblock) =       &
      SBUFF_SUM(:,:,ksbuff_sss ,iblock) + delt* &
                                          TRACER(:,:,1,2,curtime,iblock)

      SBUFF_SUM(:,:,ksbuff_dhdx,iblock) =       &
      SBUFF_SUM(:,:,ksbuff_dhdx,iblock) + delt* &
                                          GRADPX(:,:,curtime,iblock)

      SBUFF_SUM(:,:,ksbuff_dhdy,iblock) =       &
      SBUFF_SUM(:,:,ksbuff_dhdy,iblock) + delt* &
                                          GRADPY(:,:,curtime,iblock)
   end do ! block loop
   !$OMP END PARALLEL DO

   tlast_coupled = tlast_coupled + delt

!-----------------------------------------------------------------------
!EOC

 end subroutine sum_buffer

!***********************************************************************
!BOP
! !IROUTINE: subroutine coupled_diagnostics_recv
! !INTERFACE:

 subroutine coupled_diagnostics_recv(IFRC , MSLP , TAUX , TAUY , &
                                     NETSW, SENHF, LWUP , LWDN , &
                                     MELTH, SALT , PREC , EVAP , &
                                     MELTW, ROFF)

! !DESCRIPTION:
!  This routine computes some global diagnostics for various fields
!  received from the flux coupler.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), &
      intent(in) :: &
      IFRC ,        &! ice fraction
      MSLP ,        &! sea level pressure
      TAUX ,        &! e-w wind stress
      TAUY ,        &! n-s wind stress
      NETSW,        &! net shortwave
      SENHF,        &! sensible heat flux
      LWUP ,        &! upward longwave
      LWDN ,        &! downward longwave
      MELTH,        &! melt heat flux
      SALT ,        &! salt flux
      PREC ,        &! precipitation
      EVAP ,        &! evaporation
      MELTW,        &! melt water flux
      ROFF           ! water flux due to runoff

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   real (r8) :: &  ! global sums of fields 
      gsum_ifrac,   &  
      gsum_mslp,    &
      gsum_taux,    &
      gsum_tauy,    &
      gsum_netsw,   &
      gsum_senhf,   &
      gsum_lwup,    &
      gsum_lwdn,    &
      gsum_melthf,  &
      gsum_salt,    &
      gsum_prec,    &
      gsum_evap,    &
      gsum_melt,    &
      gsum_roff,    &
      m2percm2   
 
   character (46), parameter :: &
      out_fmt1 = "('comm_diag ',a3,1x,a4,1x,i3,1x,es26.19,1x,a6)"

   character (52), parameter :: &
      out_fmt2 = "('comm_diag ',a3,1x,a4,1x,i3,1x,es26.19,1x,a6,1x,a9)"

!-----------------------------------------------------------------------

   m2percm2  = mpercm*mpercm
 
   gsum_ifrac  = global_sum_prod(IFRC , TAREA, distrb_clinic, &
                                 field_loc_center, RCALCT)*m2percm2
 
   gsum_mslp   = global_sum_prod(MSLP , TAREA, distrb_clinic, &
                                 field_loc_center, RCALCT)*m2percm2
 
   gsum_taux   = global_sum_prod(TAUX , TAREA, distrb_clinic, &
                                 field_loc_center, RCALCT)*m2percm2
 
   gsum_tauy   = global_sum_prod(TAUY , TAREA, distrb_clinic, &
                                 field_loc_center, RCALCT)*m2percm2
 
   gsum_netsw  = global_sum_prod(NETSW, TAREA, distrb_clinic, &
                                 field_loc_center, RCALCT)*m2percm2
 
   gsum_senhf  = global_sum_prod(SENHF, TAREA, distrb_clinic, &
                                 field_loc_center, RCALCT)*m2percm2
 
   gsum_lwup   = global_sum_prod(LWUP , TAREA, distrb_clinic, &
                                 field_loc_center, RCALCT)*m2percm2

   gsum_lwdn   = global_sum_prod(LWDN , TAREA, distrb_clinic, &
                                 field_loc_center, RCALCT)*m2percm2
 
   gsum_melthf = global_sum_prod(MELTH, TAREA, distrb_clinic, &
                                 field_loc_center, RCALCT)*m2percm2
 
   gsum_salt   = global_sum_prod(SALT , TAREA, distrb_clinic, &
                                 field_loc_center, RCALCT)*m2percm2
  
   gsum_prec   = global_sum_prod(PREC , TAREA, distrb_clinic, &
                                 field_loc_center, RCALCT)*m2percm2
 
   gsum_evap   = global_sum_prod(EVAP , TAREA, distrb_clinic, &
                                 field_loc_center, RCALCT)*m2percm2
 
   gsum_melt   = global_sum_prod(MELTW, TAREA, distrb_clinic, &
                                 field_loc_center, RCALCT)*m2percm2
 
   gsum_roff   = global_sum_prod(ROFF , TAREA, distrb_clinic, &
                                 field_loc_center, RCALCT)*m2percm2
 
!-----------------------------------------------------------------------
!
!  print diagnostics
!
!-----------------------------------------------------------------------

   if (my_task == master_task) then
      !do these already exit in time manager?
      !call int_to_char (4,iyear   ,cyear  )
      !call int_to_char (2,imonth  ,cmonth )
      !call int_to_char (2,iday    ,cday   )
      !call int_to_char (2,ihour   ,chour  )
      !call int_to_char (2,iminute ,cminute)
      !call int_to_char (2,isecond ,csecond)
      write(stdout,'(a46,1x,a4,a1,a2,a1,a2,3x,a2,a1,a2,a1,a2)') &
            ' report global averages of fluxes from cpl at ',   &
            cyear,'/',cmonth,'/',cday,chour,':',cminute,':',csecond
 
        write(stdout,out_fmt1)'ocn','recv', krbuff_ifrac ,gsum_ifrac , &
                              'ifrac ' 
        write(stdout,out_fmt1)'ocn','recv', krbuff_mslp  ,gsum_mslp  , &
                              'mslp  '
        write(stdout,out_fmt2)'ocn','recv', krbuff_taux  ,gsum_taux  , &
                              'taux  ','(Newtons)'
        write(stdout,out_fmt2)'ocn','recv', krbuff_tauy  ,gsum_tauy  , &
                              'tauy  ','(Newtons)'
        write(stdout,out_fmt2)'ocn','recv', krbuff_netsw ,gsum_netsw , &
                              'netsw ','(Watts)  '
        write(stdout,out_fmt2)'ocn','recv', krbuff_senhf ,gsum_senhf , &
                              'senhf ','(Watts)  ' 
        write(stdout,out_fmt2)'ocn','recv', krbuff_lwup  ,gsum_lwup  , &
                              'lwup  ','(Watts)  '
        write(stdout,out_fmt2)'ocn','recv', krbuff_lwdwn ,gsum_lwdn ,  &
                              'lwdn  ','(Watts)  '
        write(stdout,out_fmt2)'ocn','recv', krbuff_melthf,gsum_melthf, &
                              'melthf','(Watts)  '
        write(stdout,out_fmt2)'ocn','recv', krbuff_salt  ,gsum_salt  , &
                              'salt  ','(kg/s)   '
        write(stdout,out_fmt2)'ocn','recv', krbuff_prec  ,gsum_prec  , &
                              'prec  ','(kg/s)   '
        write(stdout,out_fmt2)'ocn','recv', krbuff_evap  ,gsum_evap  , &
                              'evap  ','(kg/s)   '
        write(stdout,out_fmt2)'ocn','recv', krbuff_melt  ,gsum_melt  , &
                              'melt  ','(kg/s)   '
        write(stdout,out_fmt2)'ocn','recv', krbuff_roff  ,gsum_roff  , &
                              'roff  ','(kg/s)   '
   endif
 
!-----------------------------------------------------------------------
!EOC

 end subroutine coupled_diagnostics_recv
 
!***********************************************************************
!BOP
! !IROUTINE: subroutine coupled_diagnostics_send
! !INTERFACE:

 subroutine coupled_diagnostics_send (SST, SSS, UUU, VVV, DHDX, DHDY, Q)

! !DESCRIPTION:
!  This routine accumulates sums for (time) averaging fields to
!  be sent to the coupler.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), &
      intent(in) :: &
      SST,          &! sea surface temperature
      SSS,          &! sea surface salinity
      UUU,          &! e-w velocity
      VVV,          &! n-s velocity
      DHDX,         &! e-w surface slope
      DHDY,         &! n-s surface slope
      Q              ! heat flux due to ice freeze/melt

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------
 
   real (r8) :: &! global sums of sent fields 
      gsum_sst,  &
      gsum_sss,  &
      gsum_uvel, &
      gsum_vvel, &
      gsum_dhdx, &
      gsum_dhdy, &
      gsum_q,    &
      m2percm2
 
   character (46), parameter :: &
      out_fmt1 = "('comm_diag ',a3,1x,a4,1x,i3,1x,es26.19,1x,a6)"

   character (52), parameter :: &
      out_fmt2 = "('comm_diag ',a3,1x,a4,1x,i3,1x,es26.19,1x,a6,1x,a9)"

!-----------------------------------------------------------------------
!
!  compute global integrals
!
!-----------------------------------------------------------------------
 
   m2percm2  = mpercm*mpercm
 
   gsum_sst  = global_sum_prod(SST , TAREA, distrb_clinic, &
                               field_loc_center, RCALCT)*m2percm2
 
   gsum_sss  = global_sum_prod(SSS , TAREA, distrb_clinic, &
                               field_loc_center, RCALCT)*m2percm2
 
   gsum_uvel = global_sum_prod(UUU , TAREA, distrb_clinic, &
                               field_loc_center, RCALCT)*m2percm2
 
   gsum_vvel = global_sum_prod(VVV , TAREA, distrb_clinic, &
                               field_loc_center, RCALCT)*m2percm2
 
   gsum_dhdx = global_sum_prod(DHDX, TAREA, distrb_clinic, &
                               field_loc_center, RCALCT)*m2percm2
 
   gsum_dhdy = global_sum_prod(DHDY, TAREA, distrb_clinic, &
                               field_loc_center, RCALCT)*m2percm2
 
   gsum_q    = global_sum_prod(Q   , TAREA, distrb_clinic, &
                               field_loc_center, RCALCT)*m2percm2
 
!-----------------------------------------------------------------------
!
!  print diagnostics
!
!-----------------------------------------------------------------------

   if (my_task == master_task) then
      !these already exit in time manager?
      !call int_to_char (4,iyear   ,cyear  )
      !call int_to_char (2,imonth  ,cmonth )
      !call int_to_char (2,iday    ,cday   )
      !call int_to_char (2,ihour   ,chour  )
      !call int_to_char (2,iminute ,cminute)
      !call int_to_char (2,isecond ,csecond)
      write(stdout,'(a44,1x,a4,a1,a2,a1,a2,3x,a2,a1,a2,a1,a2)') &
            ' report global averages of fluxes to cpl at ',     &
            cyear,'/',cmonth,'/',cday,chour,':',cminute,':',csecond
 
      write(stdout,out_fmt1)'ocn','send', ksbuff_sst , gsum_sst , &
                            'sst   '
      write(stdout,out_fmt1)'ocn','send', ksbuff_sss , gsum_sss , &
                            'sss   '
      write(stdout,out_fmt1)'ocn','send', ksbuff_uvel, gsum_uvel, &
                            'uvel  '
      write(stdout,out_fmt1)'ocn','send', ksbuff_vvel, gsum_vvel, &
                            'vvel  '
      write(stdout,out_fmt1)'ocn','send', ksbuff_dhdx, gsum_dhdx, &
                            'dhdx  '
      write(stdout,out_fmt1)'ocn','send', ksbuff_dhdy, gsum_dhdy, &
                            'dhdy  '
      write(stdout,out_fmt2)'ocn','send', ksbuff_q   , gsum_q   , &
                            'q   ','(Watts)  '
   endif
 
!-----------------------------------------------------------------------
!EOC

 end subroutine coupled_diagnostics_send
 
#endif
!***********************************************************************

 end module forcing_coupled

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
!!***********************************************************************
!
!      subroutine set_combined_forcing (STF)
!
!!-----------------------------------------------------------------------
!!
!!     this routine combines terms when the "partially-coupled"
!!     has been selected
!!
!!-----------------------------------------------------------------------
! 
!      real (kind=dbl_kind), dimension(nx,ny,nt) ::
!     &   STF    !  surface tracer fluxes
! 
!      real (kind=dbl_kind), dimension(nx,ny) :: WORK1, WORK2
! 
!      
!#if coupled
!
!      if ( shf_formulation == 'partially-coupled' ) then
!        STF(:,:,1) =  SHF_COMP(:,:,shf_comp_wrest)
!     &              + SHF_COMP(:,:,shf_comp_srest)
!     &              + SHF_COMP(:,:,shf_comp_cpl)
!      endif
!
!      if ( sfwf_formulation == 'partially-coupled' ) then
!        if (sfc_layer_type == sfc_layer_varthick .and.
!     &      .not. lfw_as_salt_flx) then
!           STF(:,:,2) =  SFWF_COMP(:,:,sfwf_comp_wrest)
!     &                 + SFWF_COMP(:,:,sfwf_comp_srest)
!           FW         =  SFWF_COMP(:,:,sfwf_comp_cpl)
!     &                 + SFWF_COMP(:,:,sfwf_comp_flxio)
!           TFW        =  TFW_COMP(:,:,:,tfw_comp_cpl)
!     &                 + TFW_COMP(:,:,:,tfw_comp_flxio)
!        else
!          if ( lms_balance ) then
!
!            WORK1 = SFWF_COMP(:,:,sfwf_comp_flxio) / salinity_factor
!            WORK2 = SFWF_COMP(:,:,sfwf_comp_cpl)
!
!            call ms_balancing ( WORK2, EVAP_F, PREC_F, MELT_F,
!     &                          ROFF_F, SALT_F, 'salt',
!     &                          ICEOCN_F=WORK1 )
!
!            STF(:,:,2) =  SFWF_COMP(:,:,sfwf_comp_wrest)
!     &                  + SFWF_COMP(:,:,sfwf_comp_srest)
!     &                  + WORK2 
!     &                  + SFWF_COMP(:,:,sfwf_comp_flxio) * MASK_SR
!          else     
!
!            STF(:,:,2) =  SFWF_COMP(:,:,sfwf_comp_wrest)
!     &                  + SFWF_COMP(:,:,sfwf_comp_srest)
!     &                  + SFWF_COMP(:,:,sfwf_comp_cpl)
!     &                  + SFWF_COMP(:,:,sfwf_comp_flxio) 
! 
!          endif
!        endif
!      endif
!
!#endif
!!-----------------------------------------------------------------------
!
!      end subroutine set_combined_forcing
!
