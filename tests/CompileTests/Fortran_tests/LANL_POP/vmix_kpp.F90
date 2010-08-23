!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module vmix_kpp

!BOP
! !MODULE: vmix_kpp
!
! !DESCRIPTION:
!  This module contains routines for initializing and computing
!  vertical mixing coefficients for the KPP parameterization
!  (see Large, McWilliams and Doney, Reviews of Geophysics, 32, 363 
!  November 1994).
!
! !REVISION HISTORY:
!  CVS:$Id: vmix_kpp.F90,v 1.19 2002/11/27 17:53:22 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod
   use blocks
   use distribution
   use domain_size
   use domain
   use constants
   use grid
   use broadcast
   use io
   use state_mod
   use exit_mod
   use forcing_shf, only: sw_absorb_frac

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:
   public :: init_vmix_kpp,   &
             vmix_coeffs_kpp, &
             add_kpp_sources

! !PUBLIC DATA MEMBERS:

   real (r8), dimension(:,:,:), allocatable, public :: & 
      HMXL,               &! mixed layer depth
      KPP_HBLT             ! boundary layer depth

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  mixing constants
!
!-----------------------------------------------------------------------

   real (r8) :: &
      rich_mix         ! coefficient for rich number term

   real (r8), dimension(:), allocatable :: &
      bckgrnd_vvc,         &! background value for viscosity
      bckgrnd_vdc           ! background value for diffusivity

   logical (log_kind) :: &
      lrich,        &! flag for computing Ri-dependent mixing
      ldbl_diff,    &! flag for computing double-diffusive mixing
      lshort_wave,  &! flag for computing short-wave forcing
      lcheckekmo     ! check Ekman, Monin-Obhukov depth limit

   integer (int_kind) :: & 
      num_v_smooth_Ri     ! num of times to vertically smooth Ri

   real (r8), parameter :: &
      epssfc = 0.1_r8       ! non-dimensional extent of sfc layer

!-----------------------------------------------------------------------
!
!  non-local mixing (counter-gradient mixing), treated as source term
!
!-----------------------------------------------------------------------

   real (r8), dimension(:,:,:,:,:), allocatable :: & 
      KPP_SRC              ! non-local mixing (treated as source term)

!-----------------------------------------------------------------------
!
!  parameters for subroutine bldepth: computes bndy layer depth 
!
!  concv   = ratio of interior buoyancy frequency to 
!            buoyancy frequency at entrainment depth
!
!-----------------------------------------------------------------------

   real (r8), parameter :: &
      Ricr   = 0.3_r8,      &! crit Rich no. for bndy layer depth
      cekman = 0.7_r8,      &! coefficient for Ekman depth
      cmonob = 1.0_r8,      &! coefficient for Monin-Obukhov depth
      concv  = 1.8_r8,      &!
      hbf    = 1.0_r8        ! frac of layer affected by sol forcing

!-----------------------------------------------------------------------
!
!  parameters for subroutine ri_iwmix which computes
!  vertical mixing coefficients below boundary layer due to shear
!  instabilities, internal waves and convection
!
!-----------------------------------------------------------------------

   real (r8), parameter :: &
      Riinfty = 0.8_r8,    &! Rich. no. limit for shear instab.
      BVSQcon = c0          ! Brunt-Vaisala square cutoff(s**-2)

!     BVSQcon = -0.2e-4_r8 ! Brunt-Vaisala square cutoff(s**-2)

!-----------------------------------------------------------------------
!
!  parameters for subroutine ddmix (double-diffusive mixing)
!
!-----------------------------------------------------------------------

   real (r8), parameter :: &
      Rrho0  = 1.9_r8,     &! limit for double-diff density ratio
      dsfmax = 10.0_r8      ! max diffusivity for salt fingering

!-----------------------------------------------------------------------
!
!  parameters for subroutine blmix: mixing within boundary layer
!
!  cstar   = proportionality coefficient for nonlocal transport
!  cg      = non-dimensional coefficient for counter-gradient term
!
!-----------------------------------------------------------------------

   real (r8), parameter :: &
      cstar = 10.0_r8       ! coeff for nonlocal transport

   real (r8) :: &
      Vtc,      &! turbulent velocity shear coefficient (for bulk Ri no)
      cg         ! coefficient for counter-gradient term

!-----------------------------------------------------------------------
!
!  parameters for velocity scale function (from Large et al.)
!
!-----------------------------------------------------------------------

   real (r8), parameter ::   & 
      zeta_m = -0.2_r8,      &
      zeta_s = -1.0_r8,      &
      c_m    =  8.38_r8,     &
      c_s    =  98.96_r8,    &
      a_m    =  1.26_r8,     &
      a_s    = -28.86_r8

!-----------------------------------------------------------------------
!
!  common vertical grid arrays used by KPP mixing
!
!-----------------------------------------------------------------------

   real (r8), dimension(:), allocatable :: & 
      zgrid,               &! depth at cell interfaces
      hwide                 ! layer thickness at interfaces

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_vmix_kpp
! !INTERFACE:

 subroutine init_vmix_kpp(VDC, VVC)

! !DESCRIPTION:
!  Initializes constants and reads options for the KPP parameterization.
!
! !REVISION HISTORY:
!  same as module

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(:,:,:,:), intent(inout) :: &
      VVC        ! viscosity for momentum diffusion

   real (r8), dimension(:,:,0:,:,:),intent(inout) :: &
      VDC        ! diffusivity for tracer diffusion

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables and namelist inputs
!
!-----------------------------------------------------------------------

   integer (int_kind) ::  & 
      k,                  &! local dummy index for vertical lvl
      nml_error            ! namelist i/o error flag

   real (r8) ::           &
      bckgrnd_vdc1,       &! diffusivity at depth dpth
      bckgrnd_vdc2,       &! variation in diffusivity
      bckgrnd_vdc_dpth,   &! depth at which diff equals vdc1
      bckgrnd_vdc_linv,   &! inverse length for transition region
      Prandtl              ! Prandtl number

   namelist /vmix_kpp_nml/bckgrnd_vdc1, bckgrnd_vdc2,           &
                          bckgrnd_vdc_dpth, bckgrnd_vdc_linv,   &
                          Prandtl, rich_mix,                    &
                          num_v_smooth_Ri, lrich, ldbl_diff,    &
                          lshort_wave, lcheckekmo

   character (16), parameter :: &
      fmt_real = '(a19,2x,1pe12.5)'

   character (11), parameter :: &
      fmt_log  = '(a19,2x,l7)'

   character (11), parameter :: &
      fmt_int  = '(a19,2x,i5)'

!-----------------------------------------------------------------------
!
!  set defaults for mixing coefficients, then read them from namelist
!
!-----------------------------------------------------------------------

   bckgrnd_vdc1    = 0.1_r8
   bckgrnd_vdc2    = c0
   bckgrnd_vdc_dpth= 2500.0e02_r8
   bckgrnd_vdc_linv= 4.5e-05_r8
   Prandtl         = 10.0_r8
   rich_mix        = 50.0_r8
   lrich           = .true.
   ldbl_diff       = .false.
   lshort_wave     = .false.
   lcheckekmo      = .false.
   num_v_smooth_Ri = 1

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=vmix_kpp_nml, iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading vmix_kpp_nml')
   endif

   if (my_task == master_task) then
      write(stdout,fmt_real) '  bckgrnd_vdc1    =', bckgrnd_vdc1
      write(stdout,fmt_real) '  bckgrnd_vdc2    =', bckgrnd_vdc2
      write(stdout,fmt_real) '  bckgrnd_vdc_dpth=', bckgrnd_vdc_dpth
      write(stdout,fmt_real) '  bckgrnd_vdc_linv=', bckgrnd_vdc_linv
      write(stdout,fmt_real) '  Prandtl         =', Prandtl
      write(stdout,fmt_real) '  rich_mix        =', rich_mix
      write(stdout,fmt_log ) '  Ri mixing       =', lrich
      write(stdout,fmt_log ) '  double-diff     =', ldbl_diff
      write(stdout,fmt_log ) '  short_wave      =', lshort_wave
      write(stdout,fmt_log ) '  lcheckekmo      =', lcheckekmo
      write(stdout,fmt_int ) '  num_smooth_Ri   =', num_v_smooth_Ri
   endif

   call broadcast_scalar(bckgrnd_vdc1,    master_task)
   call broadcast_scalar(bckgrnd_vdc2,    master_task)
   call broadcast_scalar(bckgrnd_vdc_dpth,master_task)
   call broadcast_scalar(bckgrnd_vdc_linv,master_task)
   call broadcast_scalar(Prandtl,         master_task)
   call broadcast_scalar(rich_mix,        master_task)
   call broadcast_scalar(num_v_smooth_Ri, master_task)
   call broadcast_scalar(lrich,           master_task)
   call broadcast_scalar(ldbl_diff,       master_task)
   call broadcast_scalar(lshort_wave,     master_task)
   call broadcast_scalar(lcheckekmo,      master_task)

!-----------------------------------------------------------------------
!
!  define some non-dimensional constants 
!
!-----------------------------------------------------------------------

   Vtc     = concv*sqrt(0.2_r8/c_s/epssfc)/vonkar**2/Ricr
   cg      = cstar*vonkar*(c_s*vonkar*epssfc)**p33

!-----------------------------------------------------------------------
!
!  define vertical grid coordinates and cell widths
!  compute vertical profile of background (internal wave) 
!  diffusivity and viscosity
!
!  the vertical profile has the functional form
!
!  BCKGRND_VDC(z) = vdc1 + vdc2*atan((|z| - dpth)/L) or
!                 = vdc1 + vdc2*atan((|z| - dpth)*linv)
!
!    where
!
!  vdc1 = vertical diffusivity at |z|=D              (cm^2/s)
!  vdc2 = amplitude of variation                     (cm^2/s)
!  linv = inverse length scale for transition region (1/cm)
!  dpth = depth where diffusivity is vdc1            (cm)
!
!  the viscosity has the same form but multiplied by a constant
!  Prandtl number
!
!-----------------------------------------------------------------------

   if (bckgrnd_vdc2 /= c0 .and. my_task == master_task) then
      write (stdout,blank_fmt)
      write (stdout,'(a43)') &
            'Vertical Profile for background diffusivity'
   endif

!-----------------------------------------------------------------------
!
!  initialize grid info (only need one block since the vertical grid is
!  the same across blocks)
!
!-----------------------------------------------------------------------

   allocate  (zgrid(0:km+1), hwide(0:km+1))
   allocate  (bckgrnd_vvc(km), bckgrnd_vdc(km))

   zgrid(0) = eps
   hwide(0) = eps
   do k=1,km
      zgrid(k) = -zt(k)
      hwide(k) =  dz(k)

      bckgrnd_vdc(k) = bckgrnd_vdc1 + bckgrnd_vdc2* &
                       atan(bckgrnd_vdc_linv*       &
                            (zw(k)-bckgrnd_vdc_dpth))
      bckgrnd_vvc(k) = Prandtl*bckgrnd_vdc(k)

      if (bckgrnd_vdc2 /= c0 .and. my_task == master_task) then
        write (stdout,'(2x,e12.6)') bckgrnd_vdc(k)
      endif
   end do

   zgrid(km+1) = -zw(km)
   hwide(km+1) = eps

!-----------------------------------------------------------------------
!
!  allocate and initialize KPP-specific arrays
!
!-----------------------------------------------------------------------

   allocate (HMXL     (nx_block,ny_block,nblocks_clinic), &
             KPP_HBLT (nx_block,ny_block,nblocks_clinic), &
             KPP_SRC  (nx_block,ny_block,km,nt,nblocks_clinic))

   HMXL     = c0
   KPP_HBLT = c0
   KPP_SRC  = c0
   VDC      = c0
   VVC      = c0

!-----------------------------------------------------------------------
!EOC

 end subroutine init_vmix_kpp

!***********************************************************************
!BOP
! !IROUTINE: vmix_coeffs_kpp
! !INTERFACE:

 subroutine vmix_coeffs_kpp(VDC, VVC, TRCR, UUU, VVV, STF, SHF_QSW, &
                            this_block, convect_diff, convect_visc, &
                            SMF, SMFT)

! !DESCRIPTION:
!  This is the main driver routine which calculates the vertical
!  mixing coefficients for the KPP mixing scheme as outlined in 
!  Large, McWilliams and Doney, Reviews of Geophysics, 32, 363 
!  (November 1994).  The non-local mixing is also computed here, but
!  is treated as a source term in baroclinic.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,km,nt), intent(in) :: &
      TRCR                ! tracers at current time

   real (r8), dimension(nx_block,ny_block,km), intent(in) :: &
      UUU,VVV             ! velocities at current time

   real (r8), dimension(nx_block,ny_block,nt), intent(in) :: &
      STF                 ! surface forcing for all tracers

   real (r8), dimension(nx_block,ny_block), intent(in) :: &
      SHF_QSW             ! short-wave forcing

   real (r8), dimension(nx_block,ny_block,2), intent(in), optional :: &
      SMF,               &! surface momentum forcing at U points
      SMFT                ! surface momentum forcing at T points
                         ! *** either one or the other (not
                         ! *** both) should be passed

   real (r8), intent(in) :: &
      convect_diff,      &! diffusivity to mimic convection
      convect_visc        ! viscosity   to mimic convection

   type (block), intent(in) :: &
      this_block          ! block information for current block

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,km), intent(inout) ::      &
      VVC        ! viscosity for momentum diffusion

   real (r8), dimension(nx_block,ny_block,0:km+1,2),intent(inout) :: &
      VDC        ! diffusivity for tracer diffusion

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      k,                 &! vertical level index 
      n,                 &! tracer index
      mt2,               &! index for separating temp from other trcrs
      bid                 ! local block address for this block

   integer (int_kind), dimension(nx_block,ny_block) :: &
      KBL                   ! index of first lvl below hbl

   real (r8), dimension(nx_block,ny_block) :: &
      USTAR,      &! surface friction velocity
      BFSFC,      &! surface buoyancy forcing
      STABLE       ! = 1 for stable forcing; = 0 for unstable forcing
 
   real (r8), dimension(nx_block,ny_block,km) :: &
      DBLOC,      &! buoyancy difference between adjacent levels
      DBSFC,      &! buoyancy difference between level and surface
      GHAT         ! non-local mixing coefficient

   real (r8), dimension(nx_block,ny_block,0:km+1) :: &
      VISC        ! local temp for viscosity

!-----------------------------------------------------------------------
!
!  initialize
!
!-----------------------------------------------------------------------

   bid = this_block%local_id

   if (.not. present(SMF) .and. .not. present(SMFT)) &
      call exit_POP(sigAbort, &
                    'ERROR KPP: must supply either SMF or SMFT')

!-----------------------------------------------------------------------
!
!  compute buoyancy differences at each vertical level.
!
!-----------------------------------------------------------------------

   call buoydiff(DBLOC, DBSFC, TRCR, this_block)

!-----------------------------------------------------------------------
!
!  compute mixing due to shear instability, internal waves and
!  convection
!
!-----------------------------------------------------------------------

   call ri_iwmix(DBLOC, VISC, VDC, UUU, VVV, &
                 convect_diff, convect_visc, this_block)

!-----------------------------------------------------------------------
!
!  compute double diffusion if desired
!
!-----------------------------------------------------------------------

   if (ldbl_diff) call ddmix(VDC, TRCR, this_block)

!-----------------------------------------------------------------------
!
!     compute boundary layer depth
!
!-----------------------------------------------------------------------

   if (present(SMFT)) then
      call bldepth (DBLOC, DBSFC, TRCR, UUU, VVV, STF, SHF_QSW,   &
                    KPP_HBLT(:,:,bid), USTAR, BFSFC, STABLE, KBL, & 
                    this_block, SMFT=SMFT)
   else
      call bldepth (DBLOC, DBSFC, TRCR, UUU, VVV, STF, SHF_QSW,   &
                    KPP_HBLT(:,:,bid), USTAR, BFSFC, STABLE, KBL, & 
                    this_block, SMF=SMF)
   endif

!-----------------------------------------------------------------------
!
!  compute boundary layer diffusivities
!
!-----------------------------------------------------------------------

   call blmix(VISC, VDC, KPP_HBLT(:,:,bid), USTAR, BFSFC, STABLE, &
              KBL, GHAT, this_block) 

!-----------------------------------------------------------------------
!
!  average mixing coeffs to V-grid and zero out land values
!  use USTAR as temporary storage
!
!-----------------------------------------------------------------------

   do k=1,km-1           

      call tgrid_to_ugrid(USTAR,VISC(:,:,k),bid)

      VVC(:,:,k) = merge(USTAR, c0, (k < KMU(:,:,bid)))

      where (k >= KMT(:,:,bid))
         VDC(:,:,k,1) = c0
         VDC(:,:,k,2) = c0
      endwhere
   enddo

   VDC(:,:,km,:) = c0
   VVC(:,:,km)   = c0

!-----------------------------------------------------------------------
!
!  add ghatp term from previous computation to right-hand-side 
!  source term on current row
!
!-----------------------------------------------------------------------

   do n=1,nt
      mt2=min(n,2)
      KPP_SRC(:,:,1,n,bid) = STF(:,:,n)/dz(1)           &
                             *(-VDC(:,:,1,mt2)*GHAT(:,:,1))
      if (partial_bottom_cells) then
         do k=2,km
            KPP_SRC(:,:,k,n,bid) = STF(:,:,n)/DZT(:,:,k,bid)         &
                                 *( VDC(:,:,k-1,mt2)*GHAT(:,:,k-1)   &
                                   -VDC(:,:,k  ,mt2)*GHAT(:,:,k  ))
         enddo
      else
         do k=2,km
            KPP_SRC(:,:,k,n,bid) = STF(:,:,n)/dz(k)                  &
                                 *( VDC(:,:,k-1,mt2)*GHAT(:,:,k-1)   &
                                   -VDC(:,:,k  ,mt2)*GHAT(:,:,k  ))
         enddo
      endif
   enddo

!-----------------------------------------------------------------------
!
!  compute diagnostic mixed layer depth (cm) using a max buoyancy 
!  gradient criterion.  Use USTAR and BFSFC as temps.
!
!-----------------------------------------------------------------------

   USTAR = c0
   where (KMT(:,:,bid) == 1)
      HMXL(:,:,bid) = zt(1)
   elsewhere
      HMXL(:,:,bid) = c0
   endwhere

   if (partial_bottom_cells) then
      do k=2,km
         where (k <= KMT(:,:,bid))
            STABLE = zt(k-1) + p5*(DZT(:,:,k-1,bid) + DZT(:,:,k,bid))
            USTAR = max(DBSFC(:,:,k)/STABLE,USTAR)
            HMXL(:,:,bid) = STABLE
         endwhere
      enddo

      VISC(:,:,1) = c0
      do k=2,km
         where (USTAR > c0 )
            VISC(:,:,k) = (DBSFC(:,:,k)-DBSFC(:,:,k-1))/ &
                          (p5*(DZT(:,:,k,bid) + DZT(:,:,k-1,bid)))
         end where
         where ( VISC(:,:,k) >= USTAR .and.              &
                (VISC(:,:,k)-VISC(:,:,k-1)) /= c0 .and.  &
                 USTAR > c0 )   ! avoid divide by zero
            BFSFC = (VISC(:,:,k) - USTAR)/ &
                    (VISC(:,:,k)-VISC(:,:,k-1))
            HMXL(:,:,bid) =   (zt(k-1) + p5*DZT(:,:,k-1,bid))*(c1-BFSFC) &
                            + (zt(k-1) - p5*DZT(:,:,k-1,bid))*BFSFC
            USTAR(:,:) = c0
         endwhere
      enddo
   else
      do k=2,km
         where (k <= KMT(:,:,bid))
            USTAR = max(DBSFC(:,:,k)/zt(k),USTAR)
            HMXL(:,:,bid) = zt(k)
         endwhere
      enddo

      VISC(:,:,1) = c0
      do k=2,km
         where (USTAR > c0 )
            VISC(:,:,k) = (DBSFC(:,:,k)-DBSFC(:,:,k-1))/ &
                          (zt(k) - zt(k-1))
         end where
         where ( VISC(:,:,k) >= USTAR .and.              &
                (VISC(:,:,k)-VISC(:,:,k-1)) /= c0 .and.  &
                 USTAR > c0 )   ! avoid divide by zero
            BFSFC = (VISC(:,:,k) - USTAR)/ &
                    (VISC(:,:,k)-VISC(:,:,k-1))
            HMXL(:,:,bid) = -p5*(zgrid(k  ) + zgrid(k-1))*(c1-BFSFC) &
                            -p5*(zgrid(k-1) + zgrid(k-2))*BFSFC
            USTAR(:,:) = c0
         endwhere
      enddo
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine vmix_coeffs_kpp

!***********************************************************************
!BOP
! !IROUTINE: ri_iwmix
! !INTERFACE:

 subroutine ri_iwmix(DBLOC, VISC, VDC, UUU, VVV, &
                     convect_diff, convect_visc, this_block)

! !DESCRIPTION:
!  Computes viscosity and diffusivity coefficients for the interior
!  ocean due to shear instability (richardson number dependent),
!  internal wave activity, and to static instability (Ri < 0).
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,km), intent(in) :: &
      UUU               ! U velocities at current time

   real (r8), dimension(nx_block,ny_block,km), intent(in) :: & 
      VVV,             &! V velocities at current time
      DBLOC             ! buoyancy difference between adjacent levels

   real (r8), intent(in) :: &
      convect_diff,         &! diffusivity to mimic convection
      convect_visc           ! viscosity   to mimic convection

   type (block), intent(in) :: &
      this_block          ! block information for current block

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,0:km+1,2), intent(inout) :: & 
      VDC        ! diffusivity for tracer diffusion

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,0:km+1), intent(out) :: & 
      VISC              ! viscosity

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      k,                 &! index for vertical levels
      bid,               &! local block index
      n                   ! vertical smoothing index

   real (r8), dimension(nx_block,ny_block) :: &
      VSHEAR,            &! (local velocity shear)^2
      RI_LOC,            &! local Richardson number 
      FRI,               &! function of Ri for shear
      FCON                ! function of Ri for convection

!-----------------------------------------------------------------------
!
!  compute mixing at each level
!
!-----------------------------------------------------------------------

   bid = this_block%local_id

   VISC(:,:,0) = c0

   do k = 1,km

!-----------------------------------------------------------------------
!
!     compute velocity shear squared and average to T points:
!     VSHEAR = (UUU(k)-UUU(k+1))**2+(VVV(k)-VVV(k+1))**2
!     Use FRI here as a temporary.
!
!-----------------------------------------------------------------------

      if (k < km) then

         FRI = (UUU(:,:,k)-UUU(:,:,k+1))**2 + &
               (VVV(:,:,k)-VVV(:,:,k+1))**2

         if (partial_bottom_cells) then
            FRI = FRI/(p5*(DZU(:,:,k  ,bid) + DZU(:,:,k+1,bid)))**2
         endif

         call ugrid_to_tgrid(VSHEAR,FRI,bid)

      else

         VSHEAR = c0

      endif

!-----------------------------------------------------------------------
!
!     compute local richardson number
!     use visc array as temporary Ri storage to be smoothed
!
!-----------------------------------------------------------------------

      if (partial_bottom_cells) then
         if (k < km) then
            RI_LOC = DBLOC(:,:,k)/                              &
                     (VSHEAR + eps/(p5*(DZT(:,:,k  ,bid) +      &
                                        DZT(:,:,k+1,bid)))**2)/ &
                     (p5*(DZT(:,:,k,bid) + DZT(:,:,k+1,bid)))
         else
            RI_LOC = DBLOC(:,:,k)/(VSHEAR +              &
                     eps/(p5*DZT(:,:,k,bid))**2)/(p5*DZT(:,:,k,bid))
         end if
      else
         RI_LOC = DBLOC(:,:,k)*(zgrid(k)-zgrid(k+1))/(VSHEAR + eps)
      end if

      VISC(:,:,k)   = merge(RI_LOC, VISC(:,:,k-1), k <= KMT(:,:,bid))
 
   enddo

!-----------------------------------------------------------------------
!
!  vertically smooth Ri num_v_smooth_Ri times with 1-2-1 weighting
!  result again stored temporarily in VISC and use RI_LOC and FRI
!  as temps
!
!-----------------------------------------------------------------------
 
   do n = 1,num_v_smooth_Ri
 
      FRI            =  p25 * VISC(:,:,1)
      VISC(:,:,km+1) =        VISC(:,:,km)
 
      do k=1,km
         RI_LOC      = VISC(:,:,k)
         where (KMT(:,:,bid) >= 3)
            VISC(:,:,k) = FRI + p5*RI_LOC + p25*VISC(:,:,k+1)
         end where
         FRI         = p25*RI_LOC
      enddo

   enddo

!-----------------------------------------------------------------------
!
!  now that we have a smoothed Ri field, finish computing coeffs
!  at each level
!
!-----------------------------------------------------------------------

   do k = 1,km

!-----------------------------------------------------------------------
!
!     compute function of Brunt-Vaisala squared for convection
!     RI_LOC = N**2,  FCON is function of N**2
!     FCON = 0 for N**2 > 0
!     FCON = [1-(1-Ri/BVSQcon)**2]**3 for BVSQcon < N**2 < 0
!     FCON = 1 for N**2 < BVSQcon
!
!-----------------------------------------------------------------------

      if (partial_bottom_cells) then
         if (k < km) then
            RI_LOC = DBLOC(:,:,k)/(p5*(DZT(:,:,k  ,bid) + &
                                       DZT(:,:,k+1,bid)))
         else
            RI_LOC = DBLOC(:,:,k)/(p5*DZT(:,:,k,bid))
         end if
      else
         RI_LOC = DBLOC(:,:,k)/(zgrid(k) - zgrid(k+1))
      end if

      if (BVSQcon /= c0) then
         FRI    = min(c1-(max(RI_LOC,BVSQcon))/BVSQcon, c1)
         FCON   = (c1 - FRI*FRI)**3
      else
         where (RI_LOC > c0)
            FCON = c0
         elsewhere
            FCON = c1
         end where
      endif

!-----------------------------------------------------------------------
!
!     if Ri-number mixing requested,
!     evaluate function of Ri for shear instability:
!       for 0 < Ri < Riinfty, function = (1 - (Ri/Riinfty)**2)**3
!       for     Ri > Riinfty, function = 0
!       for     Ri < 0      , function = 1
!     compute contribution due to shear instability
!     VISC holds smoothed Ri at k, but replaced by real VISC
!
!     otherwise only add iw and convection pieces
!
!-----------------------------------------------------------------------

      if (lrich) then
         FRI    = min((max(VISC(:,:,k),c0))/Riinfty, c1)

         VISC(:,:,k  ) = bckgrnd_vvc(k) + convect_visc*FCON + &
                         rich_mix*(c1 - FRI*FRI)**3

         if ( k < km ) then
            VDC (:,:,k,2) = bckgrnd_vdc(k) + convect_diff*FCON + &
                            rich_mix*(c1 - FRI*FRI)**3
            VDC(:,:,k,1) = VDC(:,:,k,2)
         endif
      else
         VISC(:,:,k  ) = bckgrnd_vvc(k) + convect_visc*FCON

         if ( k < km ) then
            VDC (:,:,k,2) = bckgrnd_vdc(k) + convect_diff*FCON
            VDC(:,:,k,1) = VDC(:,:,k,2)
         endif
      endif

!-----------------------------------------------------------------------
!
!     set seafloor values to zero
!
!-----------------------------------------------------------------------

      where ( k >= KMT(:,:,bid) )
         VISC(:,:,k  ) = c0
         VDC (:,:,k,1) = c0 
         VDC (:,:,k,2) = c0 
      endwhere

!-----------------------------------------------------------------------
!
!     move to next level.
!
!-----------------------------------------------------------------------

   end do

!-----------------------------------------------------------------------
!
!  fill extra coefficients for blmix
!
!-----------------------------------------------------------------------

   VISC(:,:,0  ) = c0
   VDC (:,:,0,:) = c0
   VISC(:,:,km+1  ) = c0 
   VDC (:,:,km+1,:) = c0 

!-----------------------------------------------------------------------
!EOC
 
 end subroutine ri_iwmix

!***********************************************************************
!BOP
! !IROUTINE: bldepth
! !INTERFACE:

 subroutine bldepth (DBLOC, DBSFC, TRCR, UUU, VVV, STF, SHF_QSW,  &
                     HBLT, USTAR, BFSFC, STABLE, KBL,             &
                     this_block, SMF, SMFT)

! !DESCRIPTION:
!  This routine computes the ocean boundary layer depth defined as
!  the shallowest depth where the bulk Richardson number is equal to
!  a critical value, Ricr.
!
!  NOTE: bulk richardson numbers are evaluated by computing 
!        differences between values at zgrid(kl) $< 0$ and surface
!        reference values. currently, the reference values are equal 
!        to the values in the surface layer.  when using higher 
!        vertical grid resolution, these reference values should be 
!        computed as the vertical averages from the surface down to 
!        epssfc*zgrid(kl).
!
!  This routine also computes where surface forcing is stable 
!  or unstable (STABLE)
!
! !REVISION HISTORY:
!  same as module


! !INPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,km,nt), intent(in) :: &
      TRCR                ! tracers at current time

   real (r8), dimension(nx_block,ny_block,km), intent(in) :: &
      UUU,VVV,       &! velocities at current time
      DBLOC,         &! buoyancy difference between adjacent levels
      DBSFC           ! buoyancy difference between level and surface

   real (r8), dimension(nx_block,ny_block,nt), intent(in) :: &
      STF                 ! surface forcing for all tracers

   real (r8), dimension(nx_block,ny_block), intent(in) :: &
      SHF_QSW             ! short-wave forcing

   real (r8), dimension(nx_block,ny_block,2), intent(in), optional :: &
      SMF,               &! surface momentum forcing at U points
      SMFT                ! surface momentum forcing at T points
                         ! *** either one or the other (not
                         ! *** both) should be passed

   type (block), intent(in) :: &
      this_block          ! block information for current block

! !OUTPUT PARAMETERS:

   integer (int_kind), dimension(nx_block,ny_block), intent(out) :: &
      KBL                    ! index of first lvl below hbl

   real (r8), dimension(nx_block,ny_block), intent(out) :: &
      HBLT,               &! boundary layer depth
      BFSFC,              &! Bo+radiation absorbed to d
      STABLE,             &! =1 stable forcing; =0 unstab
      USTAR                ! surface friction velocity

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j,               &! loop indices
      bid,               &! local block index
      kup, kdn, ktmp, kl  ! vertical level indices

   real (r8), dimension(nx_block,ny_block) :: &
      VSHEAR,            &! (velocity shear re sfc)^2
      SIGMA,             &! d/hbl
      WM, WS,            &! turb vel scale functions
      BO,                &! surface buoyancy forcing
      BOSOL,             &! radiative buoyancy forcing
      TALPHA,            &! temperature expansion coeff
      SBETA,             &! salinity    expansion coeff
      RHO1,              &! density at the surface
      WORK,              &! temp array
      HEKMAN,            &! Eckman depth limit
      HMONOB,            &! Monin-Obukhov depth limit
      HLIMIT              ! limit to mixed-layer depth

   real (r8), dimension(nx_block,ny_block,2) :: &
      RI_BULK             ! Bulk Ri number at 2 lvls

   real (r8) :: absorb_frac

!-----------------------------------------------------------------------
!
!  compute friction velocity USTAR.  compute on U-grid and average
!  to T-grid.
!
!-----------------------------------------------------------------------

   bid = this_block%local_id

   if (present(SMFT)) then
      USTAR = sqrt(sqrt(SMFT(:,:,1)**2 + SMFT(:,:,2)**2))
   else
      WORK = sqrt(sqrt(SMF(:,:,1)**2 + SMF(:,:,2)**2))
      call ugrid_to_tgrid(USTAR,WORK,bid)
   endif

!-----------------------------------------------------------------------
!
!  compute density and expansion coefficients at surface
!
!-----------------------------------------------------------------------

   WORK = merge(-c2,TRCR(:,:,1,1),TRCR(:,:,1,1) < -c2)

   call state(1,1,WORK,TRCR(:,:,1,2),this_block, &
                  RHOFULL=RHO1, DRHODT=TALPHA, DRHODS=SBETA)

!-----------------------------------------------------------------------
!
!  compute turbulent and radiative sfc buoyancy forcing
!
!-----------------------------------------------------------------------

   where (RHO1 /= c0)
      BO    = grav*(-TALPHA*STF(:,:,1) - &
                      SBETA*STF(:,:,2))/RHO1

      BOSOL = -grav*TALPHA*SHF_QSW/RHO1
   elsewhere
      BO    = c0
      BOSOL = c0
   end where

!-----------------------------------------------------------------------
!
!  Find bulk Richardson number at every grid level until > Ricr
!  max values when Ricr never satisfied are KBL = KMT and
!  HBLT = -zgrid(KMT)
!
!  NOTE: the reference depth is -epssfc/2.*zgrid(i,k), but the 
!        reference u,v,t,s values are simply the surface layer 
!        values and not the averaged values from 0 to 2*ref.depth,
!        which is necessary for very fine grids(top layer < 2m 
!        thickness)
!
!
!  Initialize hbl and kbl to bottomed out values
!
!-----------------------------------------------------------------------

   kup = 1
   kdn = 2

   RI_BULK(:,:,kup) = c0 
   KBL = merge(KMT(:,:,bid), 1, (KMT(:,:,bid) > 1))

   if (partial_bottom_cells) then
      do kl=2,km
         where (kl == KBL) HBLT = -zgrid(kl-1) + & 
                                  p5*(DZT(:,:,kl  ,bid) + &
                                      DZT(:,:,kl-1,bid))
      enddo
      where (1 == KBL) HBLT = -zgrid(1)
   else   
      do kl=1,km
         where (kl == KBL) HBLT = -zgrid(kl)
      enddo
   endif

!-----------------------------------------------------------------------
!
!  compute velocity shear squared and average to T-grid
!
!-----------------------------------------------------------------------

   do kl = 2,km
 
      WORK = (UUU(:,:,1)-UUU(:,:,kl))**2 + &
             (VVV(:,:,1)-VVV(:,:,kl))**2

      if (partial_bottom_cells) then
         WORK = WORK/(-zgrid(kl-1) + & 
                      p5*(DZU(:,:,kl  ,bid) + &
                          DZU(:,:,kl-1,bid) - &
                          DZU(:,:,1   ,bid)))**2
      endif

      call ugrid_to_tgrid(VSHEAR,WORK,bid)

!-----------------------------------------------------------------------
!
!     compute bfsfc= Bo + radiative contribution down to hbf * hbl
!     add epsilon to BFSFC to ensure never = 0
!
!-----------------------------------------------------------------------

      if (lshort_wave) then
         if (partial_bottom_cells) then
            do j = 1,ny_block
            do i = 1,nx_block
               call sw_absorb_frac(-zgrid(kl-1) +           &
                                   p5*(DZT(i,j,kl  ,bid) +  &
                                       DZT(i,j,kl-1,bid)),  &
                                   absorb_frac)
               BFSFC(i,j) = BO(i,j) + BOSOL(i,j)*(c1 - absorb_frac) 
            enddo
            enddo
         else
            call sw_absorb_frac(-zgrid(kl),absorb_frac)
            BFSFC = BO + BOSOL * (c1 - absorb_frac)
         endif
      else
         BFSFC = BO
      endif

      STABLE = MERGE(c1, c0, BFSFC >= c0)

      BFSFC  = BFSFC + STABLE*eps

!-----------------------------------------------------------------------
!
!     compute velocity scales at sigma, for hbl = -zgrid(kl)
!
!-----------------------------------------------------------------------

      if (partial_bottom_cells) then
          WORK  = -zgrid(kl-1) + p5*(DZT(:,:,kl-1,bid) + &
                                     DZT(:,:,kl  ,bid))
      else
          WORK  = -zgrid(kl)
      endif
      SIGMA = STABLE + (c1- STABLE) * epssfc

      call wscale(SIGMA, WORK, USTAR, BFSFC, 2, WM, WS)

!-----------------------------------------------------------------------
!
!     compute the turbulent shear contribution to RI_BULK and store
!     in WM.
!
!-----------------------------------------------------------------------

      if (partial_bottom_cells) then
         if (kl < km) then
            WM = WORK*Vtc*WS*sqrt(abs(                            &
                 p5*( DBLOC(:,:,kl-1)/                            &
                 (p5*(DZT(:,:,kl-1,bid) + DZT(:,:,kl  ,bid))) +   &
                 DBLOC(:,:,kl  )/                                 &
                 (p5*(DZT(:,:,kl  ,bid) + DZT(:,:,kl+1,bid))) )))
         else
            WM = WORK*Vtc*WS*sqrt(abs(                            &
                 p5*( DBLOC(:,:,kl-1)/                            &
                 (p5*(DZT(:,:,kl-1,bid) + DZT(:,:,kl  ,bid))) +   &
                 DBLOC(:,:,kl  )/(p5*DZT(:,:,kl  ,bid)) )))
         end if
      else
         WM = WORK*Vtc*WS*sqrt(abs(                             &
              p5*( DBLOC(:,:,kl-1)/(zgrid(kl-1)-zgrid(kl  )) +  &
              DBLOC(:,:,kl  )/(zgrid(kl  )-zgrid(kl+1)) ) ))
      endif

!-----------------------------------------------------------------------
! 
!     compute bulk Richardson number at new level, dunder
!     note: Ritop needs to be zero on land and ocean bottom
!           points so that the following if statement gets triggered
!           correctly. otherwise, hbl might get set to (big) negative
!           values, that might exceed the limit for the "exp" function
!           in short wave absorption
!
!-----------------------------------------------------------------------

      if (partial_bottom_cells) then
         WORK = merge( DBSFC(:,:,kl)/(-zgrid(kl-1)+            &
                                      p5*(DZT(:,:,kl-1,bid) +  &
                                          DZT(:,:,kl  ,bid) -  &
                                          DZT(:,:,1   ,bid))), & 
                       c0, KMT(:,:,bid) >= kl)
         WM = WM/(-zgrid(kl-1) +          &
                  p5*(DZT(:,:,kl-1,bid) + &
                      DZT(:,:,kl  ,bid) - &
                      DZT(:,:,1   ,bid)))**2
         RI_BULK(:,:,kdn) = WORK/(VSHEAR+WM+eps/(-zgrid(kl-1)+   &
                                   p5*(DZU(:,:,kl,bid) +         &
                                       DZU(:,:,kl-1,bid) -       &
                                       DZU(:,:,1,bid)))**2)
      else
         WORK = MERGE( (zgrid(1)-zgrid(kl))*DBSFC(:,:,kl), &
                      c0, KMT(:,:,bid) >= kl)
         RI_BULK(:,:,kdn) = WORK/(VSHEAR+WM+eps)
      endif

!-----------------------------------------------------------------------
!
!     linear interpolate to find hbl where Rib = Ricr
!
!-----------------------------------------------------------------------

      if (partial_bottom_cells) then
         where ((KBL == KMT(:,:,bid)) .and. (RI_BULK(:,:,kdn) > Ricr))
            HBLT = -zgrid(kl-1) + p5*(DZT(:,:,kl-1,bid) +        &
                                      DZT(:,:,kl  ,bid))*        &
                                (Ricr             - RI_BULK(:,:,kup))/  &
                                (RI_BULK(:,:,kdn) - RI_BULK(:,:,kup))
            KBL = kl
         endwhere
      else
         where ((KBL == KMT(:,:,bid)) .and. (RI_BULK(:,:,kdn) > Ricr))
            HBLT = -zgrid(kl-1) + (zgrid(kl-1)-zgrid(kl)) *             &
                                (Ricr             - RI_BULK(:,:,kup))/  &
                                (RI_BULK(:,:,kdn) - RI_BULK(:,:,kup))
            KBL = kl
         endwhere
      endif

!-----------------------------------------------------------------------
!
!     swap klevel index for RI_BULK and move to next level
!
!-----------------------------------------------------------------------

      ktmp = kup
      kup = kdn
      kdn = ktmp

   end do

!-----------------------------------------------------------------------
!
!  correct stability and buoyancy forcing for SW up to boundary layer
!
!-----------------------------------------------------------------------

   if (lshort_wave) then
      do j = 1,ny_block
      do i = 1,nx_block
         call sw_absorb_frac(HBLT(i,j),absorb_frac)
         BFSFC(i,j)  = BO(i,j) + BOSOL(i,j)*(c1 - absorb_frac) 
      enddo
      enddo
   endif

   STABLE = MERGE(c1, c0, BFSFC >= c0)

   BFSFC  = BFSFC + STABLE * eps ! ensures bfsfc never=0

!-----------------------------------------------------------------------
!
!  for stable forcing, bndy layer depth must not exceed h_ekman or
!  h_monin-obukhov.  if exceeds limit, compute new hbl, kbl.
!  for eckman, must compute coriolis parameter on T-grid
!
!-----------------------------------------------------------------------

   if (lcheckekmo) then

      where (BFSFC > c0)
         HEKMAN = cekman*USTAR/(abs(FCORT(:,:,bid))+eps)
         HMONOB = cmonob*USTAR*USTAR*USTAR/vonkar/BFSFC
         HLIMIT = STABLE*MIN(HEKMAN,HMONOB) + (STABLE-c1)*zgrid(km)

         HBLT = MIN(HBLT,HLIMIT)
         HBLT = MAX(HBLT,-zgrid(1))
      endwhere

      KBL = KMT(:,:,bid)
      do kl=2,km
         if (partial_bottom_cells) then
            where ((KBL == KMT(:,:,bid)) .and.                   &
                   (-zgrid(kl-1) + p5*(DZT(:,:,kl-1,bid) +       &
                                       DZT(:,:,kl,bid)) > HBLT)) &
               KBL = kl
         else
            where ((KBL == KMT(:,:,bid)) .and. (-zgrid(kl) > HBLT)) &
               KBL = kl
         endif
      end do

!-----------------------------------------------------------------------
!
!     correct stability and buoyancy forcing for SW up to boundary layer
!
!-----------------------------------------------------------------------

      if (lshort_wave) then
         do j = 1,ny_block
         do i = 1,nx_block
            call sw_absorb_frac(HBLT(i,j),absorb_frac)
            BFSFC(i,j)  = BO(i,j) + BOSOL(i,j)*(c1 - absorb_frac) 
         enddo
         enddo

         STABLE = MERGE(c1, c0, BFSFC >= c0)
         BFSFC  = BFSFC + STABLE * eps ! ensures bfsfc never=0
      endif
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine bldepth

!***********************************************************************
!BOP
! !IROUTINE: blmix
! !INTERFACE:

 subroutine blmix(VISC, VDC, HBLT, USTAR, BFSFC, STABLE, &
                  KBL, GHAT, this_block) 

! !DESCRIPTION:
!  This routine computes mixing coefficients within boundary layer 
!  which depend on surface forcing and the magnitude and gradient 
!  of interior mixing below the boundary layer (matching).  These
!  quantities have been computed in other routines.
!
!  Caution: if mixing bottoms out at hbl = -zgrid(km) then
!  fictitious layer at km+1 is needed with small but finite width 
!  hwide(km+1).
!
! !REVISION HISTORY:
!  same as module

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,0:km+1), intent(inout) :: & 
      VISC               ! interior mixing coeff on input
                         ! combined interior/bndy layer coeff output

   real (r8), dimension(nx_block,ny_block,0:km+1,2),intent(inout) :: &
      VDC        ! diffusivity for tracer diffusion

! !INPUT PARAMETERS:

   integer (int_kind), dimension(nx_block,ny_block), intent(in) ::  &
      KBL                    ! index of first lvl below hbl

   real (r8), dimension(nx_block,ny_block), intent(in) ::  &
      HBLT,                 & ! boundary layer depth
      BFSFC,                & ! surface buoyancy forcing
      STABLE,               & ! =1 stable forcing; =0 unstab
      USTAR                   ! surface friction velocity

   type (block), intent(in) :: &
      this_block          ! block information for current block

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,km),intent(out) :: &
      GHAT                ! non-local mixing coefficient

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      k,kp1,             &! dummy k level index
      bid                 ! local block index

   integer (int_kind), dimension(nx_block,ny_block) :: &
      KN                  ! klvl closest to HBLT

   real (r8), dimension(3,nx_block,ny_block,km) :: &
      BLMC                ! bndy layer mixing coefs

   real (r8), dimension(3,nx_block,ny_block) :: &
      GAT1,              &! shape function at sigma=1
      DAT1,              &! derivative of shape function 
      DKM1                ! bndy layer difs at kbl-1 lvl

   real (r8), dimension(nx_block,ny_block) :: &
      WM,WS,             &! turbulent velocity scales
      CASEA,             &! =1 in case A, =0 in case B     
      SIGMA,             &! normalized depth (d/hbl)
      VISCH,             &! viscosity at hbl
      DIFTH,             &! temp diffusivity at hbl
      DIFSH,             &! tracer diffusivity at hbl
      DELHAT, R, DVDZUP, DVDZDN, &
      VISCP, DIFTP, DIFSP, F1, &
      WORK1,WORK2

!-----------------------------------------------------------------------
!
!  compute velocity scales at hbl
!
!-----------------------------------------------------------------------

   bid = this_block%local_id

   SIGMA = STABLE + (c1- STABLE)*epssfc

   call wscale(SIGMA, HBLT, USTAR, BFSFC, 3, WM, WS)

!-----------------------------------------------------------------------
!
!  determine caseA = 0 if closer to KBL than KBL-1
!  KN is then the closest klevel to HBLT
!
!-----------------------------------------------------------------------

   if (partial_bottom_cells) then
      do k=2,km
         where (k == KBL)
            CASEA  = p5 + SIGN(p5, &
                           -zgrid(k-1)+p5*DZT(:,:,k-1,bid)-HBLT)
         endwhere
      enddo
      where (1 == KBL)
         CASEA  = p5 + SIGN(p5, -zgrid(0)-HBLT)
      end where
   else
      do k=1,km
         where (k == KBL)
            CASEA  = p5 + SIGN(p5, -zgrid(k)-p5*hwide(k)-HBLT)
         endwhere
      enddo
   endif

   KN = NINT(CASEA)*(KBL-1) + (1-NINT(CASEA))*KBL

!-----------------------------------------------------------------------
!
!  find the interior viscosities and derivatives at hbl by 
!  interpolating derivative values at vertical interfaces.  compute
!  matching conditions for shape function.  
!
!-----------------------------------------------------------------------

   F1 = STABLE*c5*BFSFC/(USTAR**4+eps)

   do k=1,km

      if (partial_bottom_cells) then

         if (k == 1) then
            WORK1 = c0
         else            
            WORK1 = DZT(:,:,k-1,bid)
         end if
         if (k == km) then
            WORK2 = eps
         else
            WORK2 = DZT(:,:,k+1,bid)
         end if

         where (k == KN)

            DELHAT = - zgrid(k-1) + DZT(:,:,k,bid) + p5*WORK1 - HBLT  
            R      = c1 - DELHAT /DZT(:,:,k,bid)
            DVDZUP = (VISC(:,:,k-1) - VISC(:,:,k  ))/DZT(:,:,k  ,bid)
            DVDZDN = (VISC(:,:,k  ) - VISC(:,:,k+1))/WORK2
            VISCP  = p5*( (c1-R)*(DVDZUP + abs(DVDZUP)) + &
                              R *(DVDZDN + abs(DVDZDN)) )
            DVDZUP = (VDC(:,:,k-1,2) - VDC(:,:,k  ,2))/DZT(:,:,k  ,bid)
            DVDZDN = (VDC(:,:,k  ,2) - VDC(:,:,k+1,2))/WORK2
            DIFSP  = p5*( (c1-R)*(DVDZUP + abs(DVDZUP)) + &
                              R *(DVDZDN + abs(DVDZDN)) )
            DVDZUP = (VDC(:,:,k-1,1) - VDC(:,:,k  ,1))/DZT(:,:,k  ,bid)
            DVDZDN = (VDC(:,:,k  ,1) - VDC(:,:,k+1,1))/WORK2
            DIFTP  = p5*( (c1-R)*(DVDZUP + abs(DVDZUP)) + &
                              R *(DVDZDN + abs(DVDZDN)) )

            VISCH = VISC(:,:,k)  + VISCP*DELHAT
            DIFSH = VDC(:,:,k,2) + DIFSP*DELHAT
            DIFTH = VDC(:,:,k,1) + DIFTP*DELHAT

            GAT1(1,:,:) = VISCH / HBLT /(WM+eps)
            DAT1(1,:,:) = -VISCP/(WM+eps) + F1*VISCH

            GAT1(2,:,:) = DIFSH / HBLT /(WS+eps)
            DAT1(2,:,:) = -DIFSP/(WS+eps) + F1*DIFSH

            GAT1(3,:,:) = DIFTH / HBLT /(WS+eps)
            DAT1(3,:,:) = -DIFTP/(WS+eps) + F1*DIFTH

         endwhere

      else

         where (k == KN)

            DELHAT = p5*hwide(k) - zgrid(k) - HBLT        
            R      = c1 - DELHAT / hwide(k)
            DVDZUP = (VISC(:,:,k-1) - VISC(:,:,k  ))/hwide(k)
            DVDZDN = (VISC(:,:,k  ) - VISC(:,:,k+1))/hwide(k+1)
            VISCP  = p5*( (c1-R)*(DVDZUP + abs(DVDZUP)) + &
                              R *(DVDZDN + abs(DVDZDN)) )
            DVDZUP = (VDC(:,:,k-1,2) - VDC(:,:,k  ,2))/hwide(k)
            DVDZDN = (VDC(:,:,k  ,2) - VDC(:,:,k+1,2))/hwide(k+1)
            DIFSP  = p5*( (c1-R)*(DVDZUP + abs(DVDZUP)) + &
                              R *(DVDZDN + abs(DVDZDN)) )
            DVDZUP = (VDC(:,:,k-1,1) - VDC(:,:,k  ,1))/hwide(k)
            DVDZDN = (VDC(:,:,k  ,1) - VDC(:,:,k+1,1))/hwide(k+1)
            DIFTP  = p5*( (c1-R)*(DVDZUP + abs(DVDZUP)) + &
                              R *(DVDZDN + abs(DVDZDN)) )
   
            VISCH = VISC(:,:,k)  + VISCP*DELHAT
            DIFSH = VDC(:,:,k,2) + DIFSP*DELHAT
            DIFTH = VDC(:,:,k,1) + DIFTP*DELHAT

            GAT1(1,:,:) = VISCH / HBLT /(WM+eps)
            DAT1(1,:,:) = -VISCP/(WM+eps) + F1*VISCH

            GAT1(2,:,:) = DIFSH / HBLT /(WS+eps)
            DAT1(2,:,:) = -DIFSP/(WS+eps) + F1*DIFSH

            GAT1(3,:,:) = DIFTH / HBLT /(WS+eps)
            DAT1(3,:,:) = -DIFTP/(WS+eps) + F1*DIFTH

         endwhere

      endif                   ! pbc

   enddo

   DAT1 = min(DAT1,c0)

!-----------------------------------------------------------------------
!
!  compute the dimensionless shape functions and diffusivities
!  at the grid interfaces.  also compute function for non-local
!  transport term (GHAT).
!
!-----------------------------------------------------------------------

   do k = 1,km       

      if (partial_bottom_cells) then
         if (k > 1) then
            SIGMA = (-zgrid(k-1) + p5*DZT(:,:,k-1,bid) +  &
                     DZT(:,:,k,bid)) / HBLT 
         else
            SIGMA = (-zgrid(k) + p5*hwide(k)) / HBLT     
         end if
      else
         SIGMA = (-zgrid(k) + p5*hwide(k)) / HBLT     
      endif
      F1    = STABLE*SIGMA + (c1-STABLE)*min(SIGMA,epssfc)

      call wscale(F1, HBLT, USTAR, BFSFC, 3, WM, WS)

      BLMC(1,:,:,k) = HBLT*WM*SIGMA*(c1 + SIGMA*((SIGMA-c2)+ &
          (c3-c2*SIGMA)*GAT1(1,:,:)+(SIGMA-c1)*DAT1(1,:,:))) 
      BLMC(2,:,:,k) = HBLT*WS*SIGMA*(c1 + SIGMA*((SIGMA-c2)+ &
          (c3-c2*SIGMA)*GAT1(2,:,:)+(SIGMA-c1)*DAT1(2,:,:)))
      BLMC(3,:,:,k) = HBLT*WS*SIGMA*(c1 + SIGMA*((SIGMA-c2)+ &    
          (c3-c2*SIGMA)*GAT1(3,:,:)+(SIGMA-c1)*DAT1(3,:,:)))

      GHAT(:,:,k) = (c1-STABLE)* cg/(WS*HBLT +eps)

   end do

!-----------------------------------------------------------------------
!
!  find diffusivities at kbl-1 grid level
!
!-----------------------------------------------------------------------

   do k=1,km
      where (k == KBL-1)
         SIGMA = -zgrid(k)/HBLT          
      endwhere
   enddo

   F1 = STABLE*SIGMA + (c1-STABLE)*min(SIGMA,epssfc)        
   call wscale(F1, HBLT, USTAR, BFSFC, 3, WM, WS)

   DKM1(1,:,:) = HBLT*WM*SIGMA*(c1+SIGMA*((SIGMA-c2) &
            +(c3-c2*SIGMA)*GAT1(1,:,:)+(SIGMA-c1)*DAT1(1,:,:)))
   DKM1(2,:,:) = HBLT*WS*SIGMA*(c1+SIGMA*((SIGMA-c2) &
            +(c3-c2*SIGMA)*GAT1(2,:,:)+(SIGMA-c1)*DAT1(2,:,:)))
   DKM1(3,:,:) = HBLT*WS*SIGMA*(c1+SIGMA*((SIGMA-c2) &       
            +(c3-c2*SIGMA)*GAT1(3,:,:)+(SIGMA-c1)*DAT1(3,:,:)))

!-----------------------------------------------------------------------
!
!  compute the enhanced mixing
!
!-----------------------------------------------------------------------

   do k=1,km-1

      if (partial_bottom_cells) then
         if (k == 1) then 
            WORK1 = -p5*DZT(:,:,k,bid)
         else
            WORK1 = zgrid(k-1) - p5*(DZT(:,:,k-1,bid) + &
                                     DZT(:,:,k  ,bid))
         end if
         where (k == (KBL - 1)) &
            DELHAT = (HBLT + WORK1)/(p5*(DZT(:,:,k  ,bid) + &
                                         DZT(:,:,k+1,bid)))
      else
         where (k == (KBL - 1)) &
            DELHAT = (HBLT + zgrid(k))/(zgrid(k)-zgrid(k+1))
      endif

      where (k == (KBL - 1))

         BLMC(1,:,:,k) = (c1-DELHAT)*VISC(:,:,k) +  DELHAT *(       &
                         (c1-DELHAT)**2 *DKM1(1,:,:) + DELHAT**2 *  &
                         (CASEA*VISC(:,:,k)                         &
                          +(c1-CASEA)*BLMC(1,:,:,k)))

         BLMC(2,:,:,k) = (c1-DELHAT)*VDC(:,:,k,2) + DELHAT*(        &
                         (c1-DELHAT)**2 * DKM1(2,:,:) + DELHAT**2 * &
                         (CASEA*VDC(:,:,k,2)                        &
                          +(c1-CASEA)*BLMC(2,:,:,k)))

         BLMC(3,:,:,k) = (c1-DELHAT)*VDC(:,:,k,1) + DELHAT *(       &
                         (c1-DELHAT)**2 * DKM1(3,:,:) + DELHAT**2 * &
                         (CASEA*VDC(:,:,k,1)                        &
                         +(c1-CASEA)*BLMC(3,:,:,k)))

         GHAT(:,:,k) = (c1-CASEA) * GHAT(:,:,k)

      endwhere
   end do

!-----------------------------------------------------------------------
!
!  combine interior and boundary layer coefficients and nonlocal term
!
!-----------------------------------------------------------------------

   do k=1,km
      where (k < KBL) 
         VISC(:,:,k)  = BLMC(1,:,:,k)
         VDC(:,:,k,2) = BLMC(2,:,:,k)
         VDC(:,:,k,1) = BLMC(3,:,:,k)
      elsewhere
         GHAT(:,:,k) = c0
      endwhere
   enddo

!-----------------------------------------------------------------------
!EOC

 end subroutine blmix

!***********************************************************************
!BOP
! !IROUTINE: wscale
! !INTERFACE:

 subroutine wscale(SIGMA, HBL, USTAR, BFSFC, m_or_s, WM, WS)

! !DESCRIPTION:
!  Computes turbulent velocity scales.
!
!  For $\zeta \geq 0, 
!    w_m = w_s = \kappa U^\star/(1+5\zeta)$
!
!  For $\zeta_m \leq \zeta < 0, 
!    w_m = \kappa U^\star (1-16\zeta)^{1\over 4}$
!
!  For $\zeta_s \leq \zeta < 0, 
!    w_s = \kappa U^\star (1-16\zeta)^{1\over 2}$
!
!  For $\zeta < \zeta_m, 
!    w_m = \kappa U^\star (a_m - c_m\zeta)^{1\over 3}$
!
!  For $\zeta < \zeta_s, 
!    w_s = \kappa U^\star (a_s - c_s\zeta)^{1\over 3}$
!
!  where $\kappa$ is the von Karman constant.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
      m_or_s              ! flag =1 for wm only, 2 for ws, 3 for both

   real (r8), dimension(nx_block,ny_block), intent(in) :: &
      SIGMA,             &! normalized depth (d/hbl)
      HBL,               &! boundary layer depth
      BFSFC,             &! surface buoyancy forcing
      USTAR               ! surface friction velocity

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(out) :: &
      WM,                &! turb velocity scales: momentum
      WS                  ! turb velocity scales: tracer

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: i,j  ! dummy loop indices

   real (r8), dimension(nx_block,ny_block) :: &
      ZETA,           &! d/L or sigma*hbl/L(monin-obk)
      ZETAH            ! sigma*hbl*vonkar*BFSFC or ZETA = ZETAH/USTAR**3

!-----------------------------------------------------------------------
!
!  compute zetah and zeta - surface layer is special case
!
!-----------------------------------------------------------------------

   ZETAH = SIGMA*HBL*vonkar*BFSFC
   ZETA  = ZETAH/(USTAR**3 + eps)

!-----------------------------------------------------------------------
!
!  compute velocity scales for momentum
!
!-----------------------------------------------------------------------

   if (m_or_s == 1 .or. m_or_s == 3) then
      do j=1,ny_block
      do i=1,nx_block
         if (ZETA(i,j) >= c0) then ! stable region
            WM(i,j) = vonkar*USTAR(i,j)/(c1 + c5*ZETA(i,j))
         else if (ZETA(i,j) >= zeta_m) then
            WM(i,j) = vonkar*USTAR(i,j)*(c1 - c16*ZETA(i,j))**p25
         else
            WM(i,j) = vonkar*(a_m*(USTAR(i,j)**3)-c_m*ZETAH(i,j))**p33
         endif
      end do
      end do
   endif

!-----------------------------------------------------------------------
!
!  compute velocity scales for tracers
!
!-----------------------------------------------------------------------

   if (m_or_s == 2 .or. m_or_s == 3) then
      do j=1,ny_block
      do i=1,nx_block
         if (ZETA(i,j) >= c0) then
            WS(i,j) = vonkar*USTAR(i,j)/(c1 + c5*ZETA(i,j))
         else if (ZETA(i,j) >= zeta_s) then
            WS(i,j) = vonkar*USTAR(i,j)*SQRT(c1 - c16*ZETA(i,j))
         else
            WS(i,j) = vonkar*(a_s*(USTAR(i,j)**3)-c_s*ZETAH(i,j))**p33
         endif
      end do
      end do
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine wscale

!***********************************************************************
!BOP
! !IROUTINE: ddmix
! !INTERFACE:

 subroutine ddmix(VDC, TRCR, this_block)

! !DESCRIPTION:
!  $R_\rho$ dependent interior flux parameterization.
!  Add double-diffusion diffusivities to Ri-mix values at blending
!  interface and below.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,km,nt), intent(in) :: &
      TRCR                ! tracers at current time

   type (block), intent(in) :: &
      this_block          ! block information for current block

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,0:km+1,2),intent(inout) :: &
      VDC        ! diffusivity for tracer diffusion

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) ::  k,kup,knxt

   real (r8), dimension(nx_block,ny_block) :: &
      ALPHADT,           &! alpha*DT  across interfaces
      BETADS,            &! beta *DS  across interfaces
      RRHO,              &! dd density ratio
      DIFFDD,            &! dd diffusivity scale
      PRANDTL             ! prandtl number

   real (r8), dimension(nx_block,ny_block,2) :: &
      TALPHA,            &! temperature expansion coeff
      SBETA               ! salinity    expansion coeff

!-----------------------------------------------------------------------
!
!  compute alpha*DT and beta*DS at interfaces.  use RRHO and
!  PRANDTL for temporary storage for call to state
!
!-----------------------------------------------------------------------

   kup  = 1
   knxt = 2

   PRANDTL = merge(-c2,TRCR(:,:,1,1),TRCR(:,:,1,1) < -c2)

   call state(1, 1, PRANDTL, TRCR(:,:,1,2), this_block, &
                    RHOFULL=RRHO, &
                    DRHODT=TALPHA(:,:,kup), DRHODS=SBETA(:,:,kup))

   do k=1,km

      if ( k < km ) then

         PRANDTL = merge(-c2,TRCR(:,:,k+1,1),TRCR(:,:,k+1,1) < -c2)

         call state(k+1, k+1, PRANDTL, TRCR(:,:,k+1,2),              &
                              this_block,                            &
                              RHOFULL=RRHO, DRHODT=TALPHA(:,:,knxt), &
                                            DRHODS= SBETA(:,:,knxt))

         ALPHADT = -p5*(TALPHA(:,:,kup) + TALPHA(:,:,knxt)) &
                      *(TRCR(:,:,k,1) - TRCR(:,:,k+1,1))

         BETADS  = p5*( SBETA(:,:,kup) +  SBETA(:,:,knxt)) &
                     *(TRCR(:,:,k,2) - TRCR(:,:,k+1,2))

         kup  = knxt
         knxt = 3 - kup

      else

         ALPHADT = c0
         BETADS  = c0

      endif       

!-----------------------------------------------------------------------
!
!     salt fingering case
!
!-----------------------------------------------------------------------

      where ( ALPHADT > BETADS .and. BETADS > c0 )

         RRHO       = MIN(ALPHADT/BETADS, Rrho0)
         DIFFDD     = dsfmax*(c1-((RRHO-c1)/(Rrho0-c1))**2)**3
         VDC(:,:,k,1) = VDC(:,:,k,1) + 0.7_r8*DIFFDD
         VDC(:,:,k,2) = VDC(:,:,k,2) + DIFFDD

      endwhere

!-----------------------------------------------------------------------
!
!     diffusive convection
!
!-----------------------------------------------------------------------

      where ( ALPHADT < c0 .and. BETADS < c0 .and. ALPHADT > BETADS )
         RRHO    = ALPHADT / BETADS
         DIFFDD  = 1.5e-2_r8*0.909_r8* &
                   exp(4.6_r8*exp(-0.54_r8*(c1/RRHO-c1)))
         PRANDTL = 0.15_r8*RRHO
      elsewhere
         RRHO    = c0
         DIFFDD  = c0
         PRANDTL = c0
      endwhere

      where (RRHO > p5) PRANDTL = (1.85_r8 - 0.85_r8/RRHO)*RRHO

      VDC(:,:,k,1) = VDC(:,:,k,1) + DIFFDD
      VDC(:,:,k,2) = VDC(:,:,k,2) + PRANDTL*DIFFDD

   end do

!-----------------------------------------------------------------------
!EOC

 end subroutine ddmix

!***********************************************************************
!BOP
! !IROUTINE: buoydiff
! !INTERFACE:

 subroutine buoydiff(DBLOC, DBSFC, TRCR, this_block)

! !DESCRIPTION:
!  This routine calculates the buoyancy differences at model levels.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,km,nt), intent(in) :: &
      TRCR                ! tracers at current time

   type (block), intent(in) :: &
      this_block          ! block information for current block

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,km), intent(out) :: & 
      DBLOC,         &! buoyancy difference between adjacent levels
      DBSFC           ! buoyancy difference between level and surface

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      k,                 &! vertical level index
      kprev, klvl, ktmp, &! indices for 2-level TEMPK array
      bid                 ! local block index

   real (r8), dimension(nx_block,ny_block) :: &
      RHO1,              &! density of sfc t,s displaced to k
      RHOKM,             &! density of t(k-1),s(k-1) displaced to k
      RHOK,              &! density at level k
      TEMPSFC,           &! adjusted temperature at surface
      TALPHA,            &! temperature expansion coefficient
      SBETA               ! salinity    expansion coefficient

   real (r8), dimension(nx_block,ny_block,2) :: &
      TEMPK               ! temp adjusted for freeze at levels k,k-1

!-----------------------------------------------------------------------
!
!  calculate density and buoyancy differences at surface
!
!-----------------------------------------------------------------------

   TEMPSFC = merge(-c2,TRCR(:,:,1,1),TRCR(:,:,1,1) < -c2)

   bid = this_block%local_id

   klvl  = 2
   kprev = 1

   TEMPK(:,:,kprev) = TEMPSFC
   DBSFC(:,:,1) = c0

!-----------------------------------------------------------------------
!
!  calculate DBLOC and DBSFC for all other levels
!
!-----------------------------------------------------------------------

   do k = 2,km

      TEMPK(:,:,klvl) = merge(-c2,TRCR(:,:,k,1),TRCR(:,:,k,1) < -c2)

      call state(k, k, TEMPSFC,          TRCR(:,:,1  ,2), &
                       this_block, RHOFULL=RHO1)
      call state(k, k, TEMPK(:,:,kprev), TRCR(:,:,k-1,2), &
                       this_block, RHOFULL=RHOKM)
      call state(k, k, TEMPK(:,:,klvl),  TRCR(:,:,k  ,2), &
                       this_block, RHOFULL=RHOK)

      where (RHOK /= c0)
         DBSFC(:,:,k)   = grav*(c1 - RHO1/RHOK)
         DBLOC(:,:,k-1) = grav*(c1 - RHOKM/RHOK)
      elsewhere
         DBSFC(:,:,k)   = c0
         DBLOC(:,:,k-1) = c0
      end where

      where (k-1 >= KMT(:,:,bid)) DBLOC(:,:,k-1) = c0

      ktmp  = klvl
      klvl  = kprev
      kprev = ktmp

   enddo

   DBLOC(:,:,km) = c0

!-----------------------------------------------------------------------
!EOC

 end subroutine buoydiff

!***********************************************************************
!BOP
! !IROUTINE: add_kpp_sources
! !INTERFACE:

 subroutine add_kpp_sources(SRCARRAY, k, this_block)

! !DESCRIPTION:
!  This routine adds KPP non local mixing term to the tracer source 
!  tendencies.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
      k                   ! vertical level index

   type (block), intent(in) :: &
      this_block          ! block information for current block

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,nt), intent(inout) :: &
      SRCARRAY                ! array of tracer sources

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      bid                 ! local block address

!-----------------------------------------------------------------------
!
!  if KPP not chosen, return
!
!-----------------------------------------------------------------------

   if (.not. allocated(KPP_SRC)) return

!-----------------------------------------------------------------------
!
!  determine location and add sources
!
!-----------------------------------------------------------------------

   bid = this_block%local_id

   SRCARRAY = SRCARRAY + KPP_SRC(:,:,k,:,bid)

!-----------------------------------------------------------------------
!EOC

 end subroutine add_kpp_sources

!***********************************************************************

 end module vmix_kpp

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
