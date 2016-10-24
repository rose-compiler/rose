!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module baroclinic

!BOP
! !MODULE: baroclinic
!
! !DESCRIPTION:
!  Contains main driver routines and variables for computing the 
!  baroclinic velocities and tracer fields.
!
! !REVISION HISTORY:
!  CVS:$Id: baroclinic.F90,v 1.24 2003/12/12 17:09:58 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod, only: int_kind, r8, log_kind
   use blocks, only: nx_block, ny_block, block, get_block
!   use distribution, only: 
   use domain, only: nblocks_clinic, blocks_clinic, bndy_clinic
   use constants, only: delim_fmt, blank_fmt, p5, field_loc_center,          &
       field_type_scalar, c0, c1, c2, grav
   use prognostic, only: TRACER, UVEL, VVEL, max_blocks_clinic, km, mixtime, &
       RHO, newtime, oldtime, curtime, PSURF, nt
   use broadcast, only: broadcast_scalar
   use boundary, only: update_ghost_cells
   use communicate, only: my_task, master_task
   use grid, only: FCOR, DZU, HUR, KMU, KMT, sfc_layer_type,                 &
       sfc_layer_varthick, partial_bottom_cells, dz, DZT, CALCT, dzw, dzr
   use advection, only: advu, advt
   use pressure_grad, only: lpressure_avg, gradp
   use horizontal_mix, only: hdiffu, hdifft
   use vertical_mix, only: vmix_coeffs, implicit_vertical_mix, vdiffu,       &
       vdifft, impvmixt, impvmixu, impvmixt_correct, convad
   use vmix_kpp, only: add_kpp_sources
   use diagnostics, only: ldiag_cfl, cfl_check, ldiag_global,                &
       DIAG_KE_ADV_2D, DIAG_KE_PRESS_2D, DIAG_KE_HMIX_2D, DIAG_KE_VMIX_2D,   &
       DIAG_TRACER_HDIFF_2D, DIAG_PE_2D, DIAG_TRACER_ADV_2D,                 &
       DIAG_TRACER_SFC_FLX, DIAG_TRACER_VDIFF_2D, DIAG_TRACER_SOURCE_2D
   use state_mod, only: state
   use ice, only: liceform, ice_formation
   use time_management, only: mix_pass, leapfrogts, impcor, c2dtu, beta,     &
       gamma, c2dtt
   use io_types, only: nml_in, nml_filename, stdout
   use tavg, only: define_tavg_field, tavg_requested, accumulate_tavg_field
   use forcing, only: STF, SMF, lsmft_avail, SMFT, TFW
   use forcing_shf, only: SHF_QSW, add_sw_absorb
!   use forcing_coupled, only: 
   use forcing_pt_interior, only: set_pt_interior
   use forcing_s_interior, only: set_s_interior
   use exit_mod, only: sigAbort, exit_pop

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_baroclinic,          &
             baroclinic_driver,        &
             baroclinic_correct_adjust

! !PUBLIC DATA MEMBERS:

   logical (log_kind) :: &
      reset_to_freezing   ! flag to prevent very cold water

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  ids for tavg diagnostics computed from baroclinic
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      tavg_UDP,          &! tavg id for pressure grad work
      tavg_TEMP,         &! tavg id for temperature
      tavg_SALT,         &! tavg id for salinity
      tavg_TEMP2,        &! tavg id for temperature squared
      tavg_SALT2,        &! tavg id for salinity    squared
      tavg_UVEL,         &! tavg id for U velocity
      tavg_VVEL,         &! tavg id for V velocity
      tavg_KE,           &! tavg id for kinetic energy
      tavg_ST,           &! tavg id for salt*temperature
      tavg_RHO,          &! tavg id for in-situ density
      tavg_UV,           &! tavg id for u times v
      tavg_T1_8,         &! tavg id for temperature in top 8 lvls
      tavg_S1_8,         &! tavg id for salinity    in top 8 lvls
      tavg_U1_8,         &! tavg id for U           in top 8 lvls
      tavg_V1_8           ! tavg id for V           in top 8 lvls

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_baroclinic
! !INTERFACE:

 subroutine init_baroclinic

! !DESCRIPTION:
!  Initializes some baroclinic options.
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
      nml_error          ! namelist i/o error flag

   namelist /baroclinic_nml/ reset_to_freezing

!-----------------------------------------------------------------------
!
!  read options from namelist and broadcast
!
!-----------------------------------------------------------------------

   reset_to_freezing = .true.

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=baroclinic_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading baroclinic_nml')
   endif

   if (my_task == master_task) then
      write(stdout,delim_fmt)
      write(stdout,blank_fmt)
      write(stdout,'(a18)') 'Baroclinic options'
      write(stdout,blank_fmt)
      write(stdout,delim_fmt)
      if (reset_to_freezing) then
         write(stdout,'(a40)') &
                          'Surface temperature reset to freezing on'
      else
         write(stdout,'(a41)') &
                          'Surface temperature reset to freezing off'
      endif
   endif

   call broadcast_scalar(reset_to_freezing, master_task)

!-----------------------------------------------------------------------
!
!  define tavg fields computed from baroclinic driver routines
!
!-----------------------------------------------------------------------

   call define_tavg_field(tavg_UDP,'UDP',3,                            &
                          long_name='Pressure work',                   &
                          units='erg', grid_loc='3221')

   call define_tavg_field(tavg_U1_8,'U1_8',2,                          &
                          long_name='Zonal Velocity lvls 1-8',         &
                          units='cm/s', grid_loc='2221')

   call define_tavg_field(tavg_V1_8,'V1_8',2,                          &
                          long_name='Meridional Velocity lvls 1-8',    &
                          units='cm/s', grid_loc='2221')

   call define_tavg_field(tavg_T1_8,'T1_8',2,                          &
                          long_name='Potential Temperature lvls 1-8',  &
                          units='degC', grid_loc='2111')

   call define_tavg_field(tavg_S1_8,'S1_8',2,                          &
                          long_name='Salinity lvls 1-8',               &
                          units='g/g', grid_loc='2111')

   call define_tavg_field(tavg_UVEL,'UVEL',3,                          &
                          long_name='Zonal Velocity',                  &
                          units='cm/s', grid_loc='3221')

   call define_tavg_field(tavg_VVEL,'VVEL',3,                          &
                          long_name='Meridional Velocity',             &
                          units='cm/s', grid_loc='3221')

   call define_tavg_field(tavg_KE,'KE',3,                              &
                          long_name='Horizontal Kinetic Energy',       &
                          units='cm^2/s^2', grid_loc='3221')

   call define_tavg_field(tavg_TEMP,'TEMP',3,                          &
                          long_name='Potential Temperature',           &
                          units='degC', grid_loc='3111')

   call define_tavg_field(tavg_SALT,'SALT',3,                          &
                          long_name='Salinity',                        &
                          units='g/g', grid_loc='3111')

   call define_tavg_field(tavg_TEMP2,'TEMP2',3,                        &
                          long_name='Temperature**2',                  &
                          units='degC^2', grid_loc='3111')

   call define_tavg_field(tavg_SALT2,'SALT2',3,                        &
                          long_name='Salinity**2 ',                    &
                          units='(g/g)^2', grid_loc='3111')

   call define_tavg_field(tavg_ST,'ST',3,                              &
                          long_name='Temperature*Salinity',            &
                          units='degC*g/g', grid_loc='3111')

   call define_tavg_field(tavg_RHO,'RHO',3,                            &
                          long_name='In-situ density',                 &
                          units='g/cm^3', grid_loc='3111')

   call define_tavg_field(tavg_UV,'UV',3,                              &
                          long_name='UV velocity product',             &
                          units='cm^2/s^2', grid_loc='3221')

!-----------------------------------------------------------------------
!EOC

 end subroutine init_baroclinic

!***********************************************************************
!BOP
! !IROUTINE: baroclinic_driver
! !INTERFACE:

 subroutine baroclinic_driver(ZX,ZY,DH,DHU)

! !DESCRIPTION:
!  This routine is the main driver for the explicit time integration of
!  baroclinic velocities $(u',v')$ and tracer fields $T$. 
!
!  Tracer equations:
!  \begin{equation}
!     (T^{n+1}-T^{n-1})/(2 \Delta t) = -L(T^n) + D_H(T^{n-1}) + 
!                                      D_V(T^{n-1}) + S
!  \end{equation}
!  where $S$ are source terms, $L$ is the advection operator and
!  $D$ is the diffusion operator in the horizontal and vertical
!  directions.
!
!  Momentum equations:
!  \begin{eqnarray}
!     (u'-u^{n-1})/(2 \Delta t) - f*\alpha*(v'-v^{n-1}) &=& F_x \\
!     (v'-v^{n-1})/(2 \Delta t) + f*\alpha*(u'-u^{n-1}) &=& F_y
!  \end{eqnarray}
!  \begin{eqnarray}
!     \tilde{u}' &=& u' - {1\over{H_U}}\sum_{k=1}^{km}dz_k u_k' \\
!     \tilde{v}' &=& v' - {1\over{H_U}}\sum_{k=1}^{km}dz_k v_k'
!  \end{eqnarray}
!
!  This routine calculates baroclinic velocities and tracers at
!  the new time level and stores them in arrays UVEL,VVEL,and TRACER
!  with time index newtime.  The arrays UVEL,VVEL, and TRACER
!  with time indices curtime and oldtime are not updated for the next
!  timestep until near the end of routine step.
!
!  The above equations are written for the case of (leapfrog)
!  implicit treatment of the coriolis terms.  if these terms are
!  treated explicitly then the coriolis terms appear only in the 
!  forcing terms $(F_x,F_y)$, which are calculated in clinic. 
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), &
      intent(in) :: &
      DH, DHU              ! change in surface height at T,U points

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), &
      intent(out) :: &
      ZX, ZY               ! vertical integrals of forcing

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) ::  &
      n,k,                &! dummy indices for vertical level, tracer
      iblock,             &! counter for block loops
      kp1,km1              ! level index for k+1, k-1 levels

   real (r8), dimension(nx_block,ny_block) :: & 
      FX,FY,              &! sum of r.h.s. forcing terms
      WORK1,WORK2,        &! local work space
      WUK,                &! vertical velocity at top of U box
      WTK                  ! vertical velocity at top of T box

   type (block) ::        &
      this_block           ! block information for current block

!-----------------------------------------------------------------------
!
!  first block loop to update tracers
!
!-----------------------------------------------------------------------

   !$OMP PARALLEL DO PRIVATE(iblock,k,kp1,km1,this_block,WTK)

   do iblock = 1,nblocks_clinic
      this_block = get_block(blocks_clinic(iblock),iblock)  

      do k = 1,km 

         kp1 = k+1
         km1 = k-1
         if (k == 1) km1 = 1
         if (k == km) kp1 = km

!-----------------------------------------------------------------------
!
!        compute vertical viscosity and diffusion coeffs
!
!-----------------------------------------------------------------------

         if (lsmft_avail) then
            call vmix_coeffs(k,TRACER (:,:,:,:,mixtime,iblock), &
                               UVEL   (:,:,:  ,mixtime,iblock), &
                               VVEL   (:,:,:  ,mixtime,iblock), &
                               RHO    (:,:,:  ,mixtime,iblock), &
                               TRACER (:,:,:,:,curtime,iblock), &
                               UVEL   (:,:,:  ,curtime,iblock), &
                               VVEL   (:,:,:  ,curtime,iblock), &
                               STF    (:,:,:          ,iblock), &
                               SHF_QSW(:,:            ,iblock), &
                               this_block, SMFT=SMFT(:,:,:,iblock))
         else
            call vmix_coeffs(k,TRACER (:,:,:,:,mixtime,iblock), &
                               UVEL   (:,:,:  ,mixtime,iblock), &
                               VVEL   (:,:,:  ,mixtime,iblock), &
                               RHO    (:,:,:  ,mixtime,iblock), &
                               TRACER (:,:,:,:,curtime,iblock), &
                               UVEL   (:,:,:  ,curtime,iblock), &
                               VVEL   (:,:,:  ,curtime,iblock), &
                               STF    (:,:,:          ,iblock), &
                               SHF_QSW(:,:            ,iblock), &
                               this_block, SMF=SMF(:,:,:,iblock))
         endif

!-----------------------------------------------------------------------
!
!        calculate level k tracers at new time
!
!-----------------------------------------------------------------------

         call tracer_update(k, WTK,                             &
                               TRACER (:,:,:,:,newtime,iblock), &
                               TRACER (:,:,:,:,oldtime,iblock), &
                               TRACER (:,:,:,:,mixtime,iblock), &
                               TRACER (:,:,:,:,curtime,iblock), &
                               UVEL   (:,:,:  ,curtime,iblock), &
                               VVEL   (:,:,:  ,curtime,iblock), &
                               UVEL   (:,:,:  ,mixtime,iblock), &
                               VVEL   (:,:,:  ,mixtime,iblock), &
                               RHO    (:,:,:  ,curtime,iblock), &
                               STF    (:,:,:          ,iblock), &
                               TFW    (:,:,:          ,iblock), &
                               SHF_QSW(:,:            ,iblock), &
                               DH     (:,:            ,iblock), &
                               PSURF  (:,:    ,oldtime,iblock), &
                               PSURF  (:,:    ,curtime,iblock), &
                               this_block)
         
!-----------------------------------------------------------------------
!
!        accumulate some tavg diagnostics if requested
!
!-----------------------------------------------------------------------

         if (mix_pass /= 1) then

         if (tavg_requested(tavg_UVEL)) then
            call accumulate_tavg_field(UVEL(:,:,k,curtime,iblock), &
                                       tavg_UVEL,iblock,k)
         endif

         if (tavg_requested(tavg_U1_8) .and. k <= 8) then
            call accumulate_tavg_field(UVEL(:,:,k,curtime,iblock), &
                                       tavg_U1_8,iblock,k)
         endif

         if (tavg_requested(tavg_VVEL)) then
            call accumulate_tavg_field(VVEL(:,:,k,curtime,iblock), &
                                       tavg_VVEL,iblock,k)
         endif

         if (tavg_requested(tavg_V1_8) .and. k <= 8) then
            call accumulate_tavg_field(VVEL(:,:,k,curtime,iblock), &
                                       tavg_V1_8,iblock,k)
         endif

         if (tavg_requested(tavg_KE)) then
            call accumulate_tavg_field(p5*(UVEL(:,:,k,curtime,iblock)**2 + &
                                           VVEL(:,:,k,curtime,iblock)**2), &
                                       tavg_KE,iblock,k)
         endif

         if (tavg_requested(tavg_UV)) then
            call accumulate_tavg_field(UVEL(:,:,k,curtime,iblock)* &
                                       VVEL(:,:,k,curtime,iblock), &
                                       tavg_UV,iblock,k)
         endif

         if (tavg_requested(tavg_TEMP)) then
            call accumulate_tavg_field(TRACER(:,:,k,1,curtime,iblock), &
                                       tavg_TEMP,iblock,k)
         endif

         if (tavg_requested(tavg_T1_8) .and. k <= 8) then
            call accumulate_tavg_field(TRACER(:,:,k,1,curtime,iblock), &
                                       tavg_T1_8,iblock,k)
         endif

         if (tavg_requested(tavg_TEMP2)) then
            call accumulate_tavg_field(TRACER(:,:,k,1,curtime,iblock)**2, &
                                       tavg_TEMP2,iblock,k)
         endif

         if (tavg_requested(tavg_SALT)) then
            call accumulate_tavg_field(TRACER(:,:,k,2,curtime,iblock), &
                                       tavg_SALT,iblock,k)
         endif

         if (tavg_requested(tavg_S1_8) .and. k <= 8) then
            call accumulate_tavg_field(TRACER(:,:,k,2,curtime,iblock), &
                                       tavg_S1_8,iblock,k)
         endif

         if (tavg_requested(tavg_SALT2)) then
            call accumulate_tavg_field(TRACER(:,:,k,2,curtime,iblock)**2, &
                                       tavg_SALT2,iblock,k)
         endif

         if (tavg_requested(tavg_ST)) then
            call accumulate_tavg_field(TRACER(:,:,k,1,curtime,iblock)* &
                                       TRACER(:,:,k,2,curtime,iblock), &
                                       tavg_ST,iblock,k)
         endif

         if (tavg_requested(tavg_RHO)) then
            call accumulate_tavg_field(RHO(:,:,k,curtime,iblock), &
                                       tavg_RHO,iblock,k)
         endif

         endif ! mix_pass

!-----------------------------------------------------------------------

      enddo  ! k loop

!-----------------------------------------------------------------------
!
!     if no implicit vertical mixing, we now have updated tracers
!     if using implicit vertical mixing and rigid lid or old free
!        surface form, update all the tracers now using impvmix
!     if using implicit vertical mixing and a variable thickness
!        surface layer, only update T,S as predicted values to
!        use for pressure averaging - the full update will
!        occur after the barotropic solver
!
!-----------------------------------------------------------------------

      if (implicit_vertical_mix) then
         if (sfc_layer_type /= sfc_layer_varthick) then
            call impvmixt(TRACER(:,:,:,:,newtime,iblock), &
                          TRACER(:,:,:,:,oldtime,iblock), &
                          PSURF (:,:,    curtime,iblock), &
                          1, nt, this_block)

         else if (lpressure_avg .and. leapfrogts) then

            !*** predictor update of T,S
            !*** with PSURF(curtime) on the LHS at k=1
 
            call impvmixt(TRACER(:,:,:,:,newtime,iblock), &
                          TRACER(:,:,:,:,oldtime,iblock), &
                          PSURF (:,:,curtime,iblock),     &
                          1, 2, this_block)

         endif
      endif

!-----------------------------------------------------------------------
!
!     end of first block loop
!
!-----------------------------------------------------------------------

   enddo ! first block loop

   !$OMP END PARALLEL DO

!-----------------------------------------------------------------------
!
!  update tracer ghost cells here outside the block loops (it
!  requires synchronization) - only update T,S ghost cells
!  for use in pressure averaging.
!
!-----------------------------------------------------------------------

   if (lpressure_avg .and. leapfrogts) then
      call update_ghost_cells(TRACER(:,:,:,1,newtime,:), bndy_clinic, &
                              field_loc_center, field_type_scalar)
      call update_ghost_cells(TRACER(:,:,:,2,newtime,:), bndy_clinic, &
                              field_loc_center, field_type_scalar)
   endif

!-----------------------------------------------------------------------
!
!  now loop over blocks to do momentum equations
!
!-----------------------------------------------------------------------

   !$OMP PARALLEL DO PRIVATE(iblock,k,km1,kp1,n,this_block, &
   !$OMP                     WUK, FX, FY, WORK1, WORK2)

   do iblock = 1,nblocks_clinic

      this_block = get_block(blocks_clinic(iblock),iblock)  

!-----------------------------------------------------------------------
!
!     initialize arrays for vertical sums.
!
!-----------------------------------------------------------------------

      ZX(:,:,iblock) = c0
      ZY(:,:,iblock) = c0

      do k = 1,km

         kp1 = k+1
         km1 = k-1
         if (k == 1) km1 = 1
         if (k == km) kp1 = km

!-----------------------------------------------------------------------
!
!        if pressure averaging is on and it is a leapfrog time step
!        we need the updated density for the pressure averaging
!
!-----------------------------------------------------------------------

         if (lpressure_avg .and. leapfrogts) then
            call state(k,k,TRACER(:,:,k,1,newtime,iblock), &
                           TRACER(:,:,k,2,newtime,iblock), &
                           this_block, RHOOUT=RHO(:,:,k,newtime,iblock))
         endif

!-----------------------------------------------------------------------
!
!        calculate forcing terms (Fx,Fy) at level k.
!
!-----------------------------------------------------------------------

         call clinic(k, FX, FY, WUK,                &
                        UVEL(:,:,:,curtime,iblock), &
                        VVEL(:,:,:,curtime,iblock), &
                        UVEL(:,:,:,oldtime,iblock), &
                        VVEL(:,:,:,oldtime,iblock), &
                        UVEL(:,:,k,mixtime,iblock), &
                        VVEL(:,:,k,mixtime,iblock), &
                        RHO (:,:,k,oldtime,iblock), &
                        RHO (:,:,k,curtime,iblock), &
                        RHO (:,:,k,newtime,iblock), &
                        SMF (:,:,:,iblock),         &
                        DHU (:,:,iblock),           &
                        this_block)

!-----------------------------------------------------------------------
!
!        store forces temporarily in UVEL(newtime),VVEL(newtime).
!
!-----------------------------------------------------------------------

         if (impcor) then   ! implicit treatment

            WORK1 = c2dtu*beta*FCOR(:,:,iblock)
            WORK2 = c2dtu/(c1 + WORK1**2)
            UVEL(:,:,k,newtime,iblock) = (FX + WORK1*FY)*WORK2 
            VVEL(:,:,k,newtime,iblock) = (FY - WORK1*FX)*WORK2 

         else               ! explicit treatment

            UVEL(:,:,k,newtime,iblock) = c2dtu*FX
            VVEL(:,:,k,newtime,iblock) = c2dtu*FY

         endif

!-----------------------------------------------------------------------
!
!        increment sum for vertically-averaged forcing ([Fx],[Fy]).
!
!-----------------------------------------------------------------------

         if (partial_bottom_cells) then
            ZX(:,:,iblock) = ZX(:,:,iblock) + FX*DZU(:,:,k,iblock)
            ZY(:,:,iblock) = ZY(:,:,iblock) + FY*DZU(:,:,k,iblock)
         else
            ZX(:,:,iblock) = ZX(:,:,iblock) + FX*dz(k)
            ZY(:,:,iblock) = ZY(:,:,iblock) + FY*dz(k)
         endif

      enddo ! vertical (k) loop

!-----------------------------------------------------------------------
!
!     normalize sums for vertical averages ([Fx],[Fy]) by dividing
!     by depth at U points.
!
!-----------------------------------------------------------------------

      ZX(:,:,iblock) = ZX(:,:,iblock)*HUR(:,:,iblock)
      ZY(:,:,iblock) = ZY(:,:,iblock)*HUR(:,:,iblock)

!-----------------------------------------------------------------------
!
!     solve tridiagonal system with implicit treatment of vertical 
!     diffusion of velocity.
!
!-----------------------------------------------------------------------

      if (implicit_vertical_mix)                   &
         call impvmixu(UVEL(:,:,:,newtime,iblock), &
                       VVEL(:,:,:,newtime,iblock), & 
                       this_block)

!-----------------------------------------------------------------------
!
!     calculate unnormalized baroclinic velocities (Upp,Vpp)
!
!-----------------------------------------------------------------------

      UVEL(:,:,:,newtime,iblock) = UVEL(:,:,:,oldtime,iblock) + &
                                   UVEL(:,:,:,newtime,iblock)  ! holds c2dtu*Fx
      VVEL(:,:,:,newtime,iblock) = VVEL(:,:,:,oldtime,iblock) + &
                                   VVEL(:,:,:,newtime,iblock)  ! holds c2dtu*Fy

!-----------------------------------------------------------------------
!
!     find vertical averages ([Upp],[Vpp]).
!
!-----------------------------------------------------------------------

      WORK1 = c0  ! initialize sums
      WORK2 = c0

      if (partial_bottom_cells) then
         do k = 1,km
            WORK1 = WORK1 + UVEL(:,:,k,newtime,iblock)*DZU(:,:,k,iblock)
            WORK2 = WORK2 + VVEL(:,:,k,newtime,iblock)*DZU(:,:,k,iblock)
         enddo
      else
         do k = 1,km
            WORK1 = WORK1 + UVEL(:,:,k,newtime,iblock)*dz(k)
            WORK2 = WORK2 + VVEL(:,:,k,newtime,iblock)*dz(k)
         enddo
      endif

      WORK1 = WORK1*HUR(:,:,iblock)  ! normalize by dividing by depth
      WORK2 = WORK2*HUR(:,:,iblock)

!-----------------------------------------------------------------------
!
!     normalize baroclinic velocites by subtracting vertical mean:
!     (Up,Vp) = (Upp,Vpp) - ([Upp],[Vpp]), zero velocities at land pts.
!
!-----------------------------------------------------------------------

      do k = 1,km
         where (k <= KMU(:,:,iblock))
            UVEL(:,:,k,newtime,iblock) = &
            UVEL(:,:,k,newtime,iblock) - WORK1
            VVEL(:,:,k,newtime,iblock) = &
            VVEL(:,:,k,newtime,iblock) - WORK2
         elsewhere 
            UVEL(:,:,k,newtime,iblock) = c0
            VVEL(:,:,k,newtime,iblock) = c0
         endwhere
      enddo

!-----------------------------------------------------------------------
!
!     note:  at this point UVEL(newtime) and VVEL(newtime) contain only 
!     the baroclinic velocities (Up,Vp) at the new time.  they are later
!     updated to the full velocities in step after barotropic is
!     is called, which calculates the barotropic velocites ([U],[V])
!     at the new time.  UVEL and VVEL at time levels oldtime ond curtime 
!     always contain the full velocites.
!
!-----------------------------------------------------------------------
!-----------------------------------------------------------------------
!
!  end of second block loop over vertical levels.
!
!-----------------------------------------------------------------------

   enddo ! second block loop

   !$OMP END PARALLEL DO

#if drifter_particles
!-----------------------------------------------------------------------
!
!  advect drifters with old time level velocities.  then if its
!  time to sample ocean variables at drifter locations, do it.
!
!-----------------------------------------------------------------------

   if (mod(iday,10).eq.0 .and. newday) then
      if((ndrifters + array_size) .gt. ndrifters_total) then
         write(stdout,*)' '
         write(stdout,*) &
                 ' No more drifter arrays remaining for deployment'
         write(stdout,*)' '
      else
         arrays_deployed = arrays_deployed + 1
         ndrifters = ndrifters + array_size
         write(stdout,*)' '
         write(stdout,*)' Deploying drifter array # ',arrays_deployed
         write(stdout,*)' '
      endif
   endif

   call drifter_move
   if(mod(ihour,n_write_drifters).eq.0 .and. newhour) then
      call drifter_prop
   endif
#endif

!-----------------------------------------------------------------------
!
!  check cfl limits
!
!-----------------------------------------------------------------------

   if (ldiag_cfl) then
      call cfl_check
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine baroclinic_driver

!***********************************************************************
!BOP
! !IROUTINE: baroclinic_correct_adjust
! !INTERFACE:

 subroutine baroclinic_correct_adjust

! !DESCRIPTION:
!  This subroutine finishes updating tracers by performing
!  adjustment-like physics (convection and ice) and completes
!  the corrector step for tracers using the new surface pressure
!  to update tracers.
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

   integer (int_kind) ::  & 
      k,                  &! vertical level index
      n,                  &! tracer index
      iblock               ! block index

   real (r8), dimension(nx_block,ny_block,nt) :: &
      RHS1                 ! r.h.s. for impvmix on corrector step

   type (block) ::       &
      this_block           ! block information for current block

!-----------------------------------------------------------------------
!
!  do everything for each sub block
!
!-----------------------------------------------------------------------

   do iblock = 1,nblocks_clinic

      this_block = get_block(blocks_clinic(iblock),iblock)  

!-----------------------------------------------------------------------
!
!     do the corrector step for variable thickness surface layer
!
!-----------------------------------------------------------------------

      if (sfc_layer_type == sfc_layer_varthick) then

         if (implicit_vertical_mix) then

            !*** if implicit vertical mixing and pressure averaging:
            !*** correct new T and S and update remaining passive
            !*** tracers with tridiagonal solves

            if (lpressure_avg .and. leapfrogts) then
        
               do n = 1,2   ! corrector for T,S only
                  where (KMT(:,:,iblock) > 0)  ! corrector RHS at k=1
                     RHS1(:,:,n)=((c2*TRACER(:,:,1,n,curtime,iblock) - &
                                      TRACER(:,:,1,n,oldtime,iblock))  &
                                 *(PSURF(:,:,curtime,iblock) -         &
                                   PSURF(:,:,oldtime,iblock))          &
                                - TRACER(:,:,1,n,newtime,iblock)*      &
                                  (PSURF(:,:,newtime,iblock) -         &
                                   PSURF(:,:,curtime,iblock)))/        &
                                   (grav*dz(1)) 
                  elsewhere
                     RHS1(:,:,n) = c0
                  endwhere
               enddo

               !*** T,S update on corrector step
               call impvmixt_correct(TRACER(:,:,:,:,newtime,iblock), &
                                     PSURF (:,:,    newtime,iblock), &
                                     RHS1, 1, 2, this_block)

               do n = 3,nt
                  !*** surface RHS for passive tracers with press avg
                  where (KMT(:,:,iblock) > 0)
                     TRACER(:,:,1,n,newtime,iblock) =               &
                                   TRACER(:,:,1,n,newtime,iblock) - &
                                   TRACER(:,:,1,n,oldtime,iblock)   & 
                                   *(PSURF(:,:,newtime,iblock) -    &
                                     PSURF(:,:,oldtime,iblock))/    &
                                     (grav*dz(1))
                  endwhere
               enddo

               !*** standard update of all passive tracers
               !***  n=3,nt with PSURF(newtime) on LHS at k=1

               call impvmixt(TRACER(:,:,:,:,newtime,iblock), &
                             TRACER(:,:,:,:,oldtime,iblock), &
                             PSURF (:,:,    newtime,iblock), &
                             3, nt, this_block)

            !*** if implicit vertical mixing but no pressure averaging
            !*** update all tracers with tridiagonal solves

            else  ! no pressure averaging or not leapfrog

               do n = 1,nt
                  !***  surface RHS for tracers with pressure avg
                  where (KMT(:,:,iblock) > 0)
                     TRACER(:,:,1,n,newtime,iblock) = &
                     TRACER(:,:,1,n,newtime,iblock) - &
                     TRACER(:,:,1,n,oldtime,iblock)   & 
                     *(PSURF(:,:,newtime,iblock) -    &
                       PSURF(:,:,mixtime,iblock))/(grav*dz(1))
                  endwhere
               enddo

               !*** standard update all tracers:  n=1,nt
               !*** with PSURF(newtime) on the LHS at k=1
               call impvmixt(TRACER(:,:,:,:,newtime,iblock), &
                             TRACER(:,:,:,:,oldtime,iblock), &
                             PSURF (:,:,    newtime,iblock), &
                             1, nt, this_block)

            endif  ! pressure averaging and leapfrog

         else ! no implicit_vertical_mix

            !*** if explicit vertical mixing and pressure averaging:
            !*** correct new tracers level k=1

            if (lpressure_avg .and. leapfrogts) then

               do n = 1,2
                  !*** correct surface T and S with pressure avg
                  where (KMT(:,:,iblock) > 0)
                     TRACER(:,:,1,n,newtime,iblock) =           &
                       (TRACER(:,:,1,n,newtime,iblock)*         &
                       (dz(1) + PSURF(:,:,curtime,iblock)/grav) &
                       +(c2*TRACER(:,:,1,n,curtime,iblock) -    &
                            TRACER(:,:,1,n,oldtime,iblock))     &
                       *(PSURF(:,:,curtime,iblock) -            &
                         PSURF(:,:,oldtime,iblock))/grav)       &
                       /(dz(1) + PSURF(:,:,newtime,iblock)/grav) 
                  elsewhere
                     TRACER(:,:,1,n,newtime,iblock) = c0 ! zero on land pts
                  endwhere
               enddo

               do n = 3,nt
                  where (KMT(:,:,iblock) > 0)
                     TRACER(:,:,1,n,newtime,iblock) =             &
                        (TRACER(:,:,1,n,oldtime,iblock)*          &
                        (dz(1) + PSURF(:,:,oldtime,iblock)/grav)  &
                        + dz(1)*TRACER(:,:,1,n,newtime,iblock))   &
                        /(dz(1) + PSURF(:,:,newtime,iblock)/grav)
                  elsewhere
                     TRACER(:,:,1,n,newtime,iblock) = c0 ! zero on land pts
                  endwhere
               enddo

            else  ! no pressure avg or leapfrog

               do n = 1,nt

                  !*** exact update of all tracers in surface layer
                  where (KMT(:,:,iblock) > 0)
                     TRACER(:,:,1,n,newtime,iblock) =              &
                        (TRACER(:,:,1,n,oldtime,iblock)*           &
                        (dz(1) + PSURF(:,:,mixtime,iblock)/grav)   &
                        + dz(1)*TRACER(:,:,1,n,newtime,iblock))/   &
                        (dz(1) + PSURF(:,:,newtime,iblock)/grav)
                  elsewhere
                     TRACER(:,:,1,n,newtime,iblock) = c0 ! zero on land pts
                  endwhere

               enddo

            endif  ! pressure avg and leapfrog

         endif ! implicit_vertical_mix

      endif ! variable thickness surface layer

!-----------------------------------------------------------------------
!
!     check for surface temperatures below freezing
!     do not reset if ice formation option is on
!
!-----------------------------------------------------------------------

      if (reset_to_freezing .and. .not. liceform) then
         TRACER(:,:,1,1,newtime,iblock) = &
            max(TRACER(:,:,1,1,newtime,iblock),-c2)
      endif

!-----------------------------------------------------------------------
!
!     convective adjustment of tracers - 
!     convad routine does nothing if convective adjustment not chosen
!     otherwise it performs convective adjustment and recomputes
!     density
!
!-----------------------------------------------------------------------

      call convad(TRACER(:,:,:,:,newtime,iblock), &
                  RHO(:,:,:,newtime,iblock), this_block, iblock)

!-----------------------------------------------------------------------
!
!     compute ice formation and adjust temperature due to ice formation
!     if this option was requested
!
!-----------------------------------------------------------------------

      if (liceform .and. mix_pass /= 1) then
         call ice_formation(TRACER(:,:,:,:,newtime,iblock),          &
                            STF(:,:,1,iblock) + SHF_QSW(:,:,iblock), &
                            this_block)
      endif

!-----------------------------------------------------------------------
!
!     compute new density based on new tracers
!
!-----------------------------------------------------------------------

      do k = 1,km  ! recalculate new density

         call state(k,k,TRACER(:,:,k,1,newtime,iblock), &
                        TRACER(:,:,k,2,newtime,iblock), & 
                        this_block, RHOOUT=RHO(:,:,k,newtime,iblock))

      enddo

!-----------------------------------------------------------------------
!
!  end of block loop
!
!-----------------------------------------------------------------------

   end do

!-----------------------------------------------------------------------
!EOC

 end subroutine baroclinic_correct_adjust

!***********************************************************************
!BOP
! !IROUTINE: clinic
! !INTERFACE:

 subroutine clinic(k, FX, FY, WUK, UCUR, VCUR, UOLD, VOLD,      &
                      UMIXK, VMIXK, RHOKOLD, RHOKCUR, RHOKNEW,  &
                      SMF_BLOCK, DHU_BLOCK, this_block)

! !DESCRIPTION:
!  Calculates forcing terms on r.h.s. of baroclinic momentum eqns.
!  \begin{eqnarray}
!     F_x &=& -L(u) + fv - \nabla p + D_H(u^{n-1}) + D_V(u^{n-1}) \\
!     F_y &=& -L(v) - fu - \nabla p + D_H(v^{n-1}) + D_V(v^{n-1})
!  \end{eqnarray}
!
!  The above equations are written for the case of explicit
!  treatment of the Coriolis terms.  If these terms are treated
!  implicitly, then the coriolis terms above should be replaced by:
!  \begin{eqnarray}
!       +fv &\rightarrow& +f(\gamma v + (1-\gamma)v^{n-1}) \\
!       -fu &\rightarrow& -f(\gamma u + (1-\gamma)u^{n-1})
!  \end{eqnarray}
!  on leapfrog timesteps and
!  \begin{eqnarray}
!       +fv &\rightarrow& +fv^{n-1} \\
!       -fu &\rightarrow& -fu^{n-1}
!  \end{eqnarray}
!  on Matsuno timesteps, where $\gamma$ is a parameter used to vary 
!  the time-centering of the Coriolis and pressure gradient terms on 
!  leapfrog steps.
!
!  The small metric terms for advection and diffusion of the
!  velocity field are calculated in the advection and horizontal 
!  diffusion routines.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: k    ! depth level index

   real (r8), dimension(nx_block,ny_block,km), intent(in) :: &
      UCUR, VCUR,           &! U,V for block at current time
      UOLD, VOLD             ! U,V for block at old     time

   real (r8), dimension(nx_block,ny_block), intent(in) :: &
      UMIXK, VMIXK,         &! U,V at level k and mix time level
      RHOKOLD,              &! density at level k and mix time level
      RHOKCUR,              &! density at level k and cur time level
      RHOKNEW,              &! density at level k and new time level
      DHU_BLOCK              ! change in surface height at U pts

   real (r8), dimension(nx_block,ny_block,2), intent(in) :: &
      SMF_BLOCK              ! surface momentum forcing for this block

   type (block), intent(in) :: &
      this_block             ! block info for the current block

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(inout) :: &
      WUK             ! vertical velocity at top of U box

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(out) :: &
      FX,       &! sum of terms contributing to Fx at level k 
      FY         ! sum of terms contributing to Fy at level k 

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables:
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      bid                ! local block address

   real (r8), dimension(nx_block,ny_block) :: &
      WORKX,WORKY     ! local work space to hold forcing terms

!-----------------------------------------------------------------------
!
!  advection L(U),L(V)
!  set vertical velocity at surface
!
!-----------------------------------------------------------------------

   bid = this_block%local_id
 
   if (k == 1) WUK = DHU_BLOCK  ! free surface

   call advu(k, WORKX, WORKY, WUK, UCUR, VCUR, this_block)

   FX =  -WORKX   ! advu returns WORKX = +L(U) 
   FY =  -WORKY   ! advu returns WORKY = +L(V) 

   if (ldiag_global) then
      if (partial_bottom_cells) then
         DIAG_KE_ADV_2D(:,:,bid) = DIAG_KE_ADV_2D(:,:,bid) -           &
                                   DZU(:,:,k,bid)*(UCUR(:,:,k)*WORKX + &
                                                   VCUR(:,:,k)*WORKY)
      else
         DIAG_KE_ADV_2D(:,:,bid) = DIAG_KE_ADV_2D(:,:,bid) -  &
                                   dz(k)*(UCUR(:,:,k)*WORKX + & 
                                          VCUR(:,:,k)*WORKY)
      endif
   endif

!-----------------------------------------------------------------------
!
!  coriolis terms
!
!-----------------------------------------------------------------------

   if (impcor .and. leapfrogts) then          ! implicit, leapfrog

      FX = FX + FCOR(:,:,bid)*(      gamma* VCUR(:,:,k) + &
                               (c1 - gamma)*VOLD(:,:,k))
      FY = FY - FCOR(:,:,bid)*(      gamma* UCUR(:,:,k) + & 
                               (c1 - gamma)*UOLD(:,:,k))

   elseif(.not.impcor .and. leapfrogts) then  ! explicit, leapfrog

      FX = FX + FCOR(:,:,bid)*VCUR(:,:,k)
      FY = FY - FCOR(:,:,bid)*UCUR(:,:,k)

   else                                  ! matsuno or foward euler

      FX = FX + FCOR(:,:,bid)*VOLD(:,:,k)
      FY = FY - FCOR(:,:,bid)*UOLD(:,:,k)

   endif

!-----------------------------------------------------------------------
!
!  hydrostatic pressure gradients
!
!-----------------------------------------------------------------------

   call gradp(k,WORKX, WORKY, RHOKOLD, RHOKCUR, RHOKNEW, this_block)

   FX = FX - WORKX   ! gradp returns WORKX as +Gradx(p)
   FY = FY - WORKY   ! gradp returns WORKY as +Grady(p)

   if (partial_bottom_cells) then
      WORKX =  -DZU(:,:,k,bid)*(UCUR(:,:,k)*WORKX + &
                                VCUR(:,:,k)*WORKY)
   else
      WORKX =  -dz(k)*(UCUR(:,:,k)*WORKX + & 
                       VCUR(:,:,k)*WORKY)
   endif

   if (tavg_requested(tavg_UDP) .and. mix_pass /= 1) then
      call accumulate_tavg_field(WORKX,tavg_UDP,bid,k)
   endif

   if (ldiag_global) then
      DIAG_KE_PRESS_2D(:,:,bid) = DIAG_KE_PRESS_2D(:,:,bid) + WORKX
   endif

!-----------------------------------------------------------------------
!
!  horizontal diffusion HDiff(Ub),HDiff(Vb)
!
!-----------------------------------------------------------------------

   call hdiffu(k, WORKX, WORKY, UMIXK, VMIXK, this_block)

   FX = FX + WORKX
   FY = FY + WORKY

   if (ldiag_global) then
      if (partial_bottom_cells) then
         DIAG_KE_HMIX_2D(:,:,bid) = DIAG_KE_HMIX_2D(:,:,bid) +        &
                                  DZU(:,:,k,bid)*(UCUR(:,:,k)*WORKX + &
                                                  VCUR(:,:,k)*WORKY)
      else
         DIAG_KE_HMIX_2D(:,:,bid) = DIAG_KE_HMIX_2D(:,:,bid) + &
                                    dz(k)*(UCUR(:,:,k)*WORKX + & 
                                           VCUR(:,:,k)*WORKY)
      endif
   endif

!-----------------------------------------------------------------------
!
!  vertical diffusion VDiff(Ub),VDiff(Vb)
!
!-----------------------------------------------------------------------

   call vdiffu(k, WORKX, WORKY, UOLD, VOLD, SMF_BLOCK, this_block)

   FX = FX + WORKX
   FY = FY + WORKY

   if (ldiag_global) then
      if (partial_bottom_cells) then
         DIAG_KE_VMIX_2D(:,:,bid) = DIAG_KE_VMIX_2D(:,:,bid) + &
                           DZU(:,:,k,bid)*(UCUR(:,:,k)*WORKX + &
                                           VCUR(:,:,k)*WORKY)
      else
         DIAG_KE_VMIX_2D(:,:,bid) = DIAG_KE_VMIX_2D(:,:,bid) + &
                                    dz(k)*(UCUR(:,:,k)*WORKX + & 
                                           VCUR(:,:,k)*WORKY)
      endif
   endif

!-----------------------------------------------------------------------
!
!  zero forces (and hence velocities) at land points
!
!-----------------------------------------------------------------------

   where (k > KMU(:,:,bid))
      FX = c0
      FY = c0
   endwhere

!-----------------------------------------------------------------------
!EOC

 end subroutine clinic

!***********************************************************************
!BOP
! !IROUTINE: tracer_update
! !INTERFACE:

 subroutine tracer_update(k, WTK, TNEW, TOLD, TMIX, TCUR,            &
                             UCUR, VCUR, UMIX, VMIX, RHOCUR,         &
                             STF_IN, TFW_IN, QSW, DH_IN, POLD, PCUR, &
                             this_block)

! !DESCRIPTION:
!  Computes explicit forcing for tracer equations:
!  \begin{equation}
!     (T^{n+1}-T^{n-1})/(2 \Delta t) = -L(T) + D_H(T^{n-1}) + 
!                                              D_V(T^{n-1}) + S
!  \end{equation}
!  where $L$ is the advection operator, $D_{H,V}$ are the diffusion
!  operators in the horizontal and vertical and $S$ are source terms.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: k     ! depth level index

   real (r8), dimension(nx_block,ny_block,km,nt), intent(in) :: &
      TCUR,                 &! tracers at current time level
      TOLD,                 &! tracers at old     time level
      TMIX                   ! tracers at mix     time level

   real (r8), dimension(nx_block,ny_block,km), intent(in) :: &
      UCUR, VCUR,           &! U,V for block at current time
      UMIX, VMIX,           &! U,V at mix time level
      RHOCUR                 ! density at current time level

   real (r8), dimension(nx_block,ny_block,nt), intent(in) :: &
      STF_IN,               &! surface tracer fluxes
      TFW_IN                 ! tracer concentration in fresh water

   real (r8), dimension(nx_block,ny_block), intent(in) :: &
      DH_IN,                &! sfc height change at tracer points
      POLD,                 &! sfc pressure at old     time
      PCUR,                 &! sfc pressure at current time
      QSW                    ! short-wave heating

   type (block), intent(in) :: &
      this_block             ! block info for the current block

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(inout) :: &
      WTK          ! on  input, vertical velocity at top    of T box
                   ! on output, vertical velocity at bottom of T box

   real (r8), dimension(nx_block,ny_block,km,nt), intent(inout) :: &
      TNEW                   ! tracers at new time level

!EOP
!BOC
!-----------------------------------------------------------------------
!         
!  local variables:
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      n,                 &! dummy tracer index
      bid                 ! local_block id

   real (r8), dimension(nx_block,ny_block,nt) :: &
      FT,                &! sum of terms in dT/dt for the nth tracer
      WORKN               ! work array used for various dT/dt terms 

   real (r8), dimension(nx_block,ny_block) :: &
      WORKSW

!-----------------------------------------------------------------------
!
!  initialize some arrays
!
!-----------------------------------------------------------------------

   FT    = c0

   bid = this_block%local_id

!-----------------------------------------------------------------------
!
!  horizontal diffusion HDiff(T)
!
!-----------------------------------------------------------------------

   call hdifft(k, WORKN, TMIX, UMIX, VMIX, this_block)

   FT = FT + WORKN

   if (ldiag_global) then
      if (partial_bottom_cells) then
         do n=1,nt
            where (k <= KMT(:,:,bid))            &
               DIAG_TRACER_HDIFF_2D(:,:,n,bid) = &
               DIAG_TRACER_HDIFF_2D(:,:,n,bid) + &
               WORKN(:,:,n)*DZT(:,:,k,bid)
         end do
      else
         do n=1,nt
            where (k <= KMT(:,:,bid))            &
               DIAG_TRACER_HDIFF_2D(:,:,n,bid) = & 
               DIAG_TRACER_HDIFF_2D(:,:,n,bid) + &
               WORKN(:,:,n)*dz(k)
         end do
      endif
   endif

!-----------------------------------------------------------------------
!
!  advection L(T)
!  set vertical velocity at surface
!  w = DH = dh/dt or dh/dt - Fw depending on surface type
!
!-----------------------------------------------------------------------

   if (k == 1) WTK = DH_IN

   if (ldiag_global) then

      if (k == 1) then
         where (CALCT(:,:,bid)) &
            DIAG_PE_2D(:,:,bid) = dzw(0)*grav*RHOCUR(:,:,1)*WTK

         if (sfc_layer_type /= sfc_layer_varthick) then
            do n=1,nt
               where (CALCT(:,:,bid))
                  DIAG_TRACER_ADV_2D (:,:,n,bid) = WTK*TCUR(:,:,1,n)
                  DIAG_TRACER_SFC_FLX(:,:,n,bid) = WTK*TCUR(:,:,1,n)
               elsewhere
                  DIAG_TRACER_ADV_2D (:,:,n,bid) = c0
                  DIAG_TRACER_SFC_FLX(:,:,n,bid) = c0
               end where
            end do
         endif

      else

         !*** For energetic consistency, we use dzw even for 
         !*** partial bottom cell case

         where (k <= KMT(:,:,bid))
            DIAG_PE_2D(:,:,bid) = DIAG_PE_2D(:,:,bid) + &
                                  dzw(k-1)*WTK*grav*p5* &
                                  (RHOCUR(:,:,k-1) + RHOCUR(:,:,k))
         endwhere

      endif

   endif

   call advt(k,WORKN,WTK,TCUR,UCUR,VCUR,this_block)

   FT = FT - WORKN   ! advt returns WORKN = +L(T) 

   if (ldiag_global) then
     if (partial_bottom_cells) then
       do n=1,nt
         where (k <= KMT(:,:,bid)) DIAG_TRACER_ADV_2D(:,:,n,bid) = & 
                                   DIAG_TRACER_ADV_2D(:,:,n,bid) - &
                                   WORKN(:,:,n)*DZT(:,:,k,bid)
       end do
     else
       do n=1,nt
         where (k <= KMT(:,:,bid)) DIAG_TRACER_ADV_2D(:,:,n,bid) = &
                                   DIAG_TRACER_ADV_2D(:,:,n,bid) - &
                                   WORKN(:,:,n)*dz(k)
       end do
     endif 
   endif

!-----------------------------------------------------------------------
!
!  vertical diffusion VDiff(T)
!
!-----------------------------------------------------------------------

   call vdifft(k, WORKN, TOLD, STF_IN, this_block)

   FT = FT + WORKN

   if (ldiag_global) then
     if (partial_bottom_cells) then
       do n=1,nt
         where (k <= KMT(:,:,bid)) DIAG_TRACER_VDIFF_2D(:,:,n,bid) = &
                                   DIAG_TRACER_VDIFF_2D(:,:,n,bid) + &
                                   WORKN(:,:,n)*DZT(:,:,k,bid)
       end do
     else
       do n=1,nt
         where (k <= KMT(:,:,bid)) DIAG_TRACER_VDIFF_2D(:,:,n,bid) = &
                                   DIAG_TRACER_VDIFF_2D(:,:,n,bid) + &
                                   WORKN(:,:,n)*dz(k)
       end do
     endif
   endif

!-----------------------------------------------------------------------
!
!  add tracer change in surface layer due to freshwater flux
!  if using variable thickness surface layer
!
!-----------------------------------------------------------------------

   if (k == 1 .and. sfc_layer_type == sfc_layer_varthick) then
      do n = 1,nt
         FT(:,:,n) = FT(:,:,n) + dzr(1)*TFW_IN(:,:,n)
      enddo

      if (ldiag_global) then
         do n = 1,nt
            DIAG_TRACER_SFC_FLX(:,:,n,bid) = TFW_IN(:,:,n)
         enddo
      endif
   endif

!-----------------------------------------------------------------------
!
!  add source terms
!
!-----------------------------------------------------------------------

   WORKN = c0

   call set_pt_interior(k,this_block,WORKN(:,:,1))
   call set_s_interior (k,this_block,WORKN(:,:,2))

!-----------------------------------------------------------------------
!
!  add source terms from KPP and from shortwave solar absorption
!    if necessary.
!  NOTE:  this is here instead of in set_{pt,s}_interior in case
!    KPP and/or shortwave solar absorption are turned on but 
!    bulk restoring is not.
!
!-----------------------------------------------------------------------

   !*** does nothing if kpp not chosen - otherwise adds kpp sources
   call add_kpp_sources(WORKN, k, this_block)

   !*** if sw flux available, add penetrative shortwave
   call add_sw_absorb(WORKN, k, this_block)

   FT = FT + WORKN

   if (ldiag_global) then
     if (partial_bottom_cells) then
       do n=1,nt
         where (k <= KMT(:,:,bid)) DIAG_TRACER_SOURCE_2D(:,:,n,bid) = &
                                   DIAG_TRACER_SOURCE_2D(:,:,n,bid) + &
                                   WORKN(:,:,n)*DZT(:,:,k,bid)
       end do
     else
       do n=1,nt
         where (k <= KMT(:,:,bid)) DIAG_TRACER_SOURCE_2D(:,:,n,bid) = &
                                   DIAG_TRACER_SOURCE_2D(:,:,n,bid) + &
                                   WORKN(:,:,n)*dz(k)
       end do
     endif
   endif

!-----------------------------------------------------------------------
!
!  save the explicit part of the RHS in TRACER(newtime)
!  if there is implicit vertical mixing
!
!  with pressure averaging and variable thickness surface layer, 
!  the RHS contains the surface height contribution for the 
!  predictor step (for T,S at k=1 only)
!
!-----------------------------------------------------------------------

   if (implicit_vertical_mix) then

      if (sfc_layer_type == sfc_layer_varthick .and. k == 1 .and. &
          lpressure_avg .and. leapfrogts) then

         do n = 1,2
            where (KMT(:,:,bid) > 0)  ! RHS for predictor
               TNEW(:,:,1,n) = c2dtt(1)*FT(:,:,n) - c2*TCUR(:,:,1,n)* &
                               (PCUR - POLD)/(grav*dz(1))
            endwhere
         enddo

         do n = 3,nt
            TNEW(:,:,k,n) = merge(c2dtt(k)*FT(:,:,n), &
                                  c0, k <= KMT(:,:,bid))
         enddo

      else

         do n = 1,nt
            TNEW(:,:,k,n) = merge(c2dtt(k)*FT(:,:,n), &
                                  c0, k <= KMT(:,:,bid))
         enddo

      endif

!-----------------------------------------------------------------------
!
!  for variable thickness surface layer, update all but surface
!    layers. 
!  at the surface:
!    if explicit vertical mixing and pressure averaging:
!      predict surface T,S and store RHS for all other tracers
!    otherwise
!      store RHS for all tracers for later update with new Psurf
!
!  if not a variable thickness surface layer, update tracers here
!     
!-----------------------------------------------------------------------

   else ! no implicit_vertical_mix

      if (sfc_layer_type == sfc_layer_varthick .and. k == 1) then
         if (lpressure_avg .and. leapfrogts) then

            !*** predict surface T and S with pressure avg

            do n = 1,2
               where (KMT(:,:,bid) > 0)
                  TNEW(:,:,1,n) = TOLD(:,:,1,n)                        &
                           + (c1/(c1 + PCUR/(grav*dz(1))))*            &
                             (c2dtt(1)*FT(:,:,n) - c2*TCUR(:,:,1,n)*   &
                             (PCUR-POLD)/(grav*dz(1)))
               elsewhere
                  TNEW(:,:,1,n) = c0  ! zero tracers on land pts
               endwhere
            end do

            !*** store RHS for other tracer surface layers

            do n = 3,nt
               TNEW(:,:,k,n) = merge(c2dtt(k)*FT(:,:,n), &
                                     c0, k <= KMT(:,:,bid))
            enddo

         else

            !*** store RHS for all tracer surface layers

            do n = 1,nt
               TNEW(:,:,k,n) = merge(c2dtt(k)*FT(:,:,n), &
                                     c0, k <= KMT(:,:,bid))
            enddo

         endif

      else   !*** update all tracers to new time

         do n = 1,nt
            where (k <= KMT(:,:,bid))
               TNEW(:,:,k,n) = TOLD(:,:,k,n) + c2dtt(k)*FT(:,:,n)
            elsewhere
               TNEW(:,:,k,n) = c0  ! zero tracers at land points
            endwhere
         enddo

      endif
   endif ! implicit_vertical_mix

!-----------------------------------------------------------------------
!EOC

 end subroutine tracer_update

!***********************************************************************

 end module baroclinic

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
