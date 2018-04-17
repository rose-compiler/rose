!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module hmix_del2

!BOP
! !MODULE: hmix_del2

! !DESCRIPTION:
!  This module contains routines for computing Laplacian horizontal
!  diffusion of momentum and tracers.
!
! !REVISION HISTORY:
!  CVS:$Id: hmix_del2.F90,v 1.20 2003/02/24 20:43:04 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod
   use blocks
   use communicate
   use distribution
   use domain_size
   use domain
   use broadcast
   use boundary
   use constants
   use topostress
   use diagnostics
   use io
   use global_reductions
   use grid
   use exit_mod

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_del2u,  &
             init_del2t,  &
             hdiffu_del2, &
             hdifft_del2

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  private module variables
!
!  operator coefficients:
!
!  DT{N,S,E,W} = {N,S,E,W} coefficients of 5-point stencil for the
!                Del**2 operator before b.c.s have been applied
!  DU{C,N,S,E,W} = central and {N,S,E,W} coefficients of 5-point
!                  stencil for the Del**2 operator acting at U points
!                  (without metric terms that mix U,V)
!  DM{C,N,S,E,W} = central and {N,S,E,W} coefficients of 5-point
!                  stencil for the metric terms that mix U,V
!  DUM = central coefficient for metric terms that do not mix U,V
!
!-----------------------------------------------------------------------

   real (r8), dimension (:,:,:), allocatable :: &
      DTN,DTS,DTE,DTW,                          &
      DUC,DUN,DUS,DUE,DUW,                      &
      DMC,DMN,DMS,DME,DMW,                      &
      DUM,                                      &
      AHF,              &! variable mixing factor for tracer   mixing
      AMF                ! variable mixing factor for momentum mixing

   real (r8) ::        &
      ah,              &! horizontal tracer   mixing coefficient
      am                ! horizontal momentum mixing coefficient

   logical (log_kind) :: &
      lauto_hmixu,       &! automatically computing mixing coeffs
      lauto_hmixt,       &! automatically computing mixing coeffs
      lvariable_hmixu,   &! spatially varying mixing coeffs
      lvariable_hmixt     ! spatially varying mixing coeffs

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_del2u
! !INTERFACE:

 subroutine init_del2u

! !DESCRIPTION:
!  This routine calculates the coefficients of the 5-point stencils for
!  the $\nabla^2$ operator acting on momentum fields and also
!  calculates coefficients for all diffusive metric terms. See the
!  description under hdiffu for the form of the operator.
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

   integer (int_kind) ::    &
      i,j,                  &! dummy loop indices
      iblock,               &! block index
      nml_error              ! error flag for namelist

   real (r8), dimension (:,:), allocatable :: &
      KXU,KYU,              &! metric factors
      DXKX,DYKY,DXKY,DYKX,  &! d{x,y}k{x,y}
      WORK1,WORK2            ! temporary work space

   logical (log_kind) ::    &
      lauto_hmix,           &! flag to internally compute mixing coeff
      lvariable_hmix         ! flag to enable spatially varying mixing

   namelist /hmix_del2u_nml/ lauto_hmix, lvariable_hmix, am

   real (r8) ::             &
      amfmin, amfmax         ! min max mixing for varible mixing

!-----------------------------------------------------------------------
!
!  read input namelist to set options
!
!-----------------------------------------------------------------------

   lauto_hmix = .false.
   lvariable_hmix = .false.
   am = c0

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=hmix_del2u_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading hmix_del2u_nml')
   endif

   if (my_task == master_task) then

      write(stdout,blank_fmt)
      write(stdout,'(a33)') 'Laplacian momentum mixing options'
      write(stdout,blank_fmt)

      lauto_hmixu = lauto_hmix

      if (.not. lauto_hmixu) then
         write(stdout,'(a33)') 'Using input horizontal viscosity:'
         write(stdout,'(a7,2x,1pe12.5)') '  am =',am
      endif

      lvariable_hmixu = lvariable_hmix

      if (.not. lvariable_hmixu) then
         write(stdout,'(a44)') &
                  'Variable horizontal momentum mixing disabled'
      endif

   endif

   call broadcast_scalar(lauto_hmixu,     master_task)
   call broadcast_scalar(lvariable_hmixu, master_task)
   call broadcast_scalar(am,              master_task)

!-----------------------------------------------------------------------
!
!  automatically set horizontal mixing coefficients if requested
!
!-----------------------------------------------------------------------

   if (lauto_hmixu) then

      ! scale to 1e7 at 1/2 degree
      am = 1.0e7_r8*(720.0_r8/float(nx_global))
      if (my_task == master_task) then
         write(stdout,'(a44)') &
              'Horizontal viscosity computed automatically:'
         write(stdout,'(a7,2x,1pe12.5)') '  am =',am
      endif

   endif

!-----------------------------------------------------------------------
!
!  set spatially variable mixing arrays if requested
!
!  this applies only to momentum or tracer mixing
!  with del2 or del4 options
!
!  functions describing spatial dependence of horizontal mixing
!  coefficients:   am -> am*AHM
!
!  for standard laplacian  mixing they scale like (cell area)**0.5
!  (note: these forms assume a global mesh with a grid line along
!  the equator, and the mixing coefficients are the set relative
!  to the average grid spacing at the equator - for cells of
!  this size AMF = AHF = 1.0).
!
!-----------------------------------------------------------------------

   if (lvariable_hmixu) then

      amfmin = c1
      amfmax = c1

      allocate(AMF(nx_block,ny_block,nblocks_clinic))

      do iblock=1,nblocks_clinic
         AMF(:,:,iblock) = sqrt(UAREA(:,:,iblock)/      &
                           (c2*pi*radius/nx_global)**2)
      end do

      !RSx3 *** set variable viscosity for x3-prime run
      !AMF = c4*am*(DXUR**2+DYUR**2)*dtu
      !where(AMF.gt.p5)
      !  AMF = p5/AMF
      !elsewhere
      !  AMF = c1
      !endwhere
      !RSx3

      amfmin = global_minval(AMF, distrb_clinic, field_loc_NEcorner, CALCU)
      amfmax = global_maxval(AMF, distrb_clinic, field_loc_NEcorner, CALCU)

      if (my_task == master_task) then
         write(stdout,'(a37)') 'Variable horizontal viscosity enabled'
         write(stdout,'(a12,1x,1pe12.5,3x,a9,1x,1pe12.5)') &
              '  Min AMF =',amfmin,'Max AMF =',amfmax
      endif

      call update_ghost_cells(AMF, bndy_clinic, field_loc_NEcorner,&
                                                field_type_scalar)

   else

      !*** allocate AMF temporarily to simplify setup

      allocate(AMF(nx_block,ny_block,nblocks_clinic))
      AMF = c1

   endif

!-----------------------------------------------------------------------
!
!  calculate operator weights
!
!-----------------------------------------------------------------------

   allocate(DUC(nx_block,ny_block,nblocks_clinic),  &
            DUN(nx_block,ny_block,nblocks_clinic),  &
            DUS(nx_block,ny_block,nblocks_clinic),  &
            DUE(nx_block,ny_block,nblocks_clinic),  &
            DUW(nx_block,ny_block,nblocks_clinic),  &
            DMC(nx_block,ny_block,nblocks_clinic),  &
            DMN(nx_block,ny_block,nblocks_clinic),  &
            DMS(nx_block,ny_block,nblocks_clinic),  &
            DME(nx_block,ny_block,nblocks_clinic),  &
            DMW(nx_block,ny_block,nblocks_clinic),  &
            DUM(nx_block,ny_block,nblocks_clinic))

   allocate(KXU   (nx_block,ny_block),  &
            KYU   (nx_block,ny_block),  &
            DXKX  (nx_block,ny_block),  &
            DYKY  (nx_block,ny_block),  &
            DXKY  (nx_block,ny_block),  &
            DYKX  (nx_block,ny_block),  &
            WORK1 (nx_block,ny_block),  &
            WORK2 (nx_block,ny_block))

   do iblock=1,nblocks_clinic

!-----------------------------------------------------------------------
!
!     calculate central and {N,S,E,W} coefficients for
!     Del**2 (without metric terms) acting on momentum.
!
!-----------------------------------------------------------------------

      WORK1 = (HUS(:,:,iblock)/HTE(:,:,iblock))*p5*(AMF(:,:,iblock) + &
                            eoshift(AMF(:,:,iblock),dim=2,shift=-1))

      DUS(:,:,iblock) = WORK1*UAREA_R(:,:,iblock)
      DUN(:,:,iblock) = eoshift(WORK1,dim=2,shift=1)*UAREA_R(:,:,iblock)

      WORK1 = (HUW(:,:,iblock)/HTN(:,:,iblock))*p5*(AMF(:,:,iblock) + &
                            eoshift(AMF(:,:,iblock),dim=1,shift=-1))

      DUW(:,:,iblock) = WORK1*UAREA_R(:,:,iblock)
      DUE(:,:,iblock) = eoshift(WORK1,dim=1,shift=1)*UAREA_R(:,:,iblock)

!-----------------------------------------------------------------------
!
!     coefficients for metric terms in Del**2(U)
!     and for metric advection terms (KXU,KYU)
!
!-----------------------------------------------------------------------

      KXU = (eoshift(HUW(:,:,iblock),dim=1,shift=1) - HUW(:,:,iblock))*&
            UAREA_R(:,:,iblock)
      KYU = (eoshift(HUS(:,:,iblock),dim=2,shift=1) - HUS(:,:,iblock))*&
            UAREA_R(:,:,iblock)

      WORK1 = (HTE(:,:,iblock) -                         &
               eoshift(HTE(:,:,iblock),dim=1,shift=-1))* &
              TAREA_R(:,:,iblock)  ! KXT

      WORK2 = p5*(WORK1 + eoshift(WORK1,dim=2,shift=1))*    &
              p5*(eoshift(AMF(:,:,iblock),dim=1,shift=-1) + &
                  AMF(:,:,iblock))

      DXKX = (eoshift(WORK2,dim=1,shift=1) - WORK2)*DXUR(:,:,iblock)

      WORK2 = p5*(WORK1 + eoshift(WORK1,dim=1,shift=1))*    &
              p5*(eoshift(AMF(:,:,iblock),dim=2,shift=-1) + &
                  AMF(:,:,iblock))

      DYKX = (eoshift(WORK2,dim=2,shift=1) - WORK2)*DYUR(:,:,iblock)

      WORK1 = (HTN(:,:,iblock) -                         &
               eoshift(HTN(:,:,iblock),dim=2,shift=-1))* &
              TAREA_R(:,:,iblock)  ! KYT

      WORK2 = p5*(WORK1 + eoshift(WORK1,dim=1,shift=1))*    &
              p5*(eoshift(AMF(:,:,iblock),dim=2,shift=-1) + &
                  AMF(:,:,iblock))

      DYKY = (eoshift(WORK2,dim=2,shift=1) - WORK2)*DYUR(:,:,iblock)

      WORK2 = p5*(WORK1 + eoshift(WORK1,dim=2,shift=1))*    &
              p5*(eoshift(AMF(:,:,iblock),dim=1,shift=-1) + &
                  AMF(:,:,iblock))

      DXKY = (eoshift(WORK2,dim=1,shift=1) - WORK2)*DXUR(:,:,iblock)

      DUM(:,:,iblock) = -(DXKX + DYKY + &
                        c2*AMF(:,:,iblock)*(KXU**2 + KYU**2))
      DMC(:,:,iblock) = DXKY - DYKX

!-----------------------------------------------------------------------
!
!     calculate central and {N,S,E,W} coefficients for
!     metric mixing terms which mix U,V.
!
!-----------------------------------------------------------------------

      WORK1 = (eoshift(AMF(:,:,iblock),dim=2,shift= 1) -    &
               eoshift(AMF(:,:,iblock),dim=2,shift=-1))/    &
              (HTE(:,:,iblock) + eoshift(HTE(:,:,iblock),dim=2,shift=1))

      DME(:,:,iblock) =  (c2*AMF(:,:,iblock)*KYU + WORK1)/  &
                         (HTN(:,:,iblock) +                 &
                          eoshift(HTN(:,:,iblock),dim=1,shift=1))

      WORK1 = (eoshift(AMF(:,:,iblock),dim=1,shift= 1) -    &
               eoshift(AMF(:,:,iblock),dim=1,shift=-1))/    &
              (HTN(:,:,iblock) + eoshift(HTN(:,:,iblock),dim=1,shift=1))

      DMN(:,:,iblock) = -(c2*AMF(:,:,iblock)*KXU + WORK1)/  &
                         (HTE(:,:,iblock) +                 &
                          eoshift(HTE(:,:,iblock),dim=2,shift=1))

   end do

   !*** these operator coefficients only needed in physical
   !*** domain and should be valid there assuming grid quantities
   !*** and AMF defined correctly in ghost cells
   !*** if so, no boundary update needed here
   !call update_ghost_cells(DUN, bndy_clinic, field_loc_u,     &
   !                                          field_type_scalar)
   !call update_ghost_cells(DUS, bndy_clinic, field_loc_u,     &
   !                                          field_type_scalar)
   !call update_ghost_cells(DUE, bndy_clinic, field_loc_u,     &
   !                                          field_type_scalar)
   !call update_ghost_cells(DUW, bndy_clinic, field_loc_u,     &
   !                                          field_type_scalar)
   !call update_ghost_cells(DUM, bndy_clinic, field_loc_u,     &
   !                                          field_type_scalar)
   !call update_ghost_cells(DMC, bndy_clinic, field_loc_u,     &
   !                                          field_type_scalar)
   !call update_ghost_cells(DME, bndy_clinic, field_loc_u,     &
   !                                          field_type_scalar)
   !call update_ghost_cells(DMN, bndy_clinic, field_loc_u,     &
   !                                          field_type_scalar)

   DUC = -(DUN + DUS + DUE + DUW)               ! scalar laplacian
   DMW = -DME
   DMS = -DMN

!-----------------------------------------------------------------------
!
!  free up memory
!
!-----------------------------------------------------------------------

   deallocate(KXU, KYU,                &
              DXKX, DYKY, DXKY, DYKX,  &
              WORK1, WORK2)

   if (.not. lvariable_hmixu) deallocate(AMF)

!-----------------------------------------------------------------------
!EOC

 end subroutine init_del2u

!***********************************************************************
!BOP
! !IROUTINE: init_del2t
! !INTERFACE:

 subroutine init_del2t

! !DESCRIPTION:
!  This routine reads parameters for Laplaciang tracer mixing and
!  calculates the coefficients of the 5-point stencils for
!  the $\nabla^2$ operator acting on tracer fields.  See the hdifft
!  routine for a description of the operator.
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
      i,j,                &! dummy loop indices
      iblock,             &! block index
      nml_error            ! error flag for namelist

   real (r8), dimension (:,:), allocatable :: &
      WORK1,WORK2          ! temporary work space

   logical (log_kind) ::  &
      lauto_hmix,         &! true to automatically determine mixing coeff
      lvariable_hmix       ! true for spatially varying mixing

   namelist /hmix_del2t_nml/ lauto_hmix, lvariable_hmix, ah

   real (r8) ::           &
      ahfmin, ahfmax       ! min max mixing for varible mixing

!-----------------------------------------------------------------------
!
!  read input namelist to set options
!
!-----------------------------------------------------------------------

   lauto_hmix = .false.
   lvariable_hmix = .false.
   ah = c0

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=hmix_del2t_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading hmix_del2t_nml')
   endif

   if (my_task == master_task) then

      write(stdout,blank_fmt)
      write(stdout,'(a31)') 'Laplacian tracer mixing options'
      write(stdout,blank_fmt)

      lauto_hmixt = lauto_hmix

      if (.not. lauto_hmixt) then
         write(stdout,'(a35)') 'Using input horizontal diffusivity:'
         write(stdout,'(a7,2x,1pe12.5)') '  ah =',ah
      endif

      lvariable_hmixt = lvariable_hmix

      if (.not. lvariable_hmixt) then
         write(stdout,'(a43)') &
              'Variable horizontal tracer mixing disabled'
      endif

   endif

   call broadcast_scalar(lauto_hmixt,     master_task)
   call broadcast_scalar(lvariable_hmixt, master_task)
   call broadcast_scalar(ah,              master_task)

!-----------------------------------------------------------------------
!
!  automatically set horizontal mixing coefficients if requested
!
!-----------------------------------------------------------------------

   if (lauto_hmixt) then

      ! scale to 1e7 at 1/2 degree
      ah = 1.0e7_r8*(720.0_r8/float(nx_global))
      if (my_task == master_task) then
         write(stdout,'(a46)') &
           'Horizontal diffusivity computed automatically:'
         write(stdout,'(a7,2x,1pe12.5)') '  ah =',ah
      endif

   endif

!-----------------------------------------------------------------------
!
!  set spatially variable mixing arrays if requested
!
!  this applies only to momentum or tracer mixing
!  with del2 or del4 options
!
!  functions describing spatial dependence of horizontal mixing
!  coefficients:   ah -> ah*AHF
!
!  for standard laplacian  mixing they scale like (cell area)**0.5
!  (note: these forms assume a global mesh with a grid line along
!  the equator, and the mixing coefficients are the set relative
!  to the average grid spacing at the equator - for cells of
!  this size AMF = AHF = 1.0).
!
!-----------------------------------------------------------------------

   if (lvariable_hmixt) then

      ahfmin = c1
      ahfmax = c1

      allocate(AHF(nx_block,ny_block,nblocks_clinic))

      do iblock=1,nblocks_clinic
         AHF(:,:,iblock) = sqrt(TAREA(:,:,iblock)/ &
                           (c2*pi*radius/nx_global)**2)
      end do

      ahfmin = global_minval(AHF, distrb_clinic, field_loc_center, CALCT)
      ahfmax = global_maxval(AHF, distrb_clinic, field_loc_center, CALCT)

      if (my_task == master_task) then
         write(stdout,'(a39)')  &
               'Variable horizontal diffusivity enabled'
         write(stdout,'(a12,1x,1pe12.5,3x,a9,1x,1pe12.5)') &
               '  Min AHF =',ahfmin,'Max AHF =',ahfmax
      endif

      call update_ghost_cells(AHF, bndy_clinic, field_loc_center, &
                                                field_type_scalar)

   else

      !*** allocate AHF temporarily to simplify setup

      allocate(AHF(nx_block,ny_block,nblocks_clinic))
      AHF = c1

   endif

!-----------------------------------------------------------------------
!
!  calculate {N,S,E,W} coefficients for Del**2 acting on tracer
!  fields (for tracers, the central coefficient is calculated as
!  minus the sum of these after boundary conditions are applied).
!
!-----------------------------------------------------------------------

   allocate(DTN(nx_block,ny_block,nblocks_clinic), &
            DTS(nx_block,ny_block,nblocks_clinic), &
            DTE(nx_block,ny_block,nblocks_clinic), &
            DTW(nx_block,ny_block,nblocks_clinic))

   allocate(WORK1 (nx_block,ny_block), &
            WORK2 (nx_block,ny_block))

   do iblock=1,nblocks_clinic
      WORK1 = (HTN(:,:,iblock)/HUW(:,:,iblock))*p5*(AHF(:,:,iblock) + &
                            eoshift(AHF(:,:,iblock),dim=2,shift=1))

      DTN(:,:,iblock) = WORK1*TAREA_R(:,:,iblock)
      DTS(:,:,iblock) = eoshift(WORK1,dim=2,shift=-1)* &
                        TAREA_R(:,:,iblock)

      WORK1 = (HTE(:,:,iblock)/HUS(:,:,iblock))*p5*(AHF(:,:,iblock) + &
                            eoshift(AHF(:,:,iblock),dim=1,shift=1))

      DTE(:,:,iblock) = WORK1*TAREA_R(:,:,iblock)
      DTW(:,:,iblock) = eoshift(WORK1,dim=1,shift=-1)* &
                        TAREA_R(:,:,iblock)

   end do

   !*** these coeffs only required in physical domain and
   !*** should be defined correctly there as long as grid
   !*** arrays have been correctly defined in ghost cells
   !*** if so, no ghost cell update required

   !call update_ghost_cells(DTN, bndy_clinic, field_loc_t,     &
   !                                          field_type_scalar)
   !call update_ghost_cells(DTS, bndy_clinic, field_loc_t,     &
   !                                          field_type_scalar)
   !call update_ghost_cells(DTE, bndy_clinic, field_loc_t,     &
   !                                          field_type_scalar)
   !call update_ghost_cells(DTW, bndy_clinic, field_loc_t,     &
   !                                          field_type_scalar)

!-----------------------------------------------------------------------
!
!  free up memory
!
!-----------------------------------------------------------------------

   deallocate(WORK1, WORK2)

   if (.not. lvariable_hmixt) deallocate(AHF)

!-----------------------------------------------------------------------
!EOC

 end subroutine init_del2t

!***********************************************************************
!BOP
! !IROUTINE: hdiffu_del2
! !INTERFACE:

 subroutine hdiffu_del2(k, HDUK, HDVK, UMIXK, VMIXK, this_block)

! !DESCRIPTION:
!  This routine computes the horizontial diffusion of momentum
!  using the Laplacian diffusion operator given by:
!  \begin{eqnarray}
!     \nabla\cdot A_M \nabla u & = &
!           {1\over{\Delta_y}}\delta_x
!           \left(\overline{A_M}^x \Delta_y \delta_x u \right)
!         + {1\over{\Delta_x}}\delta_y
!           \left(\overline{A_M}^y \Delta_x \delta_y u \right)
!           \nonumber \\
!      &  &- u\left(\delta_x k_x + \delta_y k_y +
!           2(k_x^2 + k_y^2)\right) \nonumber \\
!      &  &+ 2k_y \delta_x v - 2k_x \delta_y v \\
!     \nabla\cdot A_M \nabla v &=&
!           {1\over{\Delta_y}}\delta_x
!           \left(\overline{A_M}^x \Delta_y \delta_x v \right)
!         + {1\over{\Delta_x}}\delta_y
!           \left(\overline{A_M}^y \Delta_x \delta_y v \right)
!           \nonumber \\
!      &  &- v\left(\delta_x k_x + \delta_y k_y +
!           2(k_x^2 + k_y^2)\right) \nonumber \\
!      &  &+ 2k_y \delta_x u - 2k_x \delta_y u
!  \end{eqnarray}
!  where
!  \begin{equation}
!     k_x = {1\over{\Delta_y}}\delta_x\Delta_y
!  \end{equation}
!  and
!  \begin{equation}
!     k_y = {1\over{\Delta_x}}\delta_y\Delta_x
!  \end{equation}
!
!  Note that boundary conditions are not explicitly imposed on
!  since $u = v = 0$ on the boundaries.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: k  ! depth level index

   real (r8), dimension(nx_block,ny_block), intent(in) :: &
      UMIXK,             &! U at level k and mixing time level
      VMIXK               ! V at level k and mixing time level

   type (block), intent(in) :: &
      this_block          ! block information for this subblock

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(out) :: &
      HDUK,              &! Hdiff(Ub) at level k
      HDVK                ! Hdiff(Vb) at level k

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) ::  &
      i,j,                &! loop indices
      bid                  ! local block address

   real (r8) ::          &
      cc,                &! center fivept weight
      cn, cs, ce, cw      ! other weights for partial bottom cells

   real (r8), dimension(nx_block,ny_block) :: &
      UTMP, VTMP,        &! modified velocities to use with topostress
      HDIFFCFL            ! for cfl number diagnostics

!-----------------------------------------------------------------------
!
!  laplacian mixing
!
!  calculate Del**2(U,V) without metric terms that mix U,V
!  add metric terms that mix U,V
!
!-----------------------------------------------------------------------

   bid = this_block%local_id

   HDUK = c0
   HDVK = c0

!-----------------------------------------------------------------------
!
!  handle four cases individually to avoid unnecessary copies
!  these are all basic five point stencil operators - the topostress
!  option requires operating on a modified velocity while the
!  partial bottom cell case modifies the weights.
!
!-----------------------------------------------------------------------

   if (ltopostress) then

      UTMP = merge(UMIXK(:,:) - TSU(:,:,bid), UMIXK(:,:), &
                   k <= KMU(:,:,bid))
      VTMP = merge(VMIXK(:,:) - TSV(:,:,bid), VMIXK(:,:), &
                   k <= KMU(:,:,bid))

      if (partial_bottom_cells) then

         do j=this_block%jb,this_block%je
         do i=this_block%ib,this_block%ie

            !*** add metric contrib to central coeff
            cc = DUC(i,j,bid) + DUM(i,j,bid)

            cn = DUN(i,j,bid)*min(DZU(i,j+1,k,bid),  &
                                  DZU(i,j  ,k,bid))/DZU(i,j  ,k,bid)
            cs = DUS(i,j,bid)*min(DZU(i,j-1,k,bid),  &
                                  DZU(i,j  ,k,bid))/DZU(i,j  ,k,bid)
            ce = DUE(i,j,bid)*min(DZU(i+1,j,k,bid),  &
                                  DZU(i  ,j,k,bid))/DZU(i  ,j,k,bid)
            cw = DUW(i,j,bid)*min(DZU(i-1,j,k,bid),  &
                                  DZU(i  ,j,k,bid))/DZU(i  ,j,k,bid)

            HDUK(i,j) = am*((cc*UTMP(i  ,j  ) +                     &
                             cn*UTMP(i  ,j+1) + cs*UTMP(i  ,j-1)  + &
                             ce*UTMP(i+1,j  ) + cw*UTMP(i-1,j  )) + &
                            (DMC(i,j,bid)*VTMP(i  ,j  ) +  &
                             DMN(i,j,bid)*VTMP(i  ,j+1) +  &
                             DMS(i,j,bid)*VTMP(i  ,j-1) +  &
                             DME(i,j,bid)*VTMP(i+1,j  ) +  &
                             DMW(i,j,bid)*VTMP(i-1,j  )))

            HDVK(i,j) = am*((cc*VTMP(i  ,j  ) +                     &
                             cn*VTMP(i  ,j+1) + cs*VTMP(i  ,j-1)  + &
                             ce*VTMP(i+1,j  ) + cw*VTMP(i-1,j  )) - &
                            (DMC(i,j,bid)*UTMP(i  ,j  ) +  &
                             DMN(i,j,bid)*UTMP(i  ,j+1) +  &
                             DMS(i,j,bid)*UTMP(i  ,j-1) +  &
                             DME(i,j,bid)*UTMP(i+1,j  ) +  &
                             DMW(i,j,bid)*UTMP(i-1,j  )))

         end do
         end do

      else  ! no partial bottom cells

         do j=this_block%jb,this_block%je
         do i=this_block%ib,this_block%ie

            !*** add metric contrib to central coeff
            cc = DUC(i,j,bid) + DUM(i,j,bid)

            HDUK(i,j) = am*((cc          *UTMP(i  ,j  ) +  &
                             DUN(i,j,bid)*UTMP(i  ,j+1) +  &
                             DUS(i,j,bid)*UTMP(i  ,j-1) +  &
                             DUE(i,j,bid)*UTMP(i+1,j  ) +  &
                             DUW(i,j,bid)*UTMP(i-1,j  ))+  &
                            (DMC(i,j,bid)*VTMP(i  ,j  ) +  &
                             DMN(i,j,bid)*VTMP(i  ,j+1) +  &
                             DMS(i,j,bid)*VTMP(i  ,j-1) +  &
                             DME(i,j,bid)*VTMP(i+1,j  ) +  &
                             DMW(i,j,bid)*VTMP(i-1,j  )))

            HDVK(i,j) = am*((cc          *VTMP(i  ,j  ) +  &
                             DUN(i,j,bid)*VTMP(i  ,j+1) +  &
                             DUS(i,j,bid)*VTMP(i  ,j-1) +  &
                             DUE(i,j,bid)*VTMP(i+1,j  ) +  &
                             DUW(i,j,bid)*VTMP(i-1,j  ))-  &
                            (DMC(i,j,bid)*UTMP(i  ,j  ) +  &
                             DMN(i,j,bid)*UTMP(i  ,j+1) +  &
                             DMS(i,j,bid)*UTMP(i  ,j-1) +  &
                             DME(i,j,bid)*UTMP(i+1,j  ) +  &
                             DMW(i,j,bid)*UTMP(i-1,j  )))

         end do
         end do

      endif ! partial bottom cells

   else ! no topostress

      if (partial_bottom_cells) then

         do j=this_block%jb,this_block%je
         do i=this_block%ib,this_block%ie

            !*** add metric contrib to central coeff
            cc = DUC(i,j,bid) + DUM(i,j,bid)

            cn = DUN(i,j,bid)*min(DZU(i,j+1,k,bid),  &
                                  DZU(i,j  ,k,bid))/DZU(i,j  ,k,bid)
            cs = DUS(i,j,bid)*min(DZU(i,j-1,k,bid),  &
                                  DZU(i,j  ,k,bid))/DZU(i,j  ,k,bid)
            ce = DUE(i,j,bid)*min(DZU(i+1,j,k,bid),  &
                                  DZU(i  ,j,k,bid))/DZU(i  ,j,k,bid)
            cw = DUW(i,j,bid)*min(DZU(i-1,j,k,bid),  &
                                  DZU(i  ,j,k,bid))/DZU(i  ,j,k,bid)

            HDUK(i,j) = am*((cc*UMIXK(i  ,j  ) +                      &
                             cn*UMIXK(i  ,j+1) + cs*UMIXK(i  ,j-1)  + &
                             ce*UMIXK(i+1,j  ) + cw*UMIXK(i-1,j  )) + &
                            (DMC(i,j,bid)*VMIXK(i  ,j  ) +  &
                             DMN(i,j,bid)*VMIXK(i  ,j+1) +  &
                             DMS(i,j,bid)*VMIXK(i  ,j-1) +  &
                             DME(i,j,bid)*VMIXK(i+1,j  ) +  &
                             DMW(i,j,bid)*VMIXK(i-1,j  )))

            HDVK(i,j) = am*((cc*VMIXK(i  ,j  ) +                      &
                             cn*VMIXK(i  ,j+1) + cs*VMIXK(i  ,j-1)  + &
                             ce*VMIXK(i+1,j  ) + cw*VMIXK(i-1,j  )) - &
                            (DMC(i,j,bid)*UMIXK(i  ,j  ) +  &
                             DMN(i,j,bid)*UMIXK(i  ,j+1) +  &
                             DMS(i,j,bid)*UMIXK(i  ,j-1) +  &
                             DME(i,j,bid)*UMIXK(i+1,j  ) +  &
                             DMW(i,j,bid)*UMIXK(i-1,j  )))

         end do
         end do

      else  ! no partial bottom cells

         do j=this_block%jb,this_block%je
         do i=this_block%ib,this_block%ie

            !*** add metric contrib to central coeff
            cc = DUC(i,j,bid) + DUM(i,j,bid)

            HDUK(i,j) = am*((cc          *UMIXK(i  ,j  ) +  &
                             DUN(i,j,bid)*UMIXK(i  ,j+1) +  &
                             DUS(i,j,bid)*UMIXK(i  ,j-1) +  &
                             DUE(i,j,bid)*UMIXK(i+1,j  ) +  &
                             DUW(i,j,bid)*UMIXK(i-1,j  ))+  &
                            (DMC(i,j,bid)*VMIXK(i  ,j  ) +  &
                             DMN(i,j,bid)*VMIXK(i  ,j+1) +  &
                             DMS(i,j,bid)*VMIXK(i  ,j-1) +  &
                             DME(i,j,bid)*VMIXK(i+1,j  ) +  &
                             DMW(i,j,bid)*VMIXK(i-1,j  )))

            HDVK(i,j) = am*((cc          *VMIXK(i  ,j  ) +  &
                             DUN(i,j,bid)*VMIXK(i  ,j+1) +  &
                             DUS(i,j,bid)*VMIXK(i  ,j-1) +  &
                             DUE(i,j,bid)*VMIXK(i+1,j  ) +  &
                             DUW(i,j,bid)*VMIXK(i-1,j  ))-  &
                            (DMC(i,j,bid)*UMIXK(i  ,j  ) +  &
                             DMN(i,j,bid)*UMIXK(i  ,j+1) +  &
                             DMS(i,j,bid)*UMIXK(i  ,j-1) +  &
                             DME(i,j,bid)*UMIXK(i+1,j  ) +  &
                             DMW(i,j,bid)*UMIXK(i-1,j  )))

         end do
         end do

      endif ! partial bottom cells

   endif  ! topostress

!-----------------------------------------------------------------------
!
!  zero fields at land points
!
!-----------------------------------------------------------------------

   where (k > KMU(:,:,bid))
      HDUK = c0
      HDVK = c0
   endwhere

!-----------------------------------------------------------------------
!
!  compute horiz diffusion cfl diagnostics if required
!
!-----------------------------------------------------------------------

   if (ldiag_cfl) then

      if (lvariable_hmixu) then
         HDIFFCFL = merge(c4*am*AMF(:,:,bid)*                    &
                          (DXUR(:,:,bid)**2 + DYUR(:,:,bid)**2), &
                          c0, KMU(:,:,bid) > k)
      else
         HDIFFCFL = merge(c4*am*                                 &
                          (DXUR(:,:,bid)**2 + DYUR(:,:,bid)**2), &
                          c0, KMU(:,:,bid) > k)
      endif
      HDIFFCFL = abs(HDIFFCFL)
      call cfl_hdiff(k,bid,HDIFFCFL,2,this_block)

   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine hdiffu_del2

!***********************************************************************
!BOP
! !IROUTINE: hdifft_del2
! !INTERFACE:

 subroutine hdifft_del2(k,HDTK,TMIX,this_block)

! !DESCRIPTION:
!  This routine computes the horizontial diffusion of tracers
!  using the Laplacian operator given by:
!  \begin{equation}
!     \nabla\cdot A_M \nabla \phi =
!           {1\over{\Delta_y}}\delta_x
!           \left(\overline{A_H}^x \Delta_y \delta_x \phi \right)
!         + {1\over{\Delta_x}}\delta_y
!           \left(\overline{A_H}^y \Delta_x \delta_y \phi \right)
!  \end{equation}
!  with the boundary conditions of zero gradients of tracers.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: k  ! depth level index

   real (r8), dimension(nx_block,ny_block,km,nt), intent(in) :: &
      TMIX                ! tracers at mix time level

   type (block), intent(in) :: &
      this_block          ! block information for this subblock

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,nt), intent(out) ::  &
      HDTK                ! HDIFF(T) for tracer n at level k

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) ::  &
      i,j,n,              &! dummy tracer index
      bid                  ! local block address

   real (r8), dimension(nx_block,ny_block) :: &
      CC,CN,CS,CE,CW,     &! coeff of 5pt stencil for Del**2
      HDIFFCFL             ! for cfl number diagnostics

!-----------------------------------------------------------------------
!
!  laplacian mixing
!
!  implement boundary conditions by setting
!  stencil coefficients to zero at land points.
!
!-----------------------------------------------------------------------

   bid = this_block%local_id

   if (partial_bottom_cells) then
      CN = c0
      CS = c0
      CE = c0
      CW = c0

      do j=this_block%jb-1,this_block%je+1
      do i=this_block%ib-1,this_block%ie+1

         CN(i,j) = DTN(i,j,bid)*min(DZT(i,j  ,k,bid),  &
                                    DZT(i,j+1,k,bid))/DZT(i,j,k,bid)
         CS(i,j) = DTS(i,j,bid)*min(DZT(i,j  ,k,bid),  &
                                    DZT(i,j-1,k,bid))/DZT(i,j,k,bid)
         CE(i,j) = DTE(i,j,bid)*min(DZT(i  ,j,k,bid),  &
                                    DZT(i+1,j,k,bid))/DZT(i,j,k,bid)
         CW(i,j) = DTW(i,j,bid)*min(DZT(i  ,j,k,bid),  &
                                    DZT(i-1,j,k,bid))/DZT(i,j,k,bid)
      end do
      end do

      CN = merge(CN          , c0, (k <= KMTN(:,:,bid)) .and. &
                                   (k <= KMT (:,:,bid)))
      CS = merge(CS          , c0, (k <= KMTS(:,:,bid)) .and. &
                                   (k <= KMT (:,:,bid)))
      CE = merge(CE          , c0, (k <= KMTE(:,:,bid)) .and. &
                                   (k <= KMT (:,:,bid)))
      CW = merge(CW          , c0, (k <= KMTW(:,:,bid)) .and. &
                                   (k <= KMT (:,:,bid)))
   else

      CN = merge(DTN(:,:,bid), c0, (k <= KMTN(:,:,bid)) .and. &
                                   (k <= KMT (:,:,bid)))
      CS = merge(DTS(:,:,bid), c0, (k <= KMTS(:,:,bid)) .and. &
                                   (k <= KMT (:,:,bid)))
      CE = merge(DTE(:,:,bid), c0, (k <= KMTE(:,:,bid)) .and. &
                                   (k <= KMT (:,:,bid)))
      CW = merge(DTW(:,:,bid), c0, (k <= KMTW(:,:,bid)) .and. &
                                   (k <= KMT (:,:,bid)))
   endif

   CC = -(CN + CS + CE + CW)  ! central coefficient

!-----------------------------------------------------------------------
!
!  calculate Del**2(T) for each tracer n
!
!-----------------------------------------------------------------------

   HDTK = c0

   do n = 1,nt
   do j=this_block%jb,this_block%je
   do i=this_block%ib,this_block%ie
      HDTK(i,j,n) = ah*(CC(i,j)*TMIX(i  ,j  ,k,n) + &
                        CN(i,j)*TMIX(i  ,j+1,k,n) + &
                        CS(i,j)*TMIX(i  ,j-1,k,n) + &
                        CE(i,j)*TMIX(i+1,j  ,k,n) + &
                        CW(i,j)*TMIX(i-1,j  ,k,n))
   enddo
   enddo
   enddo

!-----------------------------------------------------------------------
!
!  compute horiz diffusion cfl diagnostics if required
!
!-----------------------------------------------------------------------

   if (ldiag_cfl) then

      if (lvariable_hmixt) then
         HDIFFCFL = merge(c4*ah*AHF(:,:,bid)*                     &
                          (DXTR(:,:,bid)**2 + DYTR(:,:,bid)**2),  &
                          c0, KMT(:,:,bid) > k)
      else
         HDIFFCFL = merge(c4*ah*                                  &
                          (DXTR(:,:,bid)**2 + DYTR(:,:,bid)**2),  &
                          c0, KMT(:,:,bid) > k)
      endif
      HDIFFCFL = abs(HDIFFCFL)
      call cfl_hdiff(k,bid,HDIFFCFL,1,this_block)

   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine hdifft_del2

!***********************************************************************

 end module hmix_del2

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
