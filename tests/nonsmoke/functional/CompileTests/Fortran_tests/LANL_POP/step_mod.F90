!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module step_mod

!BOP
! !MODULE: step_mod

! !DESCRIPTION:
!  Contains the routine for stepping the model forward one timestep
!
! !REVISION HISTORY:
!  CVS:$Id: step_mod.F90,v 1.15 2003/12/12 16:53:04 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $
!
! !USES:

   use kinds_mod, only: int_kind, r8, log_kind
   use blocks, only: nx_block, ny_block, block, get_block
!   use distribution, only: 
   use domain, only: distrb_clinic, nblocks_clinic, bndy_clinic,            &
       blocks_clinic
   use constants, only: c2, field_loc_NEcorner, field_type_vector,          &
       field_type_scalar, c3, p5, grav
   use prognostic, only: max_blocks_clinic, mixtime, newtime,               &
       field_loc_center, km, curtime, UBTROP, VBTROP, UVEL, VVEL, RHO,      &
       TRACER, oldtime, PGUESS, GRADPX, GRADPY, PSURF, nt
!   use solvers, only: 
!   use broadcast, only: 
   use boundary, only: update_ghost_cells
   use timers, only: get_timer, timer_start, timer_stop
   use grid, only: KMU, sfc_layer_type, sfc_layer_varthick, dz, hu,         &
       ugrid_to_tgrid
!   use io, only: 
   use diagnostics, only: diag_global_preupdate, diag_global_afterupdate,   &
       diag_print, diag_transport, diag_init_sums
   use state_mod, only: state
   use time_management, only: mix_pass, matsuno_ts, leapfrogts, beta,       &
       alpha, c2dtt, c2dtu, c2dtp, dtp, c2dtq, theta, avg_ts, back_to_back, &
       time_to_do, freq_opt_nstep, dt, dtu, time_manager
   use xdisplay, only: lxdisplay, nstep_xdisplay, display
   use baroclinic, only: baroclinic_driver, baroclinic_correct_adjust
   use barotropic, only: barotropic_driver
   use surface_hgt, only: dhdt
   use tavg, only: tavg_set_flag
   use forcing, only: FW_OLD, FW, set_surface_forcing, tavg_forcing
   use forcing_coupled, only: lcoupled
   use ice, only: liceform

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: step

!EOP
!BOC
!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: step
! !INTERFACE:

 subroutine step

! !DESCRIPTION:
!  This routine advances the simulation on timestep.
!  It controls logic for leapfrog and/or Matsuno timesteps and performs
!  time-averaging if necessary.  Prognostic variables are updated for 
!  the next timestep near the end of the routine.
!  On Matsuno steps, the time (n) velocity and tracer arrays
!  UBTROP,VBTROP,UVEL,VVEL,TRACER contain the predicted new 
!  velocities from the 1st pass for use in the 2nd pass.
!
! !REVISION HISTORY:
!  same as module

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local or common variables:
!
!-----------------------------------------------------------------------
 
   integer (int_kind) :: &
      k,n,               &! loop indices
      tmptime,           &! temp space for time index swapping
      iblock,            &! block counter
      ipass,             &! pass counter
      num_passes          ! number of passes through time step
                          ! (Matsuno requires two)

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic) :: &
      ZX,ZY,             &! vertically integrated forcing terms
      DH,DHU              ! time change of surface height minus
                          ! freshwater flux at T, U points

   logical (log_kind), save :: &
      first_call = .true.      ! flag for initializing timers

   integer (int_kind), save :: &
      timer_baroclinic,        &! timer for baroclinic parts of step
      timer_barotropic          ! timer for barotropic part  of step

   type (block) ::        &
      this_block          ! block information for current block

!-----------------------------------------------------------------------
!
!  if this is the first call to step, start some timers
!
!-----------------------------------------------------------------------

   if (first_call) then
      call get_timer(timer_baroclinic,'BAROCLINIC',1, &
                                       distrb_clinic%nprocs)
      call get_timer(timer_barotropic,'BAROTROPIC',1, &
                                       distrb_clinic%nprocs)
      first_call = .false.
   endif

!-----------------------------------------------------------------------
!
!  Gather data for comparison with hydrographic data
!
!-----------------------------------------------------------------------
!
!  if(newday) call data_stations
!  if(newday .and. (mod(iday-1,3).eq.0) ) call data_slices
!
!-----------------------------------------------------------------------
!
!  Gather data for comparison with current meter data
!  THIS SECTION NOT FUNCTIONAL AT THIS TIME
!
!-----------------------------------------------------------------------
!
!  if(newday) call data_cmeters
!
!-----------------------------------------------------------------------
!
!  read fields for surface forcing
!
!-----------------------------------------------------------------------

   call set_surface_forcing

!-----------------------------------------------------------------------
!
!  update timestep counter, set corresponding model time, set
!  time-dependent logical switches to determine program flow.
!
!-----------------------------------------------------------------------

   call time_manager(lcoupled, liceform)

!-----------------------------------------------------------------------
!
!  compute and initialize some time-average diagnostics
!
!-----------------------------------------------------------------------

   call tavg_set_flag
   call tavg_forcing

!-----------------------------------------------------------------------
!
!  set timesteps and time-centering parameters for leapfrog or
!  matsuno steps.
!
!-----------------------------------------------------------------------

   mix_pass = 0
   if (matsuno_ts) then
      num_passes = 2
   else
      num_passes = 1
   endif

   do ipass = 1,num_passes

      if (matsuno_ts) mix_pass = mix_pass + 1

      if (leapfrogts) then  ! leapfrog (and averaging) timestep
         mixtime = oldtime
         beta  = alpha
         do k = 1,km
            c2dtt(k) = c2*dt(k)
         enddo
         c2dtu = c2*dtu
         c2dtp = c2*dtp    ! barotropic timestep = baroclinic timestep
         c2dtq = c2*dtu    ! turbulence timestep = mean flow timestep
      else
         mixtime = curtime
         beta  = theta
         do k = 1,km
            c2dtt(k) = dt(k)
         enddo
         c2dtu = dtu
         c2dtp = dtp       ! barotropic timestep = baroclinic timestep
         c2dtq = dtu       ! turbulence timestep = mean flow timestep
      endif

!-----------------------------------------------------------------------
!
!     on 1st pass of matsuno, set time (n-1) variables equal to
!     time (n) variables.
!
!-----------------------------------------------------------------------

      if (mix_pass == 1) then

         !$OMP PARALLEL DO
         do iblock = 1,nblocks_clinic
            UBTROP(:,:,oldtime,iblock) = UBTROP(:,:,curtime,iblock)
            VBTROP(:,:,oldtime,iblock) = VBTROP(:,:,curtime,iblock)
            UVEL(:,:,:,oldtime,iblock) = UVEL(:,:,:,curtime,iblock)
            VVEL(:,:,:,oldtime,iblock) = VVEL(:,:,:,curtime,iblock)
            RHO (:,:,:,oldtime,iblock) = RHO (:,:,:,curtime,iblock)
            TRACER(:,:,:,:,oldtime,iblock) = &
            TRACER(:,:,:,:,curtime,iblock)
         end do
         !$OMP END PARALLEL DO

      endif

!-----------------------------------------------------------------------
!
!     initialize diagnostic flags and sums
!
!-----------------------------------------------------------------------

      call diag_init_sums

!-----------------------------------------------------------------------
!
!     calculate change in surface height dh/dt from surface pressure
!
!-----------------------------------------------------------------------

      call dhdt(DH,DHU)

!-----------------------------------------------------------------------
!
!     Integrate baroclinic equations explicitly to find tracers and
!     baroclinic velocities at new time.  Update ghost cells for 
!     forcing terms leading into the barotropic solver.
!
!-----------------------------------------------------------------------

      call timer_start(timer_baroclinic)
      call baroclinic_driver(ZX,ZY,DH,DHU)
      call timer_stop(timer_baroclinic)

      call update_ghost_cells(ZX, bndy_clinic, field_loc_NEcorner, &
                                               field_type_vector)
      call update_ghost_cells(ZY, bndy_clinic, field_loc_NEcorner, &
                                               field_type_vector)

!-----------------------------------------------------------------------
!
!     Solve barotropic equations implicitly to find surface pressure
!     and barotropic velocities.
!
!-----------------------------------------------------------------------

      call timer_start(timer_barotropic)
      call barotropic_driver(ZX,ZY)
      call timer_stop(timer_barotropic)

!-----------------------------------------------------------------------
!
!     update tracers using surface height at new time
!     also peform adjustment-like physics (convection, ice formation)
!
!-----------------------------------------------------------------------

      call timer_start(timer_baroclinic)
      call baroclinic_correct_adjust
      call timer_stop(timer_baroclinic)

      call update_ghost_cells(UBTROP(:,:    ,newtime,:), bndy_clinic, &
                              field_loc_NEcorner, field_type_vector)
      call update_ghost_cells(VBTROP(:,:    ,newtime,:), bndy_clinic, &
                              field_loc_NEcorner, field_type_vector)
      call update_ghost_cells(UVEL  (:,:,:  ,newtime,:), bndy_clinic, &
                              field_loc_NEcorner, field_type_vector)
      call update_ghost_cells(VVEL  (:,:,:  ,newtime,:), bndy_clinic, &
                              field_loc_NEcorner, field_type_vector)
      call update_ghost_cells(RHO   (:,:,:  ,newtime,:), bndy_clinic, &
                              field_loc_center, field_type_scalar)
      call update_ghost_cells(TRACER(:,:,:,:,newtime,:), bndy_clinic, &
                              field_loc_center, field_type_scalar)

!-----------------------------------------------------------------------
!
!     add barotropic to baroclinic velocities at new time
!
!-----------------------------------------------------------------------

      !$OMP PARALLEL DO PRIVATE(iblock, k)
      do iblock = 1,nblocks_clinic

         do k=1,km
            where (k <= KMU(:,:,iblock))
               UVEL(:,:,k,newtime,iblock) = &
               UVEL(:,:,k,newtime,iblock) + UBTROP(:,:,newtime,iblock)
               VVEL(:,:,k,newtime,iblock) = &
               VVEL(:,:,k,newtime,iblock) + VBTROP(:,:,newtime,iblock)
            end where
         enddo

!-----------------------------------------------------------------------
!
!        on matsuno mixing steps update variables and cycle for 2nd pass
!        note: first step is forward only.
!
!-----------------------------------------------------------------------

         if (mix_pass == 1) then

            UBTROP(:,:,curtime,iblock) = UBTROP(:,:,newtime,iblock)
            VBTROP(:,:,curtime,iblock) = VBTROP(:,:,newtime,iblock)
            UVEL(:,:,:,curtime,iblock) = UVEL(:,:,:,newtime,iblock)
            VVEL(:,:,:,curtime,iblock) = VVEL(:,:,:,newtime,iblock)
            RHO (:,:,:,curtime,iblock) = RHO (:,:,:,newtime,iblock)
            TRACER(:,:,:,:,curtime,iblock) = &
            TRACER(:,:,:,:,newtime,iblock)

         endif
      enddo ! block loop
      !$OMP END PARALLEL DO

   end do ! ipass: cycle for 2nd pass in matsuno step

!-----------------------------------------------------------------------
!
!  extrapolate next guess for pressure from three known time levels
!
!-----------------------------------------------------------------------

   !$OMP PARALLEL DO
   do iblock = 1,nblocks_clinic
      PGUESS(:,:,iblock) = c3*(PSURF(:,:,newtime,iblock) -   &
                               PSURF(:,:,curtime,iblock)) +  &
                               PSURF(:,:,oldtime,iblock)
   end do
   !$OMP END PARALLEL DO

!-----------------------------------------------------------------------
!
!  compute some global diagnostics 
!  before updating prognostic variables
!
!-----------------------------------------------------------------------

   call diag_global_preupdate(DH,DHU)

!-----------------------------------------------------------------------
!
!  update prognostic variables for next timestep:
!     on normal timesteps
!        (n) -> (n-1)
!        (n+1) -> (n) 
!     on averaging timesteps
!        [(n) + (n-1)]/2 -> (n-1)
!        [(n+1) + (n)]/2 -> (n)
!
!-----------------------------------------------------------------------

   if (avg_ts .or. back_to_back) then     ! averaging step

      !$OMP PARALLEL DO PRIVATE(iblock, k, n, this_block)

      do iblock = 1,nblocks_clinic
         this_block = get_block(blocks_clinic(iblock),iblock)  

         !*** avg 2-d fields

         UBTROP(:,:,oldtime,iblock) = p5*(UBTROP(:,:,oldtime,iblock) + & 
                                          UBTROP(:,:,curtime,iblock))
         VBTROP(:,:,oldtime,iblock) = p5*(VBTROP(:,:,oldtime,iblock) + &
                                          VBTROP(:,:,curtime,iblock))
         UBTROP(:,:,curtime,iblock) = p5*(UBTROP(:,:,curtime,iblock) + &
                                          UBTROP(:,:,newtime,iblock))
         VBTROP(:,:,curtime,iblock) = p5*(VBTROP(:,:,curtime,iblock) + &
                                          VBTROP(:,:,newtime,iblock))
         GRADPX(:,:,oldtime,iblock) = p5*(GRADPX(:,:,oldtime,iblock) + &
                                          GRADPX(:,:,curtime,iblock))
         GRADPY(:,:,oldtime,iblock) = p5*(GRADPY(:,:,oldtime,iblock) + &
                                          GRADPY(:,:,curtime,iblock))
         GRADPX(:,:,curtime,iblock) = p5*(GRADPX(:,:,curtime,iblock) + &
                                          GRADPX(:,:,newtime,iblock))
         GRADPY(:,:,curtime,iblock) = p5*(GRADPY(:,:,curtime,iblock) + &
                                          GRADPY(:,:,newtime,iblock))
         FW_OLD(:,:,iblock) = p5*(FW(:,:,iblock) + FW_OLD(:,:,iblock))

         !*** avg 3-d fields

         UVEL(:,:,:,oldtime,iblock) = p5*(UVEL(:,:,:,oldtime,iblock) + &
                                          UVEL(:,:,:,curtime,iblock))
         VVEL(:,:,:,oldtime,iblock) = p5*(VVEL(:,:,:,oldtime,iblock) + &
                                          VVEL(:,:,:,curtime,iblock))
         UVEL(:,:,:,curtime,iblock) = p5*(UVEL(:,:,:,curtime,iblock) + &
                                          UVEL(:,:,:,newtime,iblock))
         VVEL(:,:,:,curtime,iblock) = p5*(VVEL(:,:,:,curtime,iblock) + &
                                          VVEL(:,:,:,newtime,iblock))

         do n=1,nt
            do k=2,km
               TRACER(:,:,k,n,oldtime,iblock) =                &
                          p5*(TRACER(:,:,k,n,oldtime,iblock) + &
                              TRACER(:,:,k,n,curtime,iblock))
               TRACER(:,:,k,n,curtime,iblock) =                &
                          p5*(TRACER(:,:,k,n,curtime,iblock) + &
                              TRACER(:,:,k,n,newtime,iblock))
            end do
         end do

         if (sfc_layer_type == sfc_layer_varthick) then

            do n = 1,nt
               TRACER(:,:,1,n,oldtime,iblock) =                   &
                   p5*((dz(1) + PSURF(:,:,oldtime,iblock)/grav)*  &
                       TRACER(:,:,1,n,oldtime,iblock) +           &
                       (dz(1) + PSURF(:,:,curtime,iblock)/grav)*  &
                       TRACER(:,:,1,n,curtime,iblock) ) 
               TRACER(:,:,1,n,curtime,iblock) =                   &
                   p5*((dz(1) + PSURF(:,:,curtime,iblock)/grav)*  &
                       TRACER(:,:,1,n,curtime,iblock) +           &
                       (dz(1) + PSURF(:,:,newtime,iblock)/grav)*  &
                       TRACER(:,:,1,n,newtime,iblock) ) 
            end do ! nt

            PSURF(:,:,oldtime,iblock) = p5*(PSURF(:,:,oldtime,iblock) + &
                                            PSURF(:,:,curtime,iblock))
            PSURF(:,:,curtime,iblock) = p5*(PSURF(:,:,curtime,iblock) + &
                                            PSURF(:,:,newtime,iblock))
            do n = 1,nt
               TRACER(:,:,1,n,oldtime,iblock) =                   &
               TRACER(:,:,1,n,oldtime,iblock)/(dz(1) +            &
                                      PSURF(:,:,oldtime,iblock)/grav)
               TRACER(:,:,1,n,curtime,iblock) =                   &
               TRACER(:,:,1,n,curtime,iblock)/(dz(1) +            &
                                      PSURF(:,:,curtime,iblock)/grav)
            enddo

         else

            do n=1,nt
               TRACER(:,:,1,n,oldtime,iblock) =                &
                          p5*(TRACER(:,:,1,n,oldtime,iblock) + &
                              TRACER(:,:,1,n,curtime,iblock))
               TRACER(:,:,1,n,curtime,iblock) =                &
                          p5*(TRACER(:,:,1,n,curtime,iblock) + &
                              TRACER(:,:,1,n,newtime,iblock))
            end do

            PSURF (:,:,oldtime,iblock) =                           &
                                  p5*(PSURF (:,:,oldtime,iblock) + &
                                      PSURF (:,:,curtime,iblock))
            PSURF (:,:,curtime,iblock) =                           &
                                  p5*(PSURF (:,:,curtime,iblock) + &
                                      PSURF (:,:,newtime,iblock))

         endif

         do k = 1,km  ! recalculate densities from averaged tracers
            call state(k,k,TRACER(:,:,k,1,oldtime,iblock), &
                           TRACER(:,:,k,2,oldtime,iblock), &
                           this_block,                     &
                         RHOOUT=RHO(:,:,k,oldtime,iblock))
            call state(k,k,TRACER(:,:,k,1,curtime,iblock), &
                           TRACER(:,:,k,2,curtime,iblock), &
                           this_block,                     &
                         RHOOUT=RHO(:,:,k,curtime,iblock))
         enddo 

         !*** correct after avg
         PGUESS(:,:,iblock) = p5*(PGUESS(:,:,iblock) + & 
                                   PSURF(:,:,newtime,iblock)) 
      end do ! block loop
      !$OMP END PARALLEL DO


   else  ! non-averaging step
  
      !$OMP PARALLEL DO
      do iblock = 1,nblocks_clinic

         if (mix_pass == 2) then ! reset time n variables on 2nd pass matsuno

            UBTROP(:,:,curtime,iblock) = UBTROP(:,:,oldtime,iblock)
            VBTROP(:,:,curtime,iblock) = VBTROP(:,:,oldtime,iblock)
            UVEL(:,:,:,curtime,iblock) = UVEL(:,:,:,oldtime,iblock)
            VVEL(:,:,:,curtime,iblock) = VVEL(:,:,:,oldtime,iblock)
            TRACER(:,:,:,:,curtime,iblock) = &
                                     TRACER(:,:,:,:,oldtime,iblock)
            RHO(:,:,:,curtime,iblock) = RHO(:,:,:,oldtime,iblock)

         endif

         FW_OLD(:,:,iblock) = FW(:,:,iblock)

      end do ! block loop
      !$OMP END PARALLEL DO


      tmptime = oldtime
      oldtime = curtime
      curtime = newtime
      newtime = tmptime

   endif

!-----------------------------------------------------------------------
!
!  end of timestep, all variables updated
!  compute and print some more diagnostics
!
!-----------------------------------------------------------------------

   call diag_global_afterupdate
   call diag_print
   call diag_transport

!-----------------------------------------------------------------------
!
!  display vertically integrated velocity and surface height in 
!  X window if requested (use DHU, DH as temp arrays to store these).
!
!-----------------------------------------------------------------------

   if (lxdisplay .and. & 
       time_to_do(freq_opt_nstep,nstep_xdisplay)) then

       do iblock = 1,nblocks_clinic

          !*** store integrated velocity in DHU

          DH(:,:,iblock) = HU(:,:,iblock)*                      &
                           sqrt(UBTROP(:,:,curtime,iblock)**2 + &
                                VBTROP(:,:,curtime,iblock)**2)

          !*** integrated velocity at t-pts
          call ugrid_to_tgrid(DHU(:,:,iblock),DH(:,:,iblock),iblock)

          DH(:,:,iblock) = PSURF(:,:,curtime,iblock)/grav ! surface height
       end do ! block loop

       call display(DHU, field_loc_NEcorner, &
                    DH , field_loc_center)

   endif ! xdisplay

!-----------------------------------------------------------------------
!EOC

   end subroutine step

!***********************************************************************

 end module step_mod

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
