!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module hmix_del4

!BOP
! !MODULE: hmix_del4

! !DESCRIPTION:
!  This module contains routines for computing biharmonic horizontal
!  diffusion of momentum and tracers.
!
! !REVISION HISTORY:
!  CVS:$Id: hmix_del4.F90,v 1.19 2003/01/30 22:12:31 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod
   use blocks
   use communicate
   use distribution
   use domain_size
   use domain
   use constants
   use grid
   use broadcast
   use io
   use global_reductions
   use boundary
   use diagnostics
   use exit_mod

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_del4u,  &
             init_del4t,  &
             hdiffu_del4, &
             hdifft_del4

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

   real (r8) ::         &
      ah,               &! horizontal tracer   mixing coefficient
      am                 ! horizontal momentum mixing coefficient

   logical (log_kind) :: &
      lvariable_hmixu,   &! spatially varying mixing coeffs
      lvariable_hmixt     ! spatially varying mixing coeffs

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_del4u
! !INTERFACE:

 subroutine init_del4u

! !DESCRIPTION:
!  This routine calculates the coefficients of the 5-point stencils for
!  the biharmonic operator acting on momentum fields and also
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

   integer (int_kind) ::   &
      i,j,                 &! dummy loop indices
      iblock,              &! block index
      nml_error             ! error flag for namelist

   real (r8), dimension (:,:), allocatable :: &
      KXU,KYU,             &! metric factors
      DXKX,DYKY,DXKY,DYKX, &! d{x,y}k{x,y}
      WORK1,WORK2           ! temporary work space

   logical (log_kind) ::   &
      lauto_hmix,          &! flag to internally compute mixing coeff
      lvariable_hmix        ! flag to enable spatially varying mixing

   namelist /hmix_del4u_nml/ lauto_hmix, lvariable_hmix, am

   real (r8) ::            &
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
         read(nml_in, nml=hmix_del4u_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading hmix_del4u_nml')
   endif

   if (my_task == master_task) then

      write(stdout,blank_fmt)
      write(stdout,'(a34)') 'Biharmonic momentum mixing options'
      write(stdout,blank_fmt)

      !***
      !*** automatically set horizontal mixing coefficients
      !*** if requested
      !***

      if (lauto_hmix) then
         am = -0.6e20_r8*(1280.0_r8/float(nx_global))
         write(stdout,'(a44)') &
              'Horizontal viscosity computed automatically:'
      else
         write(stdout,'(a33)') 'Using input horizontal viscosity:'
      endif
      write(stdout,'(a7,2x,1pe12.5)') '  am =',am

      lvariable_hmixu = lvariable_hmix

      if (.not. lvariable_hmixu) then
         write(stdout,'(a44)') &
                  'Variable horizontal momentum mixing disabled'
      endif

   endif

   call broadcast_scalar(lvariable_hmixu, master_task)
   call broadcast_scalar(am,              master_task)

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
         AMF(:,:,iblock) = (UAREA(:,:,iblock)/uarea_equator)**1.5
      end do

      amfmin = global_minval(AMF, distrb_clinic, field_loc_NEcorner, CALCU)
      amfmax = global_maxval(AMF, distrb_clinic, field_loc_NEcorner, CALCU)

      if (my_task == master_task) then
         write(stdout,'(a37)') 'Variable horizontal viscosity enabled'
         write(stdout,'(a12,1x,1pe12.5,3x,a9,1x,1pe12.5)') &
              '  Min AMF =',amfmin,'Max AMF =',amfmax
      endif

      call update_ghost_cells(AMF, bndy_clinic, field_loc_NEcorner, &
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

   allocate(DUC(nx_block,ny_block,nblocks_clinic), &
            DUN(nx_block,ny_block,nblocks_clinic), &
            DUS(nx_block,ny_block,nblocks_clinic), &
            DUE(nx_block,ny_block,nblocks_clinic), &
            DUW(nx_block,ny_block,nblocks_clinic), &
            DMC(nx_block,ny_block,nblocks_clinic), &
            DMN(nx_block,ny_block,nblocks_clinic), &
            DMS(nx_block,ny_block,nblocks_clinic), &
            DME(nx_block,ny_block,nblocks_clinic), &
            DMW(nx_block,ny_block,nblocks_clinic), &
            DUM(nx_block,ny_block,nblocks_clinic))

   allocate(KXU   (nx_block,ny_block), &
            KYU   (nx_block,ny_block), &
            DXKX  (nx_block,ny_block), &
            DYKY  (nx_block,ny_block), &
            DXKY  (nx_block,ny_block), &
            DYKX  (nx_block,ny_block), &
            WORK1 (nx_block,ny_block), &
            WORK2 (nx_block,ny_block))

   do iblock=1,nblocks_clinic

!-----------------------------------------------------------------------
!
!     calculate central and {N,S,E,W} coefficients for
!     Del**2 (without metric terms) acting on momentum.
!
!-----------------------------------------------------------------------

      WORK1 = HUS(:,:,iblock)/HTE(:,:,iblock)

      DUS(:,:,iblock) = WORK1*UAREA_R(:,:,iblock)
      DUN(:,:,iblock) = eoshift(WORK1,dim=2,shift=1)*UAREA_R(:,:,iblock)
      !*** DUN invalid now in northernmost ghost cell

      WORK1 = HUW(:,:,iblock)/HTN(:,:,iblock)

      DUW(:,:,iblock) = WORK1*UAREA_R(:,:,iblock)
      DUE(:,:,iblock) = eoshift(WORK1,dim=1,shift=1)*UAREA_R(:,:,iblock)
      !*** DUE invalid now in easternmost ghost cell

!-----------------------------------------------------------------------
!
!     coefficients for metric terms in Del**2(U)
!     and for metric advection terms (KXU,KYU)
!
!-----------------------------------------------------------------------

      KXU = (eoshift(HUW(:,:,iblock),dim=1,shift=1) - &
             HUW(:,:,iblock))*UAREA_R(:,:,iblock)
      !*** KXU invalid in easternmost ghost cell
      KYU = (eoshift(HUS(:,:,iblock),dim=2,shift=1) - &
             HUS(:,:,iblock))*UAREA_R(:,:,iblock)
      !*** KYU invalid in northernmost ghost cell

      WORK1 = (HTE(:,:,iblock) -                         &
               eoshift(HTE(:,:,iblock),dim=1,shift=-1))* &
               TAREA_R(:,:,iblock)  ! KXT
      !*** WORK1 invalid in westernmost ghost cell
      WORK2 = eoshift(WORK1,dim=1,shift=1) - WORK1
      !*** WORK2 invalid in both easternmost and
      !***                       westernmost ghost cell

      DXKX = p5*(WORK2 + eoshift(WORK2,dim=2,shift=1))*DXUR(:,:,iblock)
      !*** DXKX invalid in east/west/northernmost ghost cells

      WORK2 = eoshift(WORK1,dim=2,shift=1) - WORK1
      !*** WORK2 invalid in west/northernmost ghost cells

      DYKX = p5*(WORK2 + eoshift(WORK2,dim=1,shift=1))*DYUR(:,:,iblock)
      !*** DYKX invalid in east/west/northernmost ghost cells

      WORK1 = (HTN(:,:,iblock) -                         &
               eoshift(HTN(:,:,iblock),dim=2,shift=-1))* &
              TAREA_R(:,:,iblock)  ! KYT
      !*** WORK1 invalid in southernmost ghost cell
      WORK2 = eoshift(WORK1,dim=2,shift=1) - WORK1
      !*** WORK2 invalid in north/southernmost ghost cells

      DYKY = p5*(WORK2 + eoshift(WORK2,dim=1,shift=1))*DYUR(:,:,iblock)
      !*** DYKY invalid in east/north/southernmost ghost cells

      WORK2 = eoshift(WORK1,dim=1,shift=1) - WORK1
      !*** WORK2 invalid in east/southernmost ghost cells

      DXKY = p5*(WORK2 + eoshift(WORK2,dim=2,shift=1))*DXUR(:,:,iblock)
      !*** DYKY invalid in east/north/southernmost ghost cells

      DUM(:,:,iblock) = -(DXKX + DYKY + c2*(KXU**2 + KYU**2))
      DMC(:,:,iblock) = DXKY - DYKX

!-----------------------------------------------------------------------
!
!     calculate central and {N,S,E,W} coefficients for
!     metric mixing terms which mix U,V.
!
!-----------------------------------------------------------------------

      DME(:,:,iblock) =  c2*KYU/(HTN(:,:,iblock) + &
                                 eoshift(HTN(:,:,iblock),dim=1,shift=1))
      DMN(:,:,iblock) = -c2*KXU/(HTE(:,:,iblock) + &
                                 eoshift(HTE(:,:,iblock),dim=2,shift=1))

   end do

   !*** these coefficients need to be valid in the first layer
   !*** of ghost cells.  as long as two ghost cells are used,
   !*** and grid values are defined correctly in ghost cells,
   !*** these should be defined correctly in the first layer
   !*** and no update required
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

   deallocate(KXU, KYU,               &
              DXKX, DYKY, DXKY, DYKX, &
              WORK1, WORK2)

   if (.not. lvariable_hmixu) deallocate(AMF)

!-----------------------------------------------------------------------
!EOC

 end subroutine init_del4u

!***********************************************************************
!BOP
! !IROUTINE: init_del4t
! !INTERFACE:

 subroutine init_del4t

! !DESCRIPTION:
!  This routine reads parameters for biharmonic tracer mixing and
!  calculates the coefficients of the 5-point stencils for
!  the biharmonic operator acting on tracer fields.  See the hdifft
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

   integer (int_kind) :: &
      i,j,               &! dummy loop indices
      iblock,            &! block index
      nml_error           ! error flag for namelist

   real (r8), dimension (:,:), allocatable :: &
      WORK1                ! temporary work space

   logical (log_kind) :: &
      lauto_hmix,        &! true to automatically determine mixing coeff
      lvariable_hmix      ! true for spatially varying mixing

   namelist /hmix_del4t_nml/ lauto_hmix, lvariable_hmix, ah

   real (r8) ::          &
      ahfmin, ahfmax      ! min max mixing for varible mixing

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
         read(nml_in, nml=hmix_del4t_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading hmix_del4t_nml')
   endif

   if (my_task == master_task) then

      write(stdout,blank_fmt)
      write(stdout,'(a32)') 'Biharmonic tracer mixing options'
      write(stdout,blank_fmt)

      !***
      !***  automatically set horizontal mixing coefficients
      !***  if requested
      !***

      if (lauto_hmix) then
         ah = -0.2e20_r8*(1280.0_r8/float(nx_global))
         write(stdout,'(a46)') &
           'Horizontal diffusivity computed automatically:'
      else
         write(stdout,'(a35)') 'Using input horizontal diffusivity:'
      endif
      write(stdout,'(a7,2x,1pe12.5)') '  ah =',ah

      lvariable_hmixt = lvariable_hmix

      if (.not. lvariable_hmixt) then
         write(stdout,'(a43)') &
              'Variable horizontal tracer mixing disabled'
      endif

   endif

   call broadcast_scalar(lvariable_hmixt, master_task)
   call broadcast_scalar(ah,              master_task)

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
         AHF(:,:,iblock) = (TAREA(:,:,iblock)/uarea_equator)**1.5
      end do

      ahfmin = global_minval(AHF, distrb_clinic, field_loc_center, CALCT)
      ahfmax = global_maxval(AHF, distrb_clinic, field_loc_center, CALCT)

      if (my_task == master_task) then
         write(stdout,'(a39)') &
               'Variable horizontal diffusivity enabled'
         write(stdout,'(a12,1x,1pe12.5,3x,a9,1x,1pe12.5)') &
               '  Min AHF =',ahfmin,'Max AHF =',ahfmax
      endif

      call update_ghost_cells(AHF, bndy_clinic, field_loc_center,&
                                                field_type_scalar)

   endif

!-----------------------------------------------------------------------
!
!  calculate {N,S,E,W} coefficients for Del**2 acting on tracer
!  fields (for tracers, the central coefficient is calculated as
!  minus the sum of these after boundary conditions are applied).
!
!-----------------------------------------------------------------------

   allocate(DTN(nx_block,ny_block,nblocks_clinic),  &
            DTS(nx_block,ny_block,nblocks_clinic),  &
            DTE(nx_block,ny_block,nblocks_clinic),  &
            DTW(nx_block,ny_block,nblocks_clinic))

   allocate(WORK1 (nx_block,ny_block))

   do iblock=1,nblocks_clinic

      WORK1 = HTN(:,:,iblock)/HUW(:,:,iblock)

      DTN(:,:,iblock) = WORK1*TAREA_R(:,:,iblock)
      DTS(:,:,iblock) = eoshift(WORK1,dim=2,shift=-1)* &
                        TAREA_R(:,:,iblock)

      WORK1 = HTE(:,:,iblock)/HUS(:,:,iblock)

      DTE(:,:,iblock) = WORK1*TAREA_R(:,:,iblock)
      DTW(:,:,iblock) = eoshift(WORK1,dim=1,shift=-1)* &
                        TAREA_R(:,:,iblock)

   end do

   !*** these need to be correctly defined in the first layer
   !*** of ghost cells.  if the grid quantities are defined
   !*** correctly and the number of ghost cells is set to 2,
   !*** these should not require a ghost cell update here
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

   deallocate(WORK1)

!-----------------------------------------------------------------------
!EOC

 end subroutine init_del4t

!***********************************************************************
!BOP
! !IROUTINE: hdiffu_del4
! !INTERFACE:

 subroutine hdiffu_del4(k,HDUK,HDVK,UMIXK,VMIXK,this_block)

! !DESCRIPTION:
!  This routine computes the horizontial diffusion of momentum
!  using a biharmonic ($\nabla^4$) operator, where the biharmonic
!  operator is implemented using a repeated application of the
!  Laplacian operator:
!  \begin{equation}
!  D_H(u) = \nabla^2(A_M\nabla^2(u))
!  \end{equation}
!  where
!  \begin{eqnarray}
!  \nabla^2(u) & = &  \Delta_x\delta_x[\Delta_y\delta_x(u)]/AREA
!                   + \Delta_y\delta_y[\Delta_x\delta_y(u)]/AREA \\
!              &   &  - u*[dxkx - dyky + 2(k_x^2 + k_y^2)]
!                + 2hy\delta_x(v) - 2k_x\delta_y(v) \nonumber \\
!  \nabla^2(v) & = & \Delta_x\delta_x[\Delta_y\delta_x(v)]/AREA
!                  + \Delta_y\delta_y[\Delta_x\delta_y(v)]/AREA \\
!              &   &  - v*[dxkx - dyky + 2(k_x^2 + k_y^2)]
!                - 2hy\delta_x(u) + 2k_x\delta_y(u) \nonumber
!  \end{eqnarray}
!
!  Boundary conditions are not explicitly imposed on
!  $\nabla^2(u,v)$ since $u = v = 0$ on the boundaries.
!  In this version of the model, the  boundary conditions
!  on $\nabla^2$ acting on $\nabla^2(u,v)$ are also that
!  $\nabla^2(u,v)$ vanishes at land points.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: k  ! depth level index

   real (r8), dimension(nx_block,ny_block), intent(in) :: &
      UMIXK,             &! U velocity at level k and mix time level
      VMIXK               ! V velocity at level k and mix time level

   type (block), intent(in) :: &
      this_block             ! block info for this sub block

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(out) :: &
      HDUK,              &! Hdiff(Ub) at level k
      HDVK                ! Hdiff(Vb) at level k

!EOP
!BOC
!-----------------------------------------------------------------------
!
!     local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j,               &! loop indices
      bid                 ! local address of current block

   real (r8), dimension (nx_block,ny_block) :: &
      D2UK,D2VK,        &! intermediate Del**2 results
      CC,               &! central 5-point weight
      CN,CS,CE,CW,      &! additional weights for partial bottom cells
      HDIFFCFL           ! local hdiff cfl number for diagnostics

!-----------------------------------------------------------------------
!
!  biharmonic mixing
!
!-----------------------------------------------------------------------

   bid = this_block%local_id

!-----------------------------------------------------------------------
!
!  calculate Del**2(U,V) without metric terms that mix U,V
!  add metric terms that mix U,V, and zero fields at land points
!
!-----------------------------------------------------------------------

   !*** add metric contribution to central coefficient
   CC = DUC(:,:,bid) + DUM(:,:,bid)
   D2UK = c0
   D2VK = c0

   if (partial_bottom_cells) then

      do j=this_block%jb-1,this_block%je+1
      do i=this_block%ib-1,this_block%ie+1

         CN(i,j) = DUN(i,j,bid)*min(DZU(i,j+1,k,bid),  &
                                    DZU(i,j  ,k,bid))/DZU(i,j  ,k,bid)
         CS(i,j) = DUS(i,j,bid)*min(DZU(i,j-1,k,bid),  &
                                    DZU(i,j  ,k,bid))/DZU(i,j  ,k,bid)
         CE(i,j) = DUE(i,j,bid)*min(DZU(i+1,j,k,bid),  &
                                    DZU(i  ,j,k,bid))/DZU(i  ,j,k,bid)
         CW(i,j) = DUW(i,j,bid)*min(DZU(i-1,j,k,bid),  &
                                    DZU(i  ,j,k,bid))/DZU(i  ,j,k,bid)

         D2UK(i,j) = (CC (i,j    )*UMIXK(i  ,j  ) +        &
                      CN (i,j    )*UMIXK(i  ,j+1) +        &
                      CS (i,j    )*UMIXK(i  ,j-1) +        &
                      CE (i,j    )*UMIXK(i+1,j  ) +        &
                      CW (i,j    )*UMIXK(i-1,j  ))+        &
                     (DMC(i,j,bid)*VMIXK(i  ,j  ) +        &
                      DMN(i,j,bid)*VMIXK(i  ,j+1) +        &
                      DMS(i,j,bid)*VMIXK(i  ,j-1) +        &
                      DME(i,j,bid)*VMIXK(i+1,j  ) +        &
                      DMW(i,j,bid)*VMIXK(i-1,j  ))
         D2VK(i,j) = (CC (i,j    )*VMIXK(i  ,j  ) +        &
                      CN (i,j    )*VMIXK(i  ,j+1) +        &
                      CS (i,j    )*VMIXK(i  ,j-1) +        &
                      CE (i,j    )*VMIXK(i+1,j  ) +        &
                      CW (i,j    )*VMIXK(i-1,j  ))-        &
                     (DMC(i,j,bid)*UMIXK(i  ,j  ) +        &
                      DMN(i,j,bid)*UMIXK(i  ,j+1) +        &
                      DMS(i,j,bid)*UMIXK(i  ,j-1) +        &
                      DME(i,j,bid)*UMIXK(i+1,j  ) +        &
                      DMW(i,j,bid)*UMIXK(i-1,j  ))
      end do
      end do

   else  ! no partial bottom cells

      do j=this_block%jb-1,this_block%je+1
      do i=this_block%ib-1,this_block%ie+1

         D2UK(i,j)  =(CC (i,j    )*UMIXK(i  ,j  ) +        &
                      DUN(i,j,bid)*UMIXK(i  ,j+1) +        &
                      DUS(i,j,bid)*UMIXK(i  ,j-1) +        &
                      DUE(i,j,bid)*UMIXK(i+1,j  ) +        &
                      DUW(i,j,bid)*UMIXK(i-1,j  ))+        &
                     (DMC(i,j,bid)*VMIXK(i  ,j  ) +        &
                      DMN(i,j,bid)*VMIXK(i  ,j+1) +        &
                      DMS(i,j,bid)*VMIXK(i  ,j-1) +        &
                      DME(i,j,bid)*VMIXK(i+1,j  ) +        &
                      DMW(i,j,bid)*VMIXK(i-1,j  ))
         D2VK(i,j)  =(CC (i,j    )*VMIXK(i  ,j  ) +        &
                      DUN(i,j,bid)*VMIXK(i  ,j+1) +        &
                      DUS(i,j,bid)*VMIXK(i  ,j-1) +        &
                      DUE(i,j,bid)*VMIXK(i+1,j  ) +        &
                      DUW(i,j,bid)*VMIXK(i-1,j  ))-        &
                     (DMC(i,j,bid)*UMIXK(i  ,j  ) +        &
                      DMN(i,j,bid)*UMIXK(i  ,j+1) +        &
                      DMS(i,j,bid)*UMIXK(i  ,j-1) +        &
                      DME(i,j,bid)*UMIXK(i+1,j  ) +        &
                      DMW(i,j,bid)*UMIXK(i-1,j  ))
      end do
      end do

   endif ! partial bottom cells

   if (lvariable_hmixu) then
      where (k <= KMU(:,:,bid))
         D2UK = AMF(:,:,bid)*D2UK
         D2VK = AMF(:,:,bid)*D2VK
      elsewhere
         D2UK = c0
         D2VK = c0
      end where
   else
      where (k > KMU(:,:,bid))
         D2UK = c0
         D2VK = c0
      endwhere
   endif

!-----------------------------------------------------------------------
!
!  calculate Del**2(Del**2(U,V)) without metric terms that mix U,V
!  add metric terms, and zero fields at land points
!
!-----------------------------------------------------------------------

   HDUK = c0
   HDVK = c0

   if (partial_bottom_cells) then

      do j=this_block%jb,this_block%je
      do i=this_block%ib,this_block%ie
         HDUK(i,j)  = am*((CC (i,j    )*D2UK(i  ,j  ) +        &
                           CN (i,j    )*D2UK(i  ,j+1) +        &
                           CS (i,j    )*D2UK(i  ,j-1) +        &
                           CE (i,j    )*D2UK(i+1,j  ) +        &
                           CW (i,j    )*D2UK(i-1,j  ))+        &
                          (DMC(i,j,bid)*D2VK(i  ,j  ) +        &
                           DMN(i,j,bid)*D2VK(i  ,j+1) +        &
                           DMS(i,j,bid)*D2VK(i  ,j-1) +        &
                           DME(i,j,bid)*D2VK(i+1,j  ) +        &
                           DMW(i,j,bid)*D2VK(i-1,j  )))
         HDVK(i,j)  = am*((CC (i,j    )*D2VK(i  ,j  ) +        &
                           CN (i,j    )*D2VK(i  ,j+1) +        &
                           CS (i,j    )*D2VK(i  ,j-1) +        &
                           CE (i,j    )*D2VK(i+1,j  ) +        &
                           CW (i,j    )*D2VK(i-1,j  ))-        &
                          (DMC(i,j,bid)*D2UK(i  ,j  ) +        &
                           DMN(i,j,bid)*D2UK(i  ,j+1) +        &
                           DMS(i,j,bid)*D2UK(i  ,j-1) +        &
                           DME(i,j,bid)*D2UK(i+1,j  ) +        &
                           DMW(i,j,bid)*D2UK(i-1,j  )))
      end do
      end do

   else ! no partial bottom cells

      do j=this_block%jb,this_block%je
      do i=this_block%ib,this_block%ie
         HDUK(i,j)  = am*((CC (i,j    )*D2UK(i  ,j  ) +        &
                           DUN(i,j,bid)*D2UK(i  ,j+1) +        &
                           DUS(i,j,bid)*D2UK(i  ,j-1) +        &
                           DUE(i,j,bid)*D2UK(i+1,j  ) +        &
                           DUW(i,j,bid)*D2UK(i-1,j  ))+        &
                          (DMC(i,j,bid)*D2VK(i  ,j  ) +        &
                           DMN(i,j,bid)*D2VK(i  ,j+1) +        &
                           DMS(i,j,bid)*D2VK(i  ,j-1) +        &
                           DME(i,j,bid)*D2VK(i+1,j  ) +        &
                           DMW(i,j,bid)*D2VK(i-1,j  )))
         HDVK(i,j)  = am*((CC (i,j    )*D2VK(i  ,j  ) +        &
                           DUN(i,j,bid)*D2VK(i  ,j+1) +        &
                           DUS(i,j,bid)*D2VK(i  ,j-1) +        &
                           DUE(i,j,bid)*D2VK(i+1,j  ) +        &
                           DUW(i,j,bid)*D2VK(i-1,j  ))-        &
                          (DMC(i,j,bid)*D2UK(i  ,j  ) +        &
                           DMN(i,j,bid)*D2UK(i  ,j+1) +        &
                           DMS(i,j,bid)*D2UK(i  ,j-1) +        &
                           DME(i,j,bid)*D2UK(i+1,j  ) +        &
                           DMW(i,j,bid)*D2UK(i-1,j  )))
      end do
      end do

   endif ! partial bottom cells

   where (k > KMU(:,:,bid))
      HDUK = c0
      HDVK = c0
   endwhere

!-----------------------------------------------------------------------
!
!  compute cfl diagnostics if it is time
!
!-----------------------------------------------------------------------

   if (ldiag_cfl) then
      if (lvariable_hmixu) then
         HDIFFCFL = merge(-c16*am*AMF(:,:,bid)*                    &
                         (DXUR(:,:,bid)**2 + DYUR(:,:,bid)**2)**2, &
                          c0, KMU(:,:,bid) > k)
      else
         HDIFFCFL = merge(-c16*am*                                 &
                         (DXUR(:,:,bid)**2 + DYUR(:,:,bid)**2)**2, &
                          c0, KMU(:,:,bid) > k)
      endif
      HDIFFCFL = abs(HDIFFCFL)
      call cfl_hdiff(k,bid,HDIFFCFL,2,this_block)
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine hdiffu_del4

!***********************************************************************
!BOP
! !IROUTINE: hdifft_del4
! !INTERFACE:

 subroutine hdifft_del4(k,HDTK,TMIX,this_block)

! !DESCRIPTION:
!  This routine computes the horizontial diffusion of tracers
!  using a biharmonic ($\nabla^4$) operator, where the biharmonic
!  operator is implemented using a repeated application of the
!  Laplacian operator:
!  \begin{equation}
!  D_H(\phi) = \nabla^2(A_H\nabla^2(\phi))
!  \end{equation}
!  with
!  \begin{equation}
!  \nabla^2(\phi) = \Delta_x\delta_x[\Delta_y\delta_x(\phi)]/AREA
!                 + \Delta_y\delta_y[\Delta_x\delta_y(\phi)]/AREA
!  \end{equation}
!
!   The boundary conditions are the same on both
!   applications of the $\nabla^2$ operator:  the gradient of both
!   $T$ and $\nabla^2(T)$ vanishes across lateral boundaries.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: k  ! depth level index

   real (r8), dimension(nx_block,ny_block,km,nt), intent(in) :: &
      TMIX                   ! tracers at mix time level

   type (block), intent(in) :: &
      this_block             ! block info for this sub block

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,nt) :: &
      HDTK                ! HDIFF(T) for tracer n at level k

!EOP
!BOC
!-----------------------------------------------------------------------
!
!     local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j,n,             &! dummy loop indices
      bid                 ! local address of current block

   real (r8), dimension(nx_block,ny_block) :: &
      D2TK,              &! intermediate Del**2 result
      CC,CN,CS,CE,CW,    &! five point stencil coefficients
      HDIFFCFL            ! local hdiff cfl number for diagnostics

!-----------------------------------------------------------------------
!
!  biharmonic mixing
!
!-----------------------------------------------------------------------

   bid = this_block%local_id

!-----------------------------------------------------------------------
!
!  implement boundary conditions by setting
!  stencil coefficients to zero at land points.
!
!-----------------------------------------------------------------------

   if (partial_bottom_cells) then
      CN = c0
      CS = c0
      CE = c0
      CW = c0

      do j=this_block%jb-1,this_block%je+1
      do i=this_block%ib-1,this_block%ie+1
         CN(i,j) = DTN(i,j,bid)*min(DZT(i,j  ,k,bid),  &
                                    DZT(i,j+1,k,bid))/DZT(i,j  ,k,bid)
         CS(i,j) = DTS(i,j,bid)*min(DZT(i,j  ,k,bid),  &
                                    DZT(i,j-1,k,bid))/DZT(i,j  ,k,bid)
         CE(i,j) = DTE(i,j,bid)*min(DZT(i  ,j,k,bid),  &
                                    DZT(i+1,j,k,bid))/DZT(i  ,j,k,bid)
         CW(i,j) = DTW(i,j,bid)*min(DZT(i  ,j,k,bid),  &
                                    DZT(i-1,j,k,bid))/DZT(i  ,j,k,bid)
      end do
      end do

      where (k > KMT(:,:,bid) .or. k > KMTN(:,:,bid)) CN = c0
      where (k > KMT(:,:,bid) .or. k > KMTS(:,:,bid)) CS = c0
      where (k > KMT(:,:,bid) .or. k > KMTE(:,:,bid)) CE = c0
      where (k > KMT(:,:,bid) .or. k > KMTW(:,:,bid)) CW = c0
   else
      CN = merge(DTN(:,:,bid), c0, (k <= KMTN(:,:,bid)) .and.  &
                                   (k <= KMT (:,:,bid)))
      CS = merge(DTS(:,:,bid), c0, (k <= KMTS(:,:,bid)) .and.  &
                                   (k <= KMT (:,:,bid)))
      CE = merge(DTE(:,:,bid), c0, (k <= KMTE(:,:,bid)) .and.  &
                                   (k <= KMT (:,:,bid)))
      CW = merge(DTW(:,:,bid), c0, (k <= KMTW(:,:,bid)) .and.  &
                                   (k <= KMT (:,:,bid)))
   endif

   CC = -(CN + CS + CE + CW)  ! central coefficient

   do n = 1,nt
!-----------------------------------------------------------------------
!
!     calculate Del**2(T)
!
!-----------------------------------------------------------------------

      if (lvariable_hmixt) then

         do j=this_block%jb-1,this_block%je+1
         do i=this_block%ib-1,this_block%ie+1

            D2TK(i,j) = AHF(i,j,bid)*                    &
                       (CC(i,j)*TMIX(i  ,j  ,k,n) +      &
                        CN(i,j)*TMIX(i  ,j+1,k,n) +      &
                        CS(i,j)*TMIX(i  ,j-1,k,n) +      &
                        CE(i,j)*TMIX(i+1,j  ,k,n) +      &
                        CW(i,j)*TMIX(i-1,j  ,k,n))

         end do
         end do

      else

         do j=this_block%jb-1,this_block%je+1
         do i=this_block%ib-1,this_block%ie+1

            D2TK(i,j) = CC(i,j)*TMIX(i  ,j  ,k,n) +      &
                        CN(i,j)*TMIX(i  ,j+1,k,n) +      &
                        CS(i,j)*TMIX(i  ,j-1,k,n) +      &
                        CE(i,j)*TMIX(i+1,j  ,k,n) +      &
                        CW(i,j)*TMIX(i-1,j  ,k,n)

         end do
         end do

      endif

!-----------------------------------------------------------------------
!
!     calculate Del**2(Del**2(T))
!     multiply by diffusivity
!
!-----------------------------------------------------------------------

      HDTK(:,:,n) = c0

      do j=this_block%jb,this_block%je
      do i=this_block%ib,this_block%ie

         HDTK(i,j,n) = ah*(CC(i,j)*D2TK(i  ,j  ) +     &
                           CN(i,j)*D2TK(i  ,j+1) +     &
                           CS(i,j)*D2TK(i  ,j-1) +     &
                           CE(i,j)*D2TK(i+1,j  ) +     &
                           CW(i,j)*D2TK(i-1,j  ))

      end do
      end do

   enddo

!-----------------------------------------------------------------------
!
!  compute cfl diagnostics if it is time
!
!-----------------------------------------------------------------------

   if (ldiag_cfl) then
      if (lvariable_hmixt) then
         HDIFFCFL = merge(-c16*ah*AHF(:,:,bid)*                    &
                         (DXTR(:,:,bid)**2 + DYTR(:,:,bid)**2)**2, &
                          c0, KMT(:,:,bid) > k)
      else
         HDIFFCFL = merge(-c16*ah*                                 &
                         (DXTR(:,:,bid)**2 + DYTR(:,:,bid)**2)**2, &
                          c0, KMT(:,:,bid) > k)
      endif
      HDIFFCFL = abs(HDIFFCFL)
      call cfl_hdiff(k,bid,HDIFFCFL,1,this_block)
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine hdifft_del4

!***********************************************************************

 end module hmix_del4

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
