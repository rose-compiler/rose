!***********************************************************************

      program depth_profile

!-----------------------------------------------------------------------
!
!     computes layer thicknesses for each level for POP ocean code
!
!-----------------------------------------------------------------------

      real, parameter ::
     &  zmax = 5500      ! max depth in meters

      dimension z(0:100),dz(1:100)

      z(0) = 0.0

      open(12,file='in_depths.dat',form='formatted',status='unknown')
      do k = 1,100

        call zeroin(z(k-1),dz(k))
 
        z(k) = z(k-1) + dz(k)
        if(k.eq.1)then
          zz=z(k)*0.5
        else
          zz = 0.5*(z(k)+z(k-1))
        endif
        write(*,*)k,dz(k),z(k),zz
        write(12,*)dz(k)*100
        if(z(k).gt.zmax) go to 100

      enddo

100   continue
      close(12)

!-----------------------------------------------------------------------

      end program depth_profile

!***********************************************************************


      subroutine zeroin(z,x)

      external fz
      data err/1.0e-10/

c     ...linear search

      dx = fz(z)/10.
      x = 0.0
      g = 1.0

      do while (g.gt.0.0)

        xl = x
        x = x + dx
        g = fz(z + x/2.) - x

      enddo

c     ...newton search

      xu = x

      do while (abs(g).gt.err)

        x = 0.5*(xu + xl)
        g = fz(z + x/2.) - x

        if (g.gt.0.0) then
          xl = x
        else
          xu = x
        endif

      enddo

      return
      end 


      function fz(z)

c     fz = 300. - 295.*exp(-((z-160)/500.)**2)

c     fz = 500. - 475.*exp(-(z/891.736)**2)
c     fz = 500. - 475.*exp(-(z/1000)**2)
c     fz = 300. - 300./(1. + exp((z-1200.)/500.))

      fz = 250. - 240.*exp(-(z/695.655)**2)    ! NA 40-level

      return
      end
