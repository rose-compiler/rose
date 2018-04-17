!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module pressure_grad

!BOP
! !MODULE: pressure_grad
!
! !DESCRIPTION:
!  Contains routines for computing the pressure gradient force, 
!  including the use of pressure averaging.
!
! !REVISION HISTORY:
!  CVS:$Id: pressure_grad.F90,v 1.8 2003/01/09 15:50:56 jfd Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod, only: log_kind, r8, int_kind
!   use domain, only: 
   use blocks, only: nx_block, ny_block, block
!   use distribution, only: 
   use constants, only: grav, p5, delim_fmt, blank_fmt, c1, p25, c2, c0
   use operators, only: grad
   use grid, only: dzw
   use broadcast, only: broadcast_scalar
   use communicate, only: my_task, master_task
!   use io, only:
   use io_types, only: stdout, nml_in, nml_filename
   use state_mod, only: ref_pressure
   use time_management, only: max_blocks_clinic, km, leapfrogts
   use exit_mod, only: sigAbort, exit_pop

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_pressure_grad, &
             gradp

! !PUBLIC DATA MEMBERS:

   logical (log_kind), public :: &
      lpressure_avg       ! flag to turn on averaging of
                          ! pressure across three time levels

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  module variables
!
!-----------------------------------------------------------------------

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic) :: & 
      SUMX, SUMY,        &! incremental k sum of Grad{x,y}(P(k)) 
      RHOKMX, RHOKMY      ! x,y gradient of density at k-1 level 

!-----------------------------------------------------------------------
!
!  The factor, r(p), removes the contribution of pressure-
!  dependent compressibility from the density and thereby 
!  improves the accuracy of the Boussinesq approximation 
!  for the pressure gradient.
!
!  See Dukowicz, J. K., 2000: Reduction of Pressure and
!  Pressure Gradient Errors in Ocean Simulations, J. Phys.
!  Oceanogr., submitted.
!
!-----------------------------------------------------------------------

   logical (log_kind) :: &
      lbouss_correct     ! flag for correction to Bouss. approx.

   real (r8), dimension(km) :: &
      bouss               ! 1/r(p) factor for correction to Bouss.

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_pressure_grad
! !INTERFACE:

 subroutine init_pressure_grad

! !DESCRIPTION:
!  Initializes pressure gradient options and allocates space for
!  pressure gradient integrals.
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
      k,                 &! vertical level index
      nml_error           ! namelist i/o error flag

   namelist /pressure_grad_nml/ lpressure_avg, lbouss_correct

!-----------------------------------------------------------------------
!
!  read options from input namelist and broadcast
!
!-----------------------------------------------------------------------

   lpressure_avg = .true.
   lbouss_correct = .false.

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=pressure_grad_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading pressure_grad_nml')
   endif

   if (my_task == master_task) then
      write(stdout,delim_fmt)
      write(stdout,blank_fmt)
      write(stdout,'(a26)') ' Pressure gradient options'
      write(stdout,blank_fmt)
      write(stdout,delim_fmt)

      if (lpressure_avg) then
         write(stdout,'(a28)') ' Pressure averaging enabled '
      else
         write(stdout,'(a28)') ' Pressure averaging disabled'
      endif
      if (lbouss_correct) then
         write(stdout,'(a28)') ' Density correction enabled '
      else
         write(stdout,'(a28)') ' Density correction disabled'
      endif
   endif

   call broadcast_scalar(lpressure_avg,  master_task)
   call broadcast_scalar(lbouss_correct, master_task)

!-----------------------------------------------------------------------
!
!  Density correction factor at level k.
!
!-----------------------------------------------------------------------

   if (lbouss_correct) then
      do k=1,km
         bouss(k) = c1/(1.02819_r8 + 4.4004e-5_r8*ref_pressure(k) - &
                        2.93161e-4_r8*exp(-0.05_r8*ref_pressure(k)))
      end do
   else
      bouss(:) = c1
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine init_pressure_grad

!***********************************************************************
!BOP
! !IROUTINE: gradp
! !INTERFACE:

 subroutine gradp(k, PKX, PKY, RHOK_OLD, RHOK_CUR, RHOK_NEW, this_block)

! !DESCRIPTION:
!  This routine computes the gradient of hydrostatic pressure at 
!  level $k$:
!
!  \begin{eqnarray}
!     \delta_x \overline{p_k}^y &=& \delta_x \overline{p_s}^y + 
!           g \sum_{m=1}^k{ {1\over 2}\left[ 
!           \delta_x \overline{\rho_{m-1}}^y + 
!           \delta_x \overline{\rho_m}^y \right] dz_{m-{1\over 2}} } \\
!     \delta_y \overline{p_k}^x &=& \delta_y \overline{p_s}^x + 
!           g \sum_{m=1}^k{ {1\over 2}\left[ 
!           \delta_y \overline{\rho_{m-1}}^x + 
!           \delta_y \overline{\rho_m}^x \right] dz_{m-{1\over 2}} }
!  \end{eqnarray}
!  where $p_k$ is the hydrostatic pressure at level $k$, 
!  $\rho_m$ is the density at level $m$, and $\rho_0=\rho_1$ for $k=0$.
!
!  This routine must be called successively with $k = 1,2,3,...$
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: k   ! depth level index

   type (block), intent(in) :: &
      this_block          ! block information for current block

   real (r8), dimension(nx_block,ny_block), intent(in) :: &
      RHOK_OLD,          &! density at level k and old     time level
      RHOK_CUR,          &! density at level k and current time level
      RHOK_NEW            ! density at level k and new     time level

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(out) :: &
      PKX,PKY             ! {x,y} components of the pressure gradient

!EOP
!BOC
!-----------------------------------------------------------------------
! 
!  local or common variables:
!
!-----------------------------------------------------------------------

   integer (int_kind) :: bid ! local block address

   real (r8) :: factor  ! temporary factor

   real (r8), dimension(nx_block,ny_block) :: &
      RHOKX, RHOKY,      &! x,y gradients of level k density
      RHOAVG              ! avg density when pressure avg on

!-----------------------------------------------------------------------
!
!  gradient of density at level k
!
!-----------------------------------------------------------------------

   bid = this_block%local_id

!-----------------------------------------------------------------------
!
!  calculate density at new time for pressure averaging
!
!-----------------------------------------------------------------------
 
   if (lpressure_avg .and. leapfrogts) then
      RHOAVG = p25*(RHOK_NEW + c2*RHOK_CUR + RHOK_OLD)*bouss(k)
   else
      RHOAVG = RHOK_CUR*bouss(k)
   endif

   call grad(k,RHOKX,RHOKY,RHOAVG,this_block)

!-----------------------------------------------------------------------
!
!  set Rho(0) = Rho(1) at top level, 
!  initialize sum for pressure gradients to zero.
!
!-----------------------------------------------------------------------

   if (k == 1) then
      RHOKMX(:,:,bid) = RHOKX
      RHOKMY(:,:,bid) = RHOKY
      SUMX  (:,:,bid) = c0
      SUMY  (:,:,bid) = c0
   endif

!-----------------------------------------------------------------------
!
!  obtain pressure gradient by incrementing sum of density gradients
!  from top level down to level k.
!
!-----------------------------------------------------------------------

   factor = dzw(k-1)*grav*p5
   SUMX(:,:,bid) = SUMX(:,:,bid) + factor*(RHOKX + RHOKMX(:,:,bid))
   SUMY(:,:,bid) = SUMY(:,:,bid) + factor*(RHOKY + RHOKMY(:,:,bid))

   PKX  = SUMX(:,:,bid)
   PKY  = SUMY(:,:,bid)
      
!-----------------------------------------------------------------------
!
!  overwrite level k-1 with level k density gradients for next pass
!
!-----------------------------------------------------------------------
       
   RHOKMX(:,:,bid) = RHOKX
   RHOKMY(:,:,bid) = RHOKY
      
!-----------------------------------------------------------------------
!EOC

 end subroutine gradp

!***********************************************************************

 end module pressure_grad

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
