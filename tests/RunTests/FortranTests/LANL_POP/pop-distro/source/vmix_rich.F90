!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module vmix_rich

!BOP
! !MODULE: vmix_rich
!
! !DESCRIPTION:
!  This module contains routines for initializing and computing
!  vertical mixing coefficients for the Richardson number
!  parameterization (e.g. Pacanowski-Philander).
!
! !REVISION HISTORY:
!  CVS:$Id: vmix_rich.F90,v 1.8 2002/05/07 17:40:32 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES

   use kinds_mod
   use blocks
   use distribution
   use domain
   use constants
   use grid
   use broadcast
   use io
   use state_mod
   use time_management
   use exit_mod

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_vmix_rich,  &
             vmix_coeffs_rich

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  scalar mixing constants
!
!-----------------------------------------------------------------------

   real (r8) ::        &
      bckgrnd_vvc,     &! background value for viscosity
      bckgrnd_vdc,     &! background value for diffusivity
      rich_mix          ! coefficient for rich number term

   real (r8), dimension(:), allocatable :: &
      critnuk           ! for min diffusion based on local stability

!-----------------------------------------------------------------------
!
!  average velocities to be saved to avoid re-computing at next
!  k level
!
!-----------------------------------------------------------------------

   real (r8), dimension(:,:,:), allocatable :: &
      UTK, VTK            ! average velocities at T points level k

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_vmix_rich
! !INTERFACE:

 subroutine init_vmix_rich(VDC,VVC)

! !DESCRIPTION:
!  Initializes parameters for Richardson number vertical mixing by
!  reading from namelist input.
!
! !REVISION HISTORY:
!  same as module

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(:,:,:,:,:), intent(inout) :: &
      VDC                 ! vertical tracer diffusivity

   real (r8), dimension(:,:,:,:), intent(inout) :: &
      VVC                 ! vertical momentum viscosity

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  input namelist
!
!-----------------------------------------------------------------------

   integer (int_kind) :: & 
      k,                  &! vertical level index
      nml_error            ! namelist i/o error flag

   namelist /vmix_rich_nml/bckgrnd_vvc, bckgrnd_vdc, rich_mix

!-----------------------------------------------------------------------
!
!  set defaults for mixing coefficients, then read them from namelist
!
!-----------------------------------------------------------------------

   bckgrnd_vvc = 1.0
   bckgrnd_vdc = 0.1
   rich_mix    = 50.0
      
   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=vmix_rich_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading vmix_rich_nml')
   endif


   if (my_task == master_task) then
      write(stdout,'(a17,2x,1pe12.5)') '  bckgrnd_vvc   =',bckgrnd_vvc
      write(stdout,'(a17,2x,1pe12.5)') '  bckgrnd_vdc   =',bckgrnd_vdc
      write(stdout,'(a17,2x,1pe12.5)') '  rich_mix      =',rich_mix
   endif

   call broadcast_scalar(bckgrnd_vvc, master_task)
   call broadcast_scalar(bckgrnd_vdc, master_task)
   call broadcast_scalar(rich_mix,    master_task)

!-----------------------------------------------------------------------
!
!  set part of minimum diffusion coefficient based on stability
!  (divide by c2dtt later for full critical number)
!
!-----------------------------------------------------------------------

   allocate(critnuk(km))
   do k=1,km
      critnuk(k) = p25*dz(k)*dzw(k)
   end do

!-----------------------------------------------------------------------
!
!  initialize VVC, VDC
!
!-----------------------------------------------------------------------

   VVC = c0
   VDC = c0

   allocate(UTK(nx_block,ny_block,nblocks_clinic), &
            VTK(nx_block,ny_block,nblocks_clinic))

!-----------------------------------------------------------------------
!EOC

 end subroutine init_vmix_rich

!***********************************************************************
!BOP
! !IROUTINE: vmix_coeffs_rich
! !INTERFACE:

 subroutine vmix_coeffs_rich(k,VDC,VVC,TMIX,UMIX,VMIX,RHOMIX,  &
                               this_block, convect_diff, convect_visc)

! !DESCRIPTION:
!  Computes vertical diffusion coefficients for Richardson number
!  mixing parameterization.
!
!  \begin{itemize}
!   \item This routine must be called successively with k = 1,2,3,...
!
!   \item Richardson numbers at T-points are first calculated
!         and the richardson number at U points is taken to be
!         the average of the Richardson number at surrounding T points.
!  \end{itemize}
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: k  ! vertical level index

   real (r8), dimension(nx_block,ny_block,km,nt), intent(in) :: &
      TMIX                 ! tracers at mix time level

   real (r8), dimension(nx_block,ny_block,km), intent(in) :: &
      UMIX, VMIX,         &! U,V velocities at mix time level
      RHOMIX               ! density at mix time level

   real (r8), intent(in), optional :: &
      convect_diff,       &! diffusivity to mimic convection
      convect_visc         ! viscosity   to mimic convection

   type (block), intent(in) :: &
      this_block           ! block info for current block

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(:,:,:,:), intent(inout) :: &
      VDC                 ! vertical tracer diffusivity

   real (r8), dimension(:,:,:), intent(inout) :: &
      VVC                 ! vertical momentum viscosity

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local or common variables:
!
!-----------------------------------------------------------------------

   integer (int_kind) :: & 
      kp1,       &! k+1 for k < km, km otherwise
      kdc,       &! k index into VDC array (to handle 2-d or 3-d cases)
      kvc,       &! k index into VVC array (to handle 2-d or 3-d cases)
      bid         ! local block address

   real (r8) ::          &
      critnu              ! minimum diffusion based on local stability

   real (r8), dimension(nx_block,ny_block) :: & 
      UTKP,VTKP,         &! avg velocities at T point at k+1 level
      RHOK,              &! level k density after adiab displ to k+1
      RICH,RICHU          ! richardson number at T,U points

!-----------------------------------------------------------------------
!
!  set up k+1 index and k-index for VVC,VDC arrays
!
!-----------------------------------------------------------------------

   kp1 = min(k+1, km)

   kdc = min(k,size(VDC,DIM=3)) ! defaults to 1 if VDC 2-d array
   kvc = min(k,size(VVC,DIM=3)) ! defaults to 1 if VVC 2-d array

   bid = this_block%local_id

!-----------------------------------------------------------------------
!
!  average either velocities at k=1, k+1 T points
!  or vertical shear for partial bottom cells
!
!-----------------------------------------------------------------------

   if (partial_bottom_cells) then

      UTK(:,:,bid) = (UMIX(:,:,k)-UMIX(:,:,kp1))/ &
                     (p5*(DZU(:,:,k,bid)+DZU(:,:,kp1,bid)))
      VTK(:,:,bid) = (VMIX(:,:,k)-VMIX(:,:,kp1))/ &
                     (p5*(DZU(:,:,k,bid)+DZU(:,:,kp1,bid)))

      call ugrid_to_tgrid(UTKP,UTK(:,:,bid),bid)
      call ugrid_to_tgrid(VTKP,VTK(:,:,bid),bid)

   else

      if (k == 1) then
         call ugrid_to_tgrid(UTK(:,:,bid),UMIX(:,:,k),bid)
         call ugrid_to_tgrid(VTK(:,:,bid),VMIX(:,:,k),bid)
      endif 

      call ugrid_to_tgrid(UTKP,UMIX(:,:,kp1),bid)
      call ugrid_to_tgrid(VTKP,VMIX(:,:,kp1),bid)

   endif ! partial bottom cells

!-----------------------------------------------------------------------
!
!  density at level k after adiabatic displacement to k+1
!  for use in vertical gradient of potential density for
!  richardson number calculation
!
!-----------------------------------------------------------------------

   call state(k,kp1,TMIX(:,:,k,1),TMIX(:,:,k,2),this_block, &
                    RHOOUT=RHOK)

!-----------------------------------------------------------------------
!
!  richardson number and vertical diffusivity at bottom of
!  T box, level k 
!
!-----------------------------------------------------------------------

   if (present(convect_diff)) then
      critnu = convect_diff
   else
      critnu = critnuk(k)/c2dtt(k)
   endif

   if (partial_bottom_cells) then

      where (k < KMT(:,:,bid))

         RICH =  -grav*(RHOK - RHOMIX(:,:,kp1))/                       &
                 (p5*(DZT(:,:,k,bid)+DZT(:,:,kp1,bid)))/ &
                     (UTKP**2 + VTKP**2 + eps/                         &
                 (p5*(DZT(:,:,k,bid)+DZT(:,:,kp1,bid)))**2)

         VDC(:,:,kdc,1) = min(critnu,                                  &
                              bckgrnd_vdc + (bckgrnd_vvc +             &
                              rich_mix/(c1 + c5*RICH)**2)/(c1 + c5*RICH))

      elsewhere
         RICH = c0
         VDC(:,:,kdc,1) = c0
      endwhere

   else ! no partial bottom cells

      where (k < KMT(:,:,bid))

         RICH =  -grav*dzw(k)*(RHOK - RHOMIX(:,:,kp1))/ &
                ((UTK(:,:,bid) - UTKP)**2 +                        &
                 (VTK(:,:,bid) - VTKP)**2 + eps)
         VDC(:,:,kdc,1) = min(critnu,                              &
                              bckgrnd_vdc + (bckgrnd_vvc +         &
                              rich_mix/(c1 + c5*RICH)**2)/(c1 + c5*RICH))

      elsewhere
         RICH = c0
         VDC(:,:,kdc,1) = c0
      endwhere

   endif ! partial bottom cells

   where (RICH < c0) VDC(:,:,kdc,1) = critnu

!-----------------------------------------------------------------------
!
!  richardson number at bottom of U box, level k 
!  (average of richardson number at surrounding T points) 
!
!-----------------------------------------------------------------------

   call tgrid_to_ugrid(RICHU,RICH,bid)

!-----------------------------------------------------------------------
!
!  vertical viscosity at bottom of U box, level k 
!
!-----------------------------------------------------------------------
        
   if (present(convect_visc)) critnu = convect_visc

   where (k < KMU(:,:,bid))

      VVC(:,:,kvc)  = min(critnu, &
                          bckgrnd_vvc + rich_mix/(c1 + c5*RICHU)**2)

   elsewhere

      RICHU = c0
      VVC(:,:,kvc)  = c0

   endwhere

   where (RICHU < c0) VVC(:,:,kvc) = critnu

!-----------------------------------------------------------------------
!
!  bottom values become top values for next pass
!
!-----------------------------------------------------------------------

   if (.not. partial_bottom_cells) then
      UTK(:,:,bid) = UTKP
      VTK(:,:,bid) = VTKP 
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine vmix_coeffs_rich

!***********************************************************************

 end module vmix_rich

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
