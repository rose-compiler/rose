!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module topostress

!BOP
! !MODULE: topostress

! !DESCRIPTION:
!  This module contains routines necessary for computing stress
!  due to bottom topography.
!
! !REVISION HISTORY:
!  CVS:$Id: topostress.F90,v 1.10 2002/12/02 13:45:11 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:
   use kinds_mod
   use domain
   use blocks
   use distribution
   use constants
   use io
   use grid
   use broadcast
   use boundary
   use exit_mod

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_topostress

! !PUBLIC DATA MEMBERS:

   logical (log_kind), public :: &
     ltopostress         ! true if topographic stress desired

   real (r8), dimension(:,:,:), allocatable, public :: & 
     TSU, TSV            ! topographic stress velocities

!EOP
!BOC
!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_topostress
! !INTERFACE:

 subroutine init_topostress

! !DESCRIPTION:
!  This routine allocates stress arrays if topographic stress is
!  chosen and initializes topo stress parameters.
!
! !REVISION HISTORY:
!  same as module

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  input namelist to choose topostress
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      nsmooth_topo       ! number of passes of topography smoother

   integer (int_kind) :: nml_error ! namelist i/o error flag

   namelist /topostress_nml/ltopostress, nsmooth_topo

!-----------------------------------------------------------------------
!
!  read namelist to see if topostress desired
!
!-----------------------------------------------------------------------

   ltopostress = .false.
   nsmooth_topo= 0

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=topostress_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading topostress_nml')
   endif

   if (my_task == master_task) then
      write(stdout,delim_fmt)
      write(stdout,blank_fmt)
      write(stdout,'(a27)') ' Topographic stress options'
      write(stdout,blank_fmt)
      write(stdout,delim_fmt)
      if (ltopostress) then
         write(stdout,'(a27)') ' Topographic stress enabled'
         if (nsmooth_topo > 0) then
            write(stdout,'(a26,i2,a8)') ' Topography smoothed with ', &
                                        nsmooth_topo,' passes.'
         else
            write(stdout,'(a25)') ' Topography not smoothed.'
         endif
      else
         write(stdout,'(a28)') ' Topographic stress disabled'
      endif
   endif

   call broadcast_scalar(ltopostress,  master_task)
   call broadcast_scalar(nsmooth_topo, master_task)

!-----------------------------------------------------------------------
!
!  allocate the topographic stress velocity arrays if required
!
!-----------------------------------------------------------------------

   if (ltopostress) then
      allocate (TSU(nx_block,ny_block,nblocks_clinic), & 
                TSV(nx_block,ny_block,nblocks_clinic))

      call topo_stress(nsmooth_topo)
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine init_topostress

!***********************************************************************
!BOP
! !IROUTINE: topo_stress
! !INTERFACE:

 subroutine topo_stress(nsmooth_topo)

! !DESCRIPTION:
!  Calculate topographic stress (maximum entropy) velocities.  These
!  are time-independent 2d fields given by:
!  \begin{eqnarray}
!     \Psi^* &=& -f L^2 H \\
!     H U^* &=& -\nabla_y(\Psi^*) \\
!     H V^* &=& +\nabla_x(\Psi^*)
!  \end{eqnarray}
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
     nsmooth_topo      ! number of passes of topography smoother

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) ::   &
      i,j,iter,iblock       ! local iteration counters

   real (r8), dimension(nx_block,ny_block) :: & 
      TSP,                 &! topo stress streamfunction
      SCALE,               &! scale length
      HTOLD                 ! old topography 

   real (r8), dimension(nx_block,ny_block,nblocks_clinic) :: &
      HTNEW                 ! smoothed topography

   real (r8), parameter :: & 
      tslse = 12.0e5_r8,   &! 
      tslsp = 3.0e5_r8      !

   type (block) ::         &
      this_block            ! block information for current block

!-----------------------------------------------------------------------
!
!  smooth topography if requested
!
!-----------------------------------------------------------------------

   do iblock = 1,nblocks_clinic
      HTNEW(:,:,iblock) = HT(:,:,iblock)  ! initialize
   end do

   do iter = 1, nsmooth_topo

      do iblock = 1,nblocks_clinic
         this_block = get_block(blocks_clinic(iblock),iblock)  

         HTOLD = HTNEW(:,:,iblock)

         call smooth_topo2(HTOLD,HTNEW(:,:,iblock),this_block)
      end do

      call update_ghost_cells(HTNEW, bndy_clinic, field_loc_center, &
                                                  field_type_scalar)
   enddo

!-----------------------------------------------------------------------
!
!  calculate the topographic stress equilibrium stream function.
!
!-----------------------------------------------------------------------

   do iblock = 1,nblocks_clinic
      this_block = get_block(blocks_clinic(iblock),iblock)  

      SCALE = tslsp + (tslse - tslsp)* &
                      (p5 + p5*cos(c2*TLAT(:,:,iblock)))

      where (KMT(:,:,iblock) /= 0)
         TSP = -FCORT(:,:,iblock)*SCALE*SCALE*HTNEW(:,:,iblock)
      elsewhere
         TSP = c0
      end where

!-----------------------------------------------------------------------
!
!     calculate the topographic stress velocities from stream function.
!     compute gradient with 4 point stencil
!
!-----------------------------------------------------------------------
      
      TSU(:,:,iblock) = c0
      TSV(:,:,iblock) = c0

      do j=this_block%jb,this_block%je
      do i=this_block%ib,this_block%ie
         TSV(i,j,iblock) =  DXUR(i,j,iblock)*p5*HUR(i,j,iblock)*  &
                            (TSP(i+1,j+1) - TSP(i  ,j) -          &
                             TSP(i  ,j+1) + TSP(i+1,j))
         TSU(i,j,iblock) = -DYUR(i,j,iblock)*p5*HUR(i,j,iblock)*  &
                            (TSP(i+1,j+1) - TSP(i  ,j) +          &
                             TSP(i  ,j+1) - TSP(i+1,j))
      end do
      end do

      where (KMU(:,:,iblock) == 0)
         TSV(:,:,iblock) = c0    ! zero at land points
         TSU(:,:,iblock) = c0
      endwhere
      
      !  apply only in 'deep' water
      ! where (KMU(:,:,iblock) <= 3)
      !   TSU(:,:,iblock) = c0
      !   TSV(:,:,iblock) = c0
      ! endwhere

   end do  ! block loop

   call update_ghost_cells(TSU, bndy_clinic, field_loc_NEcorner, &
                                             field_type_vector)
   call update_ghost_cells(TSV, bndy_clinic, field_loc_NEcorner, &
                                             field_type_vector)

!-----------------------------------------------------------------------
!EOC

 end subroutine topo_stress

!***********************************************************************
!BOP
! !IROUTINE: smooth_topo2
! !INTERFACE:

 subroutine smooth_topo2(HTOLD,HTNEW,this_block)

! !DESCRIPTION:
!  This routine smooths topography using a 9-point averaging stencil
!  given by
!  \begin{equation}
!  \begin{array}{ccccc}
!              1 & -- & 2 & -- & 1  \\
!              | &    & | &    & |  \\
!              2 & -- & 4 & -- & 2  \\
!              | &    & | &    & |  \\
!              1 & -- & 2 & -- & 1
!  \end{array} \nonumber
!  \end{equation}
!  Land points are not included in the smoothing, and the
!  stencil is modified to include only ocean points in the
!  averaging.  This routine is nearly identical to the smooth 
!  topography routine in the grid module.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(in) :: & 
      HTOLD               ! old HT field to be smoothed

   type (block), intent(in) :: &
      this_block          ! block info for this sub block

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(out) :: &
      HTNEW               ! smoothed HT field

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j,               &! loop counters
      bid                 ! local block index

   integer (int_kind), dimension(nx_block,ny_block) ::  &
      NB,                 &! array to compute number of ocean neighbors
      IWORK                ! local work space

   real (r8), dimension(nx_block,ny_block) :: & 
      WORK                 ! local work space

!-----------------------------------------------------------------------
!
!  smooth topography
!
!-----------------------------------------------------------------------

   bid = this_block%local_id

   where (KMT(:,:,bid) > 0) 
      IWORK = 1
      HTNEW = HTOLD
   elsewhere
      IWORK = 0
      HTNEW = c0
   endwhere

   do j=this_block%jb,this_block%je
   do i=this_block%ib,this_block%ie

      WORK(i,j) = c4*HTNEW(i,j) +                          & 
                  c2*HTNEW(i+1,j  ) + c2*HTNEW(i-1,j  ) +  &
                  c2*HTNEW(i  ,j+1) + c2*HTNEW(i  ,j-1) +  &
                     HTNEW(i+1,j+1) +    HTNEW(i+1,j-1) +  &
                     HTNEW(i-1,j+1) +    HTNEW(i-1,j-1)

      NB(i,j) = c4*IWORK(i,j) +                          &
                c2*IWORK(i+1,j  ) + c2*IWORK(i-1,j  ) +  &
                c2*IWORK(i  ,j+1) + c2*IWORK(i  ,j-1) +  &
                   IWORK(i+1,j+1) +    IWORK(i+1,j-1) +  &
                   IWORK(i-1,j+1) +    IWORK(i-1,j-1)

   end do
   end do

!-----------------------------------------------------------------------
!
!  new depth field
!
!-----------------------------------------------------------------------

   where ((KMT(:,:,bid) /= 0) .and. (NB /= 0)) 
      HTNEW = WORK/real(NB)
   elsewhere
      HTNEW = c0
   endwhere

!-----------------------------------------------------------------------
!EOC

 end subroutine smooth_topo2

!***********************************************************************

 end module topostress

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
