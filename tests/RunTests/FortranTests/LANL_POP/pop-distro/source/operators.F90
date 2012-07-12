!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module operators

!BOP
! !MODULE: operators
!
! !DESCRIPTION:
!  This module contains routines for various common
!  mathematical operators, including gradient, divergence, curl,
!  and a vertical velocity calculation.  Note that these routines
!  do {\em not} update ghost cells so results contain invalid
!  entries in some ghost cells.
!
! !REVISION HISTORY:
!  CVS:$Id: operators.F90,v 1.5 2002/11/27 17:53:21 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod
   use blocks
   use domain_size
   use domain
   use constants
   use grid

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:
   public :: div,   &
             grad,  &
             zcurl, &
             wcalc

!EOP
!BOC
!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: div
! !INTERFACE:

 subroutine div(k,DIV_OUT,UX,UY,this_block)

! !DESCRIPTION:
!  This routine returns the divergence at T points (times the cell
!  area) of a vector field defined at U points.  The divergence
!  operator is defined as:
!
!  \begin{equation}
!  \nabla\cdot{\bf\rm u} = {1\over{\Delta_y}} \delta_x (\Delta_y u_x) +
!                          {1\over{\Delta_x}} \delta_y (\Delta_x u_y)
!  \end{equation}
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS

   integer (int_kind), intent(in) :: k   ! vertical level

   real (r8), dimension(nx_block,ny_block), intent(in) :: & 
      UX,UY              ! vector field defined at U-points

   type (block), intent(in) :: &
      this_block          ! block information for current block

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(out) :: & 
      DIV_OUT            ! divergence at T-points times cell area

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j,               &! dummy counters
      bid                 ! local block id

!-----------------------------------------------------------------------
!
!  compute divergence using a 4 point stencil
!
!-----------------------------------------------------------------------

   bid = this_block%local_id

   DIV_OUT(1,:) = c0
   DIV_OUT(:,1) = c0

   do j=2,ny_block
   do i=2,nx_block
      DIV_OUT(i,j) = p5*(UX(i  ,j  )*DYU(i  ,j  ,bid) +  & 
                         UX(i  ,j-1)*DYU(i  ,j-1,bid) -  &
                         UX(i-1,j  )*DYU(i-1,j  ,bid) -  &
                         UX(i-1,j-1)*DYU(i-1,j-1,bid) +  &
                         UY(i  ,j  )*DXU(i  ,j  ,bid) +  &
                         UY(i-1,j  )*DXU(i-1,j  ,bid) -  &
                         UY(i  ,j-1)*DXU(i  ,j-1,bid) -  &
                         UY(i-1,j-1)*DXU(i-1,j-1,bid))
   end do
   end do

   where (k > KMT(:,:,bid)) DIV_OUT = c0
      
!-----------------------------------------------------------------------
!EOC

 end subroutine div

!***********************************************************************
!BOP
! !IROUTINE: grad
! !INTERFACE:

 subroutine grad(k, GRADX, GRADY, F, this_block)

! !DESCRIPTION:
!  This routine computes the gradient in i,j directions at U points
!  based on field defined at T-points.
!
!  \begin{eqnarray}
!     \nabla_x(F) &=& \delta_x F \\
!     \nabla_y(F) &=& \delta_y F
!  \end{eqnarray}
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: k  ! vertical level

   real (r8), dimension(nx_block,ny_block), intent(in) :: & 
      F                  ! field defined at T points

   type (block), intent(in) :: &
      this_block          ! block information for current block

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(out) :: & 
      GRADX,GRADY        ! gradient in (i,j) direction at U points

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j,               &! dummy counters
      bid                 ! local block id

!-----------------------------------------------------------------------
!
!  compute gradient with a 4 point stencil
!
!-----------------------------------------------------------------------
      
   bid = this_block%local_id

   GRADX = c0
   GRADY = c0

   do j=1,ny_block-1
   do i=1,nx_block-1
      GRADX(i,j) = DXUR(i,j,bid)*p5*(F(i+1,j+1) - F(i  ,j) - & 
                                     F(i  ,j+1) + F(i+1,j))
      GRADY(i,j) = DYUR(i,j,bid)*p5*(F(i+1,j+1) - F(i  ,j) + &
                                     F(i  ,j+1) - F(i+1,j))
   end do
   end do

   where (k > KMU(:,:,bid))
      GRADX = c0    ! zero at land points
      GRADY = c0
   endwhere
      
!-----------------------------------------------------------------------
!EOC

 end subroutine grad

!***********************************************************************
!BOP
! !IROUTINE: zcurl
! !INTERFACE:

 subroutine zcurl(k,CURL,UX,UY,this_block)

! !DESCRIPTION:
!  This function returns the z-component of the curl of a vector
!  field defined at U points. 
!
!  \begin{equation}
!     \hat{\bf\rm z}\cdot\nabla\times{\bf\rm u} =
!     {1\over{\Delta_y}} \delta_x(\Delta_y u_y) -
!     {1\over{\Delta_x}} \delta_y(\Delta_x u_x)
!  \end{equation}
!
!  The result is actually multiplied by cell area and returned at 
!  T points. 
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: k  ! vertical level

   real (r8), dimension(nx_block,ny_block), intent(in) :: & 
      UX,UY              ! vector field defined at U-points

   type (block), intent(in) :: &
      this_block          ! block information for current block

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(out) :: & 
      CURL              ! z.curl(Ux,Uy) at T-points times cell area

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j,               &! dummy counters
      bid                 ! local block index

!-----------------------------------------------------------------------
!
!  compute curl using stencil similar to divergence 4 point stencil
!
!-----------------------------------------------------------------------

   bid = this_block%local_id

   CURL(1,:) = c0
   CURL(:,1) = c0

   do j=2,ny_block
   do i=2,nx_block
      CURL(i,j) = p5*(UY(i  ,j  )*DYU(i  ,j  ,bid) +  & 
                      UY(i  ,j-1)*DYU(i  ,j-1,bid) -  &
                      UY(i-1,j  )*DYU(i-1,j  ,bid) -  &
                      UY(i-1,j-1)*DYU(i-1,j-1,bid) -  &
                      UX(i  ,j  )*DXU(i  ,j  ,bid) -  &
                      UX(i-1,j  )*DXU(i-1,j  ,bid) +  &
                      UX(i  ,j-1)*DXU(i  ,j-1,bid) +  &
                      UX(i-1,j-1)*DXU(i-1,j-1,bid))
   end do
   end do

   where (k > KMT(:,:,bid)) CURL = c0
      
!-----------------------------------------------------------------------
!EOC

 end subroutine zcurl

!***********************************************************************
!BOP
! !IROUTINE: wcalc
! !INTERFACE:

 subroutine wcalc(WWW,UUU,VVV,this_block)

! !DESCRIPTION:
!  This function calculates vertical velocity $w$ at tops of T-cells
!  from horizontal velocity field (at U points) by integrating the
!  continuity equation.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,km), intent(in) :: &
      UUU, VVV       ! horizontal velocity at all vertical levels

   type (block), intent(in) :: &
      this_block          ! block information for current block

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,km), intent(out) :: & 
      WWW            ! vertical velocity at T points for all levels

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j,               &! dummy counters
      k,                 &! vert level index
      bid                 ! local block id

   real (r8) ::         &
      fvn,fvs,fue,fuw,  &! advective fluxes across sides of box
      wtkb               ! vertical velocity at bottom of box

!-----------------------------------------------------------------------
!
!  calculate velocity by integrating continuity equation at
!  each horizontal grid point
!
!-----------------------------------------------------------------------

   bid = this_block%local_id

   WWW = c0

!-----------------------------------------------------------------------
!
!  partial bottom cell case
!
!-----------------------------------------------------------------------

   if (partial_bottom_cells) then

!-----------------------------------------------------------------------
!
!     integrate from bottom up for every horizontal grid point
!
!-----------------------------------------------------------------------

      do j=this_block%jb,this_block%je
      do i=this_block%ib,this_block%ie

         wtkb = c0  ! vertical velocity zero at bottom of bottom cell

         do k=KMT(i,j,bid),1,-1

            !***
            !*** advection fluxes
            !***

            fue = p5*(UUU(i  ,j  ,k)*DYU(i  ,j    ,bid)*     &
                                     DZU(i  ,j  ,k,bid) +    &
                      UUU(i  ,j-1,k)*DYU(i  ,j-1  ,bid)*     &
                                     DZU(i  ,j-1,k,bid))
            fuw = p5*(UUU(i-1,j  ,k)*DYU(i-1,j    ,bid)*     &
                                     DZU(i-1,j  ,k,bid) +    &
                      UUU(i-1,j-1,k)*DYU(i-1,j-1  ,bid)*     &
                                     DZU(i-1,j-1,k,bid))
            fvn = p5*(VVV(i  ,j  ,k)*DXU(i  ,j    ,bid)*     &
                                     DZU(i  ,j  ,k,bid) +    &
                      VVV(i-1,j  ,k)*DXU(i-1,j    ,bid)*     &
                                     DZU(i-1,j  ,k,bid))
            fvs = p5*(VVV(i  ,j-1,k)*DXU(i  ,j-1  ,bid)*     &
                                     DZU(i  ,j-1,k,bid) +    &
                      VVV(i-1,j-1,k)*DXU(i-1,j-1  ,bid)*     &
                                     DZU(i-1,j-1,k,bid))

            !***
            !*** vertical velocity at top of box from continuity eq.
            !***

            WWW(i,j,k) = wtkb - & 
                    (fvn - fvs + fue - fuw)*TAREA_R(i,j,bid)

            wtkb = WWW(i,j,k) ! top value becomes bottom for next pass

         enddo

      end do ! horizontal loops
      end do

!-----------------------------------------------------------------------
!
!  no partial bottom cells
!
!-----------------------------------------------------------------------

   else ! no partial bottom cells

!-----------------------------------------------------------------------
!
!     integrate from bottom up for every horizontal grid point
!
!-----------------------------------------------------------------------

      do j=this_block%jb,this_block%je
      do i=this_block%ib,this_block%ie

         wtkb = c0  ! vertical velocity zero at bottom of bottom cell

         do k=KMT(i,j,bid),1,-1

            !***
            !*** advection fluxes
            !***

            fue = p5*(UUU(i  ,j  ,k)*DYU(i  ,j  ,bid) + &
                      UUU(i  ,j-1,k)*DYU(i  ,j-1,bid))
            fuw = p5*(UUU(i-1,j  ,k)*DYU(i-1,j  ,bid) + &
                      UUU(i-1,j-1,k)*DYU(i-1,j-1,bid))
            fvn = p5*(VVV(i  ,j  ,k)*DXU(i  ,j  ,bid) + &
                      VVV(i-1,j  ,k)*DXU(i-1,j  ,bid))
            fvs = p5*(VVV(i  ,j-1,k)*DXU(i  ,j-1,bid) + &
                      VVV(i-1,j-1,k)*DXU(i-1,j-1,bid))
   
            !***
            !*** vertical velocity at top of box from continuity eq.
            !***

            WWW(i,j,k) = wtkb - dz(k)* &
                         (fvn - fvs + fue - fuw)*TAREA_R(i,j,bid)

            wtkb = WWW(i,j,k) ! top value becomes bottom for next pass

         end do ! vertical loop

      end do ! horizontal loops
      end do

   endif ! partial bottom cells

!-----------------------------------------------------------------------
!EOC

 end subroutine wcalc

!***********************************************************************

 end module operators

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
