!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module advection

!BOP
! !MODULE: advection
!
! !DESCRIPTION:
!  This module contains arrays and variables necessary for performing
!  advection of momentum and tracer quantities.  Currently, the
!  module supports leapfrog centered advection of momentum and
!  both leapfrog centered advection and third-order upwinding of
!  tracers.
!
! !REVISION HISTORY:
!  CVS:$Id: advection.F90,v 1.24 2003/02/26 17:26:35 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod, only: r8, int_kind, char_len, log_kind
   use constants, only: c0, p5, p125, p25, blank_fmt, delim_fmt, c2
   use blocks, only: nx_block, ny_block, block, get_block
   use communicate, only: my_task, master_task
   use distribution, only: 
   use grid, only: dz, DXT, DYT, HUW, HUS, c2dz, KMT, HTE, UAREA_R, DZT,    &
       partial_bottom_cells, DYU, DZU, DXU, DZR, DZ2R, KMU, TAREA_R, HTN,   &
       sfc_layer_type, sfc_layer_type, sfc_layer_varthick, FCORT, KMTE,     &
       KMTW, KMTEE, KMTN, KMTS, KMTNN, ugrid_to_tgrid
   use domain, only: nblocks_clinic, blocks_clinic, distrb_clinic
   use broadcast, only: broadcast_scalar
!   use boundary, only: 
   use diagnostics, only: cfl_advect
   use state_mod, only: state
   use operators, only: zcurl
   use tavg, only: tavg_requested, ltavg_on, define_tavg_field,             &
       accumulate_tavg_field
   use io_types, only: nml_in, nml_filename, stdout
   use time_management, only: max_blocks_clinic, km, nt, mix_pass
   use timers, only: timer_start, timer_stop, get_timer
   use hmix_gm, only: U_ISOP, V_ISOP, WBOT_ISOP, WTOP_ISOP
   use exit_mod, only: sigAbort, exit_pop

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:
   public :: init_advection, &
             advt,           &
             advu

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  choices for tracer advection
!
!-----------------------------------------------------------------------

   integer (int_kind), parameter :: &
      tadvect_centered = 1,  &! centered leap-frog tracer advection
      tadvect_upwind3  = 2    ! 3rd-order upwind tracer advection

   integer (int_kind) :: &
      tadvect_itype           ! users tracer advection choice

   logical (log_kind) :: &
      luse_isopycnal          ! use isopycnal velocities generated
                              ! in Gent-McWilliams horizontal mixing

!-----------------------------------------------------------------------
!
!  coefficients for metric advection terms (KXU,KYU)
!
!-----------------------------------------------------------------------

   real (r8), dimension (nx_block,ny_block,max_blocks_clinic) :: & 
      KXU,KYU

!-----------------------------------------------------------------------
!
!  geometric arrays necessary for upwind advection
!
!-----------------------------------------------------------------------

   real (r8), dimension(:), allocatable :: &
      talfzp,tbetzp,tgamzp,                &
      talfzm,tbetzm,tdelzm

   real (r8), dimension(:,:,:), allocatable ::   &
      TALFXP,TBETXP,TGAMXP,TALFYP,TBETYP,TGAMYP, &
      TALFXM,TBETXM,TDELXM,TALFYM,TBETYM,TDELYM

   real (r8), dimension(:,:,:,:), allocatable :: &
      AUX

!-----------------------------------------------------------------------
!
!  tavg ids for tavg diagnostics related to advection
!  north, east, zonal, merid refer here to logical space only
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      tavg_UEU,          &! flux of zonal momentum across east  face
      tavg_VNU,          &! flux of zonal momentum across north face
      tavg_WTU,          &! flux of zonal momentum across top   face
      tavg_UEV,          &! flux of merid momentum across east  face
      tavg_VNV,          &! flux of merid momentum across north face
      tavg_WTV,          &! flux of merid momentum across top   face
      tavg_UET,          &! flux of heat across east  face
      tavg_VNT,          &! flux of heat across north face
      tavg_WTT,          &! flux of heat across top   face
      tavg_UES,          &! flux of salt across east  face
      tavg_VNS,          &! flux of salt across north face
      tavg_WTS,          &! flux of salt across top   face
      tavg_ADVT,         &! vertical average of advective tendency
      tavg_PV,           &! potential vorticity
      tavg_Q,            &! z-derivative of pot density
      tavg_PD,           &! potential density 
      tavg_RHOU,         &! pot density times U velocity
      tavg_RHOV,         &! pot density times V velocity
      tavg_PVWM,         &! pot vorticity flux through bottom
      tavg_PVWP,         &! pot vorticity flux through top
      tavg_UPV,          &! pot vorticity flux through east  face
      tavg_VPV,          &! pot vorticity flux through north face
      tavg_URHO,         &! pot density   flux through east  face
      tavg_VRHO,         &! pot density   flux through north face
      tavg_WRHO,         &! pot density   flux through top   face
      tavg_UQ,           &! advection of Q across east  face
      tavg_VQ             ! advection of Q across north face

!-----------------------------------------------------------------------
!
!  advection timers
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      timer_advt,        &! timer for tracer   advection
      timer_advu          ! timer for momentum advection

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_advection
! !INTERFACE:

 subroutine init_advection 

! !DESCRIPTION:
!  This subroutine initializes variables associated with advection
!  schemes.
!
! !REVISION HISTORY:
!  same as module

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  namelist input
!
!-----------------------------------------------------------------------

   character (char_len) :: &
      tadvect_ctype        ! character string for tracer advect choice

   namelist /advect_nml/ tadvect_ctype

!-----------------------------------------------------------------------
!
!  local variables for setting up upwind coefficients
!
!-----------------------------------------------------------------------

   integer (int_kind) :: & 
      i,j,k,             &! dummy loop indices
      iblock,            &! local block number
      nattempts,         &! num of attempts to read namelist input
      nml_error           ! error flag for namelist read

   real (r8) ::            &
      dxc,dxcw,dxce,dxce2, &
      dyc,dycs,dycn,dycn2          

   real (r8), dimension(:), allocatable :: & 
      dzc

   type (block) ::        &
      this_block          ! block information for current block

!-----------------------------------------------------------------------
!
!  read input namelist for choice of tracer advection method
!
!-----------------------------------------------------------------------

   tadvect_ctype = 'centered'

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old', iostat=nml_error)
      if (nml_error /= 0) then
        nml_error = -1
      else
        nml_error =  1
      endif
      !*** keep reading until find right namelist
      do while (nml_error > 0)
        read(nml_in, nml=advect_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   end if

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading advection namelist')
   endif
     
   if (my_task == master_task) then
      write(stdout,delim_fmt)
      write(stdout,blank_fmt)
      write(stdout,'(a17)') 'Advection options'
      write(stdout,blank_fmt)
      write(stdout,delim_fmt)

      select case (tadvect_ctype(1:6))

      case ('center')
         tadvect_itype = tadvect_centered
         write(stdout,'(a46)') &
                       'Using centered leapfrog advection for tracers.'
      case ('upwind')
         tadvect_itype = tadvect_upwind3
         write(stdout,'(a45)') & 
                       'Using 3rd-order upwind advection for tracers.'
      case default
         tadvect_itype = -1000
      end select

   endif
   call broadcast_scalar(tadvect_itype, master_task)

   if (tadvect_itype < 0) then
      call exit_POP(sigAbort,'ERROR: unknown tracer advection method')
   endif

!-----------------------------------------------------------------------
!
!  initialize metric advection coefficients
!
!-----------------------------------------------------------------------

   do iblock = 1,nblocks_clinic

      this_block = get_block(blocks_clinic(iblock),iblock)  

      KXU(:,:,iblock) = (eoshift(HUW(:,:,iblock),dim=1,shift=1) - & 
                                 HUW(:,:,iblock))*UAREA_R(:,:,iblock)
      KYU(:,:,iblock) = (eoshift(HUS(:,:,iblock),dim=2,shift=1) - &
                                 HUS(:,:,iblock))*UAREA_R(:,:,iblock)

   end do

   !*** KXU,KYU only needed in physical domain
   !*** no ghost cell update required assuming HUS,HUW,UAREA_R
   !*** were defined correctly in ghost cells
   !call update_ghost_cells(KXU, bndy_clinic, field_loc_u,     &
   !                                          field_type_scalar)
   !call update_ghost_cells(KYU, bndy_clinic, field_loc_u,     &
   !                                          field_type_scalar)

!-----------------------------------------------------------------------
!
!  set flag for use of isopycnal velocities generated in Gent-McWilliams
!  parameterization of horizontal mixing
!
!-----------------------------------------------------------------------

   if (allocated(U_ISOP)) then
      luse_isopycnal = .true.
   else
      luse_isopycnal = .false.
   endif

!-----------------------------------------------------------------------
!
!  allocate and initialize upwinding grid arrays if necessary
!
!-----------------------------------------------------------------------

   if (tadvect_itype == tadvect_upwind3) then

      allocate (talfzp(km), &
                tbetzp(km), &
                tgamzp(km), &
                talfzm(km), &
                tbetzm(km), &
                tdelzm(km))

      allocate (TALFXP(nx_block,ny_block,nblocks_clinic), &
                TBETXP(nx_block,ny_block,nblocks_clinic), &
                TGAMXP(nx_block,ny_block,nblocks_clinic), &
                TALFYP(nx_block,ny_block,nblocks_clinic), &
                TBETYP(nx_block,ny_block,nblocks_clinic), &
                TGAMYP(nx_block,ny_block,nblocks_clinic), &
                TALFXM(nx_block,ny_block,nblocks_clinic), &
                TBETXM(nx_block,ny_block,nblocks_clinic), &
                TDELXM(nx_block,ny_block,nblocks_clinic), &
                TALFYM(nx_block,ny_block,nblocks_clinic), &
                TBETYM(nx_block,ny_block,nblocks_clinic), &
                TDELYM(nx_block,ny_block,nblocks_clinic))

      allocate (AUX   (nx_block,ny_block,nt,nblocks_clinic))

      allocate (dzc(0:km+1))

      !***
      !*** vertical grid coefficients     
      !***

      dzc(0)    = dz(1)
      do k=1,km
         dzc(k)  = dz(k)
      enddo
      dzc(km+1) = dzc(km)

      do k=1,km-1
         talfzp(k) =  dz(k)*(c2*dz(k)+dzc(k-1))/ &
                      ((dz(k)+dz(k+1))*          &
                      (dzc(k-1)+c2*dz(k)+dz(k+1)))
         tbetzp(k) =  dz(k+1)*(c2*dz(k)+dzc(k-1))/ &
                      ((dz(k)+dz(k+1))*            &
                      (dz(k)+dzc(k-1)          ))
         tgamzp(k) =  -(dz(k)*dz(k+1))/ &
                      ((dz(k)+dzc(k-1))*           &
                      (dz(k+1)+dzc(k-1)+c2*dz(k)))
      enddo 
      tbetzp(1) = tbetzp(1) + tgamzp(1)
      tgamzp(1) = c0
      talfzp(km) = c0
      tbetzp(km) = c0
      tgamzp(km) = c0
 
      do k=1,km-1
         talfzm(k) =  dz(k)*(c2*dz(k+1)+dzc(k+2))/ &
                      ((dz(k)+dz(k+1))*            &
                      (dz(k+1)+dzc(k+2)            ))
         tbetzm(k) =  dz(k+1)*(c2*dz(k+1)+dzc(k+2))/ &
                      ((dz(k)+dz(k+1))*              &
                      (dz(k)+dzc(k+2)+c2*dz(k+1)))
         tdelzm(k) =  -(dz(k)*dz(k+1))/ &
                      ((dz(k+1)+dzc(k+2))*         &
                      (dz(k)+dzc(k+2)+c2*dz(k+1)))
      enddo    
      talfzm(km-1) = talfzm(km-1) + tdelzm(km-1)
      tdelzm(km-1) = c0
      talfzm(km) = c0
      tbetzm(km) = c0
      tdelzm(km) = c0

      deallocate (dzc)

      !***
      !*** horizontal grid coeffs
      !***

      do iblock = 1,nblocks_clinic

         this_block = get_block(blocks_clinic(iblock),iblock)  

         !***
         !*** zonal grid coefficients     
         !***

         do j=this_block%jb,this_block%je
         do i=this_block%ib-1,this_block%ie

            dxc   = DXT(i  ,j,iblock)
            dxcw  = DXT(i-1,j,iblock)
            dxce  = DXT(i+1,j,iblock)
            dxce2 = DXT(i+2,j,iblock)

            TALFXP(i,j,iblock) = dxc*(c2*dxc+dxcw)/ &
                                 ((dxc+dxce)*(dxcw+c2*dxc+dxce))
            TBETXP(i,j,iblock) = dxce*(c2*dxc+dxcw)/ &
                                 ((dxc+dxcw)*(        dxc+dxce))
            TGAMXP(i,j,iblock) =     -(   dxc*dxce)/ &
                                 ((dxc+dxcw)*(dxcw+c2*dxc+dxce))

            TALFXM(i,j,iblock) = dxc *(c2*dxce+dxce2)/ &
                                 ((dxc  +dxce)*(       dxce+dxce2))
            TBETXM(i,j,iblock) = dxce*(c2*dxce+dxce2)/ &
                                 ((dxc  +dxce)*(dxc+c2*dxce+dxce2))
            TDELXM(i,j,iblock) =     -(   dxc *dxce )/ &
                                 ((dxce2+dxce)*(dxc+c2*dxce+dxce2))

         end do
         end do

         !***
         !*** poloidal grid coefficients     
         !***

         do j=this_block%jb-1,this_block%je
         do i=this_block%ib,this_block%ie

            dyc   = DYT(i,j  ,iblock)
            dycs  = DYT(i,j-1,iblock)
            dycn  = DYT(i,j+1,iblock)
            dycn2 = DYT(i,j+2,iblock)

            TALFYP(i,j,iblock) = dyc *(c2*dyc+dycs)/ &
                                 ((dyc+dycn)*(dycs+c2*dyc+dycn))
            TBETYP(i,j,iblock) = dycn*(c2*dyc+dycs)/ &
                                 ((dyc+dycn)*(dycs+   dyc     ))
            TGAMYP(i,j,iblock) =     -(   dyc*dycn)/ &
                                 ((dyc+dycs)*(dycs+c2*dyc+dycn))
 
            TALFYM(i,j,iblock) = dyc *(c2*dycn+dycn2)/ &
                                 ((dyc+dycn)*(       dycn+dycn2))
            TBETYM(i,j,iblock) = dycn*(c2*dycn+dycn2)/ &
                                 ((dyc+dycn)*(dyc+c2*dycn+dycn2))
            TDELYM(i,j,iblock) =     -(   dyc *dycn )/ &
                                 ((dycn2+dycn)*(dyc+c2*dycn+dycn2))

         end do
         end do

      end do

      !*** assuming DXT,DYT were defined correctly in ghost cells
      !*** these are valid from (ib-1:ie,jb-1:je) and are only
      !*** accessed in that range so no update necessary

      !call update_ghost_cells(TALFXP, bndy_clinic, field_loc_t,     &
      !                                             field_type_scalar)
      !call update_ghost_cells(TBETXP, bndy_clinic, field_loc_t,     &
      !                                             field_type_scalar)
      !call update_ghost_cells(TGAMXP, bndy_clinic, field_loc_t,     &
      !                                             field_type_scalar)
      !call update_ghost_cells(TALFYP, bndy_clinic, field_loc_t,     &
      !                                             field_type_scalar)
      !call update_ghost_cells(TBETYP, bndy_clinic, field_loc_t,     &
      !                                             field_type_scalar)
      !call update_ghost_cells(TGAMYP, bndy_clinic, field_loc_t,     &
      !                                             field_type_scalar)
      !call update_ghost_cells(TALFXM, bndy_clinic, field_loc_t,     &
      !                                             field_type_scalar)
      !call update_ghost_cells(TBETXM, bndy_clinic, field_loc_t,     &
      !                                             field_type_scalar)
      !call update_ghost_cells(TDELXM, bndy_clinic, field_loc_t,     &
      !                                             field_type_scalar)
      !call update_ghost_cells(TALFYM, bndy_clinic, field_loc_t,     &
      !                                             field_type_scalar)
      !call update_ghost_cells(TBETYM, bndy_clinic, field_loc_t,     &
      !                                             field_type_scalar)
      !call update_ghost_cells(TDELYM, bndy_clinic, field_loc_t,     &
      !                                             field_type_scalar)

   endif ! 3rd order upwind setup

!-----------------------------------------------------------------------
!
!  initialize timers
!
!-----------------------------------------------------------------------

   call get_timer(timer_advu,'ADVECTION_MOMENTUM', nblocks_clinic, &
                                                   distrb_clinic%nprocs)
 
   select case (tadvect_itype)
   case(tadvect_centered)
      call get_timer(timer_advt,'ADVECTION_TRACER_CENTERED', &
                                 nblocks_clinic, distrb_clinic%nprocs)
   case(tadvect_upwind3)
      call get_timer(timer_advt,'ADVECTION_TRACER_UPWIND3', &
                                 nblocks_clinic, distrb_clinic%nprocs)
   end select

!-----------------------------------------------------------------------
!
!  define tavg fields related to advection
!
!-----------------------------------------------------------------------

   call define_tavg_field(tavg_UEU,'UEU',3,                            &
                          long_name='East Flux of Zonal Momentum',     &
                          units='cm/s^2', grid_loc='3321')

   call define_tavg_field(tavg_VNU,'VNU',3,                            &
                          long_name='North Flux of Zonal Momentum',    &
                          units='cm/s^2', grid_loc='3231')

   call define_tavg_field(tavg_UEV,'UEV',3,                            &
                          long_name='East Flux of Meridional Momentum',&
                          units='cm/s^2', grid_loc='3321')

   call define_tavg_field(tavg_VNV,'VNV',3,                            &
                        long_name='North Flux of Meridional Momentum', &
                          units='cm/s^2', grid_loc='3231')

   call define_tavg_field(tavg_WTU,'WTU',3,                            &
                          long_name='Top flux of Zonal Momentum',      &
                          units='cm/s^2', grid_loc='3222')

   call define_tavg_field(tavg_WTV,'WTV',3,                            &
                          long_name='Top flux of Meridional Momentum', &
                          units='cm/s^2', grid_loc='3222')

   call define_tavg_field(tavg_UET,'UET',3,                            &
                          long_name='East Flux of Heat',               &
                          units='degC/s', grid_loc='3211')

   call define_tavg_field(tavg_VNT,'VNT',3,                            &
                          long_name='North Flux of Heat',              &
                          units='degC/s', grid_loc='3121')

   call define_tavg_field(tavg_WTT,'WTT',3,                            &
                          long_name='Top Flux of Heat',                &
                          units='degC/s', grid_loc='3112')

   call define_tavg_field(tavg_UES,'UES',3,                            &
                          long_name='East Flux of Salt',               &
                          units='g/g/s', grid_loc='3211')

   call define_tavg_field(tavg_VNS,'VNS',3,                            &
                          long_name='North Flux of Salt',              &
                          units='g/g/s', grid_loc='3121')

   call define_tavg_field(tavg_WTS,'WTS',3,                            &
                          long_name='Top Flux of Salt',                &
                          units='g/g/s', grid_loc='3112')

   call define_tavg_field(tavg_ADVT,'ADVT',2,                          &
                    long_name='Vertically integrated T advection tend',&
                          units='degC/cm^2', grid_loc='2110')

   call define_tavg_field(tavg_PV,'PV',3,                              &
                          long_name='Potential Vorticity',             &
                          units='1/s', grid_loc='3111')

   call define_tavg_field(tavg_Q,'Q',3,                                &
                        long_name='z-derivative of potential density', &
                          units='g/cm^4', grid_loc='3111')

   call define_tavg_field(tavg_PD,'PD',3,                              &
                          long_name='Potential density ref to surface',&
                          units='g/cm^3', grid_loc='3111')

   call define_tavg_field(tavg_RHOU,'RHOU',3,                          &
                          long_name='U times potential density',       &
                          units='g/cm^2/s', grid_loc='3111')

   call define_tavg_field(tavg_RHOV,'RHOV',3,                          &
                          long_name='V times potential density',       &
                          units='g/cm^2/s', grid_loc='3111')

   call define_tavg_field(tavg_URHO,'URHO',3,                          &
                     long_name='flux of pot density across east face', &
                          units='g/cm^2/s', grid_loc='3321')

   call define_tavg_field(tavg_VRHO,'VRHO',3,                          &
                    long_name='flux of pot density across north face', &
                          units='g/cm^2/s', grid_loc='3231')

   call define_tavg_field(tavg_WRHO,'WRHO',3,                          &
                      long_name='flux of pot density across top face', &
                          units='g/cm^2/s', grid_loc='3112')

   call define_tavg_field(tavg_PVWM,'PVWM',3,                          &
                   long_name='flux of pot vorticity through top face', &
                          units='cm/s^2', grid_loc='3112')

   call define_tavg_field(tavg_PVWP,'PVWP',3,                          &
                long_name='flux of pot vorticity through bottom face', &
                          units='cm/s^2', grid_loc='3111')

   call define_tavg_field(tavg_UPV,'UPV',3,                            &
                  long_name='flux of pot vorticity through east face', &
                          units='cm/s^2', grid_loc='3321')

   call define_tavg_field(tavg_VPV,'VPV',3,                            &
                 long_name='flux of pot vorticity through north face', &
                          units='cm/s^2', grid_loc='3231')

   call define_tavg_field(tavg_UQ,'UQ',3,                              &
                          long_name='flux of Q through east face',     &
                          units='g/cm^3/s', grid_loc='3321')

   call define_tavg_field(tavg_VQ,'VQ',3,                              &
                          long_name='flux of Q through north face',    &
                          units='g/cm^3/s', grid_loc='3231')

!-----------------------------------------------------------------------
!EOC

 end subroutine init_advection 

!***********************************************************************
!BOP
! !IROUTINE: advu
! !INTERFACE:

 subroutine advu(k,LUK,LVK,WUK,UUU,VVV,this_block)

! !DESCRIPTION:
!  This routine computes the $x,y$ components of the advection of 
!  momentum and metric terms given by:
!  \begin{equation}
!    L_U(u_x) + u_x u_y k_y - u_y^2 k_x
!  \end{equation}
!  \begin{equation}
!    L_U(u_y) + u_x u_y k_x - u_x^2 k_y
!  \end{equation}
!  where
!  \begin{equation}
!     L_U(\alpha) = {1\over{\Delta_y}}
!                \delta_x\left[
!                 \overline{\left(\overline{\Delta_y u_x}^y\right)}^{xy}
!                 \overline{\alpha}^x\right] + 
!                {1\over{\Delta_x}}
!                \delta_y\left[
!                 \overline{\left(\overline{\Delta_x u_y}^x\right)}^{xy}
!                 \overline{\alpha}^y\right] 
!           + \delta_z(w^U\overline{\alpha}^z),
!  \end{equation}
!  \begin{equation}
!  k_x = {1\over{\Delta_y}}\delta_x \Delta_y
!  \end{equation}
!  and
!  \begin{equation}
!  k_y = {1\over{\Delta_x}}\delta_y \Delta_x
!  \end{equation}
!
!  Comments:
!   \begin{itemize}
!     \item this routine must be called successively with k = 1,2,3,...
!
!     \item the vertical velocity $w^U$ in U columns is determined by 
!     integrating the continuity equation $L(1) = 0$ from the surface
!     down to level k.  In the rigid-lid formulation, the integration
!     starts with $w^U = 0$ at the surface.  In the free-surface 
!     formulation, the integration starts with $w^U =$ the area-weighted
!     average of $\partial\eta/\partial t$ at surrounding T points
!     ($\partial\eta/\partial t$ is the time change of the surface 
!     height, and satisfies the barotropic continuity equation 
!     $\partial\eta/\partial t + \nabla\cdot H {\rm\bf U}=q_w$
!     where ${\rm\bf U}$ is the barotropic velocity and $q_w$
!     is the surface fresh water flux.)
!   \end{itemize}
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: k ! depth level index

   real (r8), dimension(nx_block,ny_block,km), intent(in) :: & 
      UUU,VVV             ! U,V velocity for this block 
                          ! at the current time

   type (block), intent(in) :: &
      this_block          ! block information for this block

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(inout) :: & 
      WUK             ! on  input: flux velocity at top    of U box
                      ! on output: flux velocity at bottom of U box

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(out) :: & 
      LUK,               &! advection of U-momentum
      LVK                 ! advection of V-momentum

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables:
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j,n,             &! loop indices
      ibeg,iend,         &! beginning and ending index of
      jbeg,jend,         &!  physical domain
      bid                 ! local block index

   real (r8) ::          &
      cc                  ! scalar central weight

   real (r8), dimension(nx_block,ny_block) :: &
      UUE,UUW,VUN,VUS, &! momen  flux velocities across E,W,N,S faces
      FVN,FUE,         &! stencil coeffs used by tavg
      WORK,            &! local temp space
      WUKB              ! vert velocity at bottom of level k U box

!-----------------------------------------------------------------------
!
!  advection fluxes for U box (4-point averages of adv fluxes
!  across T boxes).
!
!-----------------------------------------------------------------------

   bid  = this_block%local_id

   call timer_start(timer_advu, block_id=bid)

   ibeg = this_block%ib
   iend = this_block%ie
   jbeg = this_block%jb
   jend = this_block%je

   UUW = c0
   UUE = c0
   VUN = c0
   VUS = c0

   if (partial_bottom_cells) then

      do j=jbeg-1,jend+1
      do i=ibeg-1,iend+1

         UUW(i,j) = p25 *(UUU(i  ,j  ,k)*DYU(i  ,j    ,bid)*  &
                                         DZU(i  ,j  ,k,bid) + &
                          UUU(i-1,j  ,k)*DYU(i-1,j    ,bid)*  &
                                         DZU(i-1,j  ,k,bid))+ &
                    p125*(UUU(i  ,j-1,k)*DYU(i  ,j-1  ,bid)*  &
                                         DZU(i  ,j-1,k,bid) + &
                          UUU(i-1,j-1,k)*DYU(i-1,j-1  ,bid)*  &
                                         DZU(i-1,j-1,k,bid) + &
                          UUU(i  ,j+1,k)*DYU(i  ,j+1  ,bid)*  &
                                         DZU(i  ,j+1,k,bid) + &
                          UUU(i-1,j+1,k)*DYU(i-1,j+1  ,bid)*  &
                                         DZU(i-1,j+1,k,bid))

         UUE(i,j) = p25 *(UUU(i+1,j  ,k)*DYU(i+1,j    ,bid)*  &
                                         DZU(i+1,j  ,k,bid) + &
                          UUU(i  ,j  ,k)*DYU(i  ,j    ,bid)*  &
                                         DZU(i  ,j  ,k,bid))+ &
                    p125*(UUU(i+1,j-1,k)*DYU(i+1,j-1  ,bid)*  &
                                         DZU(i+1,j-1,k,bid) + &
                          UUU(i  ,j-1,k)*DYU(i  ,j-1  ,bid)*  &
                                         DZU(i  ,j-1,k,bid) + &
                          UUU(i+1,j+1,k)*DYU(i+1,j+1  ,bid)*  &
                                         DZU(i+1,j+1,k,bid) + &
                          UUU(i  ,j+1,k)*DYU(i  ,j+1  ,bid)*  &
                                         DZU(i  ,j+1,k,bid))
   
         VUS(i,j) = p25* (VVV(i  ,j  ,k)*DXU(i  ,j    ,bid)*  &
                                         DZU(i  ,j  ,k,bid) + &
                          VVV(i  ,j-1,k)*DXU(i  ,j-1  ,bid)*  &
                                         DZU(i  ,j-1,k,bid))+ &
                    p125*(VVV(i-1,j  ,k)*DXU(i-1,j    ,bid)*  &
                                         DZU(i-1,j  ,k,bid) + &
                          VVV(i-1,j-1,k)*DXU(i-1,j-1  ,bid)*  &
                                         DZU(i-1,j-1,k,bid) + &
                          VVV(i+1,j  ,k)*DXU(i+1,j    ,bid)*  &
                                         DZU(i+1,j  ,k,bid) + &
                          VVV(i+1,j-1,k)*DXU(i+1,j-1  ,bid)*  &
                                         DZU(i+1,j-1,k,bid))

         VUN(i,j) = p25* (VVV(i  ,j+1,k)*DXU(i  ,j+1  ,bid)*  &
                                         DZU(i  ,j+1,k,bid) + &
                          VVV(i  ,j  ,k)*DXU(i  ,j    ,bid)*  &
                                         DZU(i  ,j  ,k,bid))+ &
                    p125*(VVV(i-1,j+1,k)*DXU(i-1,j+1  ,bid)*  &
                                         DZU(i-1,j+1,k,bid) + &
                          VVV(i-1,j  ,k)*DXU(i-1,j    ,bid)*  &
                                         DZU(i-1,j  ,k,bid) + &
                          VVV(i+1,j+1,k)*DXU(i+1,j+1  ,bid)*  &
                                         DZU(i+1,j+1,k,bid) + &
                          VVV(i+1,j  ,k)*DXU(i+1,j    ,bid)*  &
                                         DZU(i+1,j  ,k,bid))

      end do
      end do

   else

      do j=jbeg-1,jend+1
      do i=ibeg-1,iend+1

         UUW(i,j) = p25 *(UUU(i  ,j  ,k)*DYU(i  ,j  ,bid) + &
                          UUU(i-1,j  ,k)*DYU(i-1,j  ,bid))+ &
                    p125*(UUU(i  ,j-1,k)*DYU(i  ,j-1,bid) + &
                          UUU(i-1,j-1,k)*DYU(i-1,j-1,bid) + &
                          UUU(i  ,j+1,k)*DYU(i  ,j+1,bid) + &
                          UUU(i-1,j+1,k)*DYU(i-1,j+1,bid))

         UUE(i,j) = p25 *(UUU(i+1,j  ,k)*DYU(i+1,j  ,bid) + &
                          UUU(i  ,j  ,k)*DYU(i  ,j  ,bid))+ &
                    p125*(UUU(i+1,j-1,k)*DYU(i+1,j-1,bid) + &
                          UUU(i  ,j-1,k)*DYU(i  ,j-1,bid) + &
                          UUU(i+1,j+1,k)*DYU(i+1,j+1,bid) + &
                          UUU(i  ,j+1,k)*DYU(i  ,j+1,bid))

         VUS(i,j) = p25* (VVV(i  ,j  ,k)*DXU(i  ,j  ,bid) + &
                          VVV(i  ,j-1,k)*DXU(i  ,j-1,bid))+ &
                    p125*(VVV(i-1,j  ,k)*DXU(i-1,j  ,bid) + &
                          VVV(i-1,j-1,k)*DXU(i-1,j-1,bid) + &
                          VVV(i+1,j  ,k)*DXU(i+1,j  ,bid) + &
                          VVV(i+1,j-1,k)*DXU(i+1,j-1,bid))

         VUN(i,j) = p25* (VVV(i  ,j+1,k)*DXU(i  ,j+1,bid) + &
                          VVV(i  ,j  ,k)*DXU(i  ,j  ,bid))+ &
                    p125*(VVV(i-1,j+1,k)*DXU(i-1,j+1,bid) + &
                          VVV(i-1,j  ,k)*DXU(i-1,j  ,bid) + &
                          VVV(i+1,j+1,k)*DXU(i+1,j+1,bid) + &
                          VVV(i+1,j  ,k)*DXU(i+1,j  ,bid))

      end do
      end do

   endif ! partial bottom cells


!-----------------------------------------------------------------------
!
!  calculate vertical velocity at bottom of kth level
!  (vertical velocity is nonzero at bottom of U columns
!  if topography is varying)
!
!-----------------------------------------------------------------------

   if (partial_bottom_cells) then
      WUKB = WUK + (VUN - VUS + UUE - UUW)*UAREA_R(:,:,bid)
   else
      WUKB = WUK + c2dz(k)*p5*(VUN - VUS + UUE - UUW)* &
                   UAREA_R(:,:,bid)
   endif

!#if drifter_particles
!#if drifter_particles != 2
!-----------------------------------------------------------------------
!
!     save the vertical velocity for advecting drifters
!
!-----------------------------------------------------------------------
!      WVEL(:,:,k) = WUKB
!#endif
!#endif

!-----------------------------------------------------------------------
!
!  advect momentum
!
!  horizontal advection
!
!-----------------------------------------------------------------------

   LUK = c0
   LVK = c0

   if (partial_bottom_cells) then

      do j=jbeg,jend
      do i=ibeg,iend

         cc = VUS(i,j+1) - VUS(i,j) + UUW(i+1,j) - UUW(i,j)

         LUK(i,j) = p5*(        cc*UUU(i  ,j  ,k) + &
                        VUS(i,j+1)*UUU(i  ,j+1,k) - &
                        VUS(i,j  )*UUU(i  ,j-1,k) + &
                        UUW(i+1,j)*UUU(i+1,j  ,k) - &
                        UUW(i  ,j)*UUU(i-1,j  ,k))* &
                        UAREA_R(i,j,bid)/    &
                        DZU(i,j,k,bid)

         LVK(i,j) = p5*(        cc*VVV(i  ,j  ,k) + &
                        VUS(i,j+1)*VVV(i  ,j+1,k) - &
                        VUS(i,j  )*VVV(i  ,j-1,k) + &
                        UUW(i+1,j)*VVV(i+1,j  ,k) - &
                        UUW(i  ,j)*VVV(i-1,j  ,k))* &
                        UAREA_R(i,j,bid)/    &
                        DZU(i,j,k,bid)

      end do
      end do

   else ! no partial bottom cells

      do j=jbeg,jend
      do i=ibeg,iend

         cc = VUS(i,j+1) - VUS(i,j) + UUW(i+1,j) - UUW(i,j)

         LUK(i,j) = p5*(        cc*UUU(i  ,j  ,k) + &
                        VUS(i,j+1)*UUU(i  ,j+1,k) - &
                        VUS(i,j  )*UUU(i  ,j-1,k) + &
                        UUW(i+1,j)*UUU(i+1,j  ,k) - &
                        UUW(i  ,j)*UUU(i-1,j  ,k))* &
                        UAREA_R(i,j,bid)

         LVK(i,j) = p5*(        cc*VVV(i  ,j  ,k) + &
                        VUS(i,j+1)*VVV(i  ,j+1,k) - &
                        VUS(i,j  )*VVV(i  ,j-1,k) + &
                        UUW(i+1,j)*VVV(i+1,j  ,k) - &
                        UUW(i  ,j)*VVV(i-1,j  ,k))* &
                        UAREA_R(i,j,bid)

      end do
      end do

   endif ! partial bottom cells

!-----------------------------------------------------------------------
!
!  vertical advection through top of U box
!
!-----------------------------------------------------------------------

   if (k == 1) then
      LUK = LUK + dzr(k)*WUK*UUU(:,:,k)
      LVK = LVK + dzr(k)*WUK*VVV(:,:,k)
   else
      if (partial_bottom_cells) then
         LUK = LUK + p5/DZU(:,:,k,bid)*WUK*(UUU(:,:,k-1) + &
                                                   UUU(:,:,k))
         LVK = LVK + p5/DZU(:,:,k,bid)*WUK*(VVV(:,:,k-1) + &
                                                   VVV(:,:,k))
      else
         LUK = LUK + dz2r(k)*WUK*(UUU(:,:,k-1) + UUU(:,:,k))
         LVK = LVK + dz2r(k)*WUK*(VVV(:,:,k-1) + VVV(:,:,k))
      endif
   endif

!-----------------------------------------------------------------------
!
!  vertical advection through bottom of U box
!  for k=km, UUU(k+1) is not defined, but WUKB=0
!
!-----------------------------------------------------------------------

   if (k < km) then  
      if (partial_bottom_cells) then
         LUK = LUK - p5/DZU(:,:,k,bid)*WUKB*(UUU(:,:,k) + &
                                                    UUU(:,:,k+1))
         LVK = LVK - p5/DZU(:,:,k,bid)*WUKB*(VVV(:,:,k) + &
                                                    VVV(:,:,k+1))
      else
         LUK = LUK - dz2r(k)*WUKB*(UUU(:,:,k) + UUU(:,:,k+1))
         LVK = LVK - dz2r(k)*WUKB*(VVV(:,:,k) + VVV(:,:,k+1))
      endif
   endif

!-----------------------------------------------------------------------
!
!  add metric terms, and zero fields at land points
!
!-----------------------------------------------------------------------

   where (k <= KMU(:,:,bid))
      LUK = LUK + UUU(:,:,k)*VVV(:,:,k)*KYU(:,:,bid) - &
                          VVV(:,:,k)**2*KXU(:,:,bid)
      LVK = LVK + UUU(:,:,k)*VVV(:,:,k)*KXU(:,:,bid) - &
                          UUU(:,:,k)**2*KYU(:,:,bid)
   elsewhere
      LUK = c0
      LVK = c0
   endwhere

!-----------------------------------------------------------------------
!
!  accumulate time average if necessary
!
!-----------------------------------------------------------------------

   if (ltavg_on .and. mix_pass /= 1) then

   if (tavg_requested(tavg_UEU) .or. tavg_requested(tavg_UEV)) then
      if (partial_bottom_cells) then
         FUE =  UUE*p5*UAREA_R(:,:,bid)/DZU(:,:,k,bid)
      else
         FUE =  UUE*p5*UAREA_R(:,:,bid)
      endif
   endif ! UEU or UEV

   if (tavg_requested(tavg_VNU) .or. tavg_requested(tavg_VNV)) then
      if (partial_bottom_cells) then
         FVN =  VUN*p5*UAREA_R(:,:,bid)/DZU(:,:,k,bid)
      else
         FVN =  VUN*p5*UAREA_R(:,:,bid)
      endif
   endif ! VNU or VNV

   if (tavg_requested(tavg_UEU)) then
      do j=jbeg,jend
      do i=ibeg,iend
         WORK(i,j) = FUE(i,j)*(UUU(i,j,k) + UUU(i+1,j,k))
      end do
      end do

      call accumulate_tavg_field(WORK,tavg_UEU,bid,k)
   endif

   if (tavg_requested(tavg_VNU)) then
      do j=jbeg,jend
      do i=ibeg,iend
         WORK(i,j) = FVN(i,j)*(UUU(i,j  ,k) + UUU(i,j+1,k))
      end do
      end do

      call accumulate_tavg_field(WORK,tavg_VNU,bid,k)
   endif

   if (tavg_requested(tavg_UEV)) then
      do j=jbeg,jend
      do i=ibeg,iend
         WORK(i,j) = FUE(i,j)*(VVV(i  ,j,k) + VVV(i+1,j,k))
      end do
      end do

      call accumulate_tavg_field(WORK,tavg_UEV,bid,k)
   endif

   if (tavg_requested(tavg_VNV)) then
      do j=jbeg,jend
      do i=ibeg,iend
         WORK(i,j) = FVN(i,j)*(VVV(i,j  ,k) + VVV(i,j+1,k))
      end do
      end do

      call accumulate_tavg_field(WORK,tavg_VNV,bid,k)
   endif

   if (tavg_requested(tavg_WTU)) then
      if (k == 1) then
         WORK = dzr(k)*WUK*UUU(:,:,k)
      else
         WORK = dz2r(k)*WUK*(UUU(:,:,k) + UUU(:,:,k-1))
      endif

      call accumulate_tavg_field(WORK,tavg_WTU,bid,k)
   endif

   if (tavg_requested(tavg_WTV)) then
      if (k == 1) then
         WORK = dzr(k)*WUK*VVV(:,:,k)
      else
         WORK = dz2r(k)*WUK*(VVV(:,:,k) + VVV(:,:,k-1))
      endif

      call accumulate_tavg_field(WORK,tavg_WTV,bid,k)
   endif

   endif ! ltavg_on and mix_pass ne 1

!-----------------------------------------------------------------------
!
!  reset W for next k level
!
!-----------------------------------------------------------------------

   WUK = WUKB   ! bottom value becomes top value for next pass

   call timer_stop(timer_advu, block_id=bid)

!-----------------------------------------------------------------------
!EOC

 end subroutine advu

!***********************************************************************
!BOP
! !IROUTINE: advt
! !INTERFACE:

 subroutine advt(k,LTK,WTK,TRCR,UUU,VVV,this_block)

! !DESCRIPTION:
!  Advection of tracers - this routine actually computes only
!  vertical velocities and then calls individual routines based
!  on the type of advection chosen
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
      k  ! depth level index

   real (r8), dimension(nx_block,ny_block,km,nt), intent(in) :: &
      TRCR                ! tracers for this block at current time

   real (r8), dimension(nx_block,ny_block,km), intent(in) :: &
      UUU, VVV            ! U,V for this block at current time

   type (block), intent(in) :: &
      this_block          ! block information for this block

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(inout) :: & 
      WTK            ! on  input flux velocity at top    of T box
                     ! on output flux velocity at bottom of T box

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,nt), intent(out) :: & 
      LTK             ! returned as L(T) for nth tracer at level k

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local or common variables:
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
     i,j,n,              &! dummy loop indices
     bid                  ! local block address

   real (r8), dimension(nx_block,ny_block) :: & 
     UTE,UTW,VTN,VTS,  &! tracer flux velocities across E,W,N,S faces
     FC,               &! local temp space
     FVN,FUE,          &! north and east advective stencil weights
     WTKB,             &! vertical velocity at bottom of T box
     RHOK1,            &! pot density at k relative to k=1
     RHOK1M,           &! pot density at k-1
     WORK,             &! local temp space
     WORK1,WORK2,      &! local temp space
     WORK3,WORK4

!-----------------------------------------------------------------------
!
!  advection fluxes for T box.
!
!-----------------------------------------------------------------------

   bid = this_block%local_id

   call timer_start(timer_advt, block_id=bid)

   UTE = c0
   UTW = c0
   VTN = c0
   VTS = c0

   if (partial_bottom_cells) then

      do j=this_block%jb-1,this_block%je+1
      do i=this_block%ib-1,this_block%ie+1

         UTE(i,j) = p5*(UUU(i  ,j  ,k)*DYU(i  ,j    ,bid)*  &
                                       DZU(i  ,j  ,k,bid) + &
                        UUU(i  ,j-1,k)*DYU(i  ,j-1  ,bid)*  &
                                       DZU(i  ,j-1,k,bid))
         UTW(i,j) = p5*(UUU(i-1,j  ,k)*DYU(i-1,j    ,bid)*  &
                                       DZU(i-1,j  ,k,bid) + &
                        UUU(i-1,j-1,k)*DYU(i-1,j-1  ,bid)*  &
                                       DZU(i-1,j-1,k,bid))

         VTN(i,j) = p5*(VVV(i  ,j  ,k)*DXU(i  ,j    ,bid)*  &
                                       DZU(i  ,j  ,k,bid) + &
                        VVV(i-1,j  ,k)*DXU(i-1,j    ,bid)*  &
                                       DZU(i-1,j  ,k,bid))
         VTS(i,j) = p5*(VVV(i  ,j-1,k)*DXU(i  ,j-1  ,bid)*  &
                                       DZU(i  ,j-1,k,bid) + &
                        VVV(i-1,j-1,k)*DXU(i-1,j-1  ,bid)*  &
                                       DZU(i-1,j-1,k,bid))

      end do
      end do

   else

      do j=this_block%jb-1,this_block%je+1
      do i=this_block%ib-1,this_block%ie+1

         UTE(i,j) = p5*(UUU(i  ,j  ,k)*DYU(i  ,j  ,bid) + &
                        UUU(i  ,j-1,k)*DYU(i  ,j-1,bid))
         UTW(i,j) = p5*(UUU(i-1,j  ,k)*DYU(i-1,j  ,bid) + &
                        UUU(i-1,j-1,k)*DYU(i-1,j-1,bid))

         VTN(i,j) = p5*(VVV(i  ,j  ,k)*DXU(i  ,j  ,bid) + &
                        VVV(i-1,j  ,k)*DXU(i-1,j  ,bid))
         VTS(i,j) = p5*(VVV(i  ,j-1,k)*DXU(i  ,j-1,bid) + &
                        VVV(i-1,j-1,k)*DXU(i-1,j-1,bid))

      end do
      end do

   endif ! partial bottom cells

!-----------------------------------------------------------------------
!
!  calculate vertical velocity at bottom of kth level
!  (vertical velocity is zero at bottom of T columns)
!
!-----------------------------------------------------------------------

   FC = p5*(VTN - VTS + UTE - UTW)*TAREA_R(:,:,bid)
   if (partial_bottom_cells) then
      WTKB = merge(WTK+c2*FC, c0, k < KMT(:,:,bid))
   else
      WTKB = merge(WTK+c2dz(k)*FC, c0, k < KMT(:,:,bid))
   endif

!-----------------------------------------------------------------------
!
!  advect tracers
!
!-----------------------------------------------------------------------

   select case (tadvect_itype)

   case (tadvect_upwind3)

      if (.not. luse_isopycnal) then

         call advt_upwind3(k,LTK,TRCR,WTK,WTKB,UTE,UTW,VTN,VTS, &
                             this_block)
 
      else
         !***
         !*** add isopycnal velocities to advective velocities
         !***

         
         FUE   = c0
         FVN   = c0
         WORK1 = c0
         WORK2 = c0
         WORK3 = c0

         if (partial_bottom_cells) then

            do j=this_block%jb-1,this_block%je+1
            do i=this_block%ib-1,this_block%ie+1
               FUE  (i,j) = UTE (i,j) + U_ISOP(i  ,j,bid)*HTE(i  ,j,bid) &
                                  *min(DZT(i,j,k,bid),DZT(i+1,j,k,bid))
               WORK1(i,j) = UTW (i,j) + U_ISOP(i-1,j,bid)*HTE(i-1,j,bid) &
                                  *min(DZT(i,j,k,bid),DZT(i-1,j,k,bid))
               FVN  (i,j) = VTN (i,j) + V_ISOP(i,j  ,bid)*HTN(i,j  ,bid) &
                                  *min(DZT(i,j,k,bid),DZT(i,j+1,k,bid))
               WORK2(i,j) = VTS (i,j) + V_ISOP(i,j-1,bid)*HTN(i,j-1,bid) &
                                  *min(DZT(i,j,k,bid),DZT(i,j-1,k,bid))
               WORK3(i,j) = WTKB(i,j) + WBOT_ISOP(i,j,bid)
            enddo
            enddo

         else

            do j=this_block%jb-1,this_block%je+1
            do i=this_block%ib-1,this_block%ie+1
               FUE  (i,j) = UTE (i,j) + U_ISOP(i  ,j,bid)*HTE(i  ,j,bid)
               WORK1(i,j) = UTW (i,j) + U_ISOP(i-1,j,bid)*HTE(i-1,j,bid)
               FVN  (i,j) = VTN (i,j) + V_ISOP(i,j  ,bid)*HTN(i,j  ,bid)
               WORK2(i,j) = VTS (i,j) + V_ISOP(i,j-1,bid)*HTN(i,j-1,bid)
               WORK3(i,j) = WTKB(i,j) + WBOT_ISOP(i,j,bid)
            end do
            end do
         endif

         call advt_upwind3(k,LTK,TRCR,WTK,WORK3,FUE,WORK1,FVN,WORK2, &
                             this_block)
 
      end if

   case(tadvect_centered)

      if (.not. luse_isopycnal) then

         call advt_centered(k,LTK,TRCR,WTK,WTKB,UTE,VTN,this_block)

      else

         !***
         !*** add isopycnal velocities to advective velocities
         !***

         if (partial_bottom_cells) then

            do j=this_block%jb-1,this_block%je+1
            do i=this_block%ib-1,this_block%ie+1
               FUE(i,j)  = UTE(i,j) + U_ISOP(i,j,bid)*HTE(i,j,bid) &
                                  *min(DZT(i,j,k,bid),DZT(i+1,j,k,bid))
               FVN(i,j)  = VTN(i,j) + V_ISOP(i,j,bid)*HTN(i,j,bid) & 
                                  *min(DZT(i,j,k,bid),DZT(i,j+1,k,bid))
               WORK1(i,j) = WTK (i,j) + WTOP_ISOP(i,j,bid)
               WORK2(i,j) = WTKB(i,j) + WBOT_ISOP(i,j,bid)
            enddo
            enddo

         else

            FUE   = UTE  + U_ISOP(:,:,bid)*HTE(:,:,bid)
            FVN   = VTN  + V_ISOP(:,:,bid)*HTN(:,:,bid)
            WORK1 = WTK  + WTOP_ISOP(:,:,bid)
            WORK2 = WTKB + WBOT_ISOP(:,:,bid)

         endif

         call advt_centered(k,LTK,TRCR,WORK1,WORK2,FUE,FVN,this_block)

      end if

   case default

      call exit_POP(sigAbort, &
                    'ERROR: unknown advection type for tracers')

   end select

   call timer_stop(timer_advt, block_id=bid)

!-----------------------------------------------------------------------
!
!  compute diagnostics if necessary
!
!-----------------------------------------------------------------------

   if (ltavg_on .and. mix_pass /= 1 ) then

      if (partial_bottom_cells) then
         FVN =  p5*VTN*TAREA_R(:,:,bid)/DZT(:,:,k,bid)
         FUE =  p5*UTE*TAREA_R(:,:,bid)/DZT(:,:,k,bid)
      else
         FVN =  p5*VTN*TAREA_R(:,:,bid)
         FUE =  p5*UTE*TAREA_R(:,:,bid)
      endif

      if (tavg_requested(tavg_UET)) then
         WORK = FUE*(        TRCR(:,:,k,1) + &
                     eoshift(TRCR(:,:,k,1),dim=1,shift=1))
         call accumulate_tavg_field(WORK,tavg_UET,bid,k)
      endif

      if (tavg_requested(tavg_VNT)) then
         WORK = FVN*(        TRCR(:,:,k,1) + &
                     eoshift(TRCR(:,:,k,1),dim=2,shift=1))
         call accumulate_tavg_field(WORK,tavg_VNT,bid,k)
      endif

      if (tavg_requested(tavg_UES)) then
         WORK = FUE*(        TRCR(:,:,k,2) + &
                     eoshift(TRCR(:,:,k,2),dim=1,shift=1))
         call accumulate_tavg_field(WORK,tavg_UES,bid,k)
      endif

      if (tavg_requested(tavg_VNS)) then
         WORK = FVN*(        TRCR(:,:,k,2) + &
                     eoshift(TRCR(:,:,k,2),dim=2,shift=1))
         call accumulate_tavg_field(WORK,tavg_VNS,bid,k)
      endif

      if (tavg_requested(tavg_WTT)) then
         if (k == 1) then
            WORK = dzr(k)*WTK*TRCR(:,:,k,1)
         else
            WORK = dz2r(k)*WTK*(TRCR(:,:,k  ,1)  &
                                         + TRCR(:,:,k-1,1))
         endif
         call accumulate_tavg_field(WORK,tavg_WTT,bid,k)
      endif

      if (tavg_requested(tavg_WTS)) then
         if (k == 1) then
            WORK = dzr(k)*WTK*TRCR(:,:,k,2)
         else
            WORK = dz2r(k)*WTK*(TRCR(:,:,k  ,2)  &
                                         + TRCR(:,:,k-1,2))
         endif
         call accumulate_tavg_field(WORK,tavg_WTS,bid,k)
      endif

      if (tavg_requested(tavg_ADVT)) then
         where (k <= KMT(:,:,bid))
            WORK = -dz(k)*LTK(:,:,1)
         elsewhere
            WORK = c0
         end where
         call accumulate_tavg_field(WORK,tavg_ADVT,bid,k)
      endif

      !***
      !*** potential density referenced to k=1 needed for a variety of 
      !*** tavg fields
      !***

      if (tavg_requested(tavg_PD) .or.  &
          tavg_requested(tavg_RHOU) .or.  &
          tavg_requested(tavg_RHOV) .or.  &
          tavg_requested(tavg_URHO) .or.  &
          tavg_requested(tavg_VRHO) .or.  &
          tavg_requested(tavg_WRHO)) then

         call state(k,1,TRCR(:,:,k,1),                         &
                        TRCR(:,:,k,2), this_block, &
                        RHOFULL=RHOK1)

         if (k == 1) then
            RHOK1M = RHOK1
         else
            call state(k-1,1,TRCR(:,:,k,1),                         &
                             TRCR(:,:,k,2), this_block, &
                             RHOFULL=RHOK1M)
         endif
      endif

      if (tavg_requested(tavg_PD)) then
         call accumulate_tavg_field(RHOK1,tavg_PD,bid,k)
      endif

      if (tavg_requested(tavg_URHO)) then
         WORK = FUE*(RHOK1 + eoshift(RHOK1,dim=1,shift=1))
         call accumulate_tavg_field(WORK,tavg_URHO,bid,k)
      endif

      if (tavg_requested(tavg_VRHO)) then
         WORK = FVN*(RHOK1 + eoshift(RHOK1,dim=2,shift=1))
         call accumulate_tavg_field(WORK,tavg_VRHO,bid,k)
      endif

      if (tavg_requested(tavg_WRHO)) then
         WORK = dz2r(k)*WTK*(RHOK1 + RHOK1M)
         call accumulate_tavg_field(WORK,tavg_WRHO,bid,k)
      endif

      if (tavg_requested(tavg_RHOU)) then
         call ugrid_to_tgrid(WORK1,UUU(:,:,k),bid)
         WORK = RHOK1*WORK1
         call accumulate_tavg_field(WORK,tavg_RHOU,bid,k)
      endif

      if (tavg_requested(tavg_RHOV)) then
         call ugrid_to_tgrid(WORK1,VVV(:,:,k),bid)
         WORK = RHOK1*WORK1
         call accumulate_tavg_field(WORK,tavg_RHOV,bid,k)
      endif

      !***
      !*** vertical density gradient and potential vorticity
      !***

      if (tavg_requested(tavg_Q)    .or.  &
          tavg_requested(tavg_PV)   .or.  &
          tavg_requested(tavg_PVWM) .or.  &
          tavg_requested(tavg_PVWP) .or.  &
          tavg_requested(tavg_UPV)  .or.  &
          tavg_requested(tavg_VPV)  .or.  &
          tavg_requested(tavg_UQ)   .or.  &
          tavg_requested(tavg_VQ) ) then

         call state(k,k,TRCR(:,:,k,1)  &
                       ,TRCR(:,:,k,2), this_block, &
                        RHOOUT=WORK)

         if (k == 1 ) then
            WORK3 = WORK
         else
            call state(k-1,k,TRCR(:,:,k-1,1)  &
                            ,TRCR(:,:,k-1,2), this_block, &
                             RHOOUT=WORK3)
            WORK3 = p5*(WORK3 + WORK)
         endif

         if (k == km) then
            WORK4 = WORK
         else
            call state(k+1,k,TRCR(:,:,k+1,1)  &
                            ,TRCR(:,:,k+1,2), this_block, &
                             RHOOUT=WORK4)

            where (k /= KMT(:,:,bid))
               WORK4 = p5*(WORK4 + WORK)
            elsewhere 
               WORK4 = WORK
            end where
         endif

         WORK1 = merge((WORK3 - WORK4)*dzr(k), &
                       c0,k <= KMT(:,:,bid))  ! drho/dz 

         call zcurl(k,WORK3,UUU(:,:,k),VVV(:,:,k),this_block)
         WORK2 = WORK1*(WORK3*TAREA_R(:,:,bid) + &
                        FCORT(:,:,bid)) ! PV = pot vorticity
      endif

      if (tavg_requested(tavg_Q)) then
         call accumulate_tavg_field(WORK1,tavg_Q,bid,k)
      endif

      if (tavg_requested(tavg_PV)) then
         call accumulate_tavg_field(WORK2,tavg_PV,bid,k)
      endif

      !***
      !*** advection of potential vorticity, Q
      !***

      if (tavg_requested(tavg_PVWM)) then
         call accumulate_tavg_field(WORK2*WTK,tavg_PVWM,bid,k)
      endif

      if (tavg_requested(tavg_PVWP)) then
         call accumulate_tavg_field(WORK2*WTKB,tavg_PVWP,bid,k)
      endif

      if (tavg_requested(tavg_UPV)) then
         WORK = FUE*(WORK2 + eoshift(WORK2,dim=1,shift=1))
         call accumulate_tavg_field(WORK,tavg_UPV,bid,k)
      endif

      if (tavg_requested(tavg_VPV)) then
         WORK = FVN*(WORK2 + eoshift(WORK2,dim=2,shift=1))
         call accumulate_tavg_field(WORK,tavg_VPV,bid,k)
      endif

      if (tavg_requested(tavg_UQ)) then
         WORK = FUE*(WORK1 + eoshift(WORK1,dim=1,shift=1))
         call accumulate_tavg_field(WORK,tavg_UQ,bid,k)
      endif

      if (tavg_requested(tavg_VQ)) then
         WORK = FVN*(WORK1 + eoshift(WORK1,dim=2,shift=1))
         call accumulate_tavg_field(WORK,tavg_VQ,bid,k)
      endif

   endif ! tavg diagnostics

   call cfl_advect(k,bid,UTE,UTW,VTN,VTS,WTK,WTKB,this_block)

!-----------------------------------------------------------------------
!
!  set bottom vertical velocity to top for next k-level
!
!-----------------------------------------------------------------------

   WTK = WTKB

!-----------------------------------------------------------------------
!EOC

 end subroutine advt

!***********************************************************************
!BOP
! !IROUTINE: advt_centered
! !INTERFACE:

 subroutine advt_centered(k,LTK,TRCR,WTK,WTKB,UTE,VTN,this_block)

! !DESCRIPTION:
!  This routine computes the tracer advection tendencies using
!  a centered differencing for leapfrog advection given by:
!  \begin{equation}
!     L_T(\phi) = {1\over{\Delta_y}}
!                  \delta_x\left[
!                   \overline{\left(\overline{\Delta_y u_x}^y\right)}^{xy}
!                   \overline{\phi}^x\right] + 
!                  {1\over{\Delta_x}}
!                  \delta_y\left[
!                   \overline{\left(\overline{\Delta_x u_y}^x\right)}^{xy}
!                   \overline{\phi}^y\right] 
!             + \delta_z(w\overline{\phi}^z),
!  \end{equation}
!  where $\phi$ is the tracer concentration.
!
!  Comments:
!   \begin{itemize}
!     \item this routine must be called successively with k = 1,2,3,...
!
!     \item the vertical velocity $w$ in T columns is determined by 
!     integrating the continuity equation $L(1) = 0$ from the surface
!     down to level k.  In the rigid-lid formulation, the integration
!     starts with $w = 0$ at the surface.  In the free-surface 
!     formulation, the integration starts with 
!     $w = \partial\eta/\partial t$ 
!     ($\partial\eta/\partial t$ is the time change of the surface 
!     height, and satisfies the barotropic continuity equation 
!     $\partial\eta/\partial t + \nabla\cdot H {\rm\bf U}=q_w$
!     where ${\rm\bf U}$ is the barotropic velocity and $q_w$
!     is the surface fresh water flux.)
!   \end{itemize}
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: k  ! depth level index

   real (r8), dimension(nx_block,ny_block,km,nt), intent(in) :: &
      TRCR                ! tracers for this block at current time

   real (r8), dimension(nx_block,ny_block), intent(in) :: & 
      UTE,VTN,         &! tracer flux velocities across E,N faces
      WTK,             &! vert velocity at top    of level k T box
      WTKB              ! vert velocity at bottom of level k T box

   type (block), intent(in) :: &
      this_block          ! block information for this block

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,nt), intent(out) :: &
      LTK                 ! returned as L(T) for nth tracer at level k

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables:
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j,n,             &! tracer loop index
      bid                 ! local block index

!-----------------------------------------------------------------------
!
!  advect tracers
!  horizontal advection
!
!-----------------------------------------------------------------------

   bid = this_block%local_id
   LTK = c0

!-----------------------------------------------------------------------

   if (partial_bottom_cells) then 

      do n=1,nt
      do j=this_block%jb,this_block%je
      do i=this_block%ib,this_block%ie
         LTK(i,j,n) = p5*((VTN(i,j)-VTN(i,j-1)+UTE(i,j)-UTE(i-1,j))  &
                                      *TRCR(i  ,j  ,k,n) +           &
                          VTN(i  ,j  )*TRCR(i  ,j+1,k,n) -           &
                          VTN(i  ,j-1)*TRCR(i  ,j-1,k,n) +           &
                          UTE(i  ,j  )*TRCR(i+1,j  ,k,n) -           &
                          UTE(i-1,j  )*TRCR(i-1,j  ,k,n))*           &
                          TAREA_R(i,j,bid)/                   &
                          DZT(i,j,k,bid)
      end do
      end do
      end do

   else ! no partial bottom cells

      do n=1,nt
      do j=this_block%jb,this_block%je
      do i=this_block%ib,this_block%ie
         LTK(i,j,n) = p5*((VTN(i,j)-VTN(i,j-1)+UTE(i,j)-UTE(i-1,j))  &
                                      *TRCR(i  ,j  ,k,n) +           &
                          VTN(i  ,j  )*TRCR(i  ,j+1,k,n) -           &
                          VTN(i  ,j-1)*TRCR(i  ,j-1,k,n) +           &
                          UTE(i  ,j  )*TRCR(i+1,j  ,k,n) -           &
                          UTE(i-1,j  )*TRCR(i-1,j  ,k,n))*           &
                          TAREA_R(i,j,bid)
      end do
      end do
      end do

   endif ! partial bottom cells

!-----------------------------------------------------------------------
!
!  vertical advection through top and bottom of T box
!
!-----------------------------------------------------------------------

   do n = 1,nt

      !*** no advection thru surface (k=1) in vs model

      if (k == 1) then
         if (sfc_layer_type /= sfc_layer_varthick) then
            LTK(:,:,n) = LTK(:,:,n) + &
                         dzr(k)*WTK*TRCR(:,:,k,n)
         endif
      else
         if (partial_bottom_cells) then
            LTK(:,:,n) = LTK(:,:,n) + p5/DZT(:,:,k,bid)*WTK*  &
                         (TRCR(:,:,k-1,n) + TRCR(:,:,k  ,n))
         else
            LTK(:,:,n) = LTK(:,:,n) + dz2r(k)*WTK*  &
                         (TRCR(:,:,k-1,n) + TRCR(:,:,k  ,n))
         endif
      endif

      !***
      !*** for k=km, TRACER(k+1) is not defined, but WTKB=0
      !***

      if (k < km) then
         if (partial_bottom_cells) then
            LTK(:,:,n) = LTK(:,:,n) - p5/DZT(:,:,k,bid)*WTKB* &
                         (TRCR(:,:,k,n) + TRCR(:,:,k+1,n))
         else
            LTK(:,:,n) = LTK(:,:,n) - dz2r(k)*WTKB* &
                         (TRCR(:,:,k,n) + TRCR(:,:,k+1,n))
         endif
      endif

   enddo

!-----------------------------------------------------------------------
!EOC

 end subroutine advt_centered

!***********************************************************************
!BOP
! !IROUTINE: advt_upwind3
! !INTERFACE:

 subroutine advt_upwind3(k,LTK,TRCR,WTK,WTKB,UTE,UTW,VTN,VTS,this_block)

! !DESCRIPTION:
!  This routine computes the advection of tracers using a 3rd-order 
!  upwinding defined by the QUICKEST scheme in
!  Leonard, B.P. 1979, {\em Comp. Meth. Applied Math. and Engineering},
!        {\bf 19}, 59-98.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: k  ! depth level index

   real (r8), dimension(nx_block,ny_block,km,nt), intent(in) :: &
      TRCR                ! tracers for this block at current time

   real (r8), dimension(nx_block,ny_block), intent(in) :: &
      UTE,UTW,VTN,VTS, &! tracer flux velocities across E,W,N,S faces
      WTK,             &! vert velocity at top    of level k T box
      WTKB              ! vert velocity at bottom of level k T box

   type (block), intent(in) :: &
      this_block          ! block information for this block

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,nt), intent(out) :: &
      LTK                ! returned as L(T) for nth tracer at level k

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables:
!
!-----------------------------------------------------------------------

   integer (int_kind) :: & 
      n,                 &! tracer loop index
      bid                 ! local block address for this block

   real (r8), dimension(nx_block,ny_block) :: &
      FVN,FVS,FUE,FUW,                        &! flux velocities
      TPLUS,TMINUS,AZMINUS,DZMINUS

   real (r8), dimension(nx_block,ny_block,nt) :: &
      AUXB

!-----------------------------------------------------------------------
!
!  advect tracers
!  horizontal advection
!
!-----------------------------------------------------------------------

   bid = this_block%local_id

   if (partial_bottom_cells) then
      FVN =  p5*VTN*TAREA_R(:,:,bid)/DZT(:,:,k,bid)
      FVS = -p5*VTS*TAREA_R(:,:,bid)/DZT(:,:,k,bid) ! note sign
      FUE =  p5*UTE*TAREA_R(:,:,bid)/DZT(:,:,k,bid)
      FUW = -p5*UTW*TAREA_R(:,:,bid)/DZT(:,:,k,bid) ! note sign
   else
      FVN =  p5*VTN*TAREA_R(:,:,bid)
      FVS = -p5*VTS*TAREA_R(:,:,bid)  ! note sign change
      FUE =  p5*UTE*TAREA_R(:,:,bid)
      FUW = -p5*UTW*TAREA_R(:,:,bid)  ! note sign change
   endif

   call hupw3(k,LTK,TRCR,FVN,FVS,FUE,FUW, &
                    bid, this_block)

!-----------------------------------------------------------------------
!
!  vertical advection through top and bottom of T box
!
!-----------------------------------------------------------------------

   where ( k < KMT(:,:,bid)-1 )
      AZMINUS = talfzm(k)
      DZMINUS = tdelzm(k)
   elsewhere
      AZMINUS = talfzm(k) + tdelzm(k)
      DZMINUS = c0
   endwhere

   do n = 1,nt

      if ( k < km-1 .and. k > 1) then
         TPLUS  = talfzp(k)*TRCR(:,:,k+1,n)  & 
                + tbetzp(k)*TRCR(:,:,k  ,n)  &
                + tgamzp(k)*TRCR(:,:,k-1,n)
         TMINUS = AZMINUS  *TRCR(:,:,k+1,n)  &
                + tbetzm(k)*TRCR(:,:,k  ,n)  &
                + DZMINUS  *TRCR(:,:,k+2,n)
         AUXB(:,:,n) = (WTKB-abs(WTKB))*TPLUS + &
                     (WTKB+abs(WTKB))*TMINUS
      else if ( k == 1) then
         TPLUS  = talfzp(k)*TRCR(:,:,k+1,n)  &
                + tbetzp(k)*TRCR(:,:,k  ,n)
         TMINUS = AZMINUS  *TRCR(:,:,k+1,n)  &
                + tbetzm(k)*TRCR(:,:,k  ,n)  &
                + DZMINUS  *TRCR(:,:,k+2,n)
         AUXB(:,:,n) = (WTKB-abs(WTKB))*TPLUS + &
                       (WTKB+abs(WTKB))*TMINUS
      else if ( k == km-1) then
         TPLUS  = talfzp(k)*TRCR(:,:,k+1,n)  &
                + tbetzp(k)*TRCR(:,:,k  ,n)  &
                + tgamzp(k)*TRCR(:,:,k-1,n)
         TMINUS = AZMINUS  *TRCR(:,:,k+1,n)  &
                + tbetzm(k)*TRCR(:,:,k  ,n)
         AUXB(:,:,n) = (WTKB-abs(WTKB))*TPLUS + &
                       (WTKB+abs(WTKB))*TMINUS
      else
         AUXB(:,:,n) = c0
      endif

      !*** no advection thru surface (k=1) in vs model

      if (k == 1) then
         if (sfc_layer_type /= sfc_layer_varthick) then
            LTK(:,:,n) = LTK(:,:,n)                            &
                       + dzr(k)*WTK*TRCR(:,:,k,n)   &
                       - dz2r(k)*AUXB(:,:,n)
         else
            LTK(:,:,n) = LTK(:,:,n)                            &
                       - dz2r(k)*AUXB(:,:,n)
         endif
      else
         if (partial_bottom_cells) then
            LTK(:,:,n) = LTK(:,:,n) + p5/DZT(:,:,k,bid)* &
                         (AUX(:,:,n,bid) - AUXB(:,:,n))
         else
            LTK(:,:,n) = LTK(:,:,n) + dz2r(k)* &
                         (AUX(:,:,n,bid) - AUXB(:,:,n))
         endif

      endif

      AUX(:,:,n,bid) = AUXB(:,:,n) ! set bottom to top for next k level

   enddo

!-----------------------------------------------------------------------
!EOC

 end subroutine advt_upwind3

!***********************************************************************
!BOP
! !IROUTINE: hupw3
! !INTERFACE:

 subroutine hupw3(k,XOUT,X,CN,CS,CE,CW,bid,this_block)

! !DESCRIPTION:
!  This routine is the horizontal stencil operator for 3rd-order upwind
!  advection.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) ::  &
      k,                 &! vertical level index
      bid                 ! local block address for this block

   real (r8), dimension(nx_block,ny_block,km,nt), intent(in) :: & 
      X                   ! input tracer array

   real (r8), dimension(nx_block,ny_block), intent(in) :: & 
      CN,CS,CE,CW           ! stencil weights based on flux velocities

   type (block), intent(in) :: &
      this_block          ! block information for this block

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,nt) :: &
      XOUT                ! output tracer advection

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j,n               ! dummy loop indices

   real (r8) ::                              &
      tplus, tminus, tplusm1, tminusm1,      &
      am, bm, dm, ap, bp, gp, work, work1,   &
      amm1, bmm1, dmm1, apm1, bpm1, gpm1

!-----------------------------------------------------------------------
!
!  3rd order upwinding stencil
!
!-----------------------------------------------------------------------

   do j=this_block%jb,this_block%je
   do i=this_block%ib,this_block%ie

      if (k <= KMTE(i,j,bid)) then
         work  = TBETXP(i,j,bid)
         ap    = TALFXP(i,j,bid)
      else
         work  = TBETXP(i,j,bid) + TALFXP(i,j,bid)
         ap    = c0
      endif
      if (k <= KMTE(i-1,j,bid)) then
         work1 = TBETXP(i-1,j,bid)
         apm1  = TALFXP(i-1,j,bid)
      else
         work1 = TBETXP(i-1,j,bid) + TALFXP(i-1,j,bid)
         apm1  = c0
      endif

      if (k <= KMTW(i,j,bid)) then
         bp   = work
         gp   = TGAMXP(i,j,bid)
      else
         bp   = work + TGAMXP(i,j,bid)
         gp   = c0
      endif
      if (k <= KMTW(i-1,j,bid)) then
         bpm1 = work1
         gpm1 = TGAMXP(i-1,j,bid)
      else
         bpm1 = work1 + TGAMXP(i-1,j,bid)
         gpm1 = c0
      endif

      if (k <= KMTEE(i,j,bid)) then
         am   = TALFXM(i,j,bid)
         dm   = TDELXM(i,j,bid)
      else
         am   = TALFXM(i,j,bid) + TDELXM(i,j,bid)
         dm   = c0
      endif
      if (k <= KMTEE(i-1,j,bid)) then
         amm1 = TALFXM(i-1,j,bid)
         dmm1 = TDELXM(i-1,j,bid)
      else
         amm1 = TALFXM(i-1,j,bid) + TDELXM(i-1,j,bid)
         dmm1 = c0
      endif
      bm   = TBETXM(i,j,bid)
      bmm1 = TBETXM(i-1,j,bid)

      do n=1,nt
         tplus    = ap  *X(i+1,j,k,n) + bp  *X(i,j,k,n) +   & 
                    gp  *X(i-1,j,k,n)
         tplusm1  = apm1*X(i  ,j,k,n) + bpm1*X(i-1,j,k,n) + &
                    gpm1*X(i-2,j,k,n)

         tminus   = am  *X(i+1,j,k,n) + bm  *X(i,j,k,n) +   &
                    dm  *X(i+2,j,k,n)
         tminusm1 = amm1*X(i  ,j,k,n) + bmm1*X(i-1,j,k,n) + &
                    dmm1*X(i+1,j,k,n)

         XOUT(i,j,n) = (CE(i,j)+abs(CE(i,j)))*tplus    +    &
                       (CE(i,j)-abs(CE(i,j)))*tminus   +    &
                       (CW(i,j)+abs(CW(i,j)))*tminusm1 +    &
                       (CW(i,j)-abs(CW(i,j)))*tplusm1
      end do
   end do
   end do


   do j=this_block%jb,this_block%je
   do i=this_block%ib,this_block%ie

      if (k <= KMTN(i,j,bid)) then
         work  = TBETYP(i,j,bid)
         ap    = TALFYP(i,j,bid)
      else
         work  = TBETYP(i,j,bid) + TALFYP(i,j,bid)
         ap    = c0
      endif
      if (k <= KMTN(i,j-1,bid)) then
         work1 = TBETYP(i,j-1,bid)
         apm1  = TALFYP(i,j-1,bid)
      else
         work1 = TBETYP(i,j-1,bid) + TALFYP(i,j-1,bid)
         apm1  = c0
      endif

      if (k <= KMTS(i,j,bid)) then
         bp   = work
         gp   = TGAMYP(i,j,bid)
      else
         bp   = work + TGAMYP(i,j,bid)
         gp   = c0
      endif
      if (k <= KMTS(i,j-1,bid)) then
         bpm1 = work1
         gpm1 = TGAMYP(i,j-1,bid)
      else
         bpm1 = work1 + TGAMYP(i,j-1,bid)
         gpm1 = c0
      endif

      if (k <= KMTNN(i,j,bid)) then
         am   = TALFYM(i,j,bid)
         dm   = TDELYM(i,j,bid)
      else
         am   = TALFYM(i,j,bid) + TDELYM(i,j,bid)
         dm   = c0
      endif
      if (k <= KMTNN(i,j-1,bid)) then
         amm1 = TALFYM(i,j-1,bid)
         dmm1 = TDELYM(i,j-1,bid)
      else
         amm1 = TALFYM(i,j-1,bid) + TDELYM(i,j-1,bid)
         dmm1 = c0
      endif
      bm   = TBETYM(i,j,bid)
      bmm1 = TBETYM(i,j-1,bid)

      do n=1,nt
         tplus    = ap  *X(i,j+1,k,n) + bp  *X(i,j,k,n) +     & 
                    gp  *X(i,j-1,k,n)
         tplusm1  = apm1*X(i,j  ,k,n) + bpm1*X(i,j-1,k,n) +   &
                    gpm1*X(i,j-2,k,n)

         tminus   = am  *X(i,j+1,k,n) + bm  *X(i,j,k,n) +     &
                    dm  *X(i,j+2,k,n)
         tminusm1 = amm1*X(i,j  ,k,n) + bmm1*X(i,j-1,k,n) +   &
                    dmm1*X(i,j+1,k,n)

         XOUT(i,j,n) = XOUT(i,j,n) +                          &
                     (CN(i,j)+abs(CN(i,j)))*tplus    +        &
                     (CN(i,j)-abs(CN(i,j)))*tminus   +        &
                     (CS(i,j)+abs(CS(i,j)))*tminusm1 +        &
                     (CS(i,j)-abs(CS(i,j)))*tplusm1
      end do
   end do
   end do

!-----------------------------------------------------------------------
!EOC

 end subroutine hupw3 

!***********************************************************************

 end module advection

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
