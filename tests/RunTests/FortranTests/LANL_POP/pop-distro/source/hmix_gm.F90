!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module hmix_gm

!BOP
! !MODULE: hmix_gm

! !DESCRIPTION:
!  This module contains routines for computing horizontal mixing
!  using the Gent-McWilliams eddy transport parameterization
!  and isopycnal diffusion.

! !REVISION HISTORY:
!  CVS:$Id: hmix_gm.F90,v 1.27 2003/12/23 22:23:27 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod
   use blocks
   use distribution
   use domain
   use constants
   use broadcast
   use boundary
   use grid
   use io
   use vertical_mix
   use vmix_kpp
   use state_mod
   use time_management
   use tavg
   use diagnostics
   use exit_mod

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_gm,   &
             hdifft_gm

! !PUBLIC DATA MEMBERS:

   real (r8), dimension(:,:,:), allocatable, public :: &
      U_ISOP, V_ISOP,       &! isopycnal velocities
      WTOP_ISOP, WBOT_ISOP

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  GM specific options
!
!  kappa_type = constant or variable
!  slope_control = tanh function (Danabosoglu and McWilliams 1995) or
!                  DM95 with replacement function to tanh or
!                  slope clipping or
!                  method of Gerdes et al (1991)
!
!-----------------------------------------------------------------------

   integer (int_kind), parameter :: &
      kappa_type_const = 1,         &
      kappa_type_var   = 2,         &
      slope_control_tanh    = 1,    &
      slope_control_notanh  = 2,    &
      slope_control_clip    = 3,    &
      slope_control_Gerd    = 4

   integer (int_kind) ::  &
      kappa_type,         &! choice of KAPPA
      slope_control        ! choice for slope control

   logical (log_kind) ::  &
      gm_bolus             ! true for explicit calc of bolus velocity

   real (r8) ::           &
      ah,                 &! diffusivity
      ah_bolus,           &! diffusivity for GM bolus transport
      ah_bkg,             &! backgroud horizontal diffusivity at k = KMT
      slm_r,              &! max. slope slope allowed for redi diffusion
      slm_b                ! max. slope slope allowed for bolus transport

!-----------------------------------------------------------------------
!
!  variables to save from one call to next
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      gm_restart_flag     ! id to access restart time flag to determine
                          ! when restarts will be written

   integer (int_kind), dimension(:), allocatable :: &
      ktp_save,          &! indices into KAPPA array to avoid copies
      kbt_save,          &
      kbt2_save,         &
      kn_save,           &
      ks_save

   real (r8), dimension(:), allocatable :: &
      kappa_depth          ! depth dependence for kappa

   real (r8), dimension(:,:,:), allocatable :: &
      HXY,                &! dx/dy for y-z plane
      HYX,                &! dy/dx for x-z plane
      HYXW, HXYS,         &! west and south-shifted values of above
      RBR,                &! Rossby radius
      BTP,                &! beta plane approximation
      RX,                 &! Dx(rho) on the east  face of T-cell
      RY,                 &! Dy(rho) on the north face of T-cell
      RXW,RYS,            &! same on west, south faces
      DRDT,DRDS,          &! expansion coefficients d(rho)/dT,S
      KSV,                &! temporary array for KAPPA
      UIT, VIT             ! tmp arrays for isopycnal mixing velocities

   real (r8), dimension(:,:,:,:,:), allocatable :: &
      SLX, SLY             ! slope of isopycnal sfcs in x,y-direction

   real (r8), dimension(:,:,:,:), allocatable :: &
      KAPPA,              &! tracer diffusion coefficient
      KAPPA_B              ! thickness diffusion coefficient

   real (r8), dimension(:,:,:,:,:), allocatable :: &
      TX,TY,TZ             ! tracer differences in each direction

   real (r8), dimension(:,:,:,:), allocatable :: &
      TXP, TYP, TZP, TEMP

   real (r8), dimension(:,:,:,:), allocatable :: &
      FZTOP                ! vertical flux

   logical (log_kind), dimension(:), allocatable :: &
      compute_kappa        ! flag for computing variable KAPPA

   logical (log_kind) :: &
      diff_tapering        ! different tapering for two diffusivities

!-----------------------------------------------------------------------
!
!  tavg ids for tavg diagnostics related to isopycnal velocities
!  zonal, merid refer here to logical space only
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      tavg_UISOP,        &! zonal      isopycnal velocity
      tavg_VISOP,        &! meridional isopycnal velocity
      tavg_WISOP          ! vertical   isopycnal velocity

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_gm
! !INTERFACE:

 subroutine init_gm

! !DESCRIPTION:
!  Initializes various choices and allocates necessary space
!  also computes some time-independent arrays
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

   type (block) ::      &
      this_block         ! block information for current block

!-----------------------------------------------------------------------
!
!  input namelist for setting various GM options
!
!-----------------------------------------------------------------------

   integer (int_kind) ::   &
      nml_error,           &! error flag for namelist
      k,                   &! vertical level index
      iblock                ! block index

   real (r8) ::            &
      kappa_depth_1,       &! parameters for variation of kappa
      kappa_depth_2,       &! with depth
      kappa_depth_scale

   character (char_len) :: &
      kappa_choice,        &! choice of KAPPA
      slope_control_choice  ! choice for slope control

   namelist /hmix_gm_nml/ kappa_choice,                      &
                          slope_control_choice,              &
                          kappa_depth_1, kappa_depth_2,      &
                          kappa_depth_scale, ah, ah_bolus,   &
                          ah_bkg, slm_r, slm_b, gm_bolus

!-----------------------------------------------------------------------
!
!  read input namelist for additional GM options
!
!  DEFAULT OPTION
!  kappa           : constant = ah
!  slope control   : method by DM95 with replacing tanh by polynomial
!
!  variation of kappa with depth is
!    kappa_depth_1 + kappa_depth_2*exp(-z/kappa_depth_scale)
!
!  ah_bolus         : diffusion for GM bolus transport
!  ah_bkg           : background horizontal diffusion
!  slm_r            : max. slope slope allowed for redi diffusion
!  slm_b            : max. slope slope allowed for bolus transport
!
!-----------------------------------------------------------------------

   kappa_type    = kappa_type_const
   slope_control = slope_control_notanh
   kappa_depth_1 = c1
   kappa_depth_2 = c0
   kappa_depth_scale = 150000.0_r8
   ah            = 0.8e7_r8
   ah_bolus      = 0.8e7_r8
   ah_bkg        = c0
   slm_r         = 0.01_r8
   slm_b         = 0.01_r8
   gm_bolus      = .false.

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=hmix_gm_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading hmix_gm_nml')
   endif

   if (my_task == master_task) then
      write(stdout,'(a26)') '  Gent-McWilliams options:'
      write(stdout,*) '    kappa choice is ',trim(kappa_choice)
      write(stdout,*) '    slope control choice is ', &
                      trim(slope_control_choice)
      write(stdout,'(a23,1pe13.6)') ' input diffusivity   = ', &
                                      ah
      write(stdout,'(a23,1pe13.6)') ' thickness diffusion = ', &
                                      ah_bolus
      write(stdout,'(a23,1pe13.6)') ' backgroud diffusion = ', &
                                      ah_bkg
      write(stdout,'(a23,1pe13.6)') ' max. slope for redi = ', &
                                      slm_r
      write(stdout,'(a23,1pe13.6)') ' max. slope for bolus = ', &
                                      slm_b
      if (kappa_depth_2 /= c0) then
         write(stdout,'(a27)') '    kappa varies with depth'
         write(stdout,'(a20,1pe13.6)') '    kappa_depth_1 = ', &
                                            kappa_depth_1
         write(stdout,'(a20,1pe13.6)') '    kappa_depth_2 = ', &
                                            kappa_depth_2
         write(stdout,'(a24,1pe13.6)') '    kappa_depth_scale = ', &
                                            kappa_depth_scale
      else
         write(stdout,'(a33)') '   kappa does not vary with depth'
      endif

      if (gm_bolus) &
         write(stdout,'(a39)') '   Bolus velocity calculated explicitly'

      select case (kappa_choice(1:3))
      case ('con')
         kappa_type = kappa_type_const
      case ('var')
         kappa_type = kappa_type_var
      case default
         kappa_type = -1000
      end select

      select case (slope_control_choice(1:4))
      case ('tanh')
         slope_control = slope_control_tanh
      case ('nota')
         slope_control = slope_control_notanh
      case ('clip')
         slope_control = slope_control_clip
      case ('Gerd')
         slope_control = slope_control_Gerd
      case default
         slope_control = -1000
      end select

   endif

   call broadcast_scalar(kappa_type,        master_task)
   call broadcast_scalar(slope_control,     master_task)
   call broadcast_scalar(kappa_depth_1,     master_task)
   call broadcast_scalar(kappa_depth_2,     master_task)
   call broadcast_scalar(kappa_depth_scale, master_task)
   call broadcast_scalar(ah,                master_task)
   call broadcast_scalar(ah_bolus,          master_task)
   call broadcast_scalar(ah_bkg  ,          master_task)
   call broadcast_scalar(slm_r,             master_task)
   call broadcast_scalar(slm_b,             master_task)
   call broadcast_scalar(gm_bolus,          master_task)

   if (kappa_type == -1000) then
      call exit_POP(sigAbort,'unknown type for KAPPA in GM setup')
   endif
   if (slope_control == -1000) then
      call exit_POP(sigAbort,'unknown slope control method in GM setup')
   endif

   if ( kappa_type == kappa_type_var  .and. &
                    zt(km) <= 1.0e5_r8 ) then
     call exit_POP(sigAbort,'the max bottom depth should be > 1000.0 m')
   endif

   if ( gm_bolus .and. partial_bottom_cells) then
     call exit_POP(sigAbort, &
       'gm_bolus currently incompatible with partial bottom cells') 
   endif

!-----------------------------------------------------------------------
!
!  initialize time flag to access restart flag
!
!-----------------------------------------------------------------------

   gm_restart_flag = init_time_flag('restart')

!-----------------------------------------------------------------------
!
!  allocate GM arrays
!
!-----------------------------------------------------------------------

   allocate (HXY (nx_block,ny_block,nblocks_clinic), &
             HYX (nx_block,ny_block,nblocks_clinic), &
             HYXW(nx_block,ny_block,nblocks_clinic), &
             HXYS(nx_block,ny_block,nblocks_clinic), &
             RBR (nx_block,ny_block,nblocks_clinic), &
             BTP (nx_block,ny_block,nblocks_clinic), &
             RX  (nx_block,ny_block,nblocks_clinic), &
             RY  (nx_block,ny_block,nblocks_clinic), &
             RXW (nx_block,ny_block,nblocks_clinic), &
             RYS (nx_block,ny_block,nblocks_clinic), &
             DRDT(nx_block,ny_block,nblocks_clinic), &
             DRDS(nx_block,ny_block,nblocks_clinic), &
             KSV (nx_block,ny_block,nblocks_clinic))

   allocate (SLX(nx_block,ny_block,2,3,nblocks_clinic), &
             SLY(nx_block,ny_block,2,3,nblocks_clinic))

   allocate (KAPPA  (nx_block,ny_block,3,nblocks_clinic), &
             KAPPA_B(nx_block,ny_block,3,nblocks_clinic))

   allocate (TX(nx_block,ny_block,2,nt,nblocks_clinic), &
             TY(nx_block,ny_block,2,nt,nblocks_clinic), &
             TZ(nx_block,ny_block,2,nt,nblocks_clinic))

   allocate (TXP (nx_block,ny_block,2,nblocks_clinic), &
             TYP (nx_block,ny_block,2,nblocks_clinic), &
             TZP (nx_block,ny_block,2,nblocks_clinic), &
             TEMP(nx_block,ny_block,2,nblocks_clinic))

   allocate (FZTOP(nx_block,ny_block,nt,nblocks_clinic))

   allocate (kappa_depth(km))

   allocate (ktp_save (nblocks_clinic), &
             kbt_save (nblocks_clinic), &
             kbt2_save(nblocks_clinic), &
             kn_save  (nblocks_clinic), &
             ks_save  (nblocks_clinic))

   allocate (compute_kappa(nblocks_clinic))

   RX     = c0
   RY     = c0
   RXW    = c0
   RYS    = c0
   DRDT   = c0
   DRDS   = c0
   SLX    = c0
   SLY    = c0
   KAPPA  = c0
   KAPPA_B = c0
   TX     = c0
   TY     = c0
   TZ     = c0
   TXP    = c0
   TYP    = c0
   TZP    = c0
   FZTOP  = c0

!-----------------------------------------------------------------------
!
!  initialize various time-independent arrays
!
!-----------------------------------------------------------------------

   do iblock = 1,nblocks_clinic
      this_block = get_block(blocks_clinic(iblock),iblock)

      if (iblock == 1) then
         do k=1,km
            kappa_depth(k) = kappa_depth_1   &
                           + kappa_depth_2*  &
                             exp(-zt(k)/kappa_depth_scale)
         enddo
      endif

      !*** Hyx = dy/dx for x-z plane
      HYX(:,:,iblock) = HTE(:,:,iblock)/HUS(:,:,iblock)

      !*** Hxy = dx/dy for y-z plane
      HXY(:,:,iblock) = HTN(:,:,iblock)/HUW(:,:,iblock)

      HYXW(:,:,iblock) = eoshift(HYX(:,:,iblock),dim=1,shift=-1)
      HXYS(:,:,iblock) = eoshift(HXY(:,:,iblock),dim=2,shift=-1)

      !***
      !*** compute the Rossby radius which will be used to
      !*** contol KAPPA Large et al (1997), JPO, 27,
      !*** pp 2418-2447. Rossby radius = c/(2*omg*sin(latitude))
      !*** where c=200cm/s is the first baroclinic wave speed.
      !*** 15km < Rossby radius < 100km
      !***
      !*** |f|/Cg, Cg = 2 m/s = 200 cm/s
      !*** Cg/|f| >= 15 km = 1.5e+6 cm
      !*** Cg/|f| <= 100 km = 1.e+7 cm

      !***     Inverse of Rossby radius
      RBR(:,:,iblock) = abs(FCORT(:,:,iblock))/200.0_r8
      RBR(:,:,iblock) = min(RBR(:,:,iblock),c1/1.5e+6_r8)
      RBR(:,:,iblock) = max(RBR(:,:,iblock),1.e-7_r8)

      !*** beta at t-points
      call ugrid_to_tgrid(BTP(:,:,iblock),ULAT(:,:,iblock),iblock)
      BTP(:,:,iblock) = c2*omega*cos(BTP(:,:,iblock))/radius

   end do

   !*** HYXW, HXYS only needed in physical domain and should
   !*** be defined correctly there.  BTP invalid on south
   !*** and westernmost ghost cells, but not needed there
   !*** as long as number of ghost cells is >1
   !call update_ghost_cells(HYXW, bndy_clinic, field_loc_t,     &
   !                                           field_type_scalar)
   !call update_ghost_cells(HXYS, bndy_clinic, field_loc_t,     &
   !                                           field_type_scalar)
   !call update_ghost_cells(BTP , bndy_clinic, field_loc_t,     &
   !                                           field_type_scalar)

   compute_kappa = .false.

   if (slm_r /= slm_b) then
      diff_tapering = .true.
   else
      diff_tapering = .false.
   endif

!-----------------------------------------------------------------------
!
!  allocate and initialize bolus velocity arrays if requested
!  define tavg fields related to bolus velocity
!
!-----------------------------------------------------------------------

   if (gm_bolus) then
      allocate(   U_ISOP(nx_block,ny_block,nblocks_clinic), &
                  V_ISOP(nx_block,ny_block,nblocks_clinic), &
               WTOP_ISOP(nx_block,ny_block,nblocks_clinic), &
               WBOT_ISOP(nx_block,ny_block,nblocks_clinic), &
                     UIT(nx_block,ny_block,nblocks_clinic), &
                     VIT(nx_block,ny_block,nblocks_clinic))

      U_ISOP    = c0
      V_ISOP    = c0
      WTOP_ISOP = c0
      WBOT_ISOP = c0
      UIT       = c0
      VIT       = c0

      call define_tavg_field(tavg_UISOP,'UISOP',3,                     &
                             long_name='Eastward Bolus Velocity',      &
                             units='cm/s', grid_loc='3221')

      call define_tavg_field(tavg_VISOP,'VISOP',3,                     &
                             long_name='Northward Bolus Velocity',     &
                             units='cm/s', grid_loc='3221')

      call define_tavg_field(tavg_WISOP,'WISOP',3,                     &
                             long_name='Vertical Bolus Velocity',      &
                             units='cm/s', grid_loc='3112')

   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine init_gm

!***********************************************************************
!BOP
! !IROUTINE: hdifft_gm
! !INTERFACE:

 subroutine hdifft_gm(k,GTK,TMIX,UMIX,VMIX,this_block)

! !DESCRIPTION:
!  Gent-McWilliams eddy transport parameterization
!  and isopycnal diffusion
!
!  This routine must be called successively with k = 1,2,3,...
!
!  Differences from the original routine:
!  \begin{enumerate}
!     \item Functional approach with different methods to compute
!           derivatives of density and slopes
!           (see Griffies et al, 1998).
!     \item nonuniform diffusion coefficient
!     \item different cfl criterions
!     \item different methods to control diffusion coefficient
!     \item different parametrization for bolus velocity (bv)
!     \item skew flux form for bv
!  \end{enumerate}
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: k  ! depth level index

   real (r8), dimension(nx_block,ny_block,km,nt), intent(in) :: &
      TMIX                  ! tracers at all vertical levels
                            !   at mixing time level

   real (r8), dimension(nx_block,ny_block,km), intent(in) :: &
      UMIX, VMIX            ! U,V  at all vertical levels
                            !   and at mixing time level

   type (block), intent(in) :: &
      this_block            ! block info for this sub block

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,nt), intent(out) :: &
     GTK     ! diffusion+bolus advection for nth tracer at level k

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind), parameter :: &
      ieast  = 1, iwest  = 2,       &
      jnorth = 1, jsouth = 2

   integer (int_kind) ::  &
      i,j,n,kk,ii,jj,     &! dummy loop counters
      ikpam,kid,kp1,      &! array indices
      ktp,kbt,kbt2,kn,ks, &! indices into KAPPA array to avoid copies
      bid                  ! local block address for this sub block

   integer (int_kind), dimension(3) :: &
      kc                                ! vertical array index

   real (r8) ::                &
      fz,                      &! vertical flux
      dz_top                    ! thickness of layer above

   real (r8), dimension(nx_block,ny_block) :: &
      CX, CY,                  &
      RZ, RZM,                 &! Dz(rho)
      SLA,                     &! absolute value of slope
      FX, FY,                  &! fluxes across east, north faces
      WORK1, WORK2,            &! local work space
      WORK3, WORK4, WORK5,     &! local work space
      WORK6, WORK7,            &! local work space
      KPTMP1, KPTMP2,          &! local work space
      KMASK, KMASKE, KMASKN,   &! ocean mask
      TAPER1, TAPER2, TAPER3,  &! tapering factor
      DZTE,DZTW,DZTN,DZTS,     &! DZT/dz for partial bottom cells
      DZTEP,DZTWP,DZTNP,DZTSP, &! DZT/dz for partial bottom cells
      UIB, VIB                  ! temps  for isopycnal mixing velocities

!-----------------------------------------------------------------------
!
!  intialize various quantities
!
!-----------------------------------------------------------------------

   bid = this_block%local_id

   if (.not. implicit_vertical_mix) call exit_POP(sigAbort, &
     'implicit vertical mixing must be used with GM horiz mixing')

   if (k == 1) then
      ktp  = 1
      kbt  = 2
      kbt2 = 3
      kn   = 1
      ks   = 2

      if (gm_bolus) then
         UIB = c0
         VIB = c0
         UIT(:,:,bid) = c0
         VIT(:,:,bid) = c0
         WBOT_ISOP(:,:,bid) = c0
      endif
   else
      ktp  = ktp_save(bid)
      kbt  = kbt_save(bid)
      kbt2 = kbt2_save(bid)
      kn   = kn_save(bid)
      ks   = ks_save(bid)
   endif

   if (k < km) then
      kp1 = k+1
   else
      kp1 = km
   endif 

   if (gm_bolus) WTOP_ISOP(:,:,bid) = WBOT_ISOP(:,:,bid)

   kc(1) = ktp
   kc(2) = kbt
   kc(3) = kbt2

   KMASK = merge(c1, c0, k < KMT(:,:,bid))

   if (partial_bottom_cells) then
      CX = merge(HYX(:,:,bid)*p25, c0, (k <= KMT (:,:,bid))   &
                                 .and. (k <= KMTE(:,:,bid)))
      CY = merge(HXY(:,:,bid)*p25, c0, (k <= KMT (:,:,bid))   &
                                 .and. (k <= KMTN(:,:,bid)))
   else
      CX = merge(dz(k)*HYX(:,:,bid)*p25,   &
                 c0, (k <= KMT (:,:,bid)) .and.      &
                     (k <= KMTE(:,:,bid)))
      CY = merge(dz(k)*HXY(:,:,bid)*p25,   &
                 c0, (k <= KMT (:,:,bid)) .and.      &
                     (k <= KMTN(:,:,bid)))
   endif

   TAPER1 = c1
   TAPER2 = c1
   TAPER3 = c1

!-----------------------------------------------------------------------
!
!  compute RX=Dx(rho), RY=Dy(rho) on the east face of T-cell at k=1.
!  For other z levels, these are computed and saved in previous call.
!
!-----------------------------------------------------------------------

   if (k == 1) then

      KMASKE = merge(c1, c0, k <= KMT (:,:,bid) .and. &
                             k <= KMTE(:,:,bid))
      KMASKN = merge(c1, c0, k <= KMT (:,:,bid) .and. &
                             k <= KMTN(:,:,bid))

      TEMP(:,:,kn,bid) = max(-c2, TMIX(:,:,k,1))

      TXP(:,:,kn,bid) = KMASKE* &
                        (eoshift(TEMP(:,:,kn,bid),dim=1,shift=1) &
                               - TEMP(:,:,kn,bid))
      TYP(:,:,kn,bid) = KMASKN* &
                        (eoshift(TEMP(:,:,kn,bid),dim=2,shift=1) &
                               - TEMP(:,:,kn,bid))

      do n=1,nt

         TX(:,:,kn,n,bid) = KMASKE* &
                            (eoshift(TMIX(:,:,k,n),dim=1,shift=1) &
                                   - TMIX(:,:,k,n))

         TY(:,:,kn,n,bid) = KMASKN* &
                            (eoshift(TMIX(:,:,k,n),dim=2,shift=1) &
                                   - TMIX(:,:,k,n))

       end do

       !***
       !*** D_T(rho) & D_S(rho) at level 1
       !***

       call state(k,k,TMIX(:,:,k,1),TMIX(:,:,k,2), &
                      this_block,                  &
                      DRHODT=DRDT(:,:,bid), DRHODS=DRDS(:,:,bid))

       !***
       !*** RX = Dx(rho) = DRDT*Dx(T) + DRDS*Dx(S)
       !*** RY = Dy(rho) = DRDT*Dy(T) + DRDS*Dy(S)
       !***

       RX (:,:,bid)  = DRDT(:,:,bid)*TXP(:,:,kn,bid) +  &
                       DRDS(:,:,bid)*TX (:,:,kn,2,bid)

       RXW(:,:,bid) =                                                 &
              DRDT(:,:,bid)*eoshift(TXP(:,:,kn,bid)  ,dim=1,shift=-1) &
            + DRDS(:,:,bid)*eoshift(TX (:,:,kn,2,bid),dim=1,shift=-1)

       RY (:,:,bid) = DRDT(:,:,bid)*TYP(:,:,kn,bid) + &
                      DRDS(:,:,bid)*TY (:,:,kn,2,bid)

       RYS(:,:,bid) =                                                 &
              DRDT(:,:,bid)*eoshift(TYP(:,:,kn,bid)  ,dim=2,shift=-1) &
            + DRDS(:,:,bid)*eoshift(TY (:,:,kn,2,bid),dim=2,shift=-1)

   end if

!-----------------------------------------------------------------------
!
!  compute RZ=Dz(rho) at k=k and
!  SLX = RX / RZ = slope of isopycnal surfaces in x-direction
!  SLY = RY / RZ = slope of isopycnal surfaces in y-direction
!
!-----------------------------------------------------------------------

   if (k < km) then

      TEMP(:,:,ks,bid) = max(-c2, TMIX(:,:,k+1,1))

      TZ(:,:,ks,1,bid) = TMIX(:,:,k,1) - TMIX(:,:,k+1,1)
      TZ(:,:,ks,2,bid) = TMIX(:,:,k,2) - TMIX(:,:,k+1,2)
      TZP(:,:,ks,bid) = TEMP(:,:,kn,bid) - TEMP(:,:,ks,bid)

      !***
      !*** RZ = Dz(rho) = DRDT*Dz(T) + DRDS*Dz(S)
      !*** RZM will be used to control KAPPA later
      !***

      RZ = DRDT(:,:,bid)*TZP(:,:,ks,bid) + &
           DRDS(:,:,bid)*TZ (:,:,ks,2,bid)
      RZ = min(RZ,-eps2)
      RZM = RZ

      SLX (:,:,ieast ,kbt,bid) = KMASK*RX (:,:,bid)/RZ
      SLX (:,:,iwest ,kbt,bid) = KMASK*RXW(:,:,bid)/RZ
      SLY (:,:,jnorth,kbt,bid) = KMASK*RY (:,:,bid)/RZ
      SLY (:,:,jsouth,kbt,bid) = KMASK*RYS(:,:,bid)/RZ

!-----------------------------------------------------------------------
!
!     compute Dx(rho), Dy(rho) at level k+1
!
!-----------------------------------------------------------------------

      KMASKE = merge(c1, c0, k+1 <= KMT (:,:,bid) .and.  &
                             k+1 <= KMTE(:,:,bid))
      KMASKN = merge(c1, c0, k+1 <= KMT (:,:,bid) .and.  &
                             k+1 <= KMTN(:,:,bid))

      TXP(:,:,ks,bid) = KMASKE*(eoshift(TEMP(:,:,ks,bid),dim=1,shift=1) &
                                      - TEMP(:,:,ks,bid))
      TYP(:,:,ks,bid) = KMASKN*(eoshift(TEMP(:,:,ks,bid),dim=2,shift=1) &
                                      - TEMP(:,:,ks,bid))

      do n=1,nt

         TX(:,:,ks,n,bid) = KMASKE*(eoshift(TMIX(:,:,k+1,n),dim=1,shift=1) &
                                          - TMIX(:,:,k+1,n))
         TY(:,:,ks,n,bid) = KMASKN*(eoshift(TMIX(:,:,k+1,n),dim=2,shift=1) &
                                          - TMIX(:,:,k+1,n))

      end do

      !***
      !*** D_T(rho) & D_S(rho) at level k+1
      !***

      call state(k+1,k+1,TMIX(:,:,k+1,1),TMIX(:,:,k+1,2), &
                         this_block,                      &
                         DRHODT=DRDT(:,:,bid), DRHODS=DRDS(:,:,bid))

      RX (:,:,bid) = DRDT(:,:,bid)*TXP(:,:,ks,bid)   &
                   + DRDS(:,:,bid)*TX (:,:,ks,2,bid) ! Dx(rho)

      RXW(:,:,bid) =                                                  &
             DRDT(:,:,bid)*eoshift(TXP(:,:,ks,bid)  ,dim=1,shift=-1)  &
           + DRDS(:,:,bid)*eoshift(TX (:,:,ks,2,bid),dim=1,shift=-1)

      RY (:,:,bid) = DRDT(:,:,bid)*TYP(:,:,ks,bid)  &
                   + DRDS(:,:,bid)*TY(:,:,ks,2,bid) ! Dy(rho)

      RYS(:,:,bid) =                                                  &
             DRDT(:,:,bid)*eoshift(TYP(:,:,ks,bid)  ,dim=2,shift=-1)  &
           + DRDS(:,:,bid)*eoshift(TY (:,:,ks,2,bid),dim=2,shift=-1)

      !*** Dz(rho)
      RZ = DRDT(:,:,bid)*TZP(:,:,ks,bid)  + &
           DRDS(:,:,bid)*TZ(:,:,ks,2,bid)
      RZ = min(RZ,-eps2)

!-----------------------------------------------------------------------
!
!     compute slope of isopycnal surfaces  at level k+1
!
!-----------------------------------------------------------------------

      where (k+1 <= KMT(:,:,bid))
         SLX (:,:,ieast, kbt2,bid) = RX (:,:,bid)/RZ
         SLX (:,:,iwest, kbt2,bid) = RXW(:,:,bid)/RZ
         SLY (:,:,jnorth,kbt2,bid) = RY (:,:,bid)/RZ
         SLY (:,:,jsouth,kbt2,bid) = RYS(:,:,bid)/RZ
      end where

!-----------------------------------------------------------------------
!
!     compute diffusion coefficient KAPPA if required
!
!-----------------------------------------------------------------------

      if (kappa_type == kappa_type_var .and. k==1) then

         if (nsteps_total == 1) KSV(:,:,bid) = ah

         if (nsteps_run == 1 .and. nsteps_total > 1)              &
            call diff_coeff(TMIX, UMIX, VMIX, this_block)

         if (compute_kappa(bid)) then
            call diff_coeff(TMIX, UMIX, VMIX, this_block)
            compute_kappa(bid) = .false.
         end if

         if (mod(nsteps_total,10) == 0 .or.     &
             check_time_flag(gm_restart_flag))  &
            compute_kappa(bid) = .true.

      endif           ! if (kappa_type == kappa_type_var. and. k==1)

!-----------------------------------------------------------------------
!
!     control slope of isopycnal surfaces or KAPPA
!
!-----------------------------------------------------------------------

      do kk = 2,3

         kid = k+kk-2

!-----------------------------------------------------------------------
!
!        control KAPPA to reduce the isopycnal mixing near the
!        ocean surface Large et al (1997), JPO, 27, pp 2418-2447.
!        WORK1 = ratio between the depth of water parcel and
!        the vertical displacement of isopycnal surfaces
!        where the vertical displacement =
!        Rossby radius * slope of isopycnal surfaces
!
!-----------------------------------------------------------------------

         if (partial_bottom_cells) then

            if (k < km) then
               SLA = p5*(DZT(:,:,k,bid)+                           &
                         DZT(:,:,k+1,bid))*sqrt(p5*(               &
                        (SLX(:,:,1,kc(kk),bid)**2 +                &
                         SLX(:,:,2,kc(kk),bid)**2)/HUS(:,:,bid)**2 &
                      + (SLY(:,:,1,kc(kk),bid)**2 +                &
                         SLY(:,:,2,kc(kk),bid)**2)/HUW(:,:,bid)**2))+eps
            else
               SLA = p5*DZT(:,:,k,bid)*sqrt(p5*(                   &
                     (SLX(:,:,1,kc(kk),bid)**2 +                   &
                      SLX(:,:,2,kc(kk),bid)**2)/HUS(:,:,bid)**2    &
                   + (SLY(:,:,1,kc(kk),bid)**2 +                   &
                      SLY(:,:,2,kc(kk),bid)**2)/HUW(:,:,bid)**2))+eps
            endif

         else ! no partial bottom cells

            SLA = dzw(k)*sqrt(p5*(                                     &
                  (SLX(:,:,1,kc(kk),bid)**2+SLX(:,:,2,kc(kk),bid)**2)/ &
                  HUS(:,:,bid)**2                                      &
                + (SLY(:,:,1,kc(kk),bid)**2+SLY(:,:,2,kc(kk),bid)**2)/ &
                  HUW(:,:,bid)**2))+eps

         endif ! partial bottom cells

         if ( kid == 1 ) then
            dz_top = c0
         else
            dz_top = zt(kid-1)
         endif

         if (slope_control == slope_control_tanh) then

            WORK1 = min(c1,zt(kid)*RBR(:,:,bid)/SLA)
            TAPER1 = p5*(c1+sin(pi*(WORK1-p5)))

            !*** use the Rossby deformation radius tapering
            !*** only within the boundary layer

            if ( vmix_itype == vmix_type_kpp ) then
	       TAPER1 = merge(TAPER1, c1, & 
                              dz_top <= KPP_HBLT(:,:,bid))
            endif

         else

            !***
            !*** sine function is replaced by
            !*** function = 4.*x*(1.-abs(x)) for |x|<0.5
            !***

            WORK1 = min(c1,zt(kid)*RBR(:,:,bid)/SLA)
            TAPER1 = (p5+c2*(WORK1-p5)*(c1-abs(WORK1-p5)))

            if ( vmix_itype == vmix_type_kpp ) then
               TAPER1 = merge(TAPER1, c1, &
                              dz_top <= KPP_HBLT(:,:,bid))
            endif

         endif

!-----------------------------------------------------------------------
!
!        control KAPPA for numerical stability
!
!-----------------------------------------------------------------------
!-----------------------------------------------------------------------
!
!        methods to control slope
!
!-----------------------------------------------------------------------

         select case (slope_control)
         case (slope_control_tanh)
            !***  method by Danabasoglu & Mcwilliams (1995)

            TAPER2 = merge(p5*(c1-tanh(c10*SLA/slm_r-c4)), &
                           c0, SLA < slm_r)

            if (diff_tapering) then
               TAPER3 = merge(p5*(c1-tanh(c10*SLA/slm_b-c4)), &
                              c0, SLA < slm_b)
            endif

         case (slope_control_notanh)
            !*** similar to DM95 except replacing tanh by
            !*** function = x*(1.-0.25*abs(x)) for |x|<2
            !***          = sign(x)            for |x|>2
            !*** (faster than DM95)

            where (SLA > 0.2_r8*slm_r .and. SLA < 0.6_r8*slm_r)
               TAPER2 = p5*(c1-(2.5_r8*SLA/slm_r-c1)*  &
                           (c4-abs(c10*SLA/slm_r-c4)))
            end where

            where (SLA >= 0.6_r8*slm_r)
               TAPER2 = c0
            end where

            if (diff_tapering) then
               where (SLA > 0.2_r8*slm_b .and. SLA < 0.6_r8*slm_b)
                  TAPER3 = p5*(c1-(2.5_r8*SLA/slm_b-c1)* &
                           (c4-abs(c10*SLA/slm_b-c4)))
               end where
               where (SLA >= 0.6_r8*slm_b)
                  TAPER3 = c0
               end where
            endif

         case (slope_control_clip)
            !*** slope clipping

            do i=1,2
               where (abs(SLX(:,:,i,kc(kk),bid)* &
                          dzw(k)/HUS(:,:,bid)) > slm_r)
                  SLX(:,:,i,kc(kk),bid) = sign(slm_r*HUS(:,:,bid)* &
                                               dzwr(k), &
                                               SLX(:,:,i,kc(kk),bid))
               end where
            enddo

            do j=1,2
               where (abs(SLY(:,:,j,kc(kk),bid)*dzw(k)/ &
                          HUW(:,:,bid)) > slm_r)
                  SLY(:,:,j,kc(kk),bid) = sign(slm_r*HUW(:,:,bid)* &
                                               dzwr(k), &
                                               SLY(:,:,j,kc(kk),bid))
               end where
            enddo

         case (slope_control_Gerd)
            !*** method by Gerdes et al (1991)

            where (SLA > slm_r)
               TAPER2 = (slm_r/SLA)**2
            end where

            if (diff_tapering) then
               where (SLA > slm_b)
                  TAPER3 = (slm_b/SLA)**2
               end where
            endif

         end select

         if (kappa_type == kappa_type_var) then
            KAPPA(:,:,kc(kk),bid) = KSV(:,:,bid)*TAPER1*TAPER2* &
                                    kappa_depth(kid)
            KAPPA_B(:,:,kc(kk),bid) = KAPPA(:,:,kc(kk),bid)
            if (diff_tapering) then
               KAPPA_B(:,:,kc(kk),bid) = KSV(:,:,bid)*TAPER1*TAPER3* &
                                         kappa_depth(kid)
            else
               if (ah /= ah_bolus) KAPPA_B(:,:,kc(kk),bid) = &
                          KSV(:,:,bid)*TAPER1*TAPER2*kappa_depth(kid)
            endif
         else
            KAPPA(:,:,kc(kk),bid) = ah*TAPER1*TAPER2*kappa_depth(kid)
            KAPPA_B(:,:,kc(kk),bid) = KAPPA(:,:,kc(kk),bid)
            if (diff_tapering) then
               KAPPA_B(:,:,kc(kk),bid) = ah_bolus*TAPER1*TAPER3* &
                                         kappa_depth(kid)
            else
              if(ah /= ah_bolus) KAPPA_B(:,:,kc(kk),bid) = &
                                 ah_bolus*TAPER1*TAPER2*kappa_depth(kid)
            endif
         endif

      end do

!-----------------------------------------------------------------------
!
!     end of k < km if block
!
!-----------------------------------------------------------------------

   endif

!-----------------------------------------------------------------------
!
!  impose the boundary conditions by setting KAPPA=0
!  in the quarter cells adjacent to rigid boundaries
!
!-----------------------------------------------------------------------

   !***
   !*** B.C. at the top
   !***

   if (k == 1) then
      KAPPA  (:,:,ktp,bid) = c0
      KAPPA_B(:,:,ktp,bid) = c0
      FZTOP  (:,:,:,bid) = c0  ! zero flux B.C. at the surface
   end if

   !***
   !*** B.C. at the bottom
   !***

   where (k == KMT(:,:,bid))
      KAPPA  (:,:,kbt ,bid) = c0
      KAPPA  (:,:,kbt2,bid) = c0
      KAPPA_B(:,:,kbt ,bid) = c0
      KAPPA_B(:,:,kbt2,bid) = c0
   end where

   if (partial_bottom_cells) then

      WORK1 = DZT(:,:,k,bid)*dzr(k)

      DZTE = eoshift(WORK1,dim=1,shift= 1)
      DZTW = eoshift(WORK1,dim=1,shift=-1)
      DZTN = eoshift(WORK1,dim=2,shift= 1)
      DZTS = eoshift(WORK1,dim=2,shift=-1)

      WORK1 = DZT(:,:,kp1,bid)*dzr(kp1)

      where (k+1 == KMT(:,:,bid))
         !*** reduce kappa for partial bottom cells
         KAPPA  (:,:,kbt ,bid) = KAPPA  (:,:,kbt ,bid)*WORK1
         KAPPA  (:,:,kbt2,bid) = KAPPA  (:,:,kbt2,bid)*WORK1
         KAPPA_B(:,:,kbt ,bid) = KAPPA_B(:,:,kbt ,bid)*WORK1
         KAPPA_B(:,:,kbt2,bid) = KAPPA_B(:,:,kbt2,bid)*WORK1
      end where

      !***
      !*** reduce kappa at the lateral boundaries
      !***

      if (k < km) then
         DZTEP = eoshift(WORK1,dim=1,shift= 1)
         DZTWP = eoshift(WORK1,dim=1,shift=-1)
         DZTNP = eoshift(WORK1,dim=2,shift= 1)
         DZTSP = eoshift(WORK1,dim=2,shift=-1)
      endif

   endif ! partial bottom cells

!-----------------------------------------------------------------------
!
!  calculate effective vertical diffusion coefficient
!  NOTE: it is assumed that VDC has been set before this
!        in vmix_coeffs or something similar.
!
!  Dz(VDC * Dz(T)) where D is derivative rather than difference
!  VDC = (Az(dz*Ax(KAPPA*HYX*SLX**2)) + Az(dz*Ay(KAPPA*HXY*SLY**2)))*
!        dzw/TAREA
!
!-----------------------------------------------------------------------

   if (k < km) then

      if (partial_bottom_cells) then
         WORK1 = p5*(DZT(:,:,k,bid)+DZT(:,:,k+1,bid))*               &
                 KMASK*TAREA_R(:,:,bid)*                             &
                 (DZT(:,:,k  ,bid)*p25*KAPPA(:,:,kbt ,bid)*          &
                 (DZTE*HYX (:,:,bid)*SLX(:,:,ieast, kbt ,bid)**2     &
                + DZTW*HYXW(:,:,bid)*SLX(:,:,iwest, kbt ,bid)**2     &
                + DZTN*HXY (:,:,bid)*SLY(:,:,jnorth,kbt ,bid)**2     &
                + DZTS*HXYS(:,:,bid)*SLY(:,:,jsouth,kbt ,bid)**2)    &
                + DZT(:,:,k+1,bid)*p25*KAPPA(:,:,kbt2,bid)*          &
                 (DZTEP*HYX (:,:,bid)*SLX(:,:,ieast, kbt2,bid)**2    &
                + DZTWP*HYXW(:,:,bid)*SLX(:,:,iwest, kbt2,bid)**2    &
                + DZTNP*HXY (:,:,bid)*SLY(:,:,jnorth,kbt2,bid)**2    &
                + DZTSP*HXYS(:,:,bid)*SLY(:,:,jsouth,kbt2,bid)**2))
      else
         WORK1 = dzw(k)*KMASK*TAREA_R(:,:,bid)*                 &
                 (dz(k  )*p25*KAPPA(:,:,kbt ,bid)*              &
                 (HYX (:,:,bid)*SLX(:,:,ieast, kbt ,bid)**2     &
                + HYXW(:,:,bid)*SLX(:,:,iwest, kbt ,bid)**2     &
                + HXY (:,:,bid)*SLY(:,:,jnorth,kbt ,bid)**2     &
                + HXYS(:,:,bid)*SLY(:,:,jsouth,kbt ,bid)**2)    &
                + dz(k+1)*p25*KAPPA(:,:,kbt2,bid)*              &
                 (HYX (:,:,bid)*SLX(:,:,ieast, kbt2,bid)**2     &
                + HYXW(:,:,bid)*SLX(:,:,iwest, kbt2,bid)**2     &
                + HXY (:,:,bid)*SLY(:,:,jnorth,kbt2,bid)**2     &
                + HXYS(:,:,bid)*SLY(:,:,jsouth,kbt2,bid)**2))
      endif

      do n=1,size(VDC,DIM=4)
         VDC(:,:,k,n,bid) = VDC(:,:,k,n,bid) + WORK1
      end do

   end if

!-----------------------------------------------------------------------
!
!   add some horizontal background diffusion
!   for the bottom half of the cells next to the lower boundary
!
!-----------------------------------------------------------------------

   if (partial_bottom_cells) then
      WORK3 =  (KAPPA(:,:,ktp,bid) + KAPPA(:,:,kbt,bid))*            &
               DZTE*DZT(:,:,k,bid) +                                 &
               eoshift((KAPPA(:,:,ktp,bid) + KAPPA(:,:,kbt,bid))*    &
                       DZTW*DZT(:,:,k,bid),dim=1,shift=1)
      WORK4 =  (KAPPA(:,:,ktp,bid) + KAPPA(:,:,kbt,bid))*            &
               DZTN*DZT(:,:,k,bid) +                                 &
               eoshift((KAPPA(:,:,ktp,bid) + KAPPA(:,:,kbt,bid))*    &
                       DZTS*DZT(:,:,k,bid),dim=2,shift=1)
   else
      WORK3 = KAPPA(:,:,ktp,bid) + KAPPA(:,:,kbt,bid)                &
            + eoshift(KAPPA(:,:,ktp,bid) +                           &
                      KAPPA(:,:,kbt,bid),dim=1,shift=1)

      WORK4 = KAPPA(:,:,ktp,bid) + KAPPA(:,:,kbt,bid)                &
            + eoshift(KAPPA(:,:,ktp,bid) +                           &
                      KAPPA(:,:,kbt,bid),dim=2,shift=1)
   endif

   if (ah_bkg /= c0) then
      WORK5 = merge(ah_bkg, c0, k == KMT(:,:,bid)) ! backgrnd diffusion
      if (partial_bottom_cells) then
         WORK6 = eoshift(WORK5*DZTW*DZT(:,:,k,bid),dim=1,shift=1)
         WORK7 = eoshift(WORK5*DZTS*DZT(:,:,k,bid),dim=2,shift=1)
         WORK5 = WORK5*DZTE*DZT(:,:,k,bid)
      else
         WORK6 = eoshift(WORK5,dim=1,shift=1)
         WORK7 = eoshift(WORK5,dim=2,shift=1)
      endif
      WORK3 = WORK3 + WORK5 + WORK6
      WORK4 = WORK4 + WORK5 + WORK7
   endif

!-----------------------------------------------------------------------
!
!  start loop over tracers
!  initialize some common factors
!
!-----------------------------------------------------------------------

   if (ah /= ah_bolus .or. diff_tapering) then
      WORK1  = KAPPA(:,:,ktp,bid) - KAPPA_B(:,:,ktp,bid)
      WORK2  = KAPPA(:,:,kbt,bid) - KAPPA_B(:,:,kbt,bid)
      KPTMP1 = p25*(KAPPA(:,:,kbt ,bid) + KAPPA_B(:,:,kbt ,bid))
      KPTMP2 = p25*(KAPPA(:,:,kbt2,bid) + KAPPA_B(:,:,kbt2,bid))
   else if ( gm_bolus ) then
      WORK1 = KAPPA(:,:,ktp,bid)
      WORK2 = KAPPA(:,:,kbt,bid)
      KPTMP1 = p25*KAPPA(:,:,kbt ,bid)
      KPTMP2 = p25*KAPPA(:,:,kbt2,bid)  
   else
      KPTMP1 = p5*KAPPA(:,:,kbt ,bid)
      KPTMP2 = p5*KAPPA(:,:,kbt2,bid)
   endif


   do n = 1,nt

!-----------------------------------------------------------------------
!
!     calculate horizontal fluxes thru vertical faces of T-cell
!     FX = dz*HYX*Ax(Az(KAPPA))*Dx(T) : flux in x-direction
!     FY = dz*HXY*Ay(Az(KAPPA))*Dy(T) : flux in y-direction
!
!-----------------------------------------------------------------------

      FX = CX*TX(:,:,kn,n,bid)*WORK3
      FY = CY*TY(:,:,kn,n,bid)*WORK4

      if (ah /= ah_bolus .or. diff_tapering .or. gm_bolus) then

         if (n > 2) then
            TZ(:,:,ks,n,bid) = TMIX(:,:,k  ,n) - TMIX(:,:,kp1,n)
         endif

         if (partial_bottom_cells) then

            do j=this_block%jb-1,this_block%je+1
            do i=this_block%ib-1,this_block%ie+1

               FX(i,j) = FX(i,j) - CX(i,j)*                            &
                 ((WORK1(i,j)*SLX(i,j,ieast,ktp,bid)*TZ(i,j,kn,n,bid)  &
                 + WORK2(i,j)*SLX(i,j,ieast,kbt,bid)*TZ(i,j,ks,n,bid)) &
                   *DZTE(i,j)*DZT(i,j,k,bid)                           &
                 +(WORK1(i+1,j)*SLX(i+1,j,iwest,ktp,bid)*              &
                                 TZ(i+1,j,kn,n,bid)                    &
                 + WORK2(i+1,j)*SLX(i+1,j,iwest,kbt,bid)*              &
                                 TZ(i+1,j,ks,n,bid))                   &
                   *DZTW(i+1,j)*DZT(i+1,j,k,bid))

               FY(i,j) = FY(i,j) - CY(i,j)*                            &
                ((WORK1(i,j)*SLY(i,j,jnorth,ktp,bid)*TZ(i,j,kn,n,bid)  &
               +  WORK2(i,j)*SLY(i,j,jnorth,kbt,bid)*TZ(i,j,ks,n,bid)) &
                  *DZTN(i,j)*DZT(i,j,k,bid)                            &
               + (WORK1(i,j+1)*SLY(i,j+1,jsouth,ktp,bid)*              &
                                TZ(i,j+1,kn,n,bid)                     &
               +  WORK2(i,j+1)*SLY(i,j+1,jsouth,kbt,bid)*              &
                                TZ(i,j+1,ks,n,bid))                    &
                  *DZTS(i,j+1)*DZT(i,j+1,k,bid))

            end do
            end do

         else ! no partial bottom cells

            FX = FX - CX*                                                 &
                ( WORK1*SLX(:,:,ieast,ktp,bid)*TZ(:,:,kn,n,bid)           &
                + WORK2*SLX(:,:,ieast,kbt,bid)*TZ(:,:,ks,n,bid)           &
                + eoshift(WORK1*SLX(:,:,iwest,ktp,bid)*TZ(:,:,kn,n,bid),  &
                          dim=1,shift=1)                                  &
                + eoshift(WORK2*SLX(:,:,iwest,kbt,bid)*TZ(:,:,ks,n,bid),  &
                          dim=1,shift=1))

            FY = FY - CY*                                                 &
                ( WORK1*SLY(:,:,jnorth,ktp,bid)*TZ(:,:,kn,n,bid)          &
                + WORK2*SLY(:,:,jnorth,kbt,bid)*TZ(:,:,ks,n,bid)          &
                + eoshift(WORK1*SLY(:,:,jsouth,ktp,bid)*TZ(:,:,kn,n,bid), &
                          dim=2,shift=1)                                  &
                + eoshift(WORK2*SLY(:,:,jsouth,kbt,bid)*TZ(:,:,ks,n,bid), &
                          dim=2,shift=1))

         endif ! partial bottom cells

      endif

!-----------------------------------------------------------------------
!
!     calculate vertical fluxes thru horizontal faces of T-cell
!     - Az(dz*Ax(HYX*KAPPA*SLX*TX)) - Az(dz*Ay(HXY*KAPPA*SLY*TY))
!     calculate isopycnal diffusion from flux differences
!     DTK = (Dx(FX)+Dy(FY)+Dz(FZ)) / volume
!
!-----------------------------------------------------------------------

      if (partial_bottom_cells) then

         GTK(:,:,n) = c0

         if (k < km) then
            do j=this_block%jb,this_block%je
            do i=this_block%ib,this_block%ie

               fz = -KMASK(i,j)*                                 &
                   (DZT(i,j,k,bid)*KPTMP1(i,j)*                  &
                    (SLX(i,j,ieast ,kbt ,bid)*HYX(i  ,j,bid)*    &
                     TX(i  ,j,kn,n,bid)*DZTE(i,j)                &
                   + SLY(i,j,jnorth,kbt ,bid)*HXY(i  ,j,bid)*    &
                     TY(i  ,j,kn,n,bid)*DZTN(i,j)                &
                   + SLX(i,j,iwest ,kbt ,bid)*HYX(i-1,j,bid)*    &
                     TX(i-1,j,kn,n,bid)*DZTW(i,j)                &
                   + SLY(i,j,jsouth,kbt ,bid)*HXY(i,j-1,bid)*    &
                     TY(i,j-1,kn,n,bid)*DZTS(i,j))               &
                   +DZT(i,j,k+1,bid)*KPTMP2(i,j)*                &
                    (SLX(i,j,ieast ,kbt2,bid)*HYX(i  ,j,bid)*    &
                     TX(i  ,j,ks,n,bid)*DZTEP(i,j)               &
                   + SLY(i,j,jnorth,kbt2,bid)*HXY(i  ,j,bid)*    &
                     TY(i  ,j,ks,n,bid)*DZTNP(i,j)               &
                   + SLX(i,j,iwest ,kbt2,bid)*HYX(i-1,j,bid)*    &
                     TX(i-1,j,ks,n,bid)*DZTWP(i,j)               &
                   + SLY(i,j,jsouth,kbt2,bid)*HXY(i,j-1,bid)*    &
                     TY(i,j-1,ks,n,bid)*DZTSP(i,j)))

               GTK(i,j,n) = (FX(i,j) - FX(i-1,j) +   &
                             FY(i,j) - FY(i,j-1) +   &
                             FZTOP(i,j,n,bid) - fz)* &
                             TAREA_R(i,j,bid)/DZT(i,j,k,bid)

               FZTOP(i,j,n,bid) = fz

            end do
            end do

         else ! k=km
            do j=this_block%jb,this_block%je
            do i=this_block%ib,this_block%ie

               ! fz = 0
               GTK(i,j,n) = (FX(i,j) - FX(i-1,j) +   &
                             FY(i,j) - FY(i,j-1) +   &
                             FZTOP(i,j,n,bid))*      &
                             TAREA_R(i,j,bid)/DZT(i,j,k,bid)

               FZTOP(i,j,n,bid) = c0

            end do
            end do
         endif

      else ! no partial bottom cells

         GTK(:,:,n) = c0

         if (k < km) then
            do j=this_block%jb,this_block%je
            do i=this_block%ib,this_block%ie

               fz = -KMASK(i,j)*                                     &
                   (dz(k  )*KPTMP1(i,j)*                             &
                    (SLX(i,j,ieast ,kbt ,bid)*HYX(i  ,j,bid)*        &
                                               TX(i  ,j,kn,n,bid)    &
                   + SLY(i,j,jnorth,kbt ,bid)*HXY(i  ,j,bid)*        &
                                               TY(i  ,j,kn,n,bid)    &
                   + SLX(i,j,iwest ,kbt ,bid)*HYX(i-1,j,bid)*        &
                                               TX(i-1,j,kn,n,bid)    &
                   + SLY(i,j,jsouth,kbt ,bid)*HXY(i,j-1,bid)*        &
                                               TY(i,j-1,kn,n,bid))   &
                   + dz(k+1)*KPTMP2(i,j)*                            &
                    (SLX(i,j,ieast ,kbt2,bid)*HYX(i  ,j,bid)*        &
                                               TX(i  ,j,ks,n,bid)    &
                   + SLY(i,j,jnorth,kbt2,bid)*HXY(i  ,j,bid)*        &
                                               TY(i  ,j,ks,n,bid)    &
                   + SLX(i,j,iwest ,kbt2,bid)*HYX(i-1,j,bid)*        &
                                               TX(i-1,j,ks,n,bid)    &
                   + SLY(i,j,jsouth,kbt2,bid)*HXY(i,j-1,bid)*        &
                                               TY(i,j-1,ks,n,bid)))
               GTK(i,j,n) = (FX(i,j) - FX(i-1,j) + FY(i,j) - FY(i,j-1) &
                          + FZTOP(i,j,n,bid) - fz)*                    &
                            dzr(k)*TAREA_R(i,j,bid)

               FZTOP(i,j,n,bid) = fz

            end do
            end do

         else ! k=km

            do j=this_block%jb,this_block%je
            do i=this_block%ib,this_block%ie

               !fz = 0
               GTK(i,j,n) = (FX(i,j) - FX(i-1,j) + FY(i,j) - FY(i,j-1) &
                          + FZTOP(i,j,n,bid))*                         &
                            dzr(k)*TAREA_R(i,j,bid)

               FZTOP(i,j,n,bid) = c0

            end do
            end do

         endif

      endif ! partial bottom cells

!-----------------------------------------------------------------------
!
!     end of tracer loop
!
!-----------------------------------------------------------------------

   end do

!-----------------------------------------------------------------------
!
!  compute explicit bolus velocity
!
!-----------------------------------------------------------------------

   if (gm_bolus) then

      if (partial_bottom_cells) then
         WORK1 = (     KAPPA_B(:,:,kbt ,bid)*SLX(:,:,ieast,kbt ,bid)  &
             +         KAPPA_B(:,:,kbt2,bid)*SLX(:,:,ieast,kbt2,bid)  &
             + eoshift(KAPPA_B(:,:,kbt ,bid)*SLX(:,:,iwest,kbt ,bid), &
                       dim=1, shift=1)                                &
             + eoshift(KAPPA_B(:,:,kbt2,bid)*SLX(:,:,iwest,kbt2,bid), &
                       dim=1, shift=1) )                              &
               *p25*HYX(:,:,bid)*p5*(DZT(:,:,k,bid)+DZT(:,:,kp1,bid))
     
         WORK2 = (     KAPPA_B(:,:,kbt ,bid)*SLY(:,:,jnorth,kbt ,bid)  &
             +         KAPPA_B(:,:,kbt2,bid)*SLY(:,:,jnorth,kbt2,bid)  &
             + eoshift(KAPPA_B(:,:,kbt ,bid)*SLY(:,:,jsouth,kbt ,bid), &
                       dim=2, shift=1)                                 &
             + eoshift(KAPPA_B(:,:,kbt2,bid)*SLY(:,:,jsouth,kbt2,bid), &
                       dim=2, shift=1) )                               &
               *p25*HXY(:,:,bid)*p5*(DZT(:,:,k,bid)+DZT(:,:,kp1,bid))
      else
         WORK1 = (     KAPPA_B(:,:,kbt ,bid)*SLX(:,:,ieast,kbt ,bid)  &
             +         KAPPA_B(:,:,kbt2,bid)*SLX(:,:,ieast,kbt2,bid)  &
             + eoshift(KAPPA_B(:,:,kbt ,bid)*SLX(:,:,iwest,kbt ,bid), &
                       dim=1, shift=1)                                &
             + eoshift(KAPPA_B(:,:,kbt2,bid)*SLX(:,:,iwest,kbt2,bid), &
                       dim=1, shift=1) )                              &
               *p25*HYX(:,:,bid)*dzw(k)
     
         WORK2 = (     KAPPA_B(:,:,kbt ,bid)*SLY(:,:,jnorth,kbt ,bid)  &
             +         KAPPA_B(:,:,kbt2,bid)*SLY(:,:,jnorth,kbt2,bid)  &
             + eoshift(KAPPA_B(:,:,kbt ,bid)*SLY(:,:,jsouth,kbt ,bid), &
                       dim=2, shift=1)                                 &
             + eoshift(KAPPA_B(:,:,kbt2,bid)*SLY(:,:,jsouth,kbt2,bid), &
                       dim=2, shift=1) )                               &
               *p25*HXY(:,:,bid)*dzw(k)
      endif
     
      UIB = merge( WORK1, c0, k < KMT(:,:,bid) .and. k < KMTE(:,:,bid) )
      VIB = merge( WORK2, c0, k < KMT(:,:,bid) .and. k < KMTN(:,:,bid) )
     
      WORK1 = merge(UIT(:,:,bid) - UIB, c0, k <= KMT (:,:,bid) .and. &
                                            k <= KMTE(:,:,bid) )
      WORK2 = merge(VIT(:,:,bid) - VIB, c0, k <= KMT (:,:,bid) .and. &
                                            k <= KMTN(:,:,bid) )

      if (partial_bottom_cells) then
         U_ISOP(:,:,bid) = WORK1/DZT(:,:,k,bid)/ HTE(:,:,bid)
         V_ISOP(:,:,bid) = WORK2/DZT(:,:,k,bid)/ HTN(:,:,bid)
      else
         U_ISOP(:,:,bid) = WORK1 * dzr(k) / HTE(:,:,bid)
         V_ISOP(:,:,bid) = WORK2 * dzr(k) / HTN(:,:,bid)
      endif
     
      where (k <= KMT(:,:,bid))
         WBOT_ISOP(:,:,bid) = WTOP_ISOP(:,:,bid) + TAREA_R(:,:,bid)*   &
                              (WORK1 - eoshift(WORK1,dim=1,shift=-1) + &
                               WORK2 - eoshift(WORK2,dim=2,shift=-1))
      elsewhere
         WBOT_ISOP(:,:,bid) = c0
      end where
     
   endif

!-----------------------------------------------------------------------
!
!  update remaining bottom-face fields to top-face fields for next
!  pass by swapping array indices to avoid copying
!
!-----------------------------------------------------------------------

   kbt2_save(bid) = kbt
   kbt_save(bid)  = ktp
   ktp_save(bid)  = kbt2

   kn_save(bid)   = ks
   ks_save(bid)   = kn

   if (gm_bolus) then
      UIT(:,:,bid) = UIB
      VIT(:,:,bid) = VIB
   endif

!-----------------------------------------------------------------------
!
!  compute horiz diffusion cfl diagnostics if required
!
!-----------------------------------------------------------------------

   if (ldiag_cfl) then

      WORK1 = merge(c4*ah*(DXTR(:,:,bid)**2 + DYTR(:,:,bid)**2), &
                    c0, KMT(:,:,bid) > k)
      WORK2 = abs(WORK1)
      call cfl_hdiff(k,bid,WORK2,1,this_block)

   endif

!-----------------------------------------------------------------------
!
!  accumulate time average if necessary
!
!-----------------------------------------------------------------------

   if (gm_bolus) then
   if (ltavg_on .and. mix_pass /= 1) then

      if (tavg_requested(tavg_UISOP)) then
         call accumulate_tavg_field(U_ISOP(:,:,bid),tavg_UISOP,bid,k)
      endif

      if (tavg_requested(tavg_VISOP)) then
         call accumulate_tavg_field(V_ISOP(:,:,bid),tavg_VISOP,bid,k)
      endif

      if (tavg_requested(tavg_UISOP)) then
         call accumulate_tavg_field(WTOP_ISOP(:,:,bid),tavg_WISOP,bid,k)
      endif

   endif ! ltavg_on and mix_pass ne 1
   endif ! bolus velocity option on

!-----------------------------------------------------------------------
!EOC

 end subroutine hdifft_gm

!***********************************************************************
!BOP
! !IROUTINE: diff_coeff
! !INTERFACE:

 subroutine diff_coeff(TMIX, UMIX, VMIX, this_block)

! !DESCRIPTION:
!  Variable kappa parametrization by Visbeck et al (1997):
!  \begin{equation}
!     \kappa = KSV = {{Cl^2}\over{T}},
!  \end{equation}
!  where $C$ is a constant, $T$ is the (Eady) time scale
!  $f/\surd\overline{Ri}$ and $l$ is the Rossby radius.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,km,nt), intent(in) :: &
      TMIX                  ! tracers at all vertical levels
                            !   at mixing time level

   real (r8), dimension(nx_block,ny_block,km), intent(in) :: &
      UMIX, VMIX            ! U,V  at all vertical levels
                            !   and at mixing time level

   type (block), intent(in) :: &
      this_block            ! block info for this sub block

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      kp1,i,j,kk,k,      &
      bid                 ! local block address for this sub block

   integer (int_kind) :: &
      k1,k2

   real (r8) ::     &
      const,        &
      zmin1, zmin2

   real (r8), dimension(nx_block,ny_block) :: &
      UKT,VKT,                                &
      UKPT, VKPT, RNUM,                       &
      TK, TKP,                                & 
      WORK1, WORK2, WORK3, WORK4,             &
      RHOT,RHOS,                              &
      GRATE, LSC

!-----------------------------------------------------------------------
!
!  initialization
!
!-----------------------------------------------------------------------

   bid = this_block%local_id

   GRATE = c0
   LSC = c0
   k1 = 0
   k2 = 0

   vert_loop: do k=1,km

      !*** -1000m < z < -50m
      if (zt(k) >= 0.5e4_r8 .and. &
          zt(k) <= 1.0e5_r8) then

!-----------------------------------------------------------------------
!
!        start computing Richardson number and baroclinic wave speed
!        average velocities at T points of level k
!
!-----------------------------------------------------------------------

         if (k1 == 0) then
            k1 = k              ! k1 is the upper limit for integration

            call ugrid_to_tgrid(UKT,UMIX(:,:,k),bid)
            call ugrid_to_tgrid(VKT,VMIX(:,:,k),bid)

            TK = max(-c2, TMIX(:,:,k,1))
         endif

!-----------------------------------------------------------------------
!
!        compute RZ=Dz(rho) at k=k
!
!-----------------------------------------------------------------------

         if (k < km) then
            kp1 = k + 1
         else
            kp1 = km
         endif

         call state(k,k,TMIX(:,:,k,1),TMIX(:,:,k,2), &
                        this_block,                  &
                        DRHODT=RHOT, DRHODS=RHOS)

         TKP = max(-c2, TMIX(:,:,kp1,1))

         WORK1 = TK - TKP
         WORK2 = TMIX(:,:,k  ,2) - TMIX(:,:,kp1,2)

         WORK3 = RHOT * WORK1 + RHOS * WORK2
         WORK3 = min(WORK3,-eps2)

!-----------------------------------------------------------------------
!
!        average velocities at T points of level k+1
!
!-----------------------------------------------------------------------

         call ugrid_to_tgrid(UKPT,UMIX(:,:,kp1),bid)
         call ugrid_to_tgrid(VKPT,VMIX(:,:,kp1),bid)

!-----------------------------------------------------------------------
!
!        local Richardson number at the bottom of T box, level k
!        = -g*Dz(RHO)/RHO_0/(Dz(u)**2+Dz(v)**2)
!
!-----------------------------------------------------------------------

         if (partial_bottom_cells) then

            where (k < KMT(:,:,bid))
               !*** RHO_0 = 1 in denominator (approximately) in cgs unit.

               WORK4 = p5*(dz(k) + DZT(:,:,k+1,bid))  ! dzw equivalent

               RNUM = -WORK4/((UKT - UKPT)**2 + (VKT - VKPT)**2 + eps)
               GRATE = GRATE + grav*RNUM*WORK4*WORK3
	       LSC = LSC - grav*WORK3

            end where

         else ! no partial bottom cells

            where (k < KMT(:,:,bid))

               !*** RHO_0 = 1 in denominator (approximately) in cgs unit.

               RNUM = -dzw(k)/((UKT - UKPT)**2 + (VKT - VKPT)**2 + eps)
               GRATE = GRATE + grav*RNUM*dzw(k)*WORK3
	       LSC = LSC - grav*WORK3

            end where

         endif ! partial bottom cells

!-----------------------------------------------------------------------
!
!        bottom values become top values for next pass
!
!-----------------------------------------------------------------------

         UKT = UKPT
         VKT = VKPT
         TK  = TKP

!-----------------------------------------------------------------------
!
!        end of computing the inverse of time scale
!
!-----------------------------------------------------------------------

      else

         if (k1 /= 0 .and. k2 == 0) then
            k2 = k              ! k2 is the lower limit for integration
            exit vert_loop
         end if

      endif                     ! if(zt(k).ge.0.5e4.and.zt(k).lt.1.0e5)

   end do vert_loop

   do j=1,ny_block
   do i=1,nx_block
      kk = KMT(i,j,bid)
      if (kk > 0) then
         zmin1 = min(zt(k1),zt(kk))
         zmin2 = min(zt(k2),zt(kk))
         GRATE(i,j) = GRATE(i,j)/(zmin2-zmin1+eps) ! Ri
         LSC(i,j)   = LSC(i,j)*(zmin2-zmin1) ! c_g^2=N^2*H^2
      else
         GRATE(i,j) = c0
         LSC(i,j)   = c0
      endif
   end do
   end do

   !*** equatorial inverse of time scale and square of length scale
   WORK1 = sqrt(c2*sqrt(LSC)*BTP(:,:,bid)) ! sqrt(c_g*2*beta)
   WORK2 = sqrt(LSC)/(c2*BTP(:,:,bid))     ! c_g/(2 beta)

   !*** inverse of time scale
   WORK1 = max(abs(FCORT(:,:,bid)),WORK1)
   GRATE = WORK1/sqrt(GRATE+eps) ! 1/T = f/sqrt(Ri)

   !*** square of length scale
   LSC = LSC/(FCORT(:,:,bid)+eps)**2  ! L^2 = c_g^2/f^2
   LSC = min(LSC,WORK2)

   !*** grid size = lower limit of length scale
   WORK1 = min(DXT(:,:,bid)**2,DYT(:,:,bid)**2)
   LSC = max(LSC,WORK1)

!-----------------------------------------------------------------------
!
!  compute KAPPA
!
!-----------------------------------------------------------------------

   !const = 0.015_r8 ! constant taken from Visbeck et al (1997)
   const = 0.13_r8
   WORK1 = const*GRATE*LSC ! const*L**2/T

   !*** KAPPA is bounded by 3.0e6 < KAPPA < 2.0e7

   KSV(:,:,bid) = min(2.0e7_r8,WORK1)
   KSV(:,:,bid) = max(3.0e6_r8,KSV(:,:,bid))

   where (KMT(:,:,bid) <= k1)       ! KAPPA=3.0e6 when depth < 50m
     KSV(:,:,bid) = 3.0e6_r8
   end where

!-----------------------------------------------------------------------
!EOC

 end subroutine diff_coeff

!***********************************************************************

 end module hmix_gm

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
