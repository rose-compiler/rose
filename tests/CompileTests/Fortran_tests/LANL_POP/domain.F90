!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module domain

!BOP
! !MODULE: domain
!
! !DESCRIPTION:
!  This module contains the model domain and routines for initializing
!  the domain.  It also initializes the decompositions and
!  distributions across processors/threads by calling relevent
!  routines in the block, distribution modules.
!
! !REVISION HISTORY:
!  CVS:$Id: domain.F90,v 1.22 2003/02/03 20:15:33 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod
   use constants
! DQ (8/4/2010): Can't file this file.
!   use communicate
!   use broadcast
   use blocks
   use distribution
   use exit_mod
   use io_types
   use boundary
   use domain_size

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS

   public  :: init_domain_blocks ,&
              init_domain_distribution

! !PUBLIC DATA MEMBERS:

   integer (int_kind), public :: &
      nblocks_clinic   ,&! actual number of blocks on this processor
      nblocks_tropic     !   in each distribution

   integer (int_kind), dimension(:), pointer, public :: &
      blocks_clinic    ,&! block ids for local blocks in baroclinic dist
      blocks_tropic      ! block ids for local blocks in barotropic dist

   type (distrb), public :: & !  block distribution info
      distrb_clinic    ,&! block distribution for baroclinic part
      distrb_tropic      ! block distribution for barotropic part

   type (bndy), public :: &!  ghost cell update info
      bndy_clinic        ,&! block distribution for baroclinic part
      bndy_tropic          ! block distribution for barotropic part

   logical (log_kind), public :: &!
      ltripole_grid        ! flag to signal use of tripole grid

!EOP
!BOC
!-----------------------------------------------------------------------
!
!   module private variables - for the most part these appear as
!   module variables to facilitate sharing info between init_domain1
!   and init_domain2.
!
!-----------------------------------------------------------------------

    character (char_len) ::      &
       clinic_distribution_type, &! method to use for distributing
       tropic_distribution_type, &!    blocks in each case
       ew_boundary_type,         &! type of domain bndy in each logical
       ns_boundary_type           !    direction (ew is i, ns is j)

    integer (int_kind) :: &! decomposition info
       nprocs_clinic     ,&! num of processors in baroclinic dist
       nprocs_tropic       ! num of processors in barotropic dist

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_domain_blocks
! !INTERFACE:

 subroutine init_domain_blocks

! !DESCRIPTION:
!  This routine reads in domain information and calls the routine
!  to set up the block decomposition.
!
! !REVISION HISTORY:
!  same as module
!EOP
!BOC
!----------------------------------------------------------------------
!
!  local variables
!
!----------------------------------------------------------------------

   integer (int_kind) :: &
      nml_error          ! namelist read error flag

!----------------------------------------------------------------------
!
!  input namelists
!
!----------------------------------------------------------------------

   namelist /domain_nml/ nprocs_clinic, nprocs_tropic, &
                         clinic_distribution_type, &
                         tropic_distribution_type, &
                         ew_boundary_type,         &
                         ns_boundary_type

!----------------------------------------------------------------------
!
!  read domain information from namelist input
!
!----------------------------------------------------------------------

   nprocs_clinic = -1
   nprocs_tropic = -1
   clinic_distribution_type = 'balanced'
   tropic_distribution_type = 'cartesian'
   ew_boundary_type = 'cyclic'
   ns_boundary_type = 'closed'

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=domain_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading domain_nml')
   endif

   call broadcast_scalar(nprocs_clinic,            master_task)
   call broadcast_scalar(nprocs_tropic,            master_task)
   call broadcast_scalar(clinic_distribution_type, master_task)
   call broadcast_scalar(tropic_distribution_type, master_task)
   call broadcast_scalar(ew_boundary_type,         master_task)
   call broadcast_scalar(ns_boundary_type,         master_task)

!----------------------------------------------------------------------
!
!  perform some basic checks on domain
!
!----------------------------------------------------------------------

   if (trim(ns_boundary_type) == 'tripole') then
      ltripole_grid = .true.
   else
      ltripole_grid = .false.
   endif

   if (nx_global < 1 .or. ny_global < 1 .or. km < 1) then
      !***
      !*** domain size zero or negative
      !***
      call exit_POP(sigAbort,'Invalid domain: size < 1') ! no domain
   else if (nt < 2) then
      !***
      !*** nt must be at least 2 to hold temp,salinitiy
      !***
      call exit_POP(sigAbort,'Invalid tracer number: nt < 2')
   else if (nprocs_clinic /= get_num_procs()) then
      !***
      !*** input nprocs does not match system (eg MPI) request
      !***
      call exit_POP(sigAbort,'Input nprocs not same as system request')
   else if (nprocs_tropic > nprocs_clinic) then
      !***
      !*** number of barotropic procs must be <= baroclinic
      !***
      call exit_POP(sigAbort, &
                    'Too many processors assigned to barotropic')
   else if (nghost < 2) then
      !***
      !*** must have at least 2 layers of ghost cells
      !***
      call exit_POP(sigAbort,'Not enough ghost cells allocated')
   endif

!----------------------------------------------------------------------
!
!  compute block decomposition and details
!
!----------------------------------------------------------------------

   call create_blocks(nx_global, ny_global, trim(ew_boundary_type), &
                                            trim(ns_boundary_type))

!----------------------------------------------------------------------
!
!  Now we need grid info before proceeding further
!  Print some domain information
!
!----------------------------------------------------------------------

   if (my_task == master_task) then
     write(stdout,delim_fmt)
     write(stdout,blank_fmt)
     write(stdout,'(a18)') 'Domain Information'
     write(stdout,blank_fmt)
     write(stdout,delim_fmt)
     write(stdout,'(a26,i6)') '  Horizontal domain: nx = ',nx_global
     write(stdout,'(a26,i6)') '                     ny = ',ny_global
     write(stdout,'(a26,i6)') '  Vertical   domain: km = ',km
     write(stdout,'(a26,i6)') '  Number of tracers: nt = ',nt
     write(stdout,'(a26,i6)') '  Block size:  nx_block = ',nx_block
     write(stdout,'(a26,i6)') '               ny_block = ',ny_block
     write(stdout,'(a29,i6)') '  Processors for baroclinic: ', &
                                 nprocs_clinic
     write(stdout,'(a29,i6)') '  Processors for barotropic: ', &
                                 nprocs_tropic
     write(stdout,'(a31,a9)') '  Distribution for baroclinic: ', &
                                 trim(clinic_distribution_type)
     write(stdout,'(a31,a9)') '  Distribution for barotropic: ', &
                                 trim(tropic_distribution_type)
     write(stdout,'(a25,i2)') '  Number of ghost cells: ', nghost
   endif

!----------------------------------------------------------------------
!EOC

 end subroutine init_domain_blocks

!***********************************************************************
!BOP
! !IROUTINE: init_domain_distribution
! !INTERFACE:

 subroutine init_domain_distribution(KMTG)

! !DESCRIPTION:
!  This routine calls appropriate setup routines to distribute blocks
!  across processors and defines arrays with block ids for any local
!  blocks. Information about ghost cell update routines is also
!  initialized here through calls to the appropriate boundary routines.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), dimension(nx_global,ny_global), intent(in) :: &
      KMTG             ! global KMT (topography) field

!EOP
!BOC
!----------------------------------------------------------------------
!
!  local variables
!
!----------------------------------------------------------------------

   character (char_len) :: outstring

   integer (int_kind), parameter :: &
      max_work_unit=10   ! quantize the work into values from 1,max

   integer (int_kind) :: &
      i,j,k,n              ,&! dummy loop indices
      count1, count2       ,&! dummy counters
      work_unit            ,&! size of quantized work unit
      nblocks_tmp          ,&! temporary value of nblocks
      nblocks_tmp_clinic   ,&! num blocks on proc for clinic
      nblocks_max_clinic   ,&! max blocks on proc for clinic
      nblocks_tmp_tropic   ,&! num blocks on proc for tropic
      nblocks_max_tropic     ! max blocks on proc for tropic

   integer (int_kind), dimension(:), allocatable :: &
      nocn               ,&! number of ocean points per block
      work_per_block       ! number of work units per block

   type (block) :: &
      this_block         ! block information for current block

!----------------------------------------------------------------------
!
!  estimate the amount of work per processor using the topography
!
!----------------------------------------------------------------------

   allocate(nocn(nblocks_tot))

   nocn = 0
   do n=1,nblocks_tot
      this_block = get_block(n,n)
      !do i=this_block%ib,this_block%ie
      !   if (KMTG(this_block%i_glob(i),&
      !            this_block%j_glob(j)) > 0) nocn(n) = nocn(n) + 1
      !end do
      !end do
      !do j=1,ny_block
      do j=this_block%jb,this_block%je
         if (this_block%j_glob(j) > 0) then
            do i=1,nx_block
               if (this_block%i_glob(i) > 0) then
                  if (KMTG(this_block%i_glob(i),&
                           this_block%j_glob(j)) > 0) nocn(n) = nocn(n) + 1
               endif
            end do
         endif
      end do

      !*** with array syntax, we actually do work on non-ocean
      !*** points, so where the block is not completely land,
      !*** reset nocn to be the full size of the block

      if (nocn(n) > 0) nocn(n) = nx_block*ny_block
   end do

   work_unit = maxval(nocn)/max_work_unit + 1

   !*** find number of work units per block

   allocate(work_per_block(nblocks_tot))

   where (nocn > 0)
     work_per_block = nocn/work_unit + 1
   elsewhere
     work_per_block = 0
   end where
   deallocate(nocn)

!----------------------------------------------------------------------
!
!  determine the distribution of blocks across processors
!
!----------------------------------------------------------------------

   distrb_tropic = create_distribution(tropic_distribution_type, &
                                       nprocs_tropic, work_per_block)

   distrb_clinic = create_distribution(clinic_distribution_type, &
                                       nprocs_clinic, work_per_block)

   deallocate(work_per_block)

!----------------------------------------------------------------------
!
!  allocate and determine block id for any local blocks in each
!  distribution.
!
!----------------------------------------------------------------------

   call create_local_block_ids(blocks_clinic, distrb_clinic)
   call create_local_block_ids(blocks_tropic, distrb_tropic)

   if (associated(blocks_clinic)) then
      nblocks_clinic = size(blocks_clinic)
   else
      nblocks_clinic = 0
   endif
   nblocks_max_clinic = 0
   do n=0,distrb_clinic%nprocs - 1
     nblocks_tmp_clinic = nblocks_clinic
     call broadcast_scalar(nblocks_tmp_clinic, n)
     nblocks_max_clinic = max(nblocks_max_clinic,nblocks_tmp_clinic)
   end do

   if (nblocks_max_clinic > max_blocks_clinic) then
     write(outstring,*) 'clinic blocks exceed max: increase max to',&
                         nblocks_max_clinic
     call exit_POP(sigAbort,trim(outstring))
   else if (nblocks_max_clinic < max_blocks_clinic) then
     write(outstring,*) 'clinic blocks too large: decrease max to',&
                         nblocks_max_clinic
     if (my_task == master_task) write(stdout,*) trim(outstring)
   endif

   if (my_task < distrb_tropic%nprocs .and. &
       associated(blocks_tropic)) then
     nblocks_tropic = size(blocks_tropic)
   else
     nblocks_tropic = 0
   endif

   nblocks_max_tropic = 0
   do n=0,distrb_tropic%nprocs - 1
     nblocks_tmp = nblocks_tropic
     call broadcast_scalar(nblocks_tmp, n)
     nblocks_max_tropic = max(nblocks_max_tropic,nblocks_tmp)
   end do

   if (nblocks_max_tropic > max_blocks_tropic) then
     write(outstring,*) 'tropic blocks exceed max: increase max to',&
                         nblocks_max_tropic
     call exit_POP(sigAbort,trim(outstring))
   else if (nblocks_max_tropic < max_blocks_tropic) then
     write(outstring,*) 'tropic blocks too large: decrease max to',&
                         nblocks_max_tropic
     if (my_task == master_task) write(stdout,*) trim(outstring)
     !call exit_POP(sigAbort,trim(outstring))
   endif

!----------------------------------------------------------------------
!
!  set up ghost cell updates for each distribution
!  Boundary types are cyclic, closed, or tripole
!
!----------------------------------------------------------------------

   call create_boundary(bndy_clinic, distrb_clinic, &
                        trim(ns_boundary_type),     &
                        trim(ew_boundary_type),     &
                        nx_global, ny_global)
   call create_boundary(bndy_tropic, distrb_tropic, &
                        trim(ns_boundary_type),     &
                        trim(ew_boundary_type),     &
                        nx_global, ny_global)

!----------------------------------------------------------------------
!EOC

 end subroutine init_domain_distribution

!***********************************************************************

 end module domain

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
