!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module forcing

!BOP
! !MODULE: forcing
!
! !DESCRIPTION:
!  This is the main driver module for all surface and interior
!  forcing.  It contains necessary forcing fields as well as
!  necessary routines for call proper initialization and
!  update routines for those fields.
!
! !REVISION HISTORY:
!  CVS:$Id: forcing.F90,v 1.9 2003/12/12 16:53:04 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $
!
! !USES:

   use constants
   use blocks
   use distribution
   use domain
   use grid
   use forcing_ws
   use forcing_shf
   use forcing_sfwf
   use forcing_pt_interior
   use forcing_s_interior
   use forcing_ap
   use forcing_coupled
   use forcing_tools
   use prognostic
   use tavg
   use time_management
   use exit_mod

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_forcing,        &
             set_surface_forcing, &
             tavg_forcing

! !PUBLIC DATA MEMBERS:

   real (r8), dimension(nx_block,ny_block,2,max_blocks_clinic), &
      public, target :: &
      SMF,  &!  surface momentum fluxes (wind stress)
      SMFT   !  surface momentum fluxes on T points if avail

   real (r8), dimension(nx_block,ny_block,nt,max_blocks_clinic), &
      public, target :: &
      STF,  &!  surface tracer fluxes
      TFW    ! tracer content in freshwater flux

   logical (log_kind), public :: &
      lsmft_avail   ! true if SMFT is an available field

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), &
      public, target ::  &
      IFRAC,             &! ice fraction
      ATM_PRESS,         &!  atmospheric pressure forcing
      FW,FW_OLD           ! freshwater flux at T points (cm/s)
                          ! FW_OLD is at time n-1

!EOP
!BOC

   integer (int_kind) :: &
      tavg_SHF,          &! tavg_id for surface heat flux
      tavg_SFWF,         &! tavg_id for surface freshwater flux
      tavg_TAUX,         &! tavg_id for wind stress in X direction
      tavg_TAUY           ! tavg_id for wind stress in Y direction

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_forcing
! !INTERFACE:

   subroutine init_forcing

! !DESCRIPTION:
!  Initializes forcing by calling a separate routines for
!  wind stress, heat flux, fresh water flux, passive tracer flux,
!  interior restoring, and atmospheric pressure.
!
! !REVISION HISTORY:
!  same as module

!-----------------------------------------------------------------------
!
!  write out header for forcing options to stdout.
!
!-----------------------------------------------------------------------

   if (my_task == master_task) then
      write(stdout,delim_fmt)
      write(stdout,blank_fmt)
      write(stdout,'(a15)') 'Forcing options'
      write(stdout,blank_fmt)
      write(stdout,delim_fmt)
   endif

!-----------------------------------------------------------------------
!
!  allocate forcing arrays
!
!-----------------------------------------------------------------------

   ATM_PRESS = c0
   FW        = c0
   FW_OLD    = c0
   SMF       = c0
   SMFT      = c0
   STF       = c0
   TFW       = c0

!-----------------------------------------------------------------------
!
!  call individual initialization routines
!
!-----------------------------------------------------------------------

   call init_ws(SMF,SMFT,lsmft_avail)

   !*** NOTE: with bulk NCEP forcing init_shf must be called before
   !***       init_sfwf

   call init_shf (STF)
   call init_sfwf(STF)
   call init_pt_interior
   call init_s_interior
   call init_ap(ATM_PRESS)
   call init_coupled(SMF, SMFT, STF, SHF_QSW, lsmft_avail)

!-----------------------------------------------------------------------
!
!  error check for coupled option
!
!-----------------------------------------------------------------------

#ifndef coupled
   if (lcoupled) then
      call exit_POP(sigAbort, &
               'ERROR: code must be compiled with coupled ifdef option')
   endif
#endif

!-----------------------------------------------------------------------
!
!  define tavg diagnostic fields
!
!-----------------------------------------------------------------------

   call define_tavg_field(tavg_SHF, 'SHF', 2,            &
                          long_name='Surface Heat Flux', &
                          units='W/m^2', grid_loc='2110')

   call define_tavg_field(tavg_SFWF,'SFWF',2,                        &
                          long_name='Surface Freshwater Flux (p-e)', &
                          units='m/year', grid_loc='2110')


   call define_tavg_field(tavg_TAUX,'TAUX',2,           &
                          long_name='Zonal windstress', &
                          units='dyne/cm^2', grid_loc='2220')

   call define_tavg_field(tavg_TAUY,'TAUY',2,                &
                          long_name='Meridional windstress', &
                          units='dyne/cm^2', grid_loc='2220')

!-----------------------------------------------------------------------
!EOC

 end subroutine init_forcing

!***********************************************************************
!BOP
! !IROUTINE: set_surface_forcing
! !INTERFACE:

 subroutine set_surface_forcing

! !DESCRIPTION:
!  Calls surface forcing routines if necessary.
!  If forcing does not depend on the ocean state, then update
!     forcing if current time is greater than the appropriate
!     interpolation time or if it is the first step.
!  If forcing DOES depend on the ocean state, then call every
!     timestep.  interpolation check will be done within the set\_*
!     routine.
!  Interior restoring is assumed to take place every
!     timestep and is set in subroutine tracer\_update, but
!     updating the data fields must occur here outside
!     any block loops.
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
!  Get any interior restoring data and interpolate if necessary.
!
!-----------------------------------------------------------------------

   call get_pt_interior_data
   call get_s_interior_data

!-----------------------------------------------------------------------
!
!  Call individual forcing update routines.
!
!-----------------------------------------------------------------------

   if (lsmft_avail) then
      call set_ws(SMF,SMFT=SMFT)
   else
      call set_ws(SMF)
   endif

   !*** NOTE: with bulk NCEP forcing set_shf must be called before &
   !***       set_sfwf

   call set_shf(STF)
   call set_sfwf(STF,FW,TFW)

   call set_ap(ATM_PRESS)

   call set_coupled_forcing(SMF,SMFT,STF,SHF_QSW,FW,TFW,IFRAC)

!-----------------------------------------------------------------------
!EOC

 end subroutine set_surface_forcing

!***********************************************************************
!BOP
! !IROUTINE: tavg_forcing
! !INTERFACE:

 subroutine tavg_forcing

! !DESCRIPTION:
!  This routine accumulates tavg diagnostics related to surface
!  forcing.
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
      iblock              ! block loop index

   type (block) ::       &
      this_block          ! block information for current block

   real (r8), dimension(nx_block,ny_block) :: &
      WORK                ! local temp space for tavg diagnostics

!-----------------------------------------------------------------------
!
!  compute and accumulate tavg forcing diagnostics
!
!-----------------------------------------------------------------------

   !$OMP PARALLEL DO PRIVATE(iblock,this_block,WORK)

   do iblock = 1,nblocks_clinic

      this_block = get_block(blocks_clinic(iblock),iblock)

      if (tavg_requested(tavg_SHF)) then
         where (KMT(:,:,iblock) > 0)
            WORK = (STF(:,:,1,iblock)+SHF_QSW(:,:,iblock))/ &
                   hflux_factor ! W/m^2
         elsewhere
            WORK = c0
         end where

         call accumulate_tavg_field(WORK,tavg_SHF,iblock,1)
      endif

      if (tavg_requested(tavg_SFWF)) then
         if (sfc_layer_type == sfc_layer_varthick .and. &
             .not. lfw_as_salt_flx) then
            where (KMT(:,:,iblock) > 0)
               WORK = FW(:,:,iblock)*seconds_in_year*mpercm ! m/yr
            elsewhere
               WORK = c0
            end where
         else
            where (KMT(:,:,iblock) > 0) ! m/yr
               WORK = STF(:,:,2,iblock)* &
                      seconds_in_year/c1000/salinity_factor
            elsewhere
               WORK = c0
            end where
         endif

         call accumulate_tavg_field(WORK,tavg_SFWF,iblock,1)
      endif

      if (tavg_requested(tavg_TAUX)) then
         call accumulate_tavg_field(SMF(:,:,1,iblock), &
                                    tavg_TAUX,iblock,1)
      endif

      if (tavg_requested(tavg_TAUY)) then
         call accumulate_tavg_field(SMF(:,:,2,iblock), &
                                    tavg_TAUY,iblock,1)
      endif

   end do

   !$OMP END PARALLEL DO

!-----------------------------------------------------------------------
!EOC

 end subroutine tavg_forcing

!***********************************************************************

 end module forcing

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
