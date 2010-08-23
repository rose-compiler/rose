!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module horizontal_mix

!BOP
! !MODULE: horizontal_mix
!
! !DESCRIPTION:
!  This module contains driver routines for managing the individual
!  horizontal tracer and momentum mixing modules.
!
! !REVISION HISTORY:
!  CVS:$Id: horizontal_mix.F90,v 1.9 2003/01/09 15:50:55 jfd Exp $
!  CVS:$Name: POP_2_0_1 $
!
! !USES:

   use kinds_mod, only: int_kind, r8, char_len
   use blocks, only: nx_block, ny_block, block
   use distribution, only: 
   use domain, only: nblocks_clinic, distrb_clinic
   use constants, only: c0, blank_fmt, delim_fmt
   use communicate, only: my_task, master_task
   use time_management, only: km, nt, mix_pass
   use broadcast, only: broadcast_scalar
   use grid, only: KMT, dz
   use io_types, only: nml_in, nml_filename, stdout
   use hmix_del2, only: init_del2u, init_del2t, hdiffu_del2, hdifft_del2
   use hmix_del4, only: init_del4u, init_del4t, hdiffu_del4, hdifft_del4
   use hmix_gm, only: init_gm, hdifft_gm
   use hmix_aniso, only: init_aniso, hdiffu_aniso
   use topostress, only: ltopostress
   use tavg, only: define_tavg_field, tavg_requested, accumulate_tavg_field
   use timers, only: timer_start, timer_stop, get_timer
   use exit_mod, only: sigAbort, exit_pop

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_horizontal_mix, &
             hdiffu, hdifft

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  horizontal mixing choices
!
!-----------------------------------------------------------------------

   integer (int_kind), parameter :: &! available choices for mixing type
      hmix_momentum_type_del2 = 1,  &
      hmix_momentum_type_del4 = 2,  &
      hmix_momentum_type_anis = 3,  &
      hmix_tracer_type_del2 = 1,    &
      hmix_tracer_type_del4 = 2,    &
      hmix_tracer_type_gm   = 3

   integer (int_kind) ::            &
      hmix_momentum_itype,          &! users choice for type of mixing
      hmix_tracer_itype,            &! users choice for type of mixing
      tavg_HDIFT                     ! tavg id for horizontal diffusion

!-----------------------------------------------------------------------
!
!  timers
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      timer_hdiffu,      &! timer for horizontal momentum mixing
      timer_hdifft        ! timer for horizontal tracer   mixing

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_horizontal_mix
! !INTERFACE:

 subroutine init_horizontal_mix

! !DESCRIPTION:
!  Initializes choice of mixing method based on namelist input.
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

   integer (int_kind) :: nml_error  ! error flag for namelist

   character (char_len) ::  &! character choice for type of mixing
      hmix_momentum_choice, &
      hmix_tracer_choice

   namelist /hmix_nml/ hmix_momentum_choice, hmix_tracer_choice

!-----------------------------------------------------------------------
!
!  read namelist input
!
!-----------------------------------------------------------------------

   hmix_momentum_choice = 'unknown_hmix_momentum_choice'
   hmix_tracer_choice = 'unknown_hmix_tracer_choice'

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=hmix_nml, iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading hmix_nml')
   endif

   if (my_task == master_task) then

      write(stdout,delim_fmt)
      write(stdout,blank_fmt)
      write(stdout,'(a25)') 'Horizontal mixing options'
      write(stdout,blank_fmt)
      write(stdout,delim_fmt)

      select case (hmix_momentum_choice(1:4))
      case ('del2')
         hmix_momentum_itype = hmix_momentum_type_del2
         write(stdout,'(a42)') &
           'Laplacian horizontal momentum mixing used.'
      case ('del4')
         hmix_momentum_itype = hmix_momentum_type_del4
         write(stdout,'(a43)') &
           'Biharmonic horizontal momentum mixing used.'
      case ('anis')
         hmix_momentum_itype = hmix_momentum_type_anis
         write(stdout,'(a44)') &
           'Anisotropic horizontal momentum mixing used.'
      case ('gent')
         hmix_momentum_itype = -1000
      case default
         hmix_momentum_itype = -2000
      end select

      select case (hmix_tracer_choice(1:4))
      case ('del2')
         hmix_tracer_itype = hmix_tracer_type_del2
         write(stdout,'(a44)') &
           'Laplacian horizontal tracer   mixing chosen.'
      case ('del4')
         hmix_tracer_itype = hmix_tracer_type_del4
         write(stdout,'(a43)') &
           'Biharmonic horizontal tracer   mixing used.'
      case ('gent')
         hmix_tracer_itype = hmix_tracer_type_gm
         write(stdout,'(a35)') &
          'Gent-McWilliams tracer mixing used.'
      case default
         hmix_tracer_itype = -1000
      end select

   endif

   call broadcast_scalar(hmix_momentum_itype, master_task)
   call broadcast_scalar(hmix_tracer_itype,   master_task)

   if (hmix_momentum_itype == -1000) then
      call exit_POP(sigAbort, &
              'Gent-McWilliams can only be used for tracer mixing')
   else if (hmix_momentum_itype == -2000) then
      call exit_POP(sigAbort, &
                    'Unknown type for horizontal momentum mixing')
   endif

   if (hmix_tracer_itype == -1000) then
      call exit_POP(sigAbort, &
                    'Unknown type for horizontal tracer mixing')
   endif

!-----------------------------------------------------------------------
!
!  calculate additional coefficients based on mixing parameterization
!  initialize timers
!
!-----------------------------------------------------------------------

   select case (hmix_momentum_itype)
   case(hmix_momentum_type_del2)
      call init_del2u
      call get_timer(timer_hdiffu,'HMIX_MOMENTUM_DEL2', &
                                  nblocks_clinic, distrb_clinic%nprocs)

   case(hmix_momentum_type_del4)
      call init_del4u
      call get_timer(timer_hdiffu,'HMIX_MOMENTUM_DEL4', &
                                  nblocks_clinic, distrb_clinic%nprocs)

   case(hmix_momentum_type_anis)
      call init_aniso 
      call get_timer(timer_hdiffu,'HMIX_MOMENTUM_ANISO', &
                                  nblocks_clinic, distrb_clinic%nprocs)
   end select

   select case (hmix_tracer_itype)
   case(hmix_tracer_type_del2)
      call init_del2t
      call get_timer(timer_hdifft,'HMIX_TRACER_DEL2', &
                                  nblocks_clinic, distrb_clinic%nprocs)

   case(hmix_tracer_type_del4)
      call init_del4t
      call get_timer(timer_hdifft,'HMIX_TRACER_DEL4', &
                                  nblocks_clinic, distrb_clinic%nprocs)

   case(hmix_tracer_type_gm)
      call init_gm  ! variables used by GM parameterization
      call get_timer(timer_hdifft,'HMIX_TRACER_GM', &
                                  nblocks_clinic, distrb_clinic%nprocs)

   end select

!-----------------------------------------------------------------------
!
!  check for compatibility with topostress
!
!-----------------------------------------------------------------------

   if (ltopostress .and. &
       hmix_momentum_itype /= hmix_momentum_type_del2) then
      if (my_task == master_task) write(stdout,'(a59)') &
         'WARNING: TOPOSTRESS HAS NO EFFECT IF DEL2 MIXING NOT CHOSEN'
   endif

!-----------------------------------------------------------------------
!
!  define tavg field for tavg diagnostics
!
!-----------------------------------------------------------------------

   call define_tavg_field(tavg_HDIFT,'HDIFT',2,                        &
                    long_name='Vertically integrated horz mix T tend', &
                          units='degC/cm^2', grid_loc='2110')

!-----------------------------------------------------------------------
!EOC

 end subroutine init_horizontal_mix

!***********************************************************************
!BOP
! !IROUTINE: hdiffu
! !INTERFACE:

 subroutine hdiffu(k,HDUK,HDVK,UMIXK,VMIXK,this_block)

! !DESCRIPTION:
!  This routine returns tendencies for horizontal diffusion of
!  momentum.  It is a driver routine which simply branches to the
!  proper horizontal mix routine based on the user choice of mixing
!  method.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: k   ! depth level index

   real (r8), dimension(nx_block,ny_block), intent(in) :: &
      UMIXK, VMIXK         ! U,V at level k and mix time level

   type (block), intent(in) :: &
      this_block           ! block information for this subblock

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(out) :: &
      HDUK,                   &! returned as Hdiff(U) at level k
      HDVK                     ! returned as Hdiff(V) at level k

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  branch to the proper mix routine
!
!-----------------------------------------------------------------------

   call timer_start(timer_hdiffu, block_id=this_block%local_id)

   select case (hmix_momentum_itype)

   case (hmix_momentum_type_del2)
      call hdiffu_del2(k, HDUK, HDVK, UMIXK, VMIXK, this_block)
   case (hmix_momentum_type_del4)
      call hdiffu_del4(k, HDUK, HDVK, UMIXK, VMIXK, this_block)
   case (hmix_momentum_type_anis)
      call hdiffu_aniso(k, HDUK, HDVK, UMIXK, VMIXK, this_block)
   end select

   call timer_stop(timer_hdiffu, block_id=this_block%local_id)

!-----------------------------------------------------------------------
!EOC

 end subroutine hdiffu

!***********************************************************************
!BOP
! !IROUTINE: hdifft
! !INTERFACE:

 subroutine hdifft(k, HDTK, TMIX, UMIX, VMIX, this_block)

! !DESCRIPTION:
!  This routine returns tendencies for horizontal diffusion of
!  tracers.  It is a driver routine which simply branches to the
!  proper horizontal mix routine based on the user choice of mixing
!  method.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: k   ! depth level index

   real (r8), dimension(nx_block,ny_block,km,nt), intent(in) :: &
      TMIX     ! tracers at mix time level

   real (r8), dimension(nx_block,ny_block,km), intent(in) :: &
      UMIX, VMIX   ! U,V velocities at mix time level

   type (block), intent(in) :: &
      this_block           ! block information for this subblock

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,nt), intent(out) :: &
      HDTK                ! Hdiff(T) for nth tracer at level k

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      bid                 ! local block id

   real (r8), dimension(nx_block,ny_block) :: &
     WORK                 ! temporary to hold tavg field

!-----------------------------------------------------------------------
!
!  branch to the proper mix routine
!
!-----------------------------------------------------------------------

   bid = this_block%local_id

   call timer_start(timer_hdifft, block_id=bid)

   select case (hmix_tracer_itype)
   case (hmix_tracer_type_del2)
      call hdifft_del2(k, HDTK, TMIX, this_block)
   case (hmix_tracer_type_del4)
      call hdifft_del4(k, HDTK, TMIX, this_block)
   case (hmix_tracer_type_gm)
      call hdifft_gm(k, HDTK, TMIX, UMIX, VMIX, this_block)
   end select

   call timer_stop(timer_hdifft, block_id=bid)

!-----------------------------------------------------------------------
!
!  compute tavg diagnostic if requested
!
!-----------------------------------------------------------------------

   if (tavg_requested(tavg_HDIFT) .and. mix_pass /= 1) then

     where (k <= KMT(:,:,bid))
        WORK = dz(k)*HDTK(:,:,1)
     elsewhere
        WORK = c0
     end where

     call accumulate_tavg_field(WORK,tavg_HDIFT,bid,k)
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine hdifft

!***********************************************************************

 end module horizontal_mix

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
