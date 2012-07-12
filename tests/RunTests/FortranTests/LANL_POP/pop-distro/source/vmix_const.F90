!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module vmix_const

!BOP
! !MODULE: vmix_const
!
! !DESCRIPTION:
!  This module initializes and computes vertical diffusivity and
!  viscosity for constant vertical mixing.
!
! !REVISION HISTORY:
!  CVS:$Id: vmix_const.F90,v 1.8 2002/11/27 17:53:22 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod
   use blocks
   use distribution
   use grid
   use domain_size
   use domain
   use constants
   use broadcast
   use io
   use state_mod
   use exit_mod

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_vmix_const,  &
             vmix_coeffs_const

!EOP
!BOC
!-----------------------------------------------------------------------
!
! module variables
!
!-----------------------------------------------------------------------

   real (r8) ::       &
      const_vvc,      &! constant value for viscosity
      const_vdc        ! constant value for diffusivity

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_vmix_const
! !INTERFACE:

 subroutine init_vmix_const(VDC,VVC)

! !DESCRIPTION:
!  In this routine the diffusivity (VDC) and viscosity (VVC) are 
!  initialized to input values and remain constant throughout the
!  simulation.
!
! !REVISION HISTORY:
!  same as module

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(:,:,:,:,:), intent(inout) :: &
      VDC                 ! vertical tracer diffusivity

   real (r8), dimension(:,:,:,:), intent(inout) :: &
      VVC                 ! vertical viscosity for momentum

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: nml_error

   namelist /vmix_const_nml/const_vvc, const_vdc

!-----------------------------------------------------------------------
!
!  set defaults for mixing coefficients, then read them from namelist
!
!-----------------------------------------------------------------------

   const_vvc = p25
   const_vdc = p25
      
   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=vmix_const_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading vmix_const_nml')
   endif

   if (my_task == master_task) then
      write(stdout,'(a17,2x,1pe12.5)') '  const_vdc     =',const_vdc
      write(stdout,'(a17,2x,1pe12.5)') '  const_vvc     =',const_vvc
   endif

   call broadcast_scalar(const_vvc, master_task)
   call broadcast_scalar(const_vdc, master_task)

!-----------------------------------------------------------------------
!
!  set mixing coefficients for all time
!
!-----------------------------------------------------------------------

   VVC = const_vvc
   VDC = const_vdc

!-----------------------------------------------------------------------
!EOC

 end subroutine init_vmix_const

!***********************************************************************
!BOP
! !IROUTINE: vmix_coeffs_const
! !INTERFACE:

 subroutine vmix_coeffs_const(k,VDC,VVC,TMIX,                &
                                this_block, convect_diff, convect_visc)

! !DESCRIPTION:
!  Computes vertical diffusion coefficients. Because these are
!  independent of time, no calculation are done here unless a
!  diffusive form of convection is chosen.  In such a case,
!  the routine checks for stability and enhances the diffusion
!  coefficients with the convective diffusion coefficient.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: k  ! vertical level index

   real (r8), dimension(nx_block,ny_block,km,nt), intent(in) :: &
      TMIX                ! tracers at mix time level

   real (r8), intent(in), optional :: &
      convect_diff,      &! diffusivity to mimic convection
      convect_visc        ! viscosity   to mimic convection

   type (block) :: &
      this_block          ! block information for current block

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(:,:,:,:), intent(inout) :: &
      VDC                 ! vertical tracer diffusivity

   real (r8), dimension(:,:,:), intent(inout) :: &
      VVC                 ! vertical viscosity for momentum

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: & 
      kp1,               &! vertical level index
      bid                 ! local block address

   real (r8), dimension(nx_block,ny_block) :: &
      RHOK, RHOKP

   real (r8) :: &
      vvconv             ! viscosity due to convection

!-----------------------------------------------------------------------
!
!  if using diffusion to simulate convection, enhance the vertical 
!  mixing coefficients if gravitationally unstable
!
!-----------------------------------------------------------------------

   if (present(convect_diff)) then

      bid = this_block%local_id

      VVC(:,:,k  ) = const_vvc
      VDC(:,:,k,1) = const_vdc

      kp1 = min(k+1,km)

      call state(k  ,kp1,TMIX(:,:,k  ,1), TMIX(:,:,k  ,2), &
                         this_block, RHOOUT=RHOK )
      call state(kp1,kp1,TMIX(:,:,kp1,1), TMIX(:,:,kp1,2), &
                         this_block, RHOOUT=RHOKP)

      if (convect_visc /= c0) then
         vvconv = convect_visc
      else
         vvconv = const_vvc  ! convection only affects tracers
      endif

      where ((RHOK > RHOKP) .and. (k < KMT(:,:,bid)))
         VDC(:,:,k,1) = convect_diff
         VVC(:,:,k)   = vvconv
      endwhere
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine vmix_coeffs_const

!***********************************************************************

 end module vmix_const

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
