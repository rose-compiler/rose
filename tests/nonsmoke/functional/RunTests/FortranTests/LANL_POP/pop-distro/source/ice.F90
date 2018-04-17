!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module ice

!BOP
! !MODULE: ice
!
! !DESCRIPTION:
!  This module currently contains routines for computing ice 
!  formation and the heat flux associated with ice formation.
!  This heat flux is sent to the ice model via the flux coupler.
!  In the future, this module could contain the driver for a
!  subroutinized ice model.
!
! !REVISION HISTORY:
!  CVS:$Id: ice.F90,v 1.11 2003/01/28 23:21:19 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod, only: int_kind, log_kind, char_len, r8
   use blocks, only: nx_block, ny_block, block
!   use distribution, only:
!   use domain, only: 
   use constants, only: cp_sw, latent_heat_fusion, delim_fmt, blank_fmt,     &
       sea_ice_salinity, ppt_to_salt, ocn_ref_salinity, c0, p5, hflux_factor
   use broadcast, only: broadcast_scalar
   use communicate, only: my_task, master_task
!   use io, only:
   use io_types, only: nml_in, nml_filename, stdout
   use time_management, only: freq_opt_never, freq_opt_nyear, freq_opt_nday, &
       freq_opt_nhour, freq_opt_nsecond, freq_opt_nstep, init_time_flag,     &
       max_blocks_clinic, km, nt, avg_ts, back_to_back, dtt, check_time_flag,&
       partial_bottom_cells, KMT, DZT, DZ, freq_opt_nmonth
!   use grid, only: 
   use exit_mod, only: sigAbort, exit_pop

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_ice,      &
             ice_formation, &
             tfreez,        &
             get_ice_flux

! !PUBLIC DATA MEMBERS:

   logical (log_kind), public :: &
      liceform            ! flag to turn on/off ice formation

   real (r8), public ::  &
      tlast_ice           ! time since last ice flux computed

   real (r8), dimension(:,:,:), allocatable, public :: &
      AQICE               ! sum of accumulated ice heat flux since tlast

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  module variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      ice_flag,          &! time flag id for ice formation
      ice_cpl_flag,      &! time flag id for coupled timestep
      kmxice              ! lowest level from which to integrate 
                          ! ice formation

   real (r8) ::          &
      salice,            &! sea ice salinity in msu
      salref,            &! ocean ref salinity in msu
      cp_over_lhfusion    ! cp_sw/latent_heat_fusion

   real (r8), dimension(:,:,:), allocatable :: &
      QICE                ! tot column cooling from ice form (in C*cm)

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE:
! !INTERFACE:

 subroutine init_ice

! !DESCRIPTION:
!  This routine initializes ice variables.  It must be called
!  before initializing restarts because this module add the accumulated
!  ice heat flux to the restart file.
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

   integer (int_kind) ::   &
      nml_error             ! namelist i/o error flag

   character (char_len) :: &
      ice_freq_opt          ! option for frequency of computing ice

   integer (int_kind) ::   &
      ice_freq_iopt,       &! int option for freq units
      ice_freq              ! freq for computing ice in above units

   namelist /ice_nml/ kmxice, ice_freq_opt, ice_freq

!-----------------------------------------------------------------------
!
!  read input namelists
!
!-----------------------------------------------------------------------

   cp_over_lhfusion = cp_sw/latent_heat_fusion

   kmxice           = 1
   ice_freq_opt     = 'never'
   ice_freq         = 100000

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=ice_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading ice_nml')
   endif

   if (my_task == master_task) then

      write(stdout,delim_fmt)
      write(stdout,blank_fmt)
      write(stdout,'(a11)') 'Ice options'
      write(stdout,blank_fmt)
      write(stdout,delim_fmt)

      !***
      !*** define salice and salref in msu
      !***
 
      salice = sea_ice_salinity*ppt_to_salt
      salref = ocn_ref_salinity*ppt_to_salt

      liceform = .true.

      select case (ice_freq_opt)
      case ('never')
         write(stdout,'(a22)') 'Ice formation disabled'
         liceform = .false.
         ice_freq_iopt = freq_opt_never
      case ('coupled')
         write(stdout,'(a44)') &
           'Ice formation computed on coupler time steps'
         ice_freq_iopt = freq_opt_never ! check coupler flag instead
      case ('nyear')
         write(stdout,'(a29,i4,a9)') 'Ice formation computed every ', &
                                      ice_freq,' years.  '
         ice_freq_iopt = freq_opt_nyear
      case ('nmonth')
         write(stdout,'(a29,i4,a9)') 'Ice formation computed every ', &
                                      ice_freq,' months. '
         ice_freq_iopt = freq_opt_nmonth
      case ('nday')
         write(stdout,'(a29,i4,a9)') 'Ice formation computed every ', &
                                      ice_freq,' days.   '
         ice_freq_iopt = freq_opt_nday
      case ('nhour')
         write(stdout,'(a29,i4,a9)') 'Ice formation computed every ', &
                                      ice_freq,' hours.  '
         ice_freq_iopt = freq_opt_nhour
      case ('nsecond')
         write(stdout,'(a29,i4,a9)') 'Ice formation computed every ', &
                                      ice_freq,' seconds.'
         ice_freq_iopt = freq_opt_nsecond
      case ('nstep')
         write(stdout,'(a29,i4,a9)') 'Ice formation computed every ', &
                                      ice_freq,' steps.  '
         ice_freq_iopt = freq_opt_nstep
      case default
         ice_freq_iopt = -1000
      end select

      if (liceform) then
         write(stdout,'(a20,1pe10.3)') 'Ice salinity(ppt) = ', &
                                        sea_ice_salinity
         write(stdout,'(a30,i3,a13)') 'Ice formation computed in top ', &
                                       kmxice, ' levels only.'
      endif

   endif

   call broadcast_scalar(ice_freq_iopt, master_task)

   if (ice_freq_iopt == -1000) then
      call exit_POP(sigAbort,'unknown restart frequency option')
   endif

   call broadcast_scalar(liceform,      master_task)

!-----------------------------------------------------------------------
!
!  if ice turned on, broadcast remaining vars and allocate memory
!
!-----------------------------------------------------------------------

   if (liceform) then
      call broadcast_scalar(ice_freq, master_task)
      call broadcast_scalar(kmxice,   master_task)
      call broadcast_scalar(salice,   master_task)
      call broadcast_scalar(salref,   master_task)
 
      !***
      !*** set up time flag
      !***

      ice_flag = init_time_flag('ice',default=.false.,    &
                                freq_opt = ice_freq_iopt, &
                                freq     = ice_freq)

      ice_cpl_flag = init_time_flag('coupled_ts')

      !***
      !*** must keep track of time since last ice flux computed
      !***

      tlast_ice = c0

      !***
      !*** allocate and initialize ice flux arrays
      !***

      allocate( QICE(nx_block,ny_block,max_blocks_clinic), & 
               AQICE(nx_block,ny_block,max_blocks_clinic))

      QICE  = c0
      AQICE = c0

   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine init_ice

!***********************************************************************
!BOP
! !IROUTINE:
! !INTERFACE:

 subroutine ice_formation(TNEW, SHF_IN, this_block)

! !DESCRIPTION:
!  This subroutine computes ocean heat flux due to sea-ice formation.
!  It forms the ice in the ocean whenever the temperature falls below
!  the freezing temperature and forms a quantity of ice equivalent
!  to the heat content corresponding to that temperature difference.
!  Once ice is formed the potential temperature and salinity fields 
!  are adjusted accordingly and the amount of heat used is accumulated
!  for sending to an ice model. Additionally, if the temperature
!  rises above the freezing temperature, this routine will use that
!  heat to melt previously formed ice and will accumulate this heat
!  flux to send to an ice model as a melt potential. The logic of this 
!  subroutine is based on a 1-d model by William Large and an
!  implementation of this model written by Ghokan Danabasoglu. 
!
! !REVISION HISTORY:
!  same as module

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,km,nt), intent(inout) :: &
      TNEW                ! tracers at new time level

! !INPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(in) :: &
      SHF_IN              ! surface heat flux

   type (block), intent(in) :: &
      this_block          ! block information for current block

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      k,                 &! vertical level index
      bid                 ! local block index

   real (r8), dimension(nx_block,ny_block) :: &
      POTICE,            &! potential amt of ice formation
      TFRZ                ! freezing temp of water

!-----------------------------------------------------------------------
!
!  increment time since last evaluation
!  exit if this is not time to compute ice formation
!
!-----------------------------------------------------------------------

   if (avg_ts .or. back_to_back) then
      tlast_ice = tlast_ice + p5*dtt
   else
      tlast_ice = tlast_ice + dtt
   endif

   if (check_time_flag(ice_flag) .or. &
       check_time_flag(ice_cpl_flag)) then

      bid = this_block%local_id

!-----------------------------------------------------------------------
!
!     initialize flux to zero
!
!-----------------------------------------------------------------------
 
      QICE(:,:,bid) = c0
      POTICE = c0
 
!-----------------------------------------------------------------------
!
!     compute frazil ice formation for sub-surface layers.  if ice
!     forms in lower layers but layers above are warm - the heat is
!     used to melt the ice.  total ice heat flux is accumulated.
!     WARNING: unless a monotone advection scheme is in place, 
!     advective errors could lead to temps that are far below freezing
!     in some locations and this scheme will form lots of ice.
!     ice formation should be limited to the top layer (kmxice=1)
!     if the advection scheme is not monotone.
!
!-----------------------------------------------------------------------
 
      do k=kmxice,1,-1

         !***
         !*** potice is the potential amount of ice formation 
         !*** (potice>0) or melting (potice<0) in layer k
         !***

         call tfreez(TFRZ,TNEW(:,:,k,2))
         if (partial_bottom_cells) then
            where (k <= KMT(:,:,bid)) &
               POTICE = (TFRZ - TNEW(:,:,k,1))*DZT(:,:,k,bid)
         else
            where (k <= KMT(:,:,bid)) &
              POTICE = (TFRZ - TNEW(:,:,k,1))*dz(k)
         endif

         !***
         !*** if potice < 0, use the heat to melt any ice
         !*** from lower layers
         !*** if potice > 0, keep on freezing (QICE < 0)
         !***

         POTICE = max(POTICE,QICE(:,:,bid))

         !***
         !*** adjust tracer values based on freeze/melt
         !***

         if (partial_bottom_cells) then
            TNEW(:,:,k,1) = TNEW(:,:,k,1) + POTICE/DZT(:,:,k,bid)  
            TNEW(:,:,k,2) = TNEW(:,:,k,2) + cp_over_lhfusion* & 
                            (salref - salice)*POTICE/DZT(:,:,k,bid)
         else
            TNEW(:,:,k,1) = TNEW(:,:,k,1) + POTICE/dz(k)  
            TNEW(:,:,k,2) = TNEW(:,:,k,2) + cp_over_lhfusion* &
                            (salref - salice)*POTICE/dz(k)
         endif

         !*** accumulate freezing potential
         QICE(:,:,bid) = QICE(:,:,bid) - POTICE

      end do ! k loop

!-----------------------------------------------------------------------
!
!     let any residual heat in the upper layer melt previously 
!     formed ice
!
!-----------------------------------------------------------------------
 
      k = 1
      AQICE(:,:,bid) = AQICE(:,:,bid) + QICE(:,:,bid)

!-----------------------------------------------------------------------
!
!     recalculate freezing potential based on adjusted T,S
!     only interested in melt potential now (POTICE < 0) - use this 
!     melt to offset any accumulated freezing (AQICE < 0) and
!     adjust T,S to reflect this melting
!
!-----------------------------------------------------------------------

      call tfreez(TFRZ,TNEW(:,:,k,2))
      if (partial_bottom_cells) then
         where (k <= KMT(:,:,bid)) &
            POTICE = (TFRZ - TNEW(:,:,k,1))*DZT(:,:,k,bid)
      else
         where (k <= KMT(:,:,bid)) &
            POTICE = (TFRZ - TNEW(:,:,k,1))*dz(k)
      endif

      POTICE = max(POTICE,AQICE(:,:,bid))

      AQICE(:,:,bid) = AQICE(:,:,bid) - POTICE

      if (partial_bottom_cells) then
         TNEW(:,:,k,1) = TNEW(:,:,k,1) + POTICE/DZT(:,:,k,bid)
         TNEW(:,:,k,2) = TNEW(:,:,k,2) + cp_over_lhfusion* &
                         (salref - salice)*POTICE/DZT(:,:,k,bid)
      else
         TNEW(:,:,k,1) = TNEW(:,:,k,1) + POTICE/dz(k)
         TNEW(:,:,k,2) = TNEW(:,:,k,2) + cp_over_lhfusion* &
                         (salref - salice)*POTICE/dz(k)
      endif

!-----------------------------------------------------------------------
!
!     compute the heat flux for input to the sea-ice model. 
!     note that either:
!       aqice<0 and TSFC=TFRZ => qflux=-aqice>0 (net ice made), or
!       aqice=0 and TSFC>TFRZ => qflux=TFRZ-TNEW(:,:,1,1)<0
!                                         (melt potential).
!
!     REMARK: qflux is needed only if it is time to communicate 
!             with the flux coupler. however, in order to isolate 
!             this subroutine from any coupling details, qflux is
!             computed at every ocean time step. note that qflux 
!             will be converted to W/m^2 later in sflx.F.
!
!-----------------------------------------------------------------------
 
      call tfreez(TFRZ,TNEW(:,:,k,2))
      if (partial_bottom_cells) then
         where (k <= KMT(:,:,bid)) &
            POTICE = (TFRZ - TNEW(:,:,k,1))*DZT(:,:,k,bid)
      else
         where (k <= KMT(:,:,bid)) &
            POTICE = (TFRZ - TNEW(:,:,k,1))*dz(k)
      endif

      QICE(:,:,bid) = POTICE - AQICE(:,:,bid)
 
   endif ! time to do ice

!-----------------------------------------------------------------------
!EOC

 end subroutine ice_formation

!***********************************************************************
!BOP
! !IROUTINE:
! !INTERFACE:

 subroutine tfreez(TFRZ,SALT)

! !DESCRIPTION:
!  This function computes the freezing point of salt water.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(in) :: &
      SALT                ! salinity in model units (g/g)

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(out) :: &
      TFRZ                ! freezing temperature of water in deg C

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  use only the first salinity term in the expansion
!
!-----------------------------------------------------------------------

   !TFRZ = -0.0544_r8*SALT*salt_to_ppt
   TFRZ = -1.8_r8

!-----------------------------------------------------------------------
!EOC

 end subroutine tfreez

!***********************************************************************
!BOP
! !IROUTINE:
! !INTERFACE:

 subroutine get_ice_flux(ICE_FLUX)

! !DESCRIPTION:
!  This function returns total accumulated ice heat flux (in W/m2) over
!  the time interval $tlast_{ice}$ and resets the ice flux accumulation.
!  The returned flux is for {\em all} blocks so this routine must be
!  called outside of a block loop.
!
! !REVISION HISTORY:
!  same as module

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), &
      intent(out) :: &
      ICE_FLUX

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  normalized total heat by time interval and convert to W/m2
!
!-----------------------------------------------------------------------

   if (tlast_ice /= c0) then
      ICE_FLUX = QICE/tlast_ice/hflux_factor
   else
      ICE_FLUX = c0    ! on simulation start tlast_ice=0
   endif

!-----------------------------------------------------------------------
!
!  reset accumulated ice heat flux and time since last ice
!  flux request
!
!-----------------------------------------------------------------------

   tlast_ice = c0
   AQICE = c0

!-----------------------------------------------------------------------
!EOC

 end subroutine get_ice_flux

!***********************************************************************

 end module ice

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
