!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module grid

!BOP
! !MODULE: grid
!
! !DESCRIPTION:
!  This module contains grid info and routines for setting up the
!  POP grid quantities.
!
! !REVISION HISTORY:
!  CVS:$Id: grid.F90,v 1.31 2004/01/21 20:19:14 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod
   use communicate
   use blocks
   use distribution
   use domain_size
   use domain
   use constants
   use io
   use broadcast
   use gather_scatter
   use global_reductions
   use boundary
   use exit_mod

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public  :: init_grid1,     &
              init_grid2,     &
              tgrid_to_ugrid, &
              ugrid_to_tgrid, &
              fill_points

! !PUBLIC DATA MEMBERS:

   real (r8), public :: &
      area_u, area_t       ,&! total ocean area of U,T cells
      volume_u, volume_t   ,&! total ocean volume of U,T cells
      volume_t_marg        ,&! volume of marginal seas (T cells)
      area_t_marg          ,&! area of marginal seas (T cells)
      uarea_equator          ! area of equatorial cell

   real (r8), dimension(km), public :: &
      area_t_k             ,&! total ocean area (T cells) at each dpth
      volume_t_k           ,&! total ocean volume (T cells) at each dpth
      volume_t_marg_k        ! tot marginal seas vol (T cells) at each dpth

   integer (int_kind), public :: &
      sfc_layer_type         ! choice for type of surface layer

   integer (int_kind), parameter, public :: &
      sfc_layer_varthick = 1,  &! variable thickness surface layer
      sfc_layer_rigid    = 2,  &! rigid lid surface layer
      sfc_layer_oldfree  = 3    ! old free surface form

   logical (log_kind), public ::    &
      topo_smooth,          &! flag to smooth topography
      partial_bottom_cells   ! flag for partial bottom cells

   integer (int_kind), dimension(:,:), allocatable, public :: &
      KMT_G            ! k index of deepest grid cell on global T grid
                       ! for use in performing work distribution

!-----------------------------------------------------------------------
!
!  grid information for all local blocks
!  the local blocks are by default in baroclinic distribution
!
!-----------------------------------------------------------------------

   !*** dimension(1:km)

   real (r8), dimension(km), public :: &
      dz                ,&! thickness of layer k
      c2dz              ,&! 2*dz
      dzr, dz2r         ,&! reciprocals of dz, c2dz
      zt                ,&! vert dist from sfc to midpoint of layer
      zw                  ! vert dist from sfc to bottom of layer

   !*** dimension(0:km)

   real (r8), dimension(0:km), public :: &
      dzw, dzwr          ! midpoint of k to midpoint of k+1
                         !   and its reciprocal

   !*** geometric 2d arrays

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), public :: &
      DXU, DYU            ,&! {x,y} spacing centered at U points
      DXT, DYT            ,&! {x,y} spacing centered at T points
      DXUR, DYUR          ,&! reciprocals of DXU, DYU
      DXTR, DYTR          ,&! reciprocals of DXT, DYT
      HTN, HTE            ,&! cell widths on {N,E} sides of T cell
      HUS, HUW            ,&! cell widths on {S,W} sides of U cell
      ULAT, ULON          ,&! {latitude,longitude} of U points
      TLAT, TLON          ,&! {latitude,longitude} of U points
      ANGLE, ANGLET       ,&! angle grid makes with latitude line
      FCOR, FCORT         ,&! coriolis parameter at U,T points
      UAREA, TAREA        ,&! area of U,T cells
      UAREA_R, TAREA_R    ,&! reciprocal of area of U,T cells
      HT, HU, HUR           ! ocean depth at T,U points

   !*** 3d depth fields for partial bottom cells

   real (r8), dimension(:,:,:,:), allocatable, public :: &
      DZU, DZT               ! thickness of U,T cell for pbc

   !*** 2d landmasks

   integer (int_kind), dimension(nx_block,ny_block,max_blocks_clinic), &
      public :: &
      KMT            ,&! k index of deepest grid cell on T grid
      KMU            ,&! k index of deepest grid cell on U grid
      KMTOLD           ! KMT field before smoothing

   logical (log_kind), dimension(nx_block,ny_block,max_blocks_clinic), &
      public :: &
      CALCT          ,&! flag=true if point is an ocean point
      CALCU            !   at the surface

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), public :: &
      RCALCT         ,&! real equiv of CALCT,U to use as more
      RCALCU           !   efficient multiplicative mask

   integer (int_kind), dimension(nx_block,ny_block,max_blocks_clinic), &
      public :: &
      KMTN,KMTS,KMTE,KMTW   ,&! KMT field at neighbor points
      KMUN,KMUS,KMUE,KMUW     ! KMU field at neighbor points

   integer (int_kind), dimension(nx_block,ny_block,max_blocks_clinic), &
      public :: &
      KMTEE,KMTNN      ! KMT field 2 cells away for upwind stencil
                       ! allocated and computed in advection module

   integer (int_kind), dimension(:,:,:), allocatable, public :: &
      REGION_MASK      ! mask defining regions, marginal seas

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  module private data
!
!-----------------------------------------------------------------------

   !*** geometric scalars

   integer (int_kind) ::       &
      jeq,                     &! j index of equatorial cell
      nocean_u, nocean_t,      &! num of ocean U,T points
      nsurface_u, nsurface_t    ! num of ocean U,T points at surface

   logical (log_kind) ::  &
      flat_bottom,        &! flag for flat-bottom topography
      lremove_points       ! flag for removing isolated points

   real (r8), dimension(:,:), allocatable :: &
      ULAT_G, ULON_G        ! {latitude,longitude} of U points
                            ! in global-sized array

!-----------------------------------------------------------------------
!
!     area-weighted averaging coefficients
!     AT{0,S,W,SW} = {central,s,w,sw} coefficients for area-weighted
!       averaging of four U points surrounding a T point
!     AU{0,N,E,NE} = {central,n,e,ne} coefficients for area-weighted
!       averaging of four T points surrounding a U point
!
!-----------------------------------------------------------------------

   real (r8), dimension (nx_block,ny_block,max_blocks_clinic) :: &
      AT0,ATS,ATW,ATSW,AU0,AUN,AUE,AUNE

!-----------------------------------------------------------------------
!
!  variables which are shared between init_grid1,init_grid2
!
!-----------------------------------------------------------------------

   character (char_len) ::  &
      horiz_grid_opt,       &! horizontal grid option
      vert_grid_opt,        &! vertical grid option
      sfc_layer_opt,        &! choice for surface layer type
      topography_opt,       &! topography (KMT) option
      horiz_grid_file,      &! input file for reading horiz grid info
      vert_grid_file,       &! input file for reading horiz grid info
      topography_file,      &! input file for reading horiz grid info
      region_mask_file,     &! input file for region mask
      bottom_cell_file       ! input file for thickness of pbc

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_grid1
! !INTERFACE:

 subroutine init_grid1

! !DESCRIPTION:
!  Initializes only grid quantities necessary for completing
!  decomposition setup (ULAT, ULON, KMT).
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

   namelist /grid_nml/horiz_grid_opt, vert_grid_opt, topography_opt,   &
                      horiz_grid_file, vert_grid_file, topography_file,&
                      topo_smooth, flat_bottom, lremove_points,        &
                      region_mask_file, sfc_layer_opt,                 &
                      partial_bottom_cells, bottom_cell_file

   integer (int_kind) :: &
      nml_error           ! namelist i/o error flag

!-----------------------------------------------------------------------
!
!  read input namelist for grid setup options
!
!-----------------------------------------------------------------------

   horiz_grid_opt       = 'internal'
   vert_grid_opt        = 'internal'
   sfc_layer_opt        = 'varthick'
   topography_opt       = 'internal'
   horiz_grid_file      = 'unknown_horiz_grid_file'
   vert_grid_file       = 'unknown_vert_grid_file'
   topography_file      = 'unknown_topography_file'
   region_mask_file     = 'unknown_region_mask'
   topo_smooth          = .false.
   flat_bottom          = .false.
   lremove_points       = .false.
   partial_bottom_cells = .false.
   bottom_cell_file     = 'unknown_bottom_cell_file'

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=grid_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading grid_nml')
   endif

   call broadcast_scalar(horiz_grid_opt,       master_task)
   call broadcast_scalar(vert_grid_opt ,       master_task)
   call broadcast_scalar(sfc_layer_opt ,       master_task)
   call broadcast_scalar(topography_opt,       master_task)
   call broadcast_scalar(topo_smooth,          master_task)
   call broadcast_scalar(flat_bottom,          master_task)
   call broadcast_scalar(lremove_points,       master_task)
   call broadcast_scalar(region_mask_file,     master_task)
   call broadcast_scalar(partial_bottom_cells, master_task)

   if (partial_bottom_cells) then
      call broadcast_scalar(bottom_cell_file, master_task)
   endif

!-----------------------------------------------------------------------
!
!  get global ULAT,ULON
!
!-----------------------------------------------------------------------

   select case (horiz_grid_opt)
   case ('internal')
      call horiz_grid_internal(.true.)
   case ('file')
      call broadcast_scalar(horiz_grid_file, master_task)
      call read_horiz_grid(horiz_grid_file,.true.)
   case default
      call exit_POP(sigAbort,'ERROR: unknown horizontal grid option')
   end select

!-----------------------------------------------------------------------
!
!  set up topography by getting global KMT field (used for
!  creating a load balanced block distribution).
!
!-----------------------------------------------------------------------

   select case (topography_opt)
   case ('internal')
      call topography_internal(.true.)
      flat_bottom = .true.
   case ('file')
      call broadcast_scalar(topography_file, master_task)
      call read_topography(topography_file,.true.)
   case default
      call exit_POP(sigAbort,'ERROR: unknown topography option')
   end select

!-----------------------------------------------------------------------
!EOC

 end subroutine init_grid1

!***********************************************************************
!BOP
! !IROUTINE: init_grid2
! !INTERFACE:

 subroutine init_grid2

! !DESCRIPTION:
!  Initializes all grid quantities
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
      i,j,k,n,iblock,    &! dummy loop index variables
      range_count         ! counter for angle out of range

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic) :: &
      WORK                 ! local temp space

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic) :: &
      DZBC                 ! thickness of bottom T cell for pbc

   character (*), parameter ::  &! output formats
      vgrid_fmt1 = "(3x,' k ',3x,'Thickness (cm)',3x,' Depth (cm) ')", &
      vgrid_fmt2 = "(3x,'---',3x,'--------------',3x,'------------')", &
      vgrid_fmt3 = "(3x,i3,4x,1pe12.5,4x,1pe12.5)"                   , &
      topo_fmt1  = "(' # surface (T,U) points',2x,i10,2x,i10)"       , &
      topo_fmt2  = "(' # ocean   (T,U) points',2x,i10,2x,i10)"       , &
      topo_fmt3  = "(' T-area,   U-area   (km**2)',2(2x,1pe23.15))"  , &
      topo_fmt4  = "(' T-volume, U-volume (km**3)',2(2x,1pe23.15))"

   type (block) :: &
      this_block  ! block info for current block

   real (r8) ::         &
      angle_0, angle_w, &! temporaries for computing angle at T points
      angle_s, angle_sw 

!-----------------------------------------------------------------------
!
!  output grid setup options to log file
!
!-----------------------------------------------------------------------

   if (my_task == master_task) then
      write(stdout,delim_fmt)
      write(stdout,blank_fmt)
      write(stdout,'(a13)') ' Grid options'
      write(stdout,blank_fmt)
      write(stdout,delim_fmt)
   endif

!-----------------------------------------------------------------------
!
!  set up horizontal grid
!
!-----------------------------------------------------------------------

   select case (horiz_grid_opt)
   case ('internal')
      if (my_task == master_task) then
         write(stdout,'(a36)') ' Creating horizontal grid internally'
      endif
      call horiz_grid_internal(.false.)
   case ('file')
      if (my_task == master_task) then
         write(stdout,*) ' Reading horizontal grid from file:', &
                         trim(horiz_grid_file)
      endif
      call broadcast_scalar(horiz_grid_file, master_task)
      call read_horiz_grid(horiz_grid_file,.false.)
   case default
      call exit_POP(sigAbort,'ERROR: unknown horizontal grid option')
   end select

!-----------------------------------------------------------------------
!
!  if boundaries are closed, extend physical domain values into ghost
!  cells
!  compute other derived quantities like areas and reciprocals
!
!-----------------------------------------------------------------------

   !$OMP PARALLEL DO PRIVATE(i,j,this_block)
   do iblock=1,nblocks_clinic
      this_block = get_block(blocks_clinic(iblock),iblock)

      if (this_block%i_glob(1) == 0) then ! closed western bndy
         do j=1,ny_block
         do i=1,this_block%ib-1
            DXU(i,j,iblock) = DXU(this_block%ib,j,iblock)
            DYU(i,j,iblock) = DYU(this_block%ib,j,iblock)
            DXT(i,j,iblock) = DXT(this_block%ib,j,iblock)
            DYT(i,j,iblock) = DYT(this_block%ib,j,iblock)
         end do
         end do
      endif

      if (this_block%i_glob(this_block%ie+1) == 0) then ! closed east bndy
         do j=1,ny_block
         do i=this_block%ie+1,nx_block
            DXU(i,j,iblock) = DXU(this_block%ie,j,iblock)
            DYU(i,j,iblock) = DYU(this_block%ie,j,iblock)
            DXT(i,j,iblock) = DXT(this_block%ie,j,iblock)
            DYT(i,j,iblock) = DYT(this_block%ie,j,iblock)
         end do
         end do
      endif

      if (this_block%j_glob(1) == 0) then ! closed southern bndy
         do j=1,this_block%jb-1
         do i=1,nx_block
            DXU(i,j,iblock) = DXU(i,this_block%jb,iblock)
            DYU(i,j,iblock) = DYU(i,this_block%jb,iblock)
            DXT(i,j,iblock) = DXT(i,this_block%jb,iblock)
            DYT(i,j,iblock) = DYT(i,this_block%jb,iblock)
         end do
         end do
      endif

      if (this_block%j_glob(this_block%je+1) == 0) then ! closed north bndy
         do j=this_block%je+1,ny_block
         do i=1,nx_block
            DXU(i,j,iblock) = DXU(i,this_block%je,iblock)
            DYU(i,j,iblock) = DYU(i,this_block%je,iblock)
            DXT(i,j,iblock) = DXT(i,this_block%je,iblock)
            DYT(i,j,iblock) = DYT(i,this_block%je,iblock)
         end do
         end do
      endif

      DXUR(:,:,iblock) = c1/DXU(:,:,iblock)
      DYUR(:,:,iblock) = c1/DYU(:,:,iblock)

      UAREA(:,:,iblock) = DXU(:,:,iblock)*DYU(:,:,iblock)
      UAREA_R(:,:,iblock) = c1/UAREA(:,:,iblock)

      DXTR(:,:,iblock) = c1/DXT(:,:,iblock)
      DYTR(:,:,iblock) = c1/DYT(:,:,iblock)

      TAREA(:,:,iblock) = DXT(:,:,iblock)*DYT(:,:,iblock)
      TAREA_R(:,:,iblock) = c1/TAREA(:,:,iblock)

   end do
   !$OMP END PARALLEL DO

!-----------------------------------------------------------------------
!
!  calculate stencil coefficients for area-averaging
!
!-----------------------------------------------------------------------

   call cf_area_avg  ! coefficients for area-weighted averages

!-----------------------------------------------------------------------
!
!  calculate lat/lon of T points and calculate ANGLET from ANGLE
!
!-----------------------------------------------------------------------

   call calc_tpoints

   !***
   !*** first, ensure that -pi <= ANGLE <= pi
   !***

   range_count = global_count ((ANGLE < - pi .or. ANGLE > pi), &
                               distrb_clinic, field_loc_NEcorner)

   if (range_count > 0) call exit_POP(sigAbort, &
                        'ERROR: ANGLE is outside its expected range')

   !***
   !*** compute ANGLE on T-grid
   !***

   !$OMP PARALLEL DO PRIVATE (i,j,angle_0,angle_w,angle_s,angle_sw, & 
   !$OMP                      this_block)

   do n=1,nblocks_clinic
      this_block = get_block(blocks_clinic(n),n)

      do j=this_block%jb,this_block%je
         do i=this_block%ib,this_block%ie

            angle_0  = ANGLE(i,  j  ,n)
            angle_w  = ANGLE(i-1,j  ,n)
            angle_s  = ANGLE(i,  j-1,n)
            angle_sw = ANGLE(i-1,j-1,n)

            if ( angle_0 < c0 ) then
               if ( abs(angle_w -angle_0) > pi ) &
                  angle_w  = angle_w  - pi2 
               if ( abs(angle_s -angle_0) > pi ) &
                  angle_s  = angle_s  - pi2 
               if ( abs(angle_sw-angle_0) > pi ) &
                  angle_sw = angle_sw - pi2 
            endif

            ANGLET(i,j,n) =  angle_0 *AT0 (i,j,n) + &
                             angle_w *ATW (i,j,n) + &
                             angle_s *ATS (i,j,n) + &
                             angle_sw*ATSW(i,j,n)

         enddo

         !***
         !*** set ANGLET to zero for all of (global) j=1 row 
         !*** (bottom row of ANGLET is not used, but is written to file)
         !***

         if (this_block%j_glob(j) == 1) ANGLET(:,j,n) = c0

      enddo
   enddo
   !$OMP END PARALLEL DO

   call update_ghost_cells(ANGLET, bndy_clinic, field_loc_center, &
                                                field_type_angle)

!-----------------------------------------------------------------------
!
!  set up vertical grid
!
!-----------------------------------------------------------------------

   select case (trim(sfc_layer_opt))
   case ('varthick')  ! variable thickness sfc layer
      if (my_task == master_task) write(stdout,'(a39)') &
         ' Using variable thickness surface layer'
      sfc_layer_type = sfc_layer_varthick
   case ('rigid')     ! rigid lid
      if (my_task == master_task) write(stdout,'(a30)') &
         ' Using rigid lid approximation'
      sfc_layer_type = sfc_layer_rigid
   case ('oldfree')   ! old free surface implementation
      if (my_task == master_task) write(stdout,'(a40)') &
         ' Using original free surface formulation'
      sfc_layer_type = sfc_layer_oldfree
   case default
      call exit_POP(sigAbort,'ERROR: unknown surface layer option')
   end select

   select case (vert_grid_opt)
   case ('internal')
      if (my_task == master_task) then
         write(stdout,'(a34)') ' Creating vertical grid internally'
      endif
      call vert_grid_internal
   case ('file')
      if (my_task == master_task) then
         write(stdout,*) ' Reading vertical grid from file:', &
                         trim(vert_grid_file)
      endif
      call broadcast_scalar(vert_grid_file, master_task)
      call read_vert_grid(vert_grid_file)
   case default
      call exit_POP(sigAbort,'ERROR: unknown vertical grid option')
   end select

   !***
   !*** calculate other vertical grid quantities
   !***

   dzw(0)  = p5*dz(1)
   dzw(km) = p5*dz(km)
   dzwr(0) = c1/dzw(0)
   zw(1) = dz(1)
   zt(1) = dzw(0)

   do k = 1,km-1
      dzw(k) = p5*(dz(k) + dz(k+1))
      zw(k+1) = zw(k) + dz(k+1)
      zt(k+1) = zt(k) + dzw(k)
   enddo

   do k = 1,km
      c2dz(k) = c2*dz(k)
      dzr(k)  = c1/dz(k)
      dz2r(k) = c1/c2dz(k)
      dzwr(k) = c1/dzw(k)
   enddo

   if (my_task == master_task) then
      write(stdout,'(a15)') ' Vertical grid:'
      write(stdout,vgrid_fmt1)
      write(stdout,vgrid_fmt2)
      do k=1,km
         write(stdout,vgrid_fmt3) k,dz(k),zt(k)
      end do
   endif

!-----------------------------------------------------------------------
!
!  set up topography
!
!-----------------------------------------------------------------------

   select case (topography_opt)
   case ('internal')
      if (my_task == master_task) write(stdout,'(a33)') &
         ' Generating topography internally'
      call topography_internal(.false.)
      flat_bottom = .true.
   case ('file')
      if (my_task == master_task) write(stdout,'(a30,a)') &
         ' Reading topography from file:', trim(topography_file)
      call broadcast_scalar(topography_file, master_task)
      call read_topography(topography_file,.false.)
   case default
      call exit_POP(sigAbort,'ERROR: unknown topography option')
   end select

   !***
   !*** remove isolated lakes and disconnected points from grid
   !***

   if (lremove_points) then
      if (my_task == master_task) write(stdout,'(a58)') &
         ' Removing isolated lakes and disconnected points from grid'
      call remove_points
   endif

   !***
   !*** smooth topography
   !***

   if (topo_smooth) then
      if (my_task == master_task) write(stdout,'(a21)') &
         ' Smoothing topography'
      call smooth_topography
   endif

   !***
   !*** flat bottom
   !***

   if (flat_bottom) then
      if (my_task == master_task) write(stdout,'(a33)') &
         ' Enforcing flat-bottom topography'
      where (KMT /= 0) KMT = km
   endif

   !***
   !*** set up partial bottom cells
   !***

   if (partial_bottom_cells) then

      if (my_task == master_task) then
         write(stdout,'(a30)') ' Partial bottom cells  enabled'
         write(stdout,'(a27,a)') ' Reading bottom cell file: ', &
                                 trim(bottom_cell_file)
      endif
      call read_bottom_cell(DZBC,bottom_cell_file)
      allocate (DZT(nx_block,ny_block,km,max_blocks_clinic), &
                DZU(nx_block,ny_block,km,max_blocks_clinic))

      !$OMP PARALLEL DO PRIVATE(k,i,j)
      do n=1,nblocks_clinic
      do k=1,km
         do j=1,ny_block
         do i=1,nx_block
            if (KMT(i,j,n) == k) then
               DZT(i,j,k,n) = DZBC(i,j,n)
            else
               DZT(i,j,k,n) = dz(k)
            end if
         end do
         end do

         !*** DZU = min of surrounding DZTs

         do j=1,ny_block-1
         do i=1,nx_block-1
            DZU(i,j,k,n) = min(DZT(i  ,j  ,k,n), &
                               DZT(i+1,j  ,k,n), &
                               DZT(i  ,j+1,k,n), &
                               DZT(i+1,j+1,k,n))
         end do
         end do
      end do
      end do
      !$OMP END PARALLEL DO
      call update_ghost_cells(DZU, bndy_clinic, field_loc_NEcorner, &
                                                field_type_scalar)

   else
      if (my_task == master_task) write(stdout,'(a30)') &
         ' Partial bottom cells disabled'
   endif

   !***
   !*** calculate number of levels at U points (KMU field)
   !*** KMU = minimum of surrounding KMTs
   !***

   do n=1,nblocks_clinic
   do j=1,ny_block-1
   do i=1,nx_block-1
      KMU(i,j,n) = min(KMT(i,j  ,n),KMT(i+1,j  ,n), &
                       KMT(i,j+1,n),KMT(i+1,j+1,n))
   end do
   end do
   end do
   call update_ghost_cells(KMU, bndy_clinic, field_loc_NEcorner, &
                                             field_type_scalar)

   !***
   !*** calculate depth field at T,U points
   !***

   if (partial_bottom_cells) then
      !$OMP PARALLEL DO PRIVATE(k,i,j)
      do n=1,nblocks_clinic
         HT (:,:,n) = c0
         HU (:,:,n) = c0
         HUR(:,:,n) = c0

         do k=1,km
            do j=1,ny_block
            do i=1,nx_block
               if (k == KMT(i,j,n)) HT(i,j,n) = zw(k-1) + DZT(i,j,k,n)
               if (k == KMU(i,j,n)) then
                  HU(i,j,n) = zw(k-1) + DZU(i,j,k,n)
                  HUR(i,j,n) = c1/HU(i,j,n)
               else if (k > KMU(i,j,n)) then
                  DZU(i,j,k,n) = dz(k) !*** to prevent divide by zero
               endif
            enddo
            enddo
         enddo
      enddo
      !$OMP END PARALLEL DO
   else
      !$OMP PARALLEL DO PRIVATE(k,i,j)
      do n=1,nblocks_clinic
         HT (:,:,n) = c0
         HU (:,:,n) = c0
         HUR(:,:,n) = c0

         do k=1,km
            do j=1,ny_block
            do i=1,nx_block
               if (k == KMT(i,j,n)) HT(i,j,n) = zw(k)
               if (k == KMU(i,j,n)) then
                  HU (i,j,n) = zw(k)
                  HUR(i,j,n) = c1/zw(k)
               endif
            enddo
            enddo
         enddo
      enddo
      !$OMP END PARALLEL DO
   endif

!-----------------------------------------------------------------------
!
!  landmasks
!
!-----------------------------------------------------------------------

   call landmasks

!-----------------------------------------------------------------------
!
!  calculate area, volume, # surface points, # ocean points
!
!-----------------------------------------------------------------------

   area_t   = global_sum(TAREA, distrb_clinic, field_loc_center, RCALCT)
   area_u   = global_sum(UAREA, distrb_clinic, field_loc_NEcorner, RCALCU)
   WORK = TAREA*HT
   volume_t = global_sum(WORK, distrb_clinic, field_loc_center, RCALCT)
   WORK = UAREA*HU
   volume_u = global_sum(WORK, distrb_clinic, field_loc_NEcorner, RCALCU)
   area_t_k(1) = area_t
   volume_t_k(1) = global_sum(TAREA*dz(1), distrb_clinic, &
                              field_loc_center, RCALCT)
   do k=2,km
      WORK = merge(TAREA, c0, k <= KMT)
      area_t_k(k) = global_sum(WORK, distrb_clinic, field_loc_center)
      WORK = merge(TAREA*dz(k), c0, k <= KMT)
      volume_t_k(k) = global_sum(WORK, distrb_clinic, field_loc_center)
   end do

   nsurface_t = global_count(RCALCT, distrb_clinic, field_loc_center)
   nsurface_u = global_count(RCALCU, distrb_clinic, field_loc_NEcorner)
   nocean_t = global_sum(KMT, distrb_clinic, field_loc_center)
   nocean_u = global_sum(KMU, distrb_clinic, field_loc_NEcorner)

   if (my_task == master_task) then
      write(stdout,blank_fmt)
      write(stdout,topo_fmt1) nsurface_t, nsurface_u
      write(stdout,topo_fmt2) nocean_t, nocean_u
      write(stdout,topo_fmt3) area_t*1.0e-10_r8, &
                              area_u*1.0e-10_r8
      write(stdout,topo_fmt4) volume_t*1.0e-15_r8, &
                              volume_u*1.0e-15_r8
   endif

!-----------------------------------------------------------------------
!
!  set region-masks and calculate active-ocean and marginal-seas
!  areas and volumes
!
!-----------------------------------------------------------------------

   if (trim(region_mask_file) /= 'unknown_region_mask') then
      if (my_task == master_task) write(stdout,'(a36,a)') &
         'Region masks initialized from file: ',trim(region_mask_file)
      call area_masks(region_mask_file)
   else
      if (my_task == master_task) write(stdout,'(a24)') &
         ' No region masks defined'
   endif

!-----------------------------------------------------------------------
!
!  compute min area of equatorial cell for use in variable_hmix
!
!-----------------------------------------------------------------------

   WORK = abs(ULAT)
   uarea_equator = global_minval(WORK, distrb_clinic, field_loc_NEcorner, CALCU)

   where (WORK == uarea_equator)
      WORK = UAREA
   elsewhere
      WORK = 1.e+20_r8
   end where

   uarea_equator = global_minval(WORK, distrb_clinic, field_loc_NEcorner, CALCU)

!-----------------------------------------------------------------------
!
!  compute coriolis parameter 2*omega*sin(true_latitude)
!
!-----------------------------------------------------------------------

   FCOR  = c2*omega*sin(ULAT)    ! at u-points
   FCORT = c2*omega*sin(TLAT)    ! at t-points

!-----------------------------------------------------------------------
!EOC

 end subroutine init_grid2

!***********************************************************************
!BOP
! !IROUTINE: horiz_grid_internal
! !INTERFACE:

 subroutine horiz_grid_internal(latlon_only)

! !DESCRIPTION:
!  Creates a lat/lon grid with equal spacing in each direction
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   logical (log_kind), intent(in) :: &
      latlon_only       ! flag requesting only ULAT, ULON

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j,ig,jg,jm1,n    ! dummy counters

   real (r8) :: &
      dlat, dlon,       &! lat/lon spacing for idealized grid
      lathalf,          &! lat at T points
      xdeg               ! temporary longitude variable

   type (block) :: &
      this_block    ! block info for this block

!-----------------------------------------------------------------------
!
!  calculate lat/lon coords of U points
!  long range (-180,180)
!
!-----------------------------------------------------------------------

   dlon = 360.0_r8/real(nx_global)
   dlat = 180.0_r8/real(ny_global)

   if (latlon_only) then

      allocate (ULAT_G(nx_global, ny_global), &
                ULON_G(nx_global, ny_global))

      do i=1,nx_global
         xdeg = i*dlon
         if (xdeg > 180.0_r8) xdeg = xdeg - 360.0_r8
         ULON_G(i,:) =  xdeg/radian
      enddo

      do j = 1,ny_global
         ULAT_G(:,j)  = (-90.0_r8 + j*dlat)/radian
      enddo

!-----------------------------------------------------------------------
!
!  calculate grid spacings and other quantities
!  compute here to avoid bad ghost cell values due to dropped land 
!  blocks
!
!-----------------------------------------------------------------------

   else ! not latlon_only

      !$OMP PARALLEL DO PRIVATE(this_block, i, j, ig, jg, lathalf)
      do n=1,nblocks_clinic

         this_block = get_block(blocks_clinic(n),n)

         do j=1,ny_block
            jg = this_block%j_glob(j)
            jm1 = jg - 1
            if (jm1 < 1) jm1 = ny_global

            do i=1,nx_block
               !***
               !*** calculate grid lengths
               !***

               HTN(i,j,n) = dlon*radius/radian  ! convert to cm
               HTE(i,j,n) = dlat*radius/radian  ! convert to cm
               HUS(i,j,n) = dlon*radius/radian  ! convert to cm
               HUW(i,j,n) = dlat*radius/radian  ! convert to cm
               DYT(i,j,n) = dlat*radius/radian  ! convert to cm
               DYU(i,j,n) = dlat*radius/radian  ! convert to cm
               ANGLE(i,j,n) = c0

               ig = this_block%i_glob(i)
               if (ig > 0 .and. jg > 0) then
                  ULON(i,j,n) = ULON_G(ig,jg)
                  ULAT(i,j,n) = ULAT_G(ig,jg)
                  HTN (i,j,n) = HTN(i,j,n)*cos(ULAT(i,j,n))
                  DXU (i,j,n) = HTN(i,j,n)
                  lathalf = (-90.0_r8 + (jg-p5)*dlat)/radian
                  HUS (i,j,n) = HUS(i,j,n)*cos(lathalf)
                  DXT (i,j,n) = dlon*radius/radian*       &
                                p5*(cos(ULAT_G(ig,jg )) + &
                                    cos(ULAT_G(ig,jm1)))
               else
                  ULON(i,j,n) = c0
                  ULAT(i,j,n) = c0
                  HTN (i,j,n) = c1 ! to prevent divide by zero
                  HUS (i,j,n) = c1 ! to prevent divide by zero
                  DXU (i,j,n) = c1 ! fixed up later
               endif
            end do
         enddo
      enddo
      !$OMP END PARALLEL DO

      deallocate(ULAT_G,ULON_G)

   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine horiz_grid_internal

!***********************************************************************
!BOP
! !IROUTINE: read_horiz_grid
! !INTERFACE:

 subroutine read_horiz_grid(horiz_grid_file, latlon_only)

! !DESCRIPTION:
!  Reads horizontal grid information from input grid file
!
! !REVISION HISTORY:
!  same as module

!  !INPUT PARAMETERS:

   character (*), intent(in) :: &
      horiz_grid_file     ! filename of file containing grid data

   logical (log_kind), intent(in) :: &
      latlon_only       ! flag requesting only ULAT, ULON

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j,iblock        ,&! loop counters
      ip1, im1, jp1, jm1,&! shift indexes
      nu                ,&! i/o unit number
      ioerr             ,&! i/o error flag
      reclength           ! record length

   type (block) :: &
      this_block

!-----------------------------------------------------------------------
!
!  if only lat,lon are requested, read only these
!
!-----------------------------------------------------------------------

   if (latlon_only) then

      allocate (ULAT_G(nx_global,ny_global), &
                ULON_G(nx_global,ny_global))

      INQUIRE(iolength=reclength) ULAT_G
      call get_unit(nu)
      if (my_task == master_task) then
         open(nu,file=trim(horiz_grid_file),status='old', &
              form='unformatted', access='direct', recl=reclength, &
              iostat=ioerr)
      endif

      call broadcast_scalar(ioerr, master_task)
      if (ioerr /= 0) call exit_POP(sigAbort, &
                                    'Error opening horiz_grid_file')

      if (my_task == master_task) then
         read(nu,rec=1,iostat=ioerr) ULAT_G
         read(nu,rec=2,iostat=ioerr) ULON_G
         close(nu)
      endif
      call release_unit(nu)

      call broadcast_scalar(ioerr, master_task)
      if (ioerr /= 0) call exit_POP(sigAbort, &
                                    'Error reading horiz_grid_file')

      call broadcast_array(ULAT_G, master_task)
      call broadcast_array(ULON_G, master_task)

!-----------------------------------------------------------------------
!
!  otherwise, read everything else
!  compute some derived fields here to preserve information that is
!    lost once land blocks are dropped
!
!-----------------------------------------------------------------------

   else

      if (.not. allocated(ULAT_G)) then
         allocate (ULAT_G(nx_global,ny_global), &
                   ULON_G(nx_global,ny_global))
      endif

      INQUIRE(iolength=reclength) ULAT_G
      call get_unit(nu)
      ioerr = 0
      if (my_task == master_task) then
         open(nu,file=trim(horiz_grid_file),status='old', &
              form='unformatted', access='direct', recl=reclength, &
              iostat=ioerr)
      endif

      call broadcast_scalar(ioerr, master_task)
      if (ioerr /= 0) call exit_POP(sigAbort, &
                                    'Error opening horiz_grid_file')

      if (my_task == master_task) then
         read(nu,rec=1,iostat=ioerr) ULAT_G
         read(nu,rec=2,iostat=ioerr) ULON_G
      endif

      call broadcast_scalar(ioerr, master_task)
      if (ioerr /= 0) call exit_POP(sigAbort, &
                                    'Error reading horiz_grid_file')

      call scatter_global(ULAT, ULAT_G, master_task, distrb_clinic, &
                          field_loc_NEcorner, field_type_scalar)
      call scatter_global(ULON, ULON_G, master_task, distrb_clinic, &
                          field_loc_NEcorner, field_type_scalar)

      if (my_task == master_task) then
         read(nu,rec=3,iostat=ioerr) ULAT_G  ! holds HTN
      endif

      call scatter_global(HTN, ULAT_G, master_task, distrb_clinic, &
                          field_loc_Nface, field_type_scalar)

      do j=1,ny_global
      do i=1,nx_global
         ip1 = i+1
         if (i == nx_global) ip1 = 1 ! assume cyclic. non-cyclic
                                     ! will be handled during scatter
         !DXU
         ULON_G(i,j) = p5*(ULAT_G(i,j) + ULAT_G(ip1,j))
      end do
      end do
      call scatter_global(DXU, ULON_G, master_task, distrb_clinic, &
                          field_loc_NEcorner, field_type_scalar)

      do j=1,ny_global
         jm1 = j-1
         if (j == 1) jm1 = ny_global ! assume cyclic. non-cyclic
                                     ! will be handled during scatter
         do i=1,nx_global

            !DXT = p5(HTN(i,j)+HTN(i,j-1))
            ULON_G(i,j) = p5*(ULAT_G(i,j) + ULAT_G(i,jm1))
         end do
      end do
      call scatter_global(DXT, ULON_G, master_task, distrb_clinic, &
                          field_loc_center, field_type_scalar)

      if (my_task == master_task) then
         read(nu,rec=4,iostat=ioerr) ULAT_G  ! holds HTE
      endif

      call scatter_global(HTE, ULAT_G, master_task, distrb_clinic, &
                          field_loc_Eface, field_type_scalar)

      do j=1,ny_global
      do i=1,nx_global
         im1 = i-1
         if (i == 1) im1 = nx_global ! assume cyclic. non-cyclic
                                     ! will be handled during scatter
         !DYT
         ULON_G(i,j) = p5*(ULAT_G(i,j) + ULAT_G(im1,j))
      end do
      end do
      call scatter_global(DYT, ULON_G, master_task, distrb_clinic, &
                          field_loc_center, field_type_scalar)

      do j=1,ny_global
         jp1 = j+1
         if (j == ny_global) jp1 = 1 ! assume cyclic. non-cyclic
                                     ! will be handled during scatter
         do i=1,nx_global

            !DYU = p5(HTE(i,j)+HTN(i,j+1))
            ULON_G(i,j) = p5*(ULAT_G(i,j) + ULAT_G(i,jp1))
         end do
      end do
      call scatter_global(DYU, ULON_G, master_task, distrb_clinic, &
                          field_loc_NEcorner, field_type_scalar)

      if (my_task == master_task) then
         read(nu,rec=5,iostat=ioerr) ULAT_G
         read(nu,rec=6,iostat=ioerr) ULON_G
      endif

      call scatter_global(HUS, ULAT_G, master_task, distrb_clinic, &
                          field_loc_Eface, field_type_scalar)
      call scatter_global(HUW, ULON_G, master_task, distrb_clinic, &
                          field_loc_Nface, field_type_scalar)

      if (my_task == master_task) then
         read(nu,rec=7,iostat=ioerr) ULAT_G
         close(nu)
      endif

      call broadcast_scalar(ioerr, master_task)
      if (ioerr /= 0) call exit_POP(sigAbort, &
                                    'Error reading horiz_grid_file')

      call scatter_global(ANGLE, ULAT_G, master_task, distrb_clinic, &
                          field_loc_NEcorner, field_type_angle)
      call release_unit(nu)
      deallocate(ULAT_G,ULON_G)

      where (HTN <= c0) HTN = c1
      where (HTE <= c0) HTE = c1
      where (HUS <= c0) HUS = c1
      where (HUW <= c0) HUW = c1
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine read_horiz_grid

!***********************************************************************
!BOP
! !IROUTINE: vert_grid_internal
! !INTERFACE:

 subroutine vert_grid_internal

! !DESCRIPTION:
!  Creates vertical grid layer thicknesses based on km
!
! !REVISION HISTORY:
!  same as module

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  parameters for depth function choice
!
!-----------------------------------------------------------------------

   real (r8), parameter :: &
      zmax    = 5500.0_r8,  &! max depth in meters
      dz_sfc  =   25.0_r8,  &! thickness of sfc layer (meters)
      dz_deep =  400.0_r8    ! thick of deep ocn layers (meters)

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   real (r8) :: &
      depth,             &! depth based on integrated thicknesses
      zlength,           &! adjustable parameter for thickness
      d0, d1,            &! depths used by midpoint search
      zl0, zl1,          &! parameter used by midpoint search
      dzl                 ! zl1-zl0

   integer (int_kind) :: k  ! dummy vertical index

!-----------------------------------------------------------------------
!
!  initialize bisection search to find best value of zlength
!  parameter such that integrated depth = zmax
!
!-----------------------------------------------------------------------

   zl0 = eps
   zl1 = zmax
   dzl = zl1 - zl0

   call compute_dz(d0,zl0,dz_sfc,dz_deep)
   call compute_dz(d1,zl1,dz_sfc,dz_deep)

   if ((d0-zmax)*(d1-zmax) > c0) then
      if (my_task == master_task) write(stdout,*) d0,d1,zmax
      call exit_POP(sigAbort, &
                    'vert_grid: zero point not in initial interval')
   endif

!-----------------------------------------------------------------------
!
!  do bisection search
!
!-----------------------------------------------------------------------

   do while ( (dzl/zmax) > eps)

      !***
      !*** compute profile at midpoint
      !***

      zlength = zl0 + p5*dzl

      call compute_dz(depth,zlength,dz_sfc,dz_deep)

      !***
      !*** find interval to use for continuing search
      !***

      if ((d0-zmax)*(depth-zmax) < c0) then
         d1 = depth
         zl1 = zlength
      else if ((d1-zmax)*(depth-zmax) < c0) then
         d0 = depth
         zl0 = zlength
      else
         if (my_task == master_task) write(stdout,*) d0,d1,depth,zmax
         call exit_POP(sigAbort,'vert_grid: zero point not in interval')
      endif

      dzl = zl1 - zl0

   end do

   dz = dz*cmperm   ! convert to cm

!-----------------------------------------------------------------------
!
!  presumably, we have converged, but check to make sure
!
!-----------------------------------------------------------------------

   if (abs(depth-zmax)/zmax > 0.01_r8) then
      if (my_task == master_task) then
         write(stdout,*) 'Integrated depth = ',depth,'   zmax = ',zmax
      endif
      call exit_POP(sigAbort, &
                    'Unable to compute vertical grid internally')
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine vert_grid_internal

!***********************************************************************
!BOP
! !IROUTINE: compute_dz
! !INTERFACE:

 subroutine compute_dz(depth,zlength,dz_sfc,dz_deep)

! !DESCRIPTION:
!  Computes a thickness profile and total depth given the
!  parameters for the thickness function
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), intent(in) :: &
      zlength,          &! gaussian parameter for thickness func
      dz_sfc,           &! thickness of surface layer
      dz_deep            ! thickness of deep ocean layers

! !OUTPUT PARAMETERS:

   real (r8), intent(out) :: &
      depth               ! depth based on integrated thicknesses

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: k

!-----------------------------------------------------------------------

   depth = c0

   do k=1,km
      dz(k) = dz_deep - (dz_deep - dz_sfc)*exp(-(depth/zlength)**2)
      depth = depth + dz(k)
   end do

!-----------------------------------------------------------------------
!EOC

 end subroutine compute_dz

!***********************************************************************
!BOP
! !IROUTINE: read_vert_grid
! !INTERFACE:

 subroutine read_vert_grid(vert_grid_file)

! !DESCRIPTION:
!  Reads in layer thicknesses from grid input file
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (char_len), intent(in) :: &
      vert_grid_file

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      k,                 &! vertical level index
      nu,                &! i/o unit number
      ioerr               ! i/o error flag

!-----------------------------------------------------------------------
!
!  read vertical layer thickness from file
!
!-----------------------------------------------------------------------

   call get_unit(nu)
   if (my_task == master_task) then
      open(nu,file=vert_grid_file,status='old',form='formatted', &
              iostat=ioerr)
   endif
   call broadcast_scalar(ioerr, master_task)
   if (ioerr /= 0) call exit_POP(sigAbort, &
                                 'Error opening vert_grid_file')

   if (my_task == master_task) then
      if (ioerr == 0) then ! successful open
         grid_read: do k = 1,km
            read(nu,*,iostat=ioerr) dz(k)
            if (ioerr /= 0) exit grid_read
         end do grid_read
         close(nu)
      endif
   endif
   call release_unit(nu)

   call broadcast_scalar(ioerr, master_task)
   if (ioerr /= 0) call exit_POP(sigAbort, &
                                 'Error reading vert_grid_file')

   call broadcast_array(dz, master_task)

!-----------------------------------------------------------------------
!EOC

 end subroutine read_vert_grid

!***********************************************************************
!BOP
! !IROUTINE: topography_internal
! !INTERFACE:

 subroutine topography_internal(kmt_global)

! !DESCRIPTION:
!  Generates simple KMT field with idealized land masses
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   logical(log_kind), intent(in) :: &
      kmt_global       ! flag for generating only global KMT field

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j,ig,jg,k,n,bid ! dummy counters

   real (r8) :: &
      latd, lond     ! lat/lon in degrees

   type (block) :: &
      this_block    ! block info for current block

!-----------------------------------------------------------------------
!
!  compute global KMT field (for use in setting up domain)
!
!-----------------------------------------------------------------------

   if (kmt_global) then

      allocate(KMT_G(nx_global,ny_global))

      do j=1,ny_global
      do i=1,nx_global
         latd = ULAT_G(i,j)*radian
         lond = ULON_G(i,j)*radian
         if (lond < c0) lond = lond + 360.0_r8

         KMT_G(i,j) = km  ! flat bottom

         if ((latd > -35.0_r8) .and. &
             (lond > 210.0_r8) .and. &
             (lond < 250.0_r8)) KMT_G(i,j) = 0

         if ((latd >  25.0_r8) .and. &
             (lond > 210.0_r8) .and. &
             (lond < 330.0_r8)) KMT_G(i,j) = 0

         if ((latd >  60.0_r8) .and. &
             (lond > 210.0_r8) .and. &
             (lond < 150.0_r8)) KMT_G(i,j) = 0

         if ((latd > -60.0_r8) .and. &
             (lond > 110.0_r8) .and. &
             (lond < 150.0_r8)) KMT_G(i,j) = 0

         if (abs(latd) > 75.0_r8) KMT_G(i,j) = 0

      end do
      end do

!-----------------------------------------------------------------------
!
!  otherwise, set up local KMT field
!
!-----------------------------------------------------------------------

   else

      do n=1,nblocks_clinic
         this_block = get_block(blocks_clinic(n),n)

         do j=1,ny_block
            jg = this_block%j_glob(j)
            if (jg > 0) then
               do i=1,nx_block
                  ig = this_block%i_glob(i)
                  if (ig /= 0) then
                     KMT(i,j,n) = KMT_G(ig,jg)
                  else
                     KMT(i,j,n) = 0
                  endif
               end do
            else
               KMT(:,j,n) = 0
            endif
         end do
      end do

      deallocate(KMT_G)

   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine topography_internal

!***********************************************************************
!BOP
! !IROUTINE: read_topography
! !INTERFACE:

 subroutine read_topography(topography_file,kmt_global)

! !DESCRIPTION:
!  Reads in KMT field from file
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (char_len), intent(in) :: &
      topography_file     ! input file containing KMT field

   logical(log_kind), intent(in) :: &
      kmt_global       ! flag for generating only global KMT field

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      nu                ,&! i/o unit number
      ioerr             ,&! i/o error flag
      reclength           ! record length

!-----------------------------------------------------------------------
!
!  read global KMT field (for use in setting up domain)
!
!-----------------------------------------------------------------------

   if (kmt_global) then

      allocate(KMT_G(nx_global,ny_global))

      INQUIRE(iolength=reclength) KMT_G
      call get_unit(nu)
      if (my_task == master_task) then
         open(nu, file=topography_file,status='old',form='unformatted', &
                  access='direct', recl=reclength, iostat=ioerr)
      endif

      call broadcast_scalar(ioerr, master_task)
      if (ioerr /= 0) call exit_POP(sigAbort, &
                                 'Error opening topography_file')

      if (my_task == master_task) then
         read(nu, rec=1, iostat=ioerr) KMT_G
         close(nu)
      endif
      call release_unit(nu)

      call broadcast_scalar(ioerr, master_task)
      if (ioerr /= 0) call exit_POP(sigAbort, &
                                 'Error reading topography_file')

      call broadcast_array(KMT_G, master_task)

!-----------------------------------------------------------------------
!
!  otherwise read KMT field from file
!
!-----------------------------------------------------------------------

   else
      call scatter_global(KMT, KMT_G, master_task, distrb_clinic, &
                          field_loc_center, field_type_scalar)
      deallocate(KMT_G)
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine read_topography

!***********************************************************************
!BOP
! !IROUTINE: read_bottom_cell
! !INTERFACE:

 subroutine read_bottom_cell(DZBC,bottom_cell_file)

! !DESCRIPTION:
!  Reads bottom cell information from input bottom cell file
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      bottom_cell_file     ! filename of file containing cell thickness

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), &
      intent(inout) :: &
      DZBC            ! thickness of bottom cell in each column

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   real (r8), dimension(:,:), allocatable :: &
      DZBC_G              ! global bottom layer thickness (cm)

   integer (int_kind) :: &
      nu                ,&! i/o unit number
      ioerr               ! i/o error flag

!-----------------------------------------------------------------------
!
!  open a file and read thickness field
!
!-----------------------------------------------------------------------

   call get_unit(nu)
   if (my_task == master_task) then
      open(nu, file=bottom_cell_file,status='old',form='unformatted', &
               access='direct', recl=8*nx_global*ny_global, &
               iostat=ioerr)
   endif

   call broadcast_scalar(ioerr, master_task)
   if (ioerr /= 0) call exit_POP(sigAbort, &
                                 'Error opening bottom_cell_file')

   if (my_task == master_task) then
      allocate(DZBC_G(nx_global,ny_global))
      read(nu, rec=1, iostat=ioerr) DZBC_G
      close(nu)
   endif
   call release_unit(nu)

   call broadcast_scalar(ioerr, master_task)
   if (ioerr /= 0) call exit_POP(sigAbort, &
                                 'Error reading bottom_cell_file')

   call scatter_global(DZBC, DZBC_G, master_task, distrb_clinic, &
                       field_loc_center, field_type_scalar)
   if (my_task == master_task) deallocate(DZBC_G)

!-----------------------------------------------------------------------
!EOC

 end subroutine read_bottom_cell

!***********************************************************************
!BOP
! !IROUTINE: remove_points
! !INTERFACE:

 subroutine remove_points

! !DESCRIPTION:
!  Removes isolated points from grid (KMT field)
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
      i,j,n,             &! dummy loop indices
      npoints_removed     ! number of points removed

   integer(int_kind),dimension(nx_block,ny_block,max_blocks_clinic) ::&
      ICOUNT            ! record of points removed

   character (*), parameter :: &
      rmpts_fmt = "(' points removed from grid:',2x,i10)"

!-----------------------------------------------------------------------
!
!  calculate number of levels at U points
!  (KMU field before modifying KMT field)
!
!-----------------------------------------------------------------------

   do n=1,nblocks_clinic
   do j=1,ny_block-1
   do i=1,nx_block-1
      KMU(i,j,n) = min(KMT(i,j  ,n),KMT(i+1,j  ,n), &
                       KMT(i,j+1,n),KMT(i+1,j+1,n))
   end do
   end do
   end do

!-----------------------------------------------------------------------
!
!  remove disconnected points from grid.
!  if all KMUs surrounding a T point are zero, set KMT = 0.
!
!-----------------------------------------------------------------------

   do n=1,nblocks_clinic
   do j=2,ny_block
   do i=2,nx_block
      if ((KMU(i,j  ,n) + KMU(i-1,j  ,n) + &
           KMU(i,j-1,n) + KMU(i-1,j-1,n)) == 0) then
         ICOUNT(i,j,n) = 1
         KMT   (i,j,n) = 0
      else
         ICOUNT(i,j,n) = 0
      endif
   end do
   end do
   end do

   call update_ghost_cells(KMT, bndy_clinic, field_loc_center, &
                                             field_type_scalar)

   npoints_removed = global_count(ICOUNT, distrb_clinic, field_loc_center)

   if (my_task == master_task) then
      write(stdout,blank_fmt)
      write(stdout,rmpts_fmt) npoints_removed
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine remove_points

!***********************************************************************
!BOP
! !IROUTINE: smooth_topography
! !INTERFACE:

 subroutine smooth_topography

! !DESCRIPTION:
!  This routine smooths topography to create new KMT, depth fields
!  The depth field HT is constructed from the KMT field and
!  depth profile dz, HT is smoothed by by application of a
!  9-point averaging stencil:
!  \begin{verbatim}
!           1 -- 2 -- 1
!           |    |    |
!           2 -- 4 -- 2
!           |    |    |
!           1 -- 2 -- 1
!  \end{verbatim}
!  Land points are not included in the smoothing, and the
!  stencil is modified to include only ocean points in the
!  averaging.  Once the depth field has been smoothed,
!  a new KMT field is constructed from it.
!
! !REVISION HISTORY:
!  same as module
!
!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables:
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j,k,n            ! dummy loop indices

   integer(int_kind),dimension(nx_block,ny_block,max_blocks_clinic) ::&
      NB,                &! num points contributing to 9pt avg
      IWORK               ! local work space

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic) :: &
      HTNEW,             &! smoothed depth field at T points
      WORK                ! local work space

!-----------------------------------------------------------------------
!
!  old depth field in T columns
!
!-----------------------------------------------------------------------

   HT = c0
   do k = 1,km
      where (k == KMT) HT = zw(k)
   enddo

!-----------------------------------------------------------------------
!
!  smooth topography
!
!-----------------------------------------------------------------------

   where (KMT > 0)
      NB = 1
      HTNEW = HT
   elsewhere
      NB = 0
      HTNEW = c0
   endwhere

   do n=1,nblocks_clinic
   do j=2,ny_block-1
   do i=2,nx_block-1

      WORK(i,j,n) = c4*HTNEW(i,j,n) + &
            c2*HTNEW(i+1,j,n) + c2*HTNEW(i-1,j,n) + &
            c2*HTNEW(i,j+1,n) + HTNEW(i+1,j+1,n) + HTNEW(i-1,j+1,n) + &
            c2*HTNEW(i,j-1,n) + HTNEW(i+1,j-1,n) + HTNEW(i-1,j-1,n)

      IWORK(i,j,n) = c4*NB(i,j,n)  + &
            c2*NB(i+1,j,n) + c2*NB(i-1,j,n) + &
            c2*NB(i,j+1,n) + NB(i+1,j+1,n) + NB(i-1,j+1,n) + &
            c2*NB(i,j-1,n) + NB(i+1,j-1,n) + NB(i-1,j-1,n)

   end do
   end do
   end do

!-----------------------------------------------------------------------
!
!  new depth field
!
!-----------------------------------------------------------------------

   where ((KMT /= 0) .and. (IWORK /= 0))
      HTNEW = WORK/float(IWORK)
   elsewhere
      HTNEW = c0
   endwhere

!-----------------------------------------------------------------------
!
!  new KMT field
!
!-----------------------------------------------------------------------

   KMTOLD = KMT
   do k = 1,km-1
      where (HTNEW > zt(k) .and. HTNEW <= zt(k+1)) KMT = k
   enddo
   where (HTNEW > zt(k)) KMT = km

   call update_ghost_cells(KMT,  bndy_clinic, field_loc_center, &
                                              field_type_scalar)

!-----------------------------------------------------------------------
!EOC

 end subroutine smooth_topography

!***********************************************************************
!BOP
! !IROUTINE: landmasks
! !INTERFACE:

 subroutine landmasks

! !DESCRIPTION:
!  Calculates additional masks for land points at each depth level.
!  These include real masks for applying multiplicative masks
!  instead of logical masks and also KMT arrays for neighbor points.
!
! !REVISION HISTORY:
!  same as module

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  masks for surface ocean T or U points
!
!-----------------------------------------------------------------------

   where (KMT >= 1)
      CALCT = .true.
      RCALCT = c1
   elsewhere
      CALCT = .false.
      RCALCT = c0
   endwhere

   where (KMU >= 1)
      CALCU = .true.
      RCALCU = c1
   elsewhere
      CALCU = .false.
      RCALCU = c0
   endwhere

!-----------------------------------------------------------------------
!
!  depth level fields (KMT,KMU) to north,south,east,west
!
!-----------------------------------------------------------------------

   KMTN = eoshift(KMT,dim=2,shift=+1)
   KMTS = eoshift(KMT,dim=2,shift=-1)
   KMTE = eoshift(KMT,dim=1,shift=+1)
   KMTW = eoshift(KMT,dim=1,shift=-1)

   KMUN = eoshift(KMU,dim=2,shift=+1)
   KMUS = eoshift(KMU,dim=2,shift=-1)
   KMUE = eoshift(KMU,dim=1,shift=+1)
   KMUW = eoshift(KMU,dim=1,shift=-1)

   KMTEE = eoshift(KMT,dim=1,shift=2)
   KMTNN = eoshift(KMT,dim=2,shift=2)

   !call update_ghost_cells(KMTN, bndy_clinic, field_loc_center, &
   !                                           field_type_scalar)
   !call update_ghost_cells(KMTS, bndy_clinic, field_loc_center, &
   !                                           field_type_scalar)
   !call update_ghost_cells(KMTE, bndy_clinic, field_loc_center, &
   !                                           field_type_scalar)
   !call update_ghost_cells(KMTW, bndy_clinic, field_loc_center, &
   !                                           field_type_scalar)
   !call update_ghost_cells(KMUN, bndy_clinic, field_loc_NEcorner, &
   !                                           field_type_scalar)
   !call update_ghost_cells(KMUS, bndy_clinic, field_loc_NEcorner, &
   !                                           field_type_scalar)
   !call update_ghost_cells(KMUE, bndy_clinic, field_loc_NEcorner, &
   !                                           field_type_scalar)
   !call update_ghost_cells(KMUW, bndy_clinic, field_loc_NEcorner, &
   !                                           field_type_scalar)
   !call update_ghost_cells(KMTEE, bndy_clinic, field_loc_center, &
   !                                            field_type_scalar)
   !call update_ghost_cells(KMTNN, bndy_clinic, field_loc_center, &
   !                                            field_type_scalar)

!-----------------------------------------------------------------------
!EOC

 end subroutine landmasks

!***********************************************************************
!BOP
! !IROUTINE: area_masks
! !INTERFACE:

 subroutine area_masks(mask_filename)

! !DESCRIPTION:
!   This subroutine reads in file with regional area mask and
!   marginal seas defined
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character(*), intent(in) :: &
      mask_filename            ! name of file containing region masks

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      k, n,              &! loop counters
      nu,                &! i/o unit number
      reclength,         &! record length of file
      ioerr,             &! i/o error flag
      num_regions,       &! number of different regions
      region              ! region counter

   real (r8) :: &
      tmp_vol,  &! temporary volume
      sea_area, &! total volume of a particular sea
      sea_vol    ! total volume of a particular sea

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic) :: &
      WORK                ! temporary space

   integer (int_kind), dimension(:,:), allocatable :: &
      REGION_G            ! global-sized region mask

!-----------------------------------------------------------------------
!
!  read in regional area masks, including marginal seas.  then
!  calculate related variables.
!
!-----------------------------------------------------------------------

   allocate(REGION_MASK(nx_block,ny_block,max_blocks_clinic))

   call get_unit(nu)
   if (my_task == master_task) then
      allocate(REGION_G(nx_global,ny_global))
      inquire (iolength=reclength) REGION_G
      open(nu, file=mask_filename,status='old',form='unformatted', &
               access='direct', recl=reclength, iostat=ioerr)
   endif

   call broadcast_scalar(ioerr, master_task)
   if (ioerr /= 0) call exit_POP(sigAbort, &
                                 'Error opening region mask file')

   if (my_task == master_task) then
      read(nu, rec=1, iostat=ioerr) REGION_G
      close(nu)
   endif
   call release_unit(nu)

   call broadcast_scalar(ioerr, master_task)
   if (ioerr /= 0) call exit_POP(sigAbort, &
                                 'Error reading region mask file')

   call scatter_global(REGION_MASK, REGION_G, master_task,distrb_clinic, &
                       field_loc_center, field_type_scalar)
   if (my_task == master_task) deallocate(REGION_G)

!-----------------------------------------------------------------------
!
!  a negative value of REGION_MASK designates a marginal sea.  if
!  a region is a marginal sea, calculate the area and volume
!
!-----------------------------------------------------------------------

   area_t_marg = c0
   volume_t_marg = c0
   volume_t_marg_k = c0

   num_regions = global_maxval(abs(REGION_MASK), &
                               distrb_clinic, field_loc_center)

   do region = 1, num_regions

      WORK = merge(TAREA, c0, 1 <= KMT .and. REGION_MASK == -region)
      sea_area = global_sum(WORK, distrb_clinic, field_loc_center)
      area_t_marg = area_t_marg + sea_area

      sea_vol = c0
      do k = 1, km
         WORK = merge(TAREA*dz(k), c0, &
                      k <= KMT .and. REGION_MASK == -region)
         tmp_vol = global_sum(WORK, distrb_clinic, field_loc_center)
         sea_vol = sea_vol + tmp_vol
         volume_t_marg_k(k) = volume_t_marg_k(k) + tmp_vol
      enddo
      volume_t_marg = volume_t_marg + sea_vol

      if (my_task == master_task .and. sea_area /= c0) then
         write(stdout,"('Region #',i2,' is a marginal sea')") region
         write(stdout, &
             "('  area (km^2) = ',e12.5, '  volume (km^3) = ',e12.5)") &
             sea_area*1.0e-10_r8, sea_vol*1.0e-15_r8
      endif

   enddo

!-----------------------------------------------------------------------
!EOC

 end subroutine area_masks

!***********************************************************************

 subroutine cf_area_avg

!-----------------------------------------------------------------------
!
!  calculate the coefficients of the 4-point stencils for
!  area-weighted averaging at T and U points
!
!-----------------------------------------------------------------------

!-----------------------------------------------------------------------
!
!  calculate {central,s,w,sw} coefficients for area-averaging
!  to T points.
!
!-----------------------------------------------------------------------

!  AT0  = UAREA
!  call s_shift(ATS,UAREA)
!  call w_shift(ATW,UAREA)
!  call s_shift(ATSW,ATW)

!  AT0  = AT0 *p25*TAREA_R
!  ATS  = ATS *p25*TAREA_R
!  ATW  = ATW *p25*TAREA_R
!  ATSW = ATSW*p25*TAREA_R

   AT0  = p25
   ATS  = p25
   ATW  = p25
   ATSW = p25

!-----------------------------------------------------------------------
!
!  calculate {central,n,e,ne} coefficients for area-averaging
!   to U points.
!
!-----------------------------------------------------------------------

   AU0  = TAREA
   AUN  = eoshift(TAREA,dim=2,shift=+1)
   AUE  = eoshift(TAREA,dim=1,shift=+1)
   AUNE = eoshift(AUE  ,dim=2,shift=+1)

   AU0  = AU0 *p25*UAREA_R
   AUN  = AUN *p25*UAREA_R
   AUE  = AUE *p25*UAREA_R
   AUNE = AUNE*p25*UAREA_R

!-----------------------------------------------------------------------

 end subroutine cf_area_avg

!***********************************************************************
!BOP
! !IROUTINE: calc_tpoints
! !INTERFACE:

 subroutine calc_tpoints

! !DESCRIPTION:
!  Calculates lat/lon coordinates of T points from U points
!  using a simple average of four neighbors in Cartesian 3d space.
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

   integer (int_kind) :: i,j,n

   real (r8) ::                   &
      xc,yc,zc,xs,ys,zs,xw,yw,zw, &! Cartesian coordinates for
      xsw,ysw,zsw,tx,ty,tz,da      !    nbr points

   type (block) :: &
      this_block    ! block info for this block

!-----------------------------------------------------------------------
!
!  TLAT, TLON are southwest 4-point averages of ULAT,ULON
!  for general grids, must drop into 3-d Cartesian space to prevent
!  problems near the pole
!
!-----------------------------------------------------------------------

   !$OMP PARALLEL DO PRIVATE(i,j,xsw,ysw,zsw,xw,yw,zw,xs,ys,zs,xc,yc,zc, &
   !$OMP                     tx,ty,tz,da)

   do n=1,nblocks_clinic
      this_block = get_block(blocks_clinic(n),n)

      do j=2,ny_block
      do i=2,nx_block

         !***
         !*** set up averaging weights
         !***

         !wt0  = AT0 (i,j,n)
         !wts  = ATS (i,j,n)
         !wtw  = ATW (i,j,n)
         !wtsw = ATSW(i,j,n)

         !***
         !*** convert neighbor U-cell coordinates to 3-d Cartesian coordinates 
         !*** to prevent problems with averaging near the pole
         !***

         zsw = cos(ULAT(i-1,j-1,n))
         xsw = cos(ULON(i-1,j-1,n))*zsw
         ysw = sin(ULON(i-1,j-1,n))*zsw
         zsw = sin(ULAT(i-1,j-1,n))

         zs  = cos(ULAT(i  ,j-1,n))
         xs  = cos(ULON(i  ,j-1,n))*zs
         ys  = sin(ULON(i  ,j-1,n))*zs
         zs  = sin(ULAT(i  ,j-1,n))

         zw  = cos(ULAT(i-1,j  ,n))
         xw  = cos(ULON(i-1,j  ,n))*zw
         yw  = sin(ULON(i-1,j  ,n))*zw
         zw  = sin(ULAT(i-1,j  ,n))

         zc  = cos(ULAT(i  ,j  ,n))
         xc  = cos(ULON(i  ,j  ,n))*zc
         yc  = sin(ULON(i  ,j  ,n))*zc
         zc  = sin(ULAT(i  ,j  ,n))

         !***
         !*** straight 4-point average to T-cell Cartesian coords
         !***

         tx = p25*(xc + xs + xw + xsw)
         ty = p25*(yc + ys + yw + ysw)
         tz = p25*(zc + zs + zw + zsw)

         !***
         !*** convert to lat/lon in radians
         !***

         da = sqrt(tx**2 + ty**2 + tz**2)

         TLAT(i,j,n) = asin(tz/da)

         if (tx /= c0 .or. ty /= c0) then
            TLON(i,j,n) = atan2(ty,tx)
         else
            TLON(i,j,n) = c0
         endif
           
      end do
      end do

      !***
      !*** for bottom row of domain where sw 4pt average is not valid,
      !*** extrapolate from interior
      !*** NOTE: THIS ASSUMES A CLOSED SOUTH BOUNDARY - WILL NOT
      !***       WORK CORRECTLY FOR CYCLIC OPTION
      !***

      if (this_block%j_glob(this_block%jb) == 1) then
         do i=this_block%ib,this_block%ie
            TLON(i,this_block%jb,n) = TLON(i,this_block%jb+1,n)
            TLAT(i,this_block%jb,n) = c2*TLAT(i,this_block%jb+1,n) - &
                                         TLAT(i,this_block%jb+2,n)
         end do
      endif

      where (TLON(:,:,n) > pi2) TLON(:,:,n) = TLON(:,:,n) - pi2
      where (TLON(:,:,n) < c0 ) TLON(:,:,n) = TLON(:,:,n) + pi2

   end do
   !$OMP END PARALLEL DO

!-----------------------------------------------------------------------
!
!  Update boundaries
!
!-----------------------------------------------------------------------

   call update_ghost_cells(TLAT, bndy_clinic, field_loc_center, &
                                              field_type_scalar)
   call update_ghost_cells(TLON, bndy_clinic, field_loc_center, &
                                              field_type_scalar)

!-----------------------------------------------------------------------

 end subroutine calc_tpoints

!***********************************************************************

 subroutine fill_points(k,F)

!-----------------------------------------------------------------------
!
!  if the depth-level field KMT has been smoothed, fill in
!  values of a 2-d field at new ocean points that have appeared
!  due to the smoothing using averaged values from nearby points.
!
!* given the smoothed KMT field and the original unsmoothed field
!  KMTOLD, values at new points of the field F (defined at level k)
!  are filled in with averaged values of F defined at nearby points.
!  the averaging is done using a 9-point averaging stencil like
!  the one used in 'smooth_topography':
!
!  9-point averaging stencil:
!
!           1 -- 2 -- 1
!           |    |    |
!           2 -- 4 -- 2
!           |    |    |
!           1 -- 2 -- 1
!
!  the stencil is modified to include only old ocean points in the
!  averaging.
!
!* since only those new points which have neighboring old points
!  will be filled in, this procedure must be applied several times
!  until all points are filled in.
!
!* if an area weighted average is desired, multiply the field F
!  by the cell area before calling the routine, then divide by
!  the cell area after the field is returned.
!
!-----------------------------------------------------------------------
!-----------------------------------------------------------------------
!
!  input variables:
!
!-----------------------------------------------------------------------

   integer (int_kind), intent(in) :: &
     k                      ! depth level at which field is defined
                            ! (k = 1 for sfc or vert-avg fields)

!-----------------------------------------------------------------------
!
!  input/output:
!
!-----------------------------------------------------------------------

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), &
     intent(inout) :: &
     F             ! input as field well-defined at old ocean points
                   ! output as field with new points filled in

!-----------------------------------------------------------------------
!
!  local variables:
!
!-----------------------------------------------------------------------

   integer (int_kind) ::  &
     i,j,n,               &! dummy indices
     npass,               &! num of passes thru grid filling points
     nfill_points          ! num of points left to be filled

   logical(log_kind),dimension(nx_block,ny_block,max_blocks_clinic) ::&
     MASKOLD,             &! true at old ocean points
     MASKNEW               ! true at new points not in old ocn field

   integer(int_kind),dimension(nx_block,ny_block,max_blocks_clinic) ::&
     NB,                  &! num points contributing to 9pt avg
     IWORK                 ! local work space

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic) ::  &
     FNEW,                &! smoothed F field after npass passes
     WORK                  ! local work space

!-----------------------------------------------------------------------
!
!  initialize land mask and pass counter
!
!-----------------------------------------------------------------------

   MASKOLD = (k <= KMTOLD)
   npass = 0
   nfill_points = 100

!-----------------------------------------------------------------------
!
!  pass through grid until all points are filled
!
!-----------------------------------------------------------------------

   do while (nfill_points /= 0)

     npass = npass + 1

     if (npass > nx_global) then
        call exit_POP(sigAbort,'error (fill_points): too many passes')
     endif

!-----------------------------------------------------------------------
!
!    find new ocean points not in old ocean field
!
!-----------------------------------------------------------------------

     MASKNEW = (k <= KMT) .and. (.not. MASKOLD)

!-----------------------------------------------------------------------
!
!    find smoothed field: fill all points in this field with
!    the (9-point) average of the surrounding neighboring points,
!    including in the average only old ocean points.
!
!-----------------------------------------------------------------------

     where (MASKOLD)
       WORK  = F
       IWORK = 1
     elsewhere
       IWORK = 0
       WORK  = c0
     endwhere

     do n=1,nblocks_clinic
     do j=2,ny_block-1
     do i=2,nx_block-1

       FNEW(i,j,n) = c4*WORK(i,j,n) + &
             c2*WORK(i+1,j,n) + c2*WORK(i-1,j,n) + &
             c2*WORK(i,j+1,n) + WORK(i+1,j+1,n) + WORK(i-1,j+1,n) + &
             c2*WORK(i,j-1,n) + WORK(i+1,j-1,n) + WORK(i-1,j-1,n)

       NB(i,j,n) = 4*IWORK(i,j,n)  + &
             2*IWORK(i+1,j,n) + 2*IWORK(i-1,j,n) + &
             2*IWORK(i,j+1,n) + IWORK(i+1,j+1,n) + IWORK(i-1,j+1,n) + &
             2*IWORK(i,j-1,n) + IWORK(i+1,j-1,n) + IWORK(i-1,j-1,n)

     end do
     end do
     end do

!-----------------------------------------------------------------------
!
!    fill in F at new ocean points with the smoothed field,
!    only fill in points which have neighboring old ocean points.
!
!-----------------------------------------------------------------------

     where (MASKNEW .and. (NB /= 0))
       F = FNEW/float(NB)
       MASKOLD = .true.  ! update for next pass
     endwhere

     call update_ghost_cells(F, bndy_clinic, field_loc_center, &
                                             field_type_scalar)

!-----------------------------------------------------------------------
!
!    return for another pass if more points remain to be filled.
!
!-----------------------------------------------------------------------

     nfill_points = global_count((MASKNEW .and. (IWORK == 0)), &
                                 distrb_clinic, field_loc_center)
   enddo

!-----------------------------------------------------------------------

 end subroutine fill_points

!***********************************************************************
!BOP
! !IROUTINE: ugrid_to_tgrid
! !INTERFACE:

 subroutine ugrid_to_tgrid(ARRAY_TGRID, ARRAY_UGRID, iblock)

! !DESCRIPTION:
!  Interpolates values at U points on a B grid to T points.
!  Note that ghost cells are not updated.
!  Also note that the input array is assumed to be in the baroclinic
!  distribution (where the stencil weights are defined).
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
     iblock                  ! index for block in baroclinic distrb

   real (r8), dimension(nx_block,ny_block), intent(in) :: &
     ARRAY_UGRID             ! field on U points

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(out) :: &
     ARRAY_TGRID             ! field on T points

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
     i,j                 ! dummy indices

!-----------------------------------------------------------------------
!
!  southwest 4pt average
!
!-----------------------------------------------------------------------

   do j=2,ny_block
   do i=2,nx_block

     ARRAY_TGRID(i,j) = AT0 (i,j,iblock)*ARRAY_UGRID(i  ,j  ) + &
                        ATS (i,j,iblock)*ARRAY_UGRID(i  ,j-1) + &
                        ATW (i,j,iblock)*ARRAY_UGRID(i-1,j  ) + &
                        ATSW(i,j,iblock)*ARRAY_UGRID(i-1,j-1)

   end do
   end do

   ARRAY_TGRID(:,1) = c0
   ARRAY_TGRID(1,:) = c0

!-----------------------------------------------------------------------
!EOC

 end subroutine ugrid_to_tgrid

!***********************************************************************
!BOP
! !IROUTINE: tgrid_to_ugrid
! !INTERFACE:

 subroutine tgrid_to_ugrid(ARRAY_UGRID, ARRAY_TGRID, iblock)

! !DESCRIPTION:
!  Interpolates values at T points on a B grid to U points.
!  Note that ghost cells are not updated.
!  Also note that the input array is assumed to be in the baroclinic
!  distribution (where the stencil weights are defined).
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
     iblock                  ! index for block in baroclinic distrb

   real (r8), dimension(nx_block,ny_block), intent(in) :: &
     ARRAY_TGRID    ! field on T points

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(out) :: &
     ARRAY_UGRID    ! field on U points

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
     i,j                 ! dummy indices

!-----------------------------------------------------------------------
!
!  northeast 4pt average
!
!-----------------------------------------------------------------------

   do j=1,ny_block-1
   do i=1,nx_block-1

     ARRAY_UGRID(i,j) = AU0 (i,j,iblock)*ARRAY_TGRID(i  ,j  ) + &
                        AUN (i,j,iblock)*ARRAY_TGRID(i  ,j+1) + &
                        AUE (i,j,iblock)*ARRAY_TGRID(i+1,j  ) + &
                        AUNE(i,j,iblock)*ARRAY_TGRID(i+1,j+1)

   end do
   end do

   ARRAY_UGRID(:,ny_block) = c0
   ARRAY_UGRID(nx_block,:) = c0

!-----------------------------------------------------------------------
!EOC

 end subroutine tgrid_to_ugrid

!***********************************************************************

 end module grid

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

