!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module initial

!BOP
! !MODULE: intitial
! !DESCRIPTION:
!  This module contains routines for initializing a POP simulation,
!  mostly by calling individual initialization routines for each
!  POP module.
!
! !REVISION HISTORY:
!  CVS:$Id: initial.F90,v 1.16 2004/01/21 20:50:49 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $
!
! !USES:

   use kinds_mod, only: r8, int_kind, log_kind, char_len
   use blocks, only: block, nx_block, ny_block, get_block
   !use distribution, only: 
   use domain, only: nblocks_clinic, blocks_clinic, init_domain_blocks,    &
       init_domain_distribution
   use constants, only: delim_fmt, blank_fmt, field_loc_center, blank_fmt, &
       c0, ppt_to_salt, mpercm, c1, field_type_scalar, init_constants
   use communicate, only: my_task, master_task, init_communicate
   !use boundary, only: 
   use broadcast, only: broadcast_array, broadcast_scalar
   use prognostic, only: init_prognostic, max_blocks_clinic, nx_global,    &
       ny_global, km, nt, TRACER, curtime, RHO, newtime, oldtime
   use solvers, only: init_solvers
   use grid, only: init_grid1, init_grid2, kmt, kmt_g, topo_smooth, zt,    &
       fill_points
   use io, only:  data_set
   use io_types, only: init_io, stdout, datafile, io_field_desc, io_dim,   &
       nml_in, nml_filename, construct_file, construct_io_dim,             &
       construct_io_field, rec_type_dbl, destroy_file, get_unit,           &
       release_unit, destroy_io_field
   use baroclinic, only: init_baroclinic
   use barotropic, only: init_barotropic
   use pressure_grad, only: init_pressure_grad
   use surface_hgt, only: init_surface_hgt
   use vertical_mix, only: init_vertical_mix
   use horizontal_mix, only: init_horizontal_mix
   use advection, only: init_advection
   use diagnostics, only: init_diagnostics
   use state_mod, only: init_state, state
   use time_management, only: first_step, init_time_manager1, init_time_manager2
   use topostress, only: init_topostress
   use ice, only: init_ice
   use xdisplay, only: init_xdisplay
   use output, only: init_output
   use tavg, only: init_tavg, ltavg_restart
   !use hydro_sections
   !use current_meters
   !use drifters
   use forcing, only: init_forcing
   use forcing_coupled, only: init_coupled
   use exit_mod, only: sigAbort, exit_pop
   use restart, only: read_restart
   use global_reductions, only: init_global_reductions
   use timers, only: init_timers

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: initialize_POP

!EOP
!BOC
!-----------------------------------------------------------------------
!
!     module variables
!
!-----------------------------------------------------------------------

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: initialize_POP
! !INTERFACE:

 subroutine initialize_POP

! !DESCRIPTION:
!  This routine initializes a POP run by calling various module
!  initialization routines and setting up the initial temperature
!  and salinity
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
      k,                      &! dummy vertical level index
      ier                      ! error flag

!-----------------------------------------------------------------------
!
!  initialize message-passing or other communication protocol
!
!-----------------------------------------------------------------------

   call init_communicate

   if (my_task == master_task) then
      write(stdout,delim_fmt)
      write(stdout,blank_fmt)
      write(stdout,'(a30)') ' Parallel Ocean Program (POP) '
      write(stdout,'(a36)') ' Version 2.0.1 Released 21 Jan 2004'
      write(stdout,blank_fmt)
      write(stdout,delim_fmt)
   endif

!-----------------------------------------------------------------------
!
!  initialize constants and i/o stuff
!
!-----------------------------------------------------------------------

   call init_io
   call init_constants

!-----------------------------------------------------------------------
!
!  initialize domain and grid
!
!-----------------------------------------------------------------------

   call init_domain_blocks
   call init_grid1
   call init_domain_distribution(KMT_G)
   call init_grid2

!-----------------------------------------------------------------------
!
!  initialize timers and additional communication routines
!
!-----------------------------------------------------------------------

   call init_timers
   call init_global_reductions

!-----------------------------------------------------------------------
!
!  compute time step and initialize time-related quantities
!
!-----------------------------------------------------------------------

   call init_time_manager1

!-----------------------------------------------------------------------
!
!  initialize equation of state
!
!-----------------------------------------------------------------------

   call init_state

!-----------------------------------------------------------------------
!
!  calculate topographic stress (maximum entropy) velocities
!  initialize horizontal mixing variables
!
!-----------------------------------------------------------------------

   call init_topostress
   call init_horizontal_mix

!-----------------------------------------------------------------------
!
!  initialize advection variables
!
!-----------------------------------------------------------------------

   call init_advection

!-----------------------------------------------------------------------
!
!  initialize vertical mixing variables
!
!-----------------------------------------------------------------------

   call init_vertical_mix

!-----------------------------------------------------------------------
!
!  calculate time-independent stencil coefficients
!
!-----------------------------------------------------------------------

   call init_solvers    ! initialize barotropic solver and operators

!-----------------------------------------------------------------------
!
!  initialize pressure gradient (pressure averaging)
!  initialize baroclinic (reset to freezing)
!  initialize barotropic (barotropic-related diagnostics)
!  initialize surface_hgt (ssh-related diagnostics)
!
!-----------------------------------------------------------------------

   call init_pressure_grad
   call init_baroclinic
   call init_barotropic
   call init_surface_hgt

!-----------------------------------------------------------------------
!
!  initialize prognostic fields
!
!-----------------------------------------------------------------------

   call init_prognostic

!-----------------------------------------------------------------------
!
!  initialize ice module
!
!-----------------------------------------------------------------------

   call init_ice

!-----------------------------------------------------------------------
!
!  set initial temperature and salinity profiles (includes read of
!  restart file
!
!-----------------------------------------------------------------------

   call init_ts

!-----------------------------------------------------------------------
!
!  finish computing time-related quantities after restart info
!  available
!
!-----------------------------------------------------------------------

   call init_time_manager2

!-----------------------------------------------------------------------
!
!  initialize diagnostics
!
!-----------------------------------------------------------------------

   call init_diagnostics

!-----------------------------------------------------------------------
!
!  initialize fields for surface forcing
!
!-----------------------------------------------------------------------

   call init_forcing

!-----------------------------------------------------------------------
!
!  initialize output
!
!-----------------------------------------------------------------------

   call init_output

!-----------------------------------------------------------------------
!
!  Initialize drifters, hydrographic sections and current meters
!
!-----------------------------------------------------------------------

   !call init_drifters
   !call init_hydro_sections
   !call init_current_meters

!-----------------------------------------------------------------------
!
!  initialize X display 
!
!-----------------------------------------------------------------------

   call init_xdisplay

!-----------------------------------------------------------------------
!
!  output delimiter to log file
!
!-----------------------------------------------------------------------

   if (my_task == master_task) then
      write(stdout,delim_fmt)
      write(stdout,blank_fmt)
      write(stdout,'(" End of initialization")')
      write(stdout,blank_fmt)
      write(stdout,delim_fmt)
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine initialize_POP

!***********************************************************************
!BOP
! !IROUTINE: init_ts
! !INTERFACE:

 subroutine init_ts

! !DESCRIPTION:
!  Initializes temperature and salinity and
!  initializes prognostic variables from restart if required
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

   integer (int_kind) :: nml_error ! namelist i/o error flag

   character (char_len) :: &
      init_ts_option,      &! option for initializing t,s
      init_ts_file,        &! filename for input T,S file
      init_ts_file_fmt      ! format (bin or nc) for input file

   namelist /init_ts_nml/ init_ts_option, init_ts_file, init_ts_file_fmt

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      k,                 &! vertical level index
      n,                 &! tracer index
      kk,                &! indices for interpolating levitus data
      nu,                &! i/o unit for mean profile file
      iblock              ! local block address

   logical (log_kind) :: &
      lbranch             ! flag for telling restart branch init

   type (block) ::       &
      this_block          ! block information for current block

   real (r8) ::          &
      sinterp,           &! factor for interpolating levitus data
      dpth_meters         ! depth of level in meters

   real (r8), dimension(km) :: &
      tinit, sinit        ! mean initial state as function of depth

   type (datafile) ::    &
      in_file             ! data file type for init ts file

   type (io_field_desc) :: &
      io_temp, io_salt    ! io field descriptors for input T,S

   type (io_dim) :: &
      i_dim, j_dim, k_dim ! dimension descriptors

   real (r8), dimension(:,:,:,:), allocatable :: &
      TEMP_DATA           ! temp array for reading T,S data

   !***
   !*** 1992 Levitus mean climatology for internal generation of t,s
   !***

   real (r8), dimension(33) ::                   &
      depth_levitus = (/                         &
                   0.0_r8,   10.0_r8,   20.0_r8, &
                  30.0_r8,   50.0_r8,   75.0_r8, &
                 100.0_r8,  125.0_r8,  150.0_r8, &
                 200.0_r8,  250.0_r8,  300.0_r8, &
                 400.0_r8,  500.0_r8,  600.0_r8, &
                 700.0_r8,  800.0_r8,  900.0_r8, &
                1000.0_r8, 1100.0_r8, 1200.0_r8, &
                1300.0_r8, 1400.0_r8, 1500.0_r8, &
                1750.0_r8, 2000.0_r8, 2500.0_r8, &
                3000.0_r8, 3500.0_r8, 4000.0_r8, &
                4500.0_r8, 5000.0_r8, 5500.0_r8 /)

   real (r8), dimension(33) ::                   &
      tmean_levitus = (/                         &
                 18.27_r8, 18.22_r8, 18.09_r8,   &
                 17.87_r8, 17.17_r8, 16.11_r8,   &
                 15.07_r8, 14.12_r8, 13.29_r8,   &
                 11.87_r8, 10.78_r8,  9.94_r8,   &
                  8.53_r8,  7.35_r8,  6.38_r8,   &
                  5.65_r8,  5.06_r8,  4.57_r8,   &
                  4.13_r8,  3.80_r8,  3.51_r8,   &
                  3.26_r8,  3.05_r8,  2.86_r8,   &
                  2.47_r8,  2.19_r8,  1.78_r8,   &
                  1.49_r8,  1.26_r8,  1.05_r8,   &
                  0.91_r8,  0.87_r8,  1.00_r8 /)

   real (r8), dimension(33) ::                   &
      smean_levitus = (/                         &
                 34.57_r8, 34.67_r8, 34.73_r8,   &
                 34.79_r8, 34.89_r8, 34.97_r8,   &
                 35.01_r8, 35.03_r8, 35.03_r8,   &
                 34.98_r8, 34.92_r8, 34.86_r8,   &
                 34.76_r8, 34.68_r8, 34.63_r8,   &
                 34.60_r8, 34.59_r8, 34.60_r8,   &
                 34.61_r8, 34.63_r8, 34.65_r8,   &
                 34.66_r8, 34.68_r8, 34.70_r8,   &
                 34.72_r8, 34.74_r8, 34.75_r8,   &
                 34.74_r8, 34.74_r8, 34.73_r8,   &
                 34.73_r8, 34.72_r8, 34.72_r8 /)

!-----------------------------------------------------------------------
!
!  read input namelist and broadcast 
!
!-----------------------------------------------------------------------

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=init_ts_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading init_ts_nml')
   endif

   call broadcast_scalar(init_ts_option  , master_task)
   call broadcast_scalar(init_ts_file    , master_task)
   call broadcast_scalar(init_ts_file_fmt, master_task)

!-----------------------------------------------------------------------
!
!  initialize t,s or call restart based on init_ts_option
!
!-----------------------------------------------------------------------

   select case (init_ts_option)

!-----------------------------------------------------------------------
!
!  set initial state from restart file
!
!-----------------------------------------------------------------------

   case ('restart')
      first_step = .false.
      lbranch = .false.
      if (my_task == master_task) then
         write(stdout,'(a35,a)') 'Initial T,S read from restart file:',&
                                  trim(init_ts_file)
      endif
      call read_restart(init_ts_file,lbranch,init_ts_file_fmt)
      ltavg_restart = .true.

!-----------------------------------------------------------------------
!
!  set initial state from restart file
!
!-----------------------------------------------------------------------

   case ('branch')
      first_step = .false.
      lbranch = .true.
      if (my_task == master_task) then
         write(stdout,'(a40,a)') &
            'Initial T,S branching from restart file:', &
            trim(init_ts_file)
      endif
      call read_restart(init_ts_file,lbranch,init_ts_file_fmt)
      ltavg_restart = .false.

!-----------------------------------------------------------------------
!
!  read full 3-d t,s from input file
!
!-----------------------------------------------------------------------

   case ('file')
      first_step = .true.

      if (my_task == master_task) then
         write(stdout,'(a31,a)') 'Initial 3-d T,S read from file:', &
                                 trim(init_ts_file)
      endif

      allocate(TEMP_DATA(nx_block,ny_block,km,max_blocks_clinic))

      in_file = construct_file(init_ts_file_fmt,             &
                               full_name=trim(init_ts_file), &
                               record_length = rec_type_dbl, &
                               recl_words=nx_global*ny_global)
      call data_set(in_file,'open_read')

      i_dim = construct_io_dim('i',nx_global)
      j_dim = construct_io_dim('j',ny_global)
      k_dim = construct_io_dim('k',km)

      io_temp = construct_io_field('TEMPERATURE', &
                 i_dim, j_dim, dim3=k_dim,        &
                 field_loc = field_loc_center,    &
                 field_type = field_type_scalar,  &
                 d3d_array=TEMP_DATA)
      io_salt = construct_io_field('SALINITY',    &
                 i_dim, j_dim, dim3=k_dim,        &
                 field_loc = field_loc_center,    &
                 field_type = field_type_scalar,  &
                 d3d_array=TEMP_DATA)

      call data_set(in_file,'define',io_temp)
      call data_set(in_file,'define',io_salt)

      call data_set(in_file,'read'  ,io_temp)
      do iblock=1,nblocks_clinic
         TRACER(:,:,:,1,curtime,iblock) = TEMP_DATA(:,:,:,iblock)
      end do
      call data_set(in_file,'read'  ,io_salt)
      do iblock=1,nblocks_clinic
         TRACER(:,:,:,2,curtime,iblock) = TEMP_DATA(:,:,:,iblock)
      end do

      call destroy_io_field(io_temp)
      call destroy_io_field(io_salt)

      deallocate(TEMP_DATA)

      call data_set(in_file,'close')
      call destroy_file(in_file)

      if (my_task == master_task) then
         write(stdout,blank_fmt)
         write(stdout,'(a12,a)') ' file read: ', trim(init_ts_file)
      endif


      !$OMP PARALLEL DO PRIVATE(iblock, k, n)
      do iblock = 1,nblocks_clinic
         do n=1,nt
         do k=1,km
             where (k > KMT(:,:,iblock)) &
                TRACER(:,:,k,n,curtime,iblock) = c0
         end do
         end do
 
         !*** convert salinity to model units
         TRACER(:,:,:,2,curtime,iblock) = &
         TRACER(:,:,:,2,curtime,iblock)*ppt_to_salt
      end do
      !$OMP END PARALLEL DO

      if (topo_smooth) then
         do k=1,km
            call fill_points(k,TRACER(:,:,k,1,curtime,:))
            call fill_points(k,TRACER(:,:,k,2,curtime,:))
         enddo
      endif

      do iblock=1,nblocks_clinic
         TRACER(:,:,:,:,newtime,iblock) = TRACER(:,:,:,:,curtime,iblock)
         TRACER(:,:,:,:,oldtime,iblock) = TRACER(:,:,:,:,curtime,iblock)
      end do

!-----------------------------------------------------------------------
!
!  set up t,s from input mean state as function of depth
!
!-----------------------------------------------------------------------

   case ('mean')
      first_step = .true.

      !***
      !*** open input file and read t,s profile
      !***

      call get_unit(nu)
      if (my_task == master_task) then 
         write(stdout,'(a40,a)') &
            'Initial mean T,S profile read from file:', &
            trim(init_ts_file)
         open(nu, file=init_ts_file, status='old')
         do k = 1,km
            read(nu,*) tinit(k),sinit(k)
         enddo
         close (nu)
      endif
      call release_unit(nu)

      call broadcast_array(tinit, master_task)
      call broadcast_array(sinit, master_task)

      !***
      !*** fill tracer array with appropriate values
      !***

      !$OMP PARALLEL DO PRIVATE(iblock, k)
      do iblock = 1,nblocks_clinic
         do k=1,km
            where (k <= KMT(:,:,iblock))
               TRACER(:,:,k,1,curtime,iblock) = tinit(k)
               TRACER(:,:,k,2,curtime,iblock) = sinit(k)*ppt_to_salt
            endwhere
         enddo

         TRACER(:,:,:,:,newtime,iblock)=TRACER(:,:,:,:,curtime,iblock)
         TRACER(:,:,:,:,oldtime,iblock)=TRACER(:,:,:,:,curtime,iblock)
      end do
      !$OMP END PARALLEL DO

!-----------------------------------------------------------------------
!
!  set up initial profile from 1992 Levitus mean ocean data
!
!-----------------------------------------------------------------------

   case ('internal')
      first_step = .true.
      if (my_task == master_task) then
         write(stdout,'(a63)') & 
        'Initial T,S profile computed internally from 1992 Levitus data'
      endif

      !$OMP PARALLEL DO PRIVATE(iblock, k, kk, &
      !$OMP                     dpth_meters, sinterp, tinit, sinit)

      do iblock = 1,nblocks_clinic
         do k=1,km

            dpth_meters = zt(k)*mpercm

            intrp_loop: do kk=1,32
               if (dpth_meters >= depth_levitus(kk) .and. &
                   dpth_meters <  depth_levitus(kk+1)) exit intrp_loop
            end do intrp_loop

            sinterp = (dpth_meters         - depth_levitus(kk))/ &
                      (depth_levitus(kk+1) - depth_levitus(kk))
 
            tinit(k) = (c1 - sinterp)*tmean_levitus(kk) + &
                             sinterp *tmean_levitus(kk+1)
            sinit(k) = (c1 - sinterp)*smean_levitus(kk) + &
                             sinterp *smean_levitus(kk+1)

            where (k <= KMT(:,:,iblock))
               TRACER(:,:,k,1,curtime,iblock) = tinit(k)
               TRACER(:,:,k,2,curtime,iblock) = sinit(k)*ppt_to_salt
            endwhere

         enddo

         TRACER(:,:,:,:,newtime,iblock)=TRACER(:,:,:,:,curtime,iblock)
         TRACER(:,:,:,:,oldtime,iblock)=TRACER(:,:,:,:,curtime,iblock)
      enddo
      !$OMP END PARALLEL DO

!-----------------------------------------------------------------------
!
!  bad initialization option
!
!-----------------------------------------------------------------------

   case default
      call exit_POP(sigAbort,'Unknown t,s initialization option')
   end select

!-----------------------------------------------------------------------
!
!  calculate RHO from TRACER at time levels curtime and oldtime
!
!-----------------------------------------------------------------------

   !$OMP PARALLEL DO PRIVATE(iblock, k, this_block)
   do iblock = 1,nblocks_clinic
      this_block = get_block(blocks_clinic(iblock),iblock)

      do k=1,km
         call state(k,k,TRACER(:,:,k,1,curtime,iblock), &
                        TRACER(:,:,k,2,curtime,iblock), &
                        this_block,                     &
                        RHOOUT=RHO(:,:,k,curtime,iblock))
         call state(k,k,TRACER(:,:,k,1,oldtime,iblock), &
                        TRACER(:,:,k,2,oldtime,iblock), &
                        this_block,                     &
                        RHOOUT=RHO(:,:,k,oldtime,iblock))
      enddo

   enddo ! block loop
   !$OMP END PARALLEL DO

!-----------------------------------------------------------------------
!EOC

 end subroutine init_ts

!***********************************************************************

 end module initial

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
