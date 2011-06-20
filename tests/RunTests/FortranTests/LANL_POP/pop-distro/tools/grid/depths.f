!***********************************************************************

      program depth_profile

!-----------------------------------------------------------------------
!
!     computes layer thicknesses for each level for POP ocean code
!
!-----------------------------------------------------------------------

      implicit none

      integer, parameter :: 
     &  km = 20             ! number of vertical levels

      real, parameter ::
     &  zmax    = 5500.0,   ! max depth in meters
     &  dz_sfc  =   25.0,   ! thickness of surface layer
     &  dz_deep =  400.0,   ! thickness of deep ocean layers
     &  eps     = 1.e-6     ! convergence criterion 

      real, dimension(km) :: 
     &  z,                  ! depth at bottom of layer
     &  dz                  ! layer thickness of each layer

      real ::
     &  depth,              ! depth based on integrated thicknesses
     &  zlength,            ! adjustable parameter for thickness
     &  d0, d1,             ! depths used by midpoint search
     &  zl0, zl1,           ! parameter used by midpoint search
     &  dzl                 ! zl1-zl0

      integer k

!-----------------------------------------------------------------------
!
!     initialize bisection search to find best value of zlength
!     parameter such that integrated depth = zmax
!
!-----------------------------------------------------------------------

      zl0 = eps
      zl1 = zmax
      dzl = zl1 - zl0

      call compute_dz(dz,d0,zl0,dz_sfc,dz_deep,km)
      call compute_dz(dz,d1,zl1,dz_sfc,dz_deep,km)

      if ((d0-zmax)*(d1-zmax) > 0.0) then
        print *,d0,d1,zmax
        stop 'zero point not in initial interval'
      endif

!-----------------------------------------------------------------------
!
!     do bisection search
!
!-----------------------------------------------------------------------

      do while ( (dzl/zmax) > eps)

        !***
        !*** compute profile at midpoint
        !***

        zlength = zl0 + 0.5*dzl

        call compute_dz(dz,depth,zlength,dz_sfc,dz_deep,km)

        !***
        !*** find interval to use for continuing search
        !***

        if ((d0-zmax)*(depth-zmax) < 0.0) then
          d1 = depth
          zl1 = zlength
        else if ((d1-zmax)*(depth-zmax) < 0.0) then
          d0 = depth
          zl0 = zlength
        else
         print *,d0,d1,depth,zmax
         stop 'zero point not in interval'
        endif

        dzl = zl1 - zl0

      end do

!-----------------------------------------------------------------------
!
!     presumably, we have converged, but check to make sure
!
!-----------------------------------------------------------------------

      print *,'Integrated depth = ',depth,'    zmax = ',zmax

!-----------------------------------------------------------------------
!
!     write results to file and stdout
!
!-----------------------------------------------------------------------

      open(12,file='in_depths.dat',form='formatted',status='unknown')

      depth = 0.0
      do k =1,km
        depth = depth + dz(k)

        print *,k,dz(k),depth
        write(12,*) dz(k)*100.,'   !   ',depth
      end do

      close(12)

!-----------------------------------------------------------------------

      end program depth_profile

!***********************************************************************

      subroutine compute_dz(dz,depth,zlength,dz_sfc,dz_deep,km)

!-----------------------------------------------------------------------
!
!     computes a thickness profile and total depth given the 
!     parameters for the thickness function
!
!-----------------------------------------------------------------------
!-----------------------------------------------------------------------
!
!     input variables
!
!-----------------------------------------------------------------------

      integer, intent(in) ::
     &  km                 ! number of vertical levels

      real, intent(in) ::
     &  zlength,           ! gaussian parameter for thickness func
     &  dz_sfc,            ! thickness of surface layer
     &  dz_deep            ! thickness of deep ocean layers

!-----------------------------------------------------------------------
!
!     output variables
!
!-----------------------------------------------------------------------

      real, dimension(km), intent(out) :: 
     &  dz                  ! layer thickness of each layer

      real, intent(out) ::
     &  depth               ! depth based on integrated thicknesses

!-----------------------------------------------------------------------
!
!     local variables
!
!-----------------------------------------------------------------------

      integer k

!-----------------------------------------------------------------------

      depth = 0.0

      do k=1,km

        dz(k) = dz_deep - (dz_deep - dz_sfc)*exp(-(depth/zlength)**2)
        depth = depth + dz(k)

      end do

!-----------------------------------------------------------------------

      end subroutine compute_dz

!***********************************************************************
