!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module hmix_aniso

!BOP
! !MODULE: hmix_aniso
!
! !DESCRIPTION:
!  This module contains routines for computing horizontal friction
!  terms from the divergence of a stress derived from the
!  rate-of-strain tensor; the stress is anisotropic in general,
!  although the viscous coefficients may be chosen to make the
!  stress isotropic.
!
! !REVISION HISTORY:
!  CVS:$Id: hmix_aniso.F90,v 1.17 2003/03/25 13:38:32 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod
   use blocks
   use communicate
   use distribution
   use domain
   use constants
   use broadcast
   use global_reductions
   use gather_scatter
   use diagnostics
   use time_management
   use grid
   use io
   use exit_mod

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_aniso,   &
             hdiffu_aniso

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  anisotropic viscosity variables
!
!-----------------------------------------------------------------------

   integer (int_kind), parameter ::  &! choices for alignment type
      hmix_alignment_type_flow = 1,  &
      hmix_alignment_type_east = 2,  &
      hmix_alignment_type_grid = 3

   integer (int_kind) ::     &
      hmix_alignment_itype,  &! integer choice for alignment type
      vconst_5                ! int parameter for variable visc form

   logical (log_kind) ::     &
      lvariable_hmix_aniso,  &! spatially varying mixing coeffs
      lsmag_aniso             ! smagorinski nonlinear viscosity

   real (r8) ::   &
      visc_para,  &! viscosity parallel      to alignment direction
      visc_perp,  &! viscosity perpendicular to alignment direction
      vconst_1,   &! coefficients for variable viscosity form
      vconst_2,   &! coefficients for variable viscosity form
      vconst_3,   &! coefficients for variable viscosity form
      vconst_4,   &! coefficients for variable viscosity form
      vconst_6,   &! coefficients for variable viscosity form
      smag_lat,   &! latitude at which to vary perp Smag visc
      smag_lat_fact, &! coeff of latitude-depend Smag visc
      smag_lat_gauss  ! Gaussian width of latitude-dep Smag visc

   !*** the following variables are used only with smag viscosity

   real (r8) ::   &
      c_para,     &! dimensionless smag coefficient for
      c_perp,     &!    parallel and perpendicular directions
      u_para,     &! velocities for grid reynolds number viscous
      u_perp       !    limit in parallel and perpendicular directions

   real (r8), dimension (:,:,:), allocatable :: &
       K1E,K1W,K2N,K2S,         &! metric factors
       H1E,H1W,H2N,H2S,         &! grid spacings
       AMAX_CFL,                &! 1/2 maximum cfl-allowed viscosity
       DSMIN,                   &! min(DXU, DYU)
       F_PARA_SMAG,             &! horizontal variations of the
       F_PERP_SMAG               !   Smagorinsky coefficients

!-----------------------------------------------------------------------
!
!  F_PARA and F_PERP represent the actual anisotropic viscosities 
!  used when lvariable_hmix_aniso is true. For lsmag_aniso option,
!  they contain the time-invariant "numerical" parts of the
!  anisotropic viscosity coefficients.
!
!-----------------------------------------------------------------------
 
   real (r8), dimension(:,:,:,:), allocatable :: &  
      F_PARA,                 &! spatial dependence of viscosity
                               ! parallel to alignment direction
      F_PERP                   ! spatial dependence of viscosity
                               ! perpendicular to alignment direction

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_aniso
! !INTERFACE:

 subroutine init_aniso

! !DESCRIPTION:
!  Initializes various choices, allocates necessary space and computes
!  some time-independent arrays for anisotropic viscosity.
!
! !REVISION HISTORY:
!  same as module

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables:
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      iblock,            &! block loop index
      i,j,k,             &! loop index
      cfl_warn1,         &! flag for warning about CFL limit
      cfl_warn2,         &! flag for warning about CFL limit
      nml_error           ! error flag for namelist

   real (r8), dimension(:,:), allocatable :: &
      WORKA,WORKB          ! local work arrays

   real (r8) ::               &
      f_para_min, f_para_max, &
      f_perp_min, f_perp_max

   character (char_len) ::   &
      hmix_alignment_choice, &! choice of direction that breaks isotropy
      var_viscosity_infile,  &! filename for variable viscosity factor
      var_viscosity_infile_fmt, & ! format (bin or nc) of above file
      var_viscosity_outfile, &! file for output of internally-computed
      var_viscosity_outfile_fmt  !  viscosity and format of that file

   character (16), parameter :: &
      param_fmt = '(a15,2x,1pe12.5)'

!-----------------------------------------------------------------------
!
!  input namelist for setting various anisotropic viscosity options
!
!-----------------------------------------------------------------------

   namelist /hmix_aniso_nml/                                       &
      hmix_alignment_choice, lvariable_hmix_aniso, lsmag_aniso,    &
      visc_para, visc_perp, c_para, c_perp, u_para, u_perp,        &
      vconst_1, vconst_2, vconst_3, vconst_4, vconst_5, vconst_6,  &
      smag_lat, smag_lat_fact, smag_lat_gauss,                     &
      var_viscosity_infile,  var_viscosity_infile_fmt,             &
      var_viscosity_outfile, var_viscosity_outfile_fmt

!-----------------------------------------------------------------------
!
!  read input namelist for anisotropic viscosity
!
!  DEFAULT VALUES
!  hmix_alignment_choice     : flow-aligned
!  lvariable_hmix_aniso      : false
!  lsmag_aniso               : false
!  visc_para                 : zero
!  visc_perp                 : zero
!  c_para                    : zero
!  c_perp                    : zero
!  u_para                    : zero
!  u_perp                    : zero
!  vconst_1                  : 1.e7_r8
!  vconst_2                  : 24.5_r8
!  vconst_3                  : 0.2_r8
!  vconst_4                  : 1.e-8_r8
!  vconst_5                  : 3
!  vconst_6                  : 1.e7_r8
!  smag_lat                  : 20.0_r8
!  smag_lat_fact             : 0.98_r8
!  smag_lat_gauss            : 98.0_r8
!
!-----------------------------------------------------------------------

   hmix_alignment_choice = 'flow'
   lvariable_hmix_aniso = .false.
   lsmag_aniso = .false.
   visc_para      = c0
   visc_perp      = c0
   c_para         = c0
   c_perp         = c0
   u_para         = c0
   u_perp         = c0
   vconst_1       = 1.e7_r8
   vconst_2       = 24.5_r8
   vconst_3       = 0.2_r8
   vconst_4       = 1.e-8_r8
   vconst_5       = 3
   vconst_6       = 1.e7_r8
   smag_lat       = 20.0_r8
   smag_lat_fact  = 0.98_r8
   smag_lat_gauss = 98.0_r8
   var_viscosity_infile      = 'unknown_var_viscosity_infile'  
   var_viscosity_infile_fmt  = 'bin'
   var_viscosity_outfile     = 'unknown_var_viscosity_outfile' 
   var_viscosity_outfile_fmt = 'bin'

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=hmix_aniso_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort, &
                    'ERROR reading hmix_aniso_nml')
   endif

   if (my_task == master_task) then

      write(stdout,blank_fmt)
      write(stdout,'(a30)') 'Anisotropic viscosity options:'
      write(stdout,blank_fmt)

      select case (hmix_alignment_choice(1:4))
      case ('flow')
         hmix_alignment_itype = hmix_alignment_type_flow
         write(stdout,'(a47)') &
           '  Anisotropy aligned along local flow direction'
      case ('east')
         hmix_alignment_itype = hmix_alignment_type_east
         write(stdout,'(a46)') &
           '  Anisotropy aligned along east-west direction'
      case ('grid')
         hmix_alignment_itype = hmix_alignment_type_grid
         write(stdout,'(a43)') &
           '  Anisotropy aligned along grid coordinates'
      case default
         hmix_alignment_itype = -1000
      end select

      if (.not. lvariable_hmix_aniso) then
         write(stdout,'(a52)') &
              '  Spatially variable anisotropic viscosity disabled'
      endif

      if (.not. lsmag_aniso) then
         write(stdout,'(a36)') 'Using input anisotropic viscosities:'
         write(stdout,param_fmt) '   visc_para = ',visc_para
         write(stdout,param_fmt) '   visc_perp = ',visc_perp
      else
         write(stdout,'(a38)') 'Using nonlinear Smagorinski viscosites'
         write(stdout,'(a37)') '  input anistropic smag coefficients:'
         write(stdout,param_fmt) '      c_para = ',c_para
         write(stdout,param_fmt) '      c_perp = ',c_perp
         write(stdout,param_fmt) '      u_para = ',u_para
         write(stdout,param_fmt) '      u_perp = ',u_perp
         if (smag_lat_fact /= c0) then
            write(stdout,'(a31)') 'Using latitudinal variation in '
            write(stdout,'(a35)') 'Smagorinski perpendicular viscosity'
            write(stdout,param_fmt) 'smag_lat      =',smag_lat
            write(stdout,param_fmt) 'smag_lat_fact =',smag_lat_fact
            write(stdout,param_fmt) 'smag_lat_gauss=',smag_lat_gauss
         endif
      endif
      if (lvariable_hmix_aniso) then
         if (trim(var_viscosity_infile) == 'ccsm-internal') then
            write(stdout,'(a38)') &
                          'Using variable anisotropic parameters:'
            write(stdout,param_fmt) '   vconst_1  = ',vconst_1 
            write(stdout,param_fmt) '   vconst_2  = ',vconst_2 
            write(stdout,param_fmt) '   vconst_3  = ',vconst_3 
            write(stdout,param_fmt) '   vconst_4  = ',vconst_4 
            write(stdout,'(a15,2x,i6)') '   vconst_5  = ',vconst_5 
            write(stdout,param_fmt) '   vconst_6  = ',vconst_6 
            if (trim(var_viscosity_outfile) /= &
                'unknown_var_viscosity_outfile') then
               write(stdout,'(a44,a)') &
                  'Computed variable viscosity output to file: ', &
                  trim(var_viscosity_outfile)
            endif
         else
            write(stdout,'(a47,a)') &
               'Variable anisotropic viscosity read from file: ', &
               trim(var_viscosity_infile)
         endif
      endif

   endif

   call broadcast_scalar(hmix_alignment_itype, master_task)
   call broadcast_scalar(lvariable_hmix_aniso, master_task)
   call broadcast_scalar(lsmag_aniso,          master_task)
   call broadcast_scalar(visc_para,            master_task)
   call broadcast_scalar(visc_perp,            master_task)
   call broadcast_scalar(c_para,               master_task)
   call broadcast_scalar(c_perp,               master_task)
   call broadcast_scalar(u_para,               master_task)
   call broadcast_scalar(u_perp,               master_task)
   call broadcast_scalar(vconst_1,             master_task)
   call broadcast_scalar(vconst_2,             master_task)
   call broadcast_scalar(vconst_3,             master_task)
   call broadcast_scalar(vconst_4,             master_task)
   call broadcast_scalar(vconst_5,             master_task)
   call broadcast_scalar(vconst_6,             master_task)
   call broadcast_scalar(smag_lat,             master_task)
   call broadcast_scalar(smag_lat_fact,        master_task)
   call broadcast_scalar(smag_lat_gauss,       master_task)
   call broadcast_scalar(var_viscosity_infile,      master_task)
   call broadcast_scalar(var_viscosity_infile_fmt,  master_task)
   call broadcast_scalar(var_viscosity_outfile,     master_task)
   call broadcast_scalar(var_viscosity_outfile_fmt, master_task)

   if (hmix_alignment_itype == -1000) then
      call exit_POP(sigAbort, &
                    'Unknown type for anisotropic alignment direction')
   endif

!-----------------------------------------------------------------------
!
!  allocate and initialize various 2D arrays:
!     AMAX_CFL is 1/2 the maximum allowed viscosity due to CFL limit
!
!-----------------------------------------------------------------------

   allocate(K1E(nx_block,ny_block,nblocks_clinic),        &
            K1W(nx_block,ny_block,nblocks_clinic),        &
            K2N(nx_block,ny_block,nblocks_clinic),        &
            K2S(nx_block,ny_block,nblocks_clinic),        &
            H1E(nx_block,ny_block,nblocks_clinic),        &
            H1W(nx_block,ny_block,nblocks_clinic),        &
            H2N(nx_block,ny_block,nblocks_clinic),        &
            H2S(nx_block,ny_block,nblocks_clinic),        &
            AMAX_CFL(nx_block,ny_block,nblocks_clinic),   &
            WORKA(nx_block,ny_block),                     &
            WORKB(nx_block,ny_block))

   if (lsmag_aniso) then
      allocate(DSMIN(nx_block,ny_block,nblocks_clinic))
   endif

   !$OMP PARALLEL DO PRIVATE(WORKA, WORKB)
   do iblock=1,nblocks_clinic

      H2S(:,:,iblock) = HTE(:,:,iblock)
      H1W(:,:,iblock) = HTN(:,:,iblock)

      H2N(:,:,iblock) = eoshift(H2S(:,:,iblock),dim=2,shift=1)
      H1E(:,:,iblock) = eoshift(H1W(:,:,iblock),dim=1,shift=1)

      WORKA = H2S(:,:,iblock) + H2N(:,:,iblock)
      WORKB = eoshift(WORKA,dim=1,shift=-1)
      K1W(:,:,iblock) = c2*(WORKA - WORKB)/ &
                           (WORKA + WORKB)/H1W(:,:,iblock)
      K1E(:,:,iblock) = eoshift(K1W(:,:,iblock),dim=1,shift=1)

      WORKA = H1W(:,:,iblock) + H1E(:,:,iblock)
      WORKB = eoshift(WORKA,dim=2,shift=-1)
      K2S(:,:,iblock) = c2*(WORKA - WORKB)/ &
                           (WORKA + WORKB)/H2S(:,:,iblock)
      K2N(:,:,iblock) = eoshift(K2S(:,:,iblock),dim=2,shift=1)

      AMAX_CFL(:,:,iblock) = p125/(dtu*(DXUR(:,:,iblock)**2 + &
                                        DYUR(:,:,iblock)**2))

      if (lsmag_aniso) then
         DSMIN(:,:,iblock) = min(DXU(:,:,iblock),DYU(:,:,iblock))
      endif

   end do ! block loop
   !$OMP END PARALLEL DO

!-----------------------------------------------------------------------
!
!  allocate space and compute spatial dependence of
!  anisotropic viscosity coefficients here.
!
!-----------------------------------------------------------------------

   if (lvariable_hmix_aniso) then
 
      allocate(F_PARA(nx_block,ny_block,km,nblocks_clinic), &
               F_PERP(nx_block,ny_block,km,nblocks_clinic))

      if (trim(var_viscosity_infile) == 'ccsm-internal') then
         call compute_ccsm_var_viscosity
         if (trim(var_viscosity_outfile) /= &
             'unknown_var_viscosity_outfile') then
           call write_var_viscosity(trim(var_viscosity_outfile),   &
                                    trim(var_viscosity_outfile_fmt))
         endif
      else
         call read_var_viscosity(trim(var_viscosity_infile),   &
                                 trim(var_viscosity_infile_fmt))
      endif

      do k=1,km
         f_para_min = global_minval(F_PARA(:,:,k,:), distrb_clinic, &
                                                     field_loc_NEcorner)
         f_para_max = global_maxval(F_PARA(:,:,k,:), distrb_clinic, &
                                                     field_loc_NEcorner)
         f_perp_min = global_minval(F_PERP(:,:,k,:), distrb_clinic, &
                                                     field_loc_NEcorner)
         f_perp_max = global_maxval(F_PERP(:,:,k,:), distrb_clinic, &
                                                     field_loc_NEcorner)
         if (my_task == master_task) then
            write(stdout,"('  vertical level = ',i3)") k
            write(stdout,'(a14,1x,1pe12.5,3x,a12,1x,1pe12.5)') &
                  '  Min F_PARA =',f_para_min,                 &
                    'Max F_PARA =',f_para_max
            write(stdout,'(a14,1x,1pe12.5,3x,a12,1x,1pe12.5)') &
                  '  Min F_PERP =',f_perp_min,                 &
                    'Max F_PERP =',f_perp_max
         endif
      enddo
   endif

!-----------------------------------------------------------------------
!
!  check that viscosities are less than 1/2 CFL limit
!  taper where necessary
!
!-----------------------------------------------------------------------

   if (.not. lsmag_aniso) then
      if (lvariable_hmix_aniso) then

         ! Enforce limits through tapering

         !$OMP PARALLEL DO PRIVATE(k)
         do iblock=1,nblocks_clinic
         do k=1,km

            where (F_PARA(:,:,k,iblock) > &
                   AMAX_CFL(:,:,iblock))
               F_PARA(:,:,k,iblock) = AMAX_CFL(:,:,iblock)
            endwhere

            where (F_PERP(:,:,k,iblock) > &
                   AMAX_CFL(:,:,iblock))
               F_PERP(:,:,k,iblock) = AMAX_CFL(:,:,iblock)
            endwhere
         enddo
         enddo
         !$OMP END PARALLEL DO

      else

         cfl_warn1 = 0
         cfl_warn2 = 0

         !$OMP PARALLEL DO
         do iblock=1,nblocks_clinic
            if (ANY(visc_para > AMAX_CFL(:,:,iblock))) cfl_warn1 = 1
            if (ANY(visc_perp > AMAX_CFL(:,:,iblock))) cfl_warn2 = 1
         end do
         !$OMP END PARALLEL DO

         if (global_sum(cfl_warn1,distrb_clinic) /= 0) then
            if (my_task == master_task) then
               write(stdout,'(a21)') 'WARNING (hmix_aniso):'
               write(stdout,'(a51)') &
               '  parallel viscosity > 1/2 CFL limit at some points'
            endif
         endif

         if (global_sum(cfl_warn2,distrb_clinic) /= 0) then
            if (my_task == master_task) then
               write(stdout,'(a21)') 'WARNING (hmix_aniso):'
               write(stdout,'(a56)') &
              '  perpendicular viscosity > 1/2 CFL limit at some points'
            endif
         endif

      endif
   endif

!-----------------------------------------------------------------------
!
!  set up latitudinal variation of Smagorinsky viscosity
!
!-----------------------------------------------------------------------
 
 
   if ( lsmag_aniso ) then

      allocate(F_PARA_SMAG(nx_block,ny_block,max_blocks_clinic), &
               F_PERP_SMAG(nx_block,ny_block,max_blocks_clinic))

      F_PARA_SMAG = c1

      !*** latitude dependence of F_PERP_SMAG for viscous CCSM runs

      if (smag_lat_fact /= c0) then
         !$OMP PARALLEL DO
         do iblock=1,nblocks_clinic
            WORKA = abs(ULAT(:,:,iblock))*radian
            where (WORKA >= smag_lat)
               F_PERP_SMAG(:,:,iblock) = c1 - smag_lat_fact* &
                        exp(-(WORKA - smag_lat)**2/smag_lat_gauss)
            elsewhere
               F_PERP_SMAG(:,:,iblock) = c1 - smag_lat_fact
            endwhere
         end do
         !$OMP END PARALLEL DO
      endif

   endif

!-----------------------------------------------------------------------
!
!  deallocate work space and write viscosity if required
!
!-----------------------------------------------------------------------

   deallocate (WORKA,WORKB)

   if (my_task == master_task) then
      write(stdout,blank_fmt)
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine init_aniso

!***********************************************************************
!BOP
! !IROUTINE: hdiffu_aniso
! !INTERFACE:

 subroutine hdiffu_aniso(k,HDUK,HDVK,UMIXK,VMIXK,this_block)

! !DESCRIPTION:
!  This routine computes the viscous terms in the momentum equation
!  as the divergence of a stress tensor which is linearly related
!  to the rate-of-strain tensor with viscous coefficents $\nu_{\|}$
!  and $\nu_{\bot}$.  These coefficients represent energy dissipation
!  in directions parallel and perpendicular to a specified
!  alignment direction which breaks the isotropy of the dissipation.
!
!  A functional approach is used to derived the discrete operator,
!  which ensures positive-definite energy dissipation, provided
!  $\nu_{\|} > \nu_{\bot}$.
!
!  In the functional approach, each U-cell is subdivided horizontally
!  into 4 subcells (or quarter-cells), and the strain and stress
!  tensors are evaluated in each subcell.
!
!  The viscosities may optionally be evaluated with Smagorinsky-like
!  non-linear dependence on the deformation rate, which is
!  proportional to the norm of the strain tensor.  With the
!  smagorinski option, the viscosities are evalutated as
!  \begin{eqnarray}
!   \nu_{\|}   &=& \max[c_{\|}  |D|ds^2),u_{\|}  ds] \\
!   \nu_{\bot} &=& \max[c_{\bot}|D|ds^2),u_{\bot}ds]
!  \end{eqnarray}
!    where $ds = \min(dx,dy)$, and the $|D|$ is the deformation rate:
!    $|D| = \surd{2}|E|$,  $c_{\|}$ and $c_{\bot}$ are dimensionless
!    coefficients of order 1, and $u_{\|}$ and $u_{\bot}$ are
!    velocities associated with the grid Reynolds number which
!    determine a minimum background viscosity in regions where the
!    nonlinear viscosity is too small to control grid-point noise.
!    Typically $u_{\|}$ and $u_{\bot}$ are order 1 cm/s.
!    The non-linear viscosities are also automatically limited
!    to be no greater than 1/2 the maximum valued allowed by the
!    viscous CFL limit.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: k  ! depth level index

   real (r8), dimension(nx_block,ny_block), intent(in) :: &
      UMIXK,             &! U velocity at level k and mix time level
      VMIXK               ! V velocity at level k and mix time level

   type (block), intent(in) :: &
      this_block          ! block info for this sub block

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(out) :: &
      HDUK,                   &! Hdiff(Ub) at level k
      HDVK                     ! Hdiff(Vb) at level k

!EOP
!BOC
!-----------------------------------------------------------------------
!
!     local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j,iq,            &! dummy loop index
      bid                 ! local block address

   real (r8), dimension(nx_block,ny_block) :: &
      GE,GW,GN,GS,       &! depth ratios in each direction
      HDIFFCFL            ! for cfl diagnostics

   real (r8) :: &
      work1,work2,work3,work4,work5,work6,work7,work8, &
      ue,uw,un,us,       &! u in (e,w,n,s) mult. by gamma
      ve,vw,vn,vs,       &! v in (e,w,n,s) mult. by gamma
      A,B,C,D,           &! viscous coefficients
      FX,FY               ! area*friction terms

   real (r8), dimension(nx_block,ny_block,4) :: &
      S11,S22,S12         ! stress tensor

   real (r8), dimension (4) :: &
      E11,E22,E12         ! rate-of-strain tensor

!-----------------------------------------------------------------------
!
!    Indices for quarter-cells
!    X = U point
!    N,S,E,W label north,south,east,west faces of U cell
!    1,2,3,4 label SW,NW,NE,SE quarter cells surrounding a U-point
!
!
!              X         ------- X -------         X
!                       |        |        |
!                       |   1    |   4    |
!                       |        |        |
!               ---------------- N ----------------
!              |        |        |        |        |
!              |   3    |   2    |   3    |   2    |
!              |        |        |        |        |
!              X ------ W ------ X ------ E ------ X
!              |        |        |        |        |
!              |   4    |   1    |   4    |   1    |
!              |        |        |        |        |
!               ---------------- S ----------------
!                       |        |        |
!                       |   2    |   3    |
!                       |        |        |
!              X         ------- X -------         X
!
!
!-----------------------------------------------------------------------

!-----------------------------------------------------------------------
!
!  find local block address
!
!-----------------------------------------------------------------------

   bid = this_block%local_id

   HDUK = c0
   HDVK = c0

!-----------------------------------------------------------------------
!
!  compute gamma (depth ratios) for partial bottom cells
!
!-----------------------------------------------------------------------

   if (partial_bottom_cells) then

      do j=this_block%jb-1,this_block%je+1
      do i=this_block%ib-1,this_block%ie+1

         GW(i,j) = min(DZU(i  ,j,k,bid), &
                       DZU(i-1,j,k,bid))/DZU(i,j,k,bid)
         GE(i,j) = min(DZU(i  ,j,k,bid), &
                       DZU(i+1,j,k,bid))/DZU(i,j,k,bid)
         GS(i,j) = min(DZU(i,j  ,k,bid), &
                       DZU(i,j-1,k,bid))/DZU(i,j,k,bid)
         GN(i,j) = min(DZU(i,j  ,k,bid), &
                       DZU(i,j+1,k,bid))/DZU(i,j,k,bid)

      end do
      end do
   else
      GN = c1
      GS = c1
      GE = c1
      GW = c1
   endif

!-----------------------------------------------------------------------
!
!  compute rate-of-strain tensor in each quarter-cell
!
!-----------------------------------------------------------------------

   do j=this_block%jb-1,this_block%je+1
   do i=this_block%ib-1,this_block%ie+1

      uw = GW(i,j)*UMIXK(i-1,j)
      ue = GE(i,j)*UMIXK(i+1,j)
      us = GS(i,j)*UMIXK(i,j-1)
      un = GN(i,j)*UMIXK(i,j+1)

      vw = GW(i,j)*VMIXK(i-1,j)
      ve = GE(i,j)*VMIXK(i+1,j)
      vs = GS(i,j)*VMIXK(i,j-1)
      vn = GN(i,j)*VMIXK(i,j+1)

      work1 = (UMIXK(i,j) - uw)/H1W(i,j,bid)
      work2 = (ue - UMIXK(i,j))/H1E(i,j,bid)
      work3 = p5*K2S(i,j,bid)*(VMIXK(i,j) + vs)
      work4 = p5*K2N(i,j,bid)*(VMIXK(i,j) + vn)

      E11(1) = work1 + work3
      E11(2) = work1 + work4
      E11(3) = work2 + work4
      E11(4) = work2 + work3

      work1 = (VMIXK(i,j) - vs)/H2S(i,j,bid)
      work2 = (vn - VMIXK(i,j))/H2N(i,j,bid)
      work3 = p5*K1W(i,j,bid)*(UMIXK(i,j) + uw)
      work4 = p5*K1E(i,j,bid)*(UMIXK(i,j) + ue)

      E22(1) = work1 + work3
      E22(2) = work2 + work3
      E22(3) = work2 + work4
      E22(4) = work1 + work4

      work1 = (UMIXK(i,j) - us)/H2S(i,j,bid)
      work2 = (un - UMIXK(i,j))/H2N(i,j,bid)
      work3 = (VMIXK(i,j) - vw)/H1W(i,j,bid)
      work4 = (ve - VMIXK(i,j))/H1E(i,j,bid)
      work5 = K2S(i,j,bid)*(UMIXK(i,j) + us)
      work6 = K2N(i,j,bid)*(UMIXK(i,j) + un)
      work7 = K1W(i,j,bid)*(VMIXK(i,j) + vw)
      work8 = K1E(i,j,bid)*(VMIXK(i,j) + ve)

      E12(1) = work1 + work3 - p5*(work5 + work7)
      E12(2) = work2 + work3 - p5*(work6 + work7)
      E12(3) = work2 + work4 - p5*(work6 + work8)
      E12(4) = work1 + work4 - p5*(work5 + work8)

!-----------------------------------------------------------------------
!
!     calculate viscous coeffs A,B,C,D
!     and stress tensor from strain tensor
!
!-----------------------------------------------------------------------

      if (hmix_alignment_itype == hmix_alignment_type_flow) then

         work5 = sqrt(UMIXK(i,j)**2 + VMIXK(i,j)**2)
         if (work5 >= eps) then
            work1 = UMIXK(i,j)/work5   ! n1
            work2 = VMIXK(i,j)/work5   ! n2
         else
            work1 = c0
            work2 = c0
         endif

      endif

      if (lsmag_aniso) then    ! smagorinsky nonlinear viscosity

         do iq = 1,4              ! loop over quarter cells

            ! deformation rate |D| = sqrt(2)|E| (use temp work6)
            ! note E12 is twice the (1,2) component of the strain tensor

            work6 =  sqrt(c2*(E11(iq)**2 + E22(iq)**2) + E12(iq)**2)

            ! compute nonlinear viscosities  (ds = min(dx,dy))
            !    max[c_para|D|ds**2),u_para*ds]
            !    max[c_perp|D|ds**2),u_perp*ds]

            work3 = c_para*F_PARA_SMAG(i,j,bid)*work6* &
                    DSMIN(i,j,bid)*DSMIN(i,j,bid)
            work4 = c_perp*F_PERP_SMAG(i,j,bid)*work6* &
                    DSMIN(i,j,bid)*DSMIN(i,j,bid)

            if (lvariable_hmix_aniso) then
               work3 = max(work3, F_PARA(i,j,k,bid))
               work4 = max(work4, F_PERP(i,j,k,bid))
            endif

            ! taper viscosities when greater than 1/2 CFL limit

            work3 = min(work3,AMAX_CFL(i,j,bid))  ! parallel viscosity
            work4 = min(work4,AMAX_CFL(i,j,bid))  ! perpendicular  "

            !***
            ! compute coefficients multiplying elements strain tensor
            ! use only one of the following anisotropic forms:
            ! grid-aligned, east-aligned or flow-aligned
            !***

            select case (hmix_alignment_itype)

            case (hmix_alignment_type_grid)

               A = p5*(work3+work4) 
               B = p5*(work3+work4) 
               C = c0
               D = work4

            case (hmix_alignment_type_east)

               work1 =  cos(ANGLE(i,j,bid))
               work2 = -sin(ANGLE(i,j,bid))

               A = p5*(work3+work4) - c2*(work3-work4)*(work1*work2)**2
               B = p5*(work3+work4) - c2*(work3-work4)*(work1*work2)**2
               C = (work3-work4)*work1*work2*(work1**2-work2**2)
               D = work4 + c2*(work3-work4)*(work1*work2)**2

            case (hmix_alignment_type_flow)

               A = p5*(work3+work4) - c2*(work3-work4)*(work1*work2)**2
               B = p5*(work3+work4) - c2*(work3-work4)*(work1*work2)**2
               C = (work3-work4)*work1*work2*(work1**2-work2**2)
               D = work4 + c2*(work3-work4)*(work1*work2)**2

            end select

            ! compute stress tensor in each quarter-cell

            S11(i,j,iq) =   A*E11(iq) - B*E22(iq) + C*E12(iq)
            S22(i,j,iq) = - B*E11(iq) + A*E22(iq) - C*E12(iq)
            S12(i,j,iq) =   C*(E11(iq) - E22(iq)) + D*E12(iq)

         enddo

      else     ! no smagorinski nonlinearity

         if (lvariable_hmix_aniso) then
            work3 = F_PARA(i,j,k,bid)         ! parallel viscosity
            work4 = F_PERP(i,j,k,bid)         ! perpendicular "
         else
            work3 = visc_para
            work4 = visc_perp
         endif

         ! compute coefficients multiplying elements strain tensor
         ! use only one of the following anisotropic forms:
         ! grid-aligned, east-aligned or flow-aligned

         select case (hmix_alignment_itype)

         case (hmix_alignment_type_grid)

            A = p5*(work3+work4) 
            B = p5*(work3+work4) 
            C = c0
            D = work4

         case (hmix_alignment_type_east)

            work1 =  cos(ANGLE(i,j,bid))
            work2 = -sin(ANGLE(i,j,bid))

            A = p5*(work3+work4) - c2*(work3-work4)*(work1*work2)**2
            B = p5*(work3+work4) - c2*(work3-work4)*(work1*work2)**2
            C = (work3-work4)*work1*work2*(work1**2-work2**2)
            D = work4 + c2*(work3-work4)*(work1*work2)**2

         case (hmix_alignment_type_flow)

            A = p5*(work3+work4) - c2*(work3-work4)*(work1*work2)**2
            B = p5*(work3+work4) - c2*(work3-work4)*(work1*work2)**2
            C = (work3-work4)*work1*work2*(work1**2-work2**2)
            D = work4 + c2*(work3-work4)*(work1*work2)**2

         end select

         ! compute stress tensor in each quarter-cell

         do iq = 1,4
            S11(i,j,iq) =   A*E11(iq) - B*E22(iq) + C*E12(iq)
            S22(i,j,iq) = - B*E11(iq) + A*E22(iq) - C*E12(iq)
            S12(i,j,iq) =   C*(E11(iq) - E22(iq)) + D*E12(iq)
         enddo

      endif

   enddo  ! (i-loop)
   enddo  ! (j-loop)

!-----------------------------------------------------------------------
!
!  compute friction terms from stresses
!
!-----------------------------------------------------------------------

!-----------------------------------------------------------------------
!
!  x-component
!
!-----------------------------------------------------------------------

   do j=this_block%jb,this_block%je
   do i=this_block%ib,this_block%ie

      work1 = H2S(i,j,bid)*S11(i,j,1) + H2N(i,j,bid)*S11(i,j,2)
      work2 = H2S(i,j,bid)*S11(i,j,4) + H2N(i,j,bid)*S11(i,j,3)
      work3 = (H2S(i+1,j,bid)*S11(i+1,j,1) + &
               H2N(i+1,j,bid)*S11(i+1,j,2))*GE(i,j)
      work4 = (H2S(i-1,j,bid)*S11(i-1,j,4) + &
               H2N(i-1,j,bid)*S11(i-1,j,3))*GW(i,j)

      !*** <h2*S11>_e - <h2*S11>_w
      FX = p25*(work2 + work3 - work1 - work4)

      work1 = H1W(i,j,bid)*S12(i,j,1) + H1E(i,j,bid)*S12(i,j,4)
      work2 = H1W(i,j,bid)*S12(i,j,2) + H1E(i,j,bid)*S12(i,j,3)
      work3 = (H1W(i,j+1,bid)*S12(i,j+1,1) + &
               H1E(i,j+1,bid)*S12(i,j+1,4))*GN(i,j)
      work4 = (H1W(i,j-1,bid)*S12(i,j-1,2) + &
               H1E(i,j-1,bid)*S12(i,j-1,3))*GS(i,j)

      !***  <h1*S12>_n*(1+h2n*k2n/2)
      !*** -<h1*S12>_s*(1-h2s*k2s/2)
      FX = FX + p25*((work2 + work3)*                       &
                     (c1 + p5*H2N(i,j,bid)*K2N(i,j,bid))    &
                   - (work1 + work4)*                       &
                     (c1 - p5*H2S(i,j,bid)*K2S(i,j,bid)))

      work1 = H2S(i,j,bid)*S22(i,j,1) + H2N(i,j,bid)*S22(i,j,2)
      work2 = H2S(i,j,bid)*S22(i,j,4) + H2N(i,j,bid)*S22(i,j,3)
      work3 = (H2S(i+1,j,bid)*S22(i+1,j,1) + &
               H2N(i+1,j,bid)*S22(i+1,j,2))*GE(i,j)
      work4 = (H2S(i-1,j,bid)*S22(i-1,j,4) + &
               H2N(i-1,j,bid)*S22(i-1,j,3))*GW(i,j)

      !*** -<h2*S22>_e*h1e*k1e/2
      !*** -<h2*S22>_w*h1w*k1w/2
      FX = FX - p125*((work2 + work3)*H1E(i,j,bid)*K1E(i,j,bid)  &
                   +  (work1 + work4)*H1W(i,j,bid)*K1W(i,j,bid))

!-----------------------------------------------------------------------
!
!     y-component
!
!-----------------------------------------------------------------------

      work1 = H1W(i,j,bid)*S22(i,j,1) + H1E(i,j,bid)*S22(i,j,4)
      work2 = H1W(i,j,bid)*S22(i,j,2) + H1E(i,j,bid)*S22(i,j,3)
      work3 = (H1W(i,j+1,bid)*S22(i,j+1,1) + &
               H1E(i,j+1,bid)*S22(i,j+1,4))*GN(i,j)
      work4 = (H1W(i,j-1,bid)*S22(i,j-1,2) + &
               H1E(i,j-1,bid)*S22(i,j-1,3))*GS(i,j)

      !*** <h1*S22>_n - <h1*S22>_s
      FY = p25*(work2 + work3 - work1 - work4)

      work1 = H2S(i,j,bid)*S12(i,j,1) + H2N(i,j,bid)*S12(i,j,2)
      work2 = H2S(i,j,bid)*S12(i,j,4) + H2N(i,j,bid)*S12(i,j,3)
      work3 = (H2S(i+1,j,bid)*S12(i+1,j,1) + &
               H2N(i+1,j,bid)*S12(i+1,j,2))*GE(i,j)
      work4 = (H2S(i-1,j,bid)*S12(i-1,j,4) + &
               H2N(i-1,j,bid)*S12(i-1,j,3))*GW(i,j)

      !***  <h2*S12>_e*(1+h1e*k1e/2)
      !*** -<h2*S12>_w*(1-h1w*k1w/2)
      FY = FY + p25*((work2 + work3)*                       &
                     (c1 + p5*H1E(i,j,bid)*K1E(i,j,bid))    &
                   - (work1 + work4)*                       &
                     (c1 - p5*H1W(i,j,bid)*K1W(i,j,bid)))

      work1 = H1W(i,j,bid)*S11(i,j,1) + H1E(i,j,bid)*S11(i,j,4)
      work2 = H1W(i,j,bid)*S11(i,j,2) + H1E(i,j,bid)*S11(i,j,3)
      work3 = (H1W(i,j+1,bid)*S11(i,j+1,1) + &
               H1E(i,j+1,bid)*S11(i,j+1,4))*GN(i,j)
      work4 = (H1W(i,j-1,bid)*S11(i,j-1,2) + &
               H1E(i,j-1,bid)*S11(i,j-1,3))*GS(i,j)

      !*** -<h1*S11>_n*h2n*k2n/2 - <h1*S11>_s*h2s*k2s/2
      FY = FY - p125*((work2 + work3)*H2N(i,j,bid)*K2N(i,j,bid)  &
                   +  (work1 + work4)*H2S(i,j,bid)*K2S(i,j,bid))

!-----------------------------------------------------------------------
!
!     divide by U-cell area
!
!-----------------------------------------------------------------------

      if (KMU(i,j,bid) >= k) then
         HDUK(i,j) = FX/UAREA(i,j,bid)
         HDVK(i,j) = FY/UAREA(i,j,bid)
      else
         HDUK(i,j) = c0
         HDVK(i,j) = c0
      endif

   enddo  ! (i-loop)
   enddo  ! (j-loop)

!-----------------------------------------------------------------------
!
!  compute horiz diffusion cfl diagnostics if required
!
!-----------------------------------------------------------------------

   if (ldiag_cfl .and. .not. lsmag_aniso) then

      if (lvariable_hmix_aniso) then
         HDIFFCFL = merge(c4*F_PARA(:,:,k,bid)*                   &
                          (DXUR(:,:,bid)**2 + DYUR(:,:,bid)**2),  &
                          c0, KMU(:,:,bid) > k)
      else
         HDIFFCFL = merge(c4*visc_para*                           &
                          (DXUR(:,:,bid)**2 + DYUR(:,:,bid)**2),  &
                          c0, KMU(:,:,bid) > k)
      endif
      HDIFFCFL = abs(HDIFFCFL)
      call cfl_hdiff(k,bid,HDIFFCFL,2,this_block)

   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine hdiffu_aniso

!***********************************************************************
!BOP
! !IROUTINE: compute_ccsm_var_viscosity
! !INTERFACE:

 subroutine compute_ccsm_var_viscosity

! !DESCRIPTION:
!  This routine computes spatially-varying anisotropic viscosity
!  coefficients similar to NCOM.
!  \begin{equation}
!   \nu_A = F_{PARA} \\ 
!         = \max{(0.5*visc_vel_scale(z)*A*\max[dx,dy],vconst_6}
!
!  \end{equation}
!   where
!          A = 0.425 * cos(pi*y*radian/30) + 0.575   for |y*radian| < 30
!          A = 0.15                                  otherwise 
!
!   Here, A provides a horizontal variation for visc_vel_scale.
!
!   "B_viscosity" = F_PERP = max( bu, bv)
!
!   and 
!        F_PARA = min(F_PARA, AMAX_CFL),
!        F_PERP = min(F_PERP, AMAX_CFL) 
!
!   are enforced in init_aniso and hdiffu_aniso for the lvariable_hmix_aniso
!   and lsmag_aniso choices, respectively. 
!
!   In the above equations, 
!
!        bu  = vconst_1 * ( 1 + vconst_2
!             * ( 1 + cos( 2*y + pi ) ) )
!        bv  = vconst_3 * beta_f * dx^3
!             * exp( - (vconst_4 * distance)^2 )
!
!   with 
!        beta_f         (x,y)   = 2 * omega * cos(ULAT(i,j)) / radius
!        distance       (x,y,z) = actual distance to "vconst_5" points
!                                 west of the nearest western boundary
!        dx             (x,y)   = DXU(i,j)
!        dy             (x,y)   = DYU(i,j)
!        visc_vel_scale (z)     = 100.0 * exp(-zt(k)/visc_vel_scale_length)
!        visc_vel_scale_length  = e-folding scale ( = 1500.0e2 cm)
!        y              (x,y)   = ULAT(i,j), latitude of "u/v" grid pts in radians 
!
!   Also, "vconst_#" are input parameters defined in namelist hmix_aniso_nml. 
!   note that "vconst_1", "vconst_6", and "vconst_4" have dimensions of cm^2/s,
!   cm^2/s, and 1/cm, respectively. "vconst_5" is an INTEGER.
!
!   NOTE: The nearest western boundary computations are done along the
!         model longitudinal grid lines. Therefore, the viscosity
!         coefficients based on these are only approximate in the high
!         Northern Hemisphere when used with non-grid-aligned options.
!
!-------------------------------------------------------------------------
!
!
! !REVISION HISTORY:
!     written by:	Stephen Yeager 3/2000
!     modified by:      GD (08/2001)

! !INPUT PARAMETERS:

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables:
!
!-----------------------------------------------------------------------

   integer (int_kind) ::       &
      i,j,k,ie,is,n,ii,        &! dummy loop counters
      index,                   &! index of nearest western boundary point
      indexo,                  &! index of nwbp + buffer
      ig,jg,igp1,              &! dummy loop counters (global)
      iblock,                  &! block counter
      ncount                    ! number of western boundaries 	

   real (r8) ::                &
      visc_vel_scale,          &! viscous velocity scale		
      bu, bv                    ! B_viscosity terms

   integer (int_kind), dimension(nx_global) :: &
      iwp

   integer (int_kind), dimension(nx_global,ny_global) :: &
      NWBP_G,                  &! nearest western boundary point (global)
      KMU_G   			! kmt array at U points

   real (r8), parameter ::     &
      vvsl = 1500.e2_r8,       &! visc_vel_scale_length (cm)
      dist_max = 1.e10_r8       ! distance for ACC region

   real (r8), dimension(nx_global,ny_global) :: &
      HTN_G,                   &! zonal distance between U points
      DIST_G                    ! distance to nwbp (cm)     

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic) :: &
      BETA_F,                  &! Coriolis parameter
      DIST                      ! distance to nearest western boundary

!-----------------------------------------------------------------------
!
!  initialize variables and gather global versions of HTN, KMU
!
!-----------------------------------------------------------------------

   do iblock=1,nblocks_clinic
      BETA_F(:,:,iblock) = c2*omega*cos(ULAT(:,:,iblock))/radius
      F_PARA(:,:,:,iblock) = c0
      F_PERP(:,:,:,iblock) = c0
   end do

   call gather_global(HTN_G, HTN, master_task, distrb_clinic)
   call gather_global(KMU_G, KMU, master_task, distrb_clinic)

   do k=1,km

      !***
      !*** determine nearest western boundary
      !*** (master loops over global domain)
      !***

      if (my_task == master_task) then

         NWBP_G = 0

         do jg=1,ny_global

            ncount = 0
            iwp = 0

            do ig=1,nx_global
               igp1 = ig + 1
               if ( ig == nx_global )  igp1 = 1
               if ( (KMU_G(ig  ,jg) <  k) .and. &
                    (KMU_G(igp1,jg) >= k) ) then 
                  ncount = ncount + 1
                  iwp(ncount) = ig
               endif
            enddo
            if ( ncount > 0 ) then
               do n=1,ncount-1
                  is = iwp(n)
                  ie = iwp(n+1) - 1
                  do ig = is, ie
                    NWBP_G(ig,jg) = is
                  enddo
               enddo
               do ig=1,nx_global
                  if (NWBP_G(ig,jg) == 0 ) &
                      NWBP_G(ig,jg) = iwp(ncount)
               enddo
            endif
         enddo

         !***
         !*** determine distance to nearest western boundary
         !*** (master loops over global domain)
         !***

         do jg=1,ny_global
         do ig=1,nx_global

            index  = NWBP_G(ig,jg)
            indexo = index + vconst_5

            if ( index == 0) then
               DIST_G(ig,jg) = dist_max
            elseif ( ig >= index  .and.  ig <= indexo ) then
               DIST_G(ig,jg) = c0
            elseif ( (ig > indexo) ) then
               DIST_G(ig,jg) = HTN_G(ig,jg) + DIST_G(ig-1,jg)
            elseif ( ig < index ) then
               if (indexo <= nx_global) then
                  if (ig == 1) then
                     DIST_G(ig,jg) = c0
                     do ii=indexo+1,nx_global
                        DIST_G(ig,jg) = HTN_G(ii,jg) + DIST_G(ig,jg)
                     enddo
                     DIST_G(ig,jg) = HTN_G(ig,jg) + DIST_G(ig,jg)
                  else
                     DIST_G(ig,jg) = HTN_G(ig,jg) + DIST_G(ig-1,jg)
                  endif
               else
                  if (ig <= (indexo - nx_global)) then
                     DIST_G(ig,jg) = c0
                  else
                     DIST_G(ig,jg) = HTN_G(ig,jg) + DIST_G(ig-1,jg)
                  endif
               endif
            endif
	
         enddo
         enddo
      endif 	! (my_task == master_task)

      call scatter_global(DIST, DIST_G, master_task, distrb_clinic, &
                          field_loc_NEcorner, field_type_scalar)

      !***
      !*** calculate viscosity coefficients
      !*** (all processors loop over local subdomain)
      !***

      do iblock=1,nblocks_clinic
      do j=1,ny_block
      do i=1,nx_block

         visc_vel_scale = 100.0_r8 * exp(-zt(k)/vvsl)

         ! use bu as temp

         bu = 0.15_r8
         if ( abs(ULAT(i,j,iblock)*radian) < 30._r8 )  &
            bu = 0.425_r8*cos(pi*ULAT(i,j,iblock)*radian/30._r8) + &
                 0.575_r8

         F_PARA(i,j,k,iblock) = max(p5*visc_vel_scale*bu* &
                                    max(DXU(i,j,iblock),  &
                                        DYU(i,j,iblock)), vconst_6 )

         bu = vconst_1*(c1 + vconst_2*(c1 + &
                                       cos((c2*ULAT(i,j,iblock))+pi)))
         bv = vconst_3*BETA_F(i,j,iblock)*(DXU(i,j,iblock)**3)
         bv = bv*exp(-(vconst_4*DIST(i,j,iblock))**2)

         F_PERP(i,j,k,iblock) = max(bu,bv)
 
         ! the diffusive CFL criteria will be enforced in subroutine
         ! init_aniso

      enddo	! i
      enddo	! j
      enddo	! iblock
   enddo	! k

!-----------------------------------------------------------------------
!EOC

 end subroutine compute_ccsm_var_viscosity

!***********************************************************************
!BOP
! !IROUTINE: read_var_viscosity
! !INTERFACE:

 subroutine read_var_viscosity(infile, infile_fmt)

! !DESCRIPTION:
!  This routine reads the spatially-varying anisotropic viscosity
!  factors F_PARA, F_PERP from an input file.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      infile,                   &! input file name (with path)
      infile_fmt                 ! input file format (bin or nc)

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   type (datafile) ::     &
      visc_file           ! io file type for viscosity file

   type (io_field_desc) ::     &
      F_PARA_d, F_PERP_d        ! descriptors for para/perp visc factors

   type (io_dim) :: &
      i_dim, j_dim, &! dimension descriptors for horiz dims
      k_dim          ! dimension descriptor  for vertical levels

!-----------------------------------------------------------------------
!
!  construct io file type and open for reading
!
!-----------------------------------------------------------------------

   visc_file =  construct_file(infile_fmt, full_name=infile,     &
                               record_length=rec_type_dbl,       &
                               recl_words=nx_global*ny_global)

   call data_set(visc_file, 'open_read')

!-----------------------------------------------------------------------
!
!  define variables to be read
!
!-----------------------------------------------------------------------

   !*** define dimensions

   i_dim = construct_io_dim('i', nx_global)
   j_dim = construct_io_dim('j', ny_global)
   k_dim = construct_io_dim('k', km)

   F_PARA_d = construct_io_field('F_PARA', i_dim, j_dim, dim3=k_dim,  &
                   long_name='parallel variable viscosity ',          &
                   units    ='cm2/s',                                 &
                   grid_loc ='3221',                                  &
                   field_loc = field_loc_NEcorner,                    &
                   field_type = field_type_scalar,                    &
                   d3d_array = F_PARA)

   F_PERP_d = construct_io_field('F_PERP', i_dim, j_dim, dim3=k_dim,  &
                   long_name='perpendicular variable viscosity',      &
                   units    ='cm2/s',                                 &
                   grid_loc ='3221',                                  &
                   field_loc = field_loc_NEcorner,                    &
                   field_type = field_type_scalar,                    &
                   d3d_array = F_PERP)

   call data_set (visc_file, 'define', F_PARA_d)
   call data_set (visc_file, 'define', F_PERP_d)

!-----------------------------------------------------------------------
!
!  read arrays then clean up
!
!-----------------------------------------------------------------------

   call data_set (visc_file, 'read', F_PARA_d)
   call data_set (visc_file, 'read', F_PERP_d)

   call destroy_io_field (F_PARA_d)
   call destroy_io_field (F_PERP_d)

   call data_set (visc_file, 'close')
   call destroy_file(visc_file)

!-----------------------------------------------------------------------
!EOC

 end subroutine read_var_viscosity

!***********************************************************************
!BOP
! !IROUTINE: write_var_viscosity
! !INTERFACE:

 subroutine write_var_viscosity(outfile, outfile_fmt)

! !DESCRIPTION:
!  This routine writes the spatially-varying anisotropic viscosity
!  factors F_PARA, F_PERP which have been computed internally.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      outfile,                  &! input file name (with path)
      outfile_fmt                ! input file format (bin or nc)

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   type (datafile) ::     &
      visc_file           ! io file type for viscosity file

   type (io_field_desc) ::     &
      F_PARA_d, F_PERP_d        ! descriptors for para/perp visc factors

   type (io_dim) :: &
      i_dim, j_dim, &! dimension descriptors for horiz dims
      k_dim          ! dimension descriptor  for vertical levels

!-----------------------------------------------------------------------
!
!  construct io file type and open for writing
!
!-----------------------------------------------------------------------

   visc_file =  construct_file(outfile_fmt, root_name=outfile,    &
                               record_length=rec_type_dbl,       &
                               recl_words=nx_global*ny_global)

   call data_set(visc_file, 'open')

!-----------------------------------------------------------------------
!
!  define variables to be written
!
!-----------------------------------------------------------------------

   !*** define dimensions

   i_dim = construct_io_dim('i', nx_global)
   j_dim = construct_io_dim('j', ny_global)
   k_dim = construct_io_dim('k', km)

   F_PARA_d = construct_io_field('F_PARA', i_dim, j_dim, dim3=k_dim,  &
                   long_name='parallel variable viscosity',           &
                   units    ='cm2/s',                                 &
                   grid_loc ='3221',                                  &
                   field_loc = field_loc_NEcorner,                    &
                   field_type = field_type_scalar,                    &
                   d3d_array = F_PARA)

   F_PERP_d = construct_io_field('F_PERP', i_dim, j_dim, dim3=k_dim,  &
                   long_name='perpendicular variable viscosity',      &
                   units    ='cm2/s',                                 &
                   grid_loc ='3221',                                  &
                   field_loc = field_loc_NEcorner,                    &
                   field_type = field_type_scalar,                    &
                   d3d_array = F_PERP)

   call data_set (visc_file, 'define', F_PARA_d)
   call data_set (visc_file, 'define', F_PERP_d)

!-----------------------------------------------------------------------
!
!  write arrays then clean up
!
!-----------------------------------------------------------------------

   call data_set (visc_file, 'write', F_PARA_d)
   call data_set (visc_file, 'write', F_PERP_d)

   call destroy_io_field (F_PARA_d)
   call destroy_io_field (F_PERP_d)

   call data_set (visc_file, 'close')
   call destroy_file(visc_file)

!-----------------------------------------------------------------------
!EOC

 end subroutine write_var_viscosity

!***********************************************************************

 end module hmix_aniso

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
