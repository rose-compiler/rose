!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

   module drifters

!BOP
! !MODULE: drifters
!
! !DESCRIPTION:
!  This module contains routines necessary for moving drifters.
!  NOTE: this module currently does not really exist - this version
!  has old CM-5 code and will be replaced with some new code when
!  Mat is ready to add it in...
!
! !REVISION HISTORY:
!  CVS:$Id: drifters.F90,v 1.3 2002/02/26 22:49:04 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $
!
! !USES

   use kinds_mod

   implicit none
   private
   save

!EOP
!BOC
!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_drifters
! !INTERFACE:

 subroutine init_drifters

! !DESCRIPTION:
!  Initializes all variables for drifter diagnostics.
!
! !REVISION HISTORY:
!  same as module

!EOP
!BOC
!-----------------------------------------------------------------------
!EOC

 end subroutine init_drifters

!***********************************************************************

 end module drifters

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#ifdef does_not_exist
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c     begin file drifters.H
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

c-----------------------------------------------------------------------
c     Some things for drifters.  Note especially that there
c     is a new 3D array here (WVEL) that can increase memory
c     significantly if "drifter_particles != 2"
c-----------------------------------------------------------------------

      integer, parameter :: ndrifters_max = 400
      integer ndrifters, ndrifters_total, array_size, arrays_deployed

      TYPE drifter_ijk(3,ndrifters_max)

      common/drifter_stuff/ndrifters, ndrifters_total, array_size
     &     , arrays_deployed, drifter_ijk

#if drifter_particles != 2
      TYPE, dimension(imt,jmt,km) :: WVEL
      common/drifter_3d/WVEL
#endif

c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c     end file drifters.H
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c     begin file drifter_read.F
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

      subroutine drifter_read

c-----------------------------------------------------------------------
c     Read in initial logical positions of drifters.
c
c     If introducing drifters for the first time in a given run, then
c     positions are calculated using the pre-processing routine drifter_init.f.
c     If continuing a run then use the final dump on the out_drifters
c     file from the previous portion of the run:
c        tail -n out_drifters > in_drifters
c           where n is the number of drifters
c     then edit the in_drifters file and put n (# of drifters) on the
c     first line
c     addition of drifters resulted in alteration of the following files:
c       scalar.H/d_scalar.H
c       files.H
c       in.dat
c       in_files.dat
c       makefile
c       initial.F
c       ocean.F
c       baroclinic.F
c       advu.F
c       time_manager.F
c-----------------------------------------------------------------------

      use grid

      implicit none

      integer np,i
      real u,v,w,t,s
      logical dump_grid_data

c-----------------------------------------------------------------------

c-----------------------------------------------------------------------
c     Use the following to dump grid data for use with pre-processor
c     program drifter_init.f
c-----------------------------------------------------------------------

      dump_grid_data = .false.
      if(dump_grid_data) then
         call get_unit(nu)
         open(nu,file='out_grid.dat',form='unformatted',status='unknown')
         write(nu)ulong
         write(nu)ulat
         write(nu)kmt
         write(nu)angle
         close(nu)
         call release_unit(nu)
         stop 'Wrote grid data to out_grid.dat'
      endif

c-----------------------------------------------------------------------
c     Read in drifter positions from the in_drifters file.  These must
c     be logical position (i,j,k), not physical (x,y,z).
c-----------------------------------------------------------------------

      call get_unit(nu)
      open(nu,file=in_drifters,status='old')
      read(nu,*)ndrifters_total,array_size,arrays_deployed
      if(ndrifters_total.gt.ndrifters_max) then
         write(*,*)' ** WARNING: ndrifters > ndrifters_max'
         write(*,*)'             Only first ',ndrifters_max,
     #             ' drifters will be read in'
         ndrifters_total = ndrifters_max
      endif
      do np = 1,ndrifters_total
         read(nu,*)i,
     #      drifter_ijk(1,np),drifter_ijk(2,np),drifter_ijk(3,np)
      enddo

      write(*,*)' '
      write(*,*)ndrifters_total,' drifters read in...'
      write(*,*)arrays_deployed,' drifter arrays deployed...'
      write(*,*)' Each drifter array has ',array_size,' elements'
      write(*,*)' '
      close(nu)
      call release_unit(nu)

      ndrifters = array_size*arrays_deployed

c-----------------------------------------------------------------------
c     Open the drifters data file and write # of drifters.
c     This file will stay open throughout the calculation.
c-----------------------------------------------------------------------

      call get_unit(nu_drift)
      open(nu_drift,file=out_drifters,status='unknown')
      write(nu_drift,*)ndrifters_total

      end subroutine drifter_read

c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c     end file drifter_read.F
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c     begin file drifter_dump.F
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

      subroutine drifter_dump(filename)

c-----------------------------------------------------------------------
c     Write logical positions of drifters.
c-----------------------------------------------------------------------

      use io

      implicit none

      integer np
      integer (kind=int_kind) ::
     &   nu               ! i/o unit attached to output file

      character*80 filename

c-----------------------------------------------------------------------

c-----------------------------------------------------------------------
c     Write in drifter positions in logical space for restart.
c-----------------------------------------------------------------------

      call get_unit(nu)
      open(nu,file=filename,status='unknown')
      write(nu,*)ndrifters_total,array_size,arrays_deployed
      do np = 1,ndrifters_total
         write(nu,*)np,
     &      drifter_ijk(1,np),drifter_ijk(2,np),drifter_ijk(3,np)
      enddo
      close(nu)
      call release_unit(nu)

      write(stdout,*)' '
      write(stdout,*)'file written: ',filename

      end subroutine drifter_dump

c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c     end file drifter_dump.F
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c     begin file drifter_intp.F
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

      real function drifter_intp (A1,A2, i, ip, j, jp, wijk,
     &     wijpk, wijkp, wijpkp, wipjk, wipjpk, wipjkp, wipjpkp)

c-----------------------------------------------------------------------
c     This function tri-linearly interpolates a field to a point
c     given an array at 2 k-levels (A1,A2) and the weights (w*).
c-----------------------------------------------------------------------

      implicit none

      integer i, ip, j, jp
      real wijk, wipjk, wijpk, wipjpk, wijkp,
     &     wipjkp, wijpkp, wipjpkp

      real, dimension(imt,jmt) :: A1, A2 ! CM array

c***********************************************************************

      drifter_intp =    wijk*A1(i,j)   +
     &             wijpk*A1(i,jp)  +
     &             wijkp*A2(i,j)   +
     &            wijpkp*A2(i,jp)  +
     &             wipjk*A1(ip,j)  +
     &            wipjpk*A1(ip,jp) +
     &            wipjkp*A2(ip,j)  +
     &           wipjpkp*A2(ip,jp)


      end
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c     end file drifter_intp.F
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c     begin file drifter_move.F
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
      subroutine drifter_move

c-----------------------------------------------------------------------
c     Move each drifter using a predictor-corrector scheme.  Trilinear
c     interpolation is used to find velocity at drifter locations.
c     Drifters are moved in logical space=> array drifter_ijk holds
c     the logical positions of the drifters.  Only when drifter properties
c     are dumped (in drifter_prop) is the conversion from logical space
c     to physical (lat-long-depth) performed.  Algorithm provided
c     by Doug Kothe.
c-----------------------------------------------------------------------

      use grid
      use time_management
      use prognostic

      implicit none

      integer i, ip, j, jp, k, kp, np, im, jm, kk
      real drifter_intp
      real alpho, beto, dalpho_dx, dbeto_dy, dgammo_dz, dtpred,
     &     gammo, gammw, up, vp, wp, xp, yp, zp
      real wijk, wipjk, wijpk, wipjpk, wijkp, wipjkp, wijpkp,
     &     wipjpkp
      logical predictor

      real, dimension(imt,jmt) :: U1, U2, TEMP

c-----------------------------------------------------------------------

c-----------------------------------------------------------------------
c     Cycle through all drifters
c-----------------------------------------------------------------------

      do 1000 np = 1,ndrifters

c-----------------------------------------------------------------------
c     Predictor delta-t
c-----------------------------------------------------------------------

         predictor = .true.
         dtpred = p5*dtu
         if (mod(itt,navg).eq.0) then ! correct for averaging timesteps
            dtpred = p5*dtpred
         endif

c-----------------------------------------------------------------------
c     Initialize the particle positions
c-----------------------------------------------------------------------

         xp = drifter_ijk(1,np)
         yp = drifter_ijk(2,np)
         zp = drifter_ijk(3,np)

c-----------------------------------------------------------------------
c     Get the cell indices containing the particle and surrounding cells
c-----------------------------------------------------------------------

   10    i = int(xp)
         j = int(yp)
         k = int(zp)

         if(i.gt.imt) i = 1
         if(i.lt.1  ) i = imt
         j  = min(max(j,1),jmt)
         k  = min(max(k,0),km)      ! k can be zero

         ip = i + 1
         jp = j + 1
         kp = k + 1
         im = i - 1
         jm = j - 1

         if(ip.gt.imt) ip = 1
         if(im.lt.1  ) im = imt
         jp = min(max(jp,1),jmt)
         jm = min(max(jm,1),jmt)
         kp = min(max(kp,1),km)

c-----------------------------------------------------------------------
c     Compute weights for the tri-linear interpolation coefficients
c-----------------------------------------------------------------------

         alpho = xp - i
         beto  = yp - j
         gammw = zp - k

         kk = int(zp + p5)
         kp = kk + 1
         kk = max(kk,1)
         kp = min(kp,km)

         if(gammw.lt.p5) then
            gammo = (p5*dz(kk) + gammw*dz(kp))*dzwr(kk)
         else
            gammo = (gammw - p5)*dz(kk)*dzwr(kk)
         endif

c-----------------------------------------------------------------------
c     Compute the tri-linear interpolation coefficients
c-----------------------------------------------------------------------

         wijk    = (1. - alpho)*(1. - beto)*(1. - gammo)
         wijpk   = (1. - alpho)*beto*(1. - gammo)
         wijkp   = (1. - alpho)*(1. - beto)*gammo
         wijpkp  = (1. - alpho)*beto*gammo
         wipjk   = alpho*(1. - beto)*(1. - gammo)
         wipjpk  = alpho*beto*(1. - gammo)
         wipjkp  = alpho*(1. - beto)*gammo
         wipjpkp = alpho*beto*gammo

c-----------------------------------------------------------------------
c     Velocity components are already in local frame
c     First do U
c-----------------------------------------------------------------------

         U1 = UVEL(:,:,kk,curtime,1)
         U2 = UVEL(:,:,kp,curtime,1)
c testing
c        U1 = 0.
c        U2 = 0.
c        U1 = 100.*cos(ANGLE)  !  zonal only
c        U2 = 100.*cos(ANGLE)  !  zonal only
c        U1 = 100.*sin(ANGLE)  !  meridional only
c        U2 = 100.*sin(ANGLE)  !  meridional only

c-----------------------------------------------------------------------
c     Interpolate U velocity to the particle
c-----------------------------------------------------------------------

         up = drifter_intp(U1, U2, i, ip, j, jp, wijk, wijpk,
     &                    wijkp, wijpkp, wipjk, wipjpk, wipjkp, wipjpkp)

c-----------------------------------------------------------------------
c     Now do V
c-----------------------------------------------------------------------

         U1 = VVEL(:,:,kk,curtime,1)
         U2 = VVEL(:,:,kp,curtime,1)
c testing
c        U1 = 0.
c        U2 = 0.
c        U1 = 100.*sin(-ANGLE)  !  zonal only
c        U2 = 100.*sin(-ANGLE)  !  zonal only
c        U1 = 100.*cos(ANGLE)   !  meridional only
c        U2 = 100.*cos(ANGLE)   !  meridional only

c-----------------------------------------------------------------------
c     Interpolate V velocity to the particle
c-----------------------------------------------------------------------

         vp = drifter_intp(U1, U2, i, ip, j, jp, wijk, wijpk,
     &                    wijkp, wijpkp, wipjk, wipjpk, wipjkp, wipjpkp)

#if drifter_particles != 2
c-----------------------------------------------------------------------
c     Now do W differently because it is located at cell bottom
c     Note that the horizontal weights are the same as for U,V
c     since WVEL is calculated in advu (still cell-bottom, though).
c     Also, the vertical weights are different--they are simply
c     gammw since the vertical index for the drifters is based on
c     the W-grid.
c-----------------------------------------------------------------------

         if(k.eq.0) then
            U1 = DHU
         else
            U1 = WVEL(:,:,k)
         endif
         if(k.ge.km) then
            U2 = 0.  !  should not happen
         else
            U2 = WVEL(:,:,k + 1)
         endif
c testing...
c        U1 = 0.01
c        U2 = 0.01

c-----------------------------------------------------------------------
c     Compute the tri-linear interpolation coefficients
c-----------------------------------------------------------------------

         wijk    = (1. - alpho)*(1. - beto)*(1. - gammw)
         wijpk   = (1. - alpho)*beto*(1. - gammw)
         wijkp   = (1. - alpho)*(1. - beto)*gammw
         wijpkp  = (1. - alpho)*beto*gammw
         wipjk   = alpho*(1. - beto)*(1. - gammw)
         wipjpk  = alpho*beto*(1. - gammw)
         wipjkp  = alpho*(1. - beto)*gammw
         wipjpkp = alpho*beto*gammw

c-----------------------------------------------------------------------
c     Interpolate W velocity to the particle
c-----------------------------------------------------------------------
         wp = drifter_intp(U1, U2, i, ip, j, jp, wijk, wijpk,
     &                   wijkp,wijpkp, wipjk, wipjpk, wipjkp, wipjpkp)

#else
c-----------------------------------------------------------------------
c     Drifters are drogued => set vertical velocity to zero
c-----------------------------------------------------------------------

         wp = c0
#endif

c-----------------------------------------------------------------------
c     Compute the derivative of the metric
c     Remember that z is positive up and k increases down, so
c     dgammo_dz is actually negative.
c-----------------------------------------------------------------------

         dalpho_dx = (1. - alpho)/HUS(i,jp) + alpho/HUS(ip,jp)
         dbeto_dy = (1. - beto)/HUW(ip,j) + beto/HUW(ip,jp)
c        dgammo_dz = (1. - gammo)*dzr(kk) + gammo*dzr(kp) ! original
         dgammo_dz = (1. - gammw)*dzwr(k) + gammw*dzwr(k+1)

         dgammo_dz = - dgammo_dz
c-----------------------------------------------------------------------
c     Move the drifters and correct for longitudinal periodicity and
c     drifters moving through boundaries (ungracefully).
c-----------------------------------------------------------------------

         xp = drifter_ijk(1,np) + dtpred*up*dalpho_dx
         yp = drifter_ijk(2,np) + dtpred*vp*dbeto_dy
         zp = drifter_ijk(3,np) + dtpred*wp*dgammo_dz

         if(xp.gt.imt) xp = xp - imt
         if(xp.lt.c0 ) xp = xp + imt
         if(yp.gt.jmt) yp = jmt
         if(yp.lt.c0 ) yp = 1.
         if(zp.gt.(km + p5) ) zp = km + p5
         if(zp.lt.p5 ) zp = p5

  600    continue

c-----------------------------------------------------------------------
c     Check to see if this is the predictor or corrector step
c-----------------------------------------------------------------------

         if (predictor) then  !  do corrector step
            dtpred = 2.*dtpred
            predictor = .false.
            goto 10
         endif

c-----------------------------------------------------------------------
c     Done with this drifter=> save its position
c-----------------------------------------------------------------------

         drifter_ijk(1,np) = xp
         drifter_ijk(2,np) = yp
         drifter_ijk(3,np) = zp

 1000 continue  ! go do next drifter

      end subroutine drifter_move

c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c     end file drifter_move.F
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c     begin file drifter_prop.F
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
      subroutine drifter_prop

c-----------------------------------------------------------------------
c     Calculate field quantities at location of all drifters using
c     trilinear interpolation.  Information is then write to the file
c     specified by out_drifters in the in_files.dat file.  this file is
c     opened in drifter_read and remains open throughout the calculation.
c-----------------------------------------------------------------------

      use grid
      use time_management
      use prognostic

      implicit none

      real drifter_intp
      integer np,i,j,ip,jp,k,kk,kp
      real dx,dy,zz,dlong,dlat,tan_alpha,dulong,zd
      real drifter_long,drifter_lat,drifter_z,up,vp,wp,tempp,saltp
      real wijk, wipjk, wijpk, wipjpk, wijkp, wipjkp, wijpkp,
     &     wipjpkp

      real, dimension(imt,jmt) :: U1, U2, TEMP

c-----------------------------------------------------------------------

c-----------------------------------------------------------------------
c     Write the time to the out_drifters file
c-----------------------------------------------------------------------

      write(37,*)tday

c-----------------------------------------------------------------------
c     Cycle over all drifters.  Include even those that are not
c     deployed yet.
c-----------------------------------------------------------------------

      do np = 1,ndrifters_total

c-----------------------------------------------------------------------
c     Find each particle position in physical coordinates
c-----------------------------------------------------------------------

         i = int(drifter_ijk(1,np))
         j = int(drifter_ijk(2,np))
         k = int(drifter_ijk(3,np))

         dx = drifter_ijk(1,np) - i
         dy = drifter_ijk(2,np) - j
         zz = drifter_ijk(3,np) - k  ! cannot use dz

         ip = i + 1
         if(ip.gt.imt) ip = 1
         jp = j + 1
         if(jp.gt.jmt) jp = j

         kk = int(drifter_ijk(3,np) + p5)
         kp = kk + 1
         kk = max(kk,1)
         kp = min(kp,km)

         if(zz.lt.p5) then
            zd = (p5*dz(kk) + zz*dz(kp))*dzwr(kk)
         else
            zd = (zz - p5)*dz(kk)*dzwr(kk)
         endif
c-----------------------------------------------------------------------
c     Check for crossing 0/360 longitude
c-----------------------------------------------------------------------

         dulong = ULONG(ip,j) - ULONG(i,j)
         if(dulong.lt.-5.0) then
            dulong = ULONG(ip,j) - ULONG(i,j) + 2.*pi
         elseif(dulong.gt.5.0) then
            dulong = ULONG(ip,j) - ULONG(i,j) - 2.*pi
         endif

c-----------------------------------------------------------------------
c     First find latitude/longitude
c-----------------------------------------------------------------------

         if(dx.ne.c0) then
            tan_alpha = dy*HTE(i,jp)/(dx*HTN(ip,j))
            dlong = dx*dulong*(c1 - tan(ANGLE(i,j))*tan_alpha)
            if(dy.ne.c0) then
               dlat  = dy*(ULAT(i,jp) - ULAT(i,j))
     &                 *(c1 + tan(ANGLE(i,j))/tan_alpha)
            else
               dlat = c0
            endif
         else
            dlong = c0
            dlat  = dy*(ULAT(i,jp) - ULAT(i,j))
         endif

         drifter_long = ULONG(i,j) + dlong
         drifter_lat  = ULAT (i,j) + dlat

c-----------------------------------------------------------------------
c     Now find vertical position
c-----------------------------------------------------------------------

         if(k.eq.0) then
            drifter_z = zz*dz(k+1)
         else
            drifter_z = zz*dz(k+1) + zw(k)
         endif

c-----------------------------------------------------------------------
c     Convert to degrees, meters
c-----------------------------------------------------------------------

         drifter_long = 180.*drifter_long/pi
         if(drifter_long.gt.360.0) drifter_long = drifter_long - 360.
         if(drifter_long.lt.0.0) drifter_long = drifter_long + 360.
         drifter_lat  = 180.*drifter_lat/pi
         drifter_z    = .01*drifter_z

c-----------------------------------------------------------------------
c     Calculate interpolation coefficients for calculating
c     horizontal velocity at drifter location.
c
c     Note that U and V are defined relative to the local coordinate
c     frame so we need to rotate them to get true zonal
c     and meridional velocities.
c-----------------------------------------------------------------------

         wijk    = (1. - dx)*(1. - dy)*(1. - zd)
         wijpk   = (1. - dx)*dy*(1. - zd)
         wijkp   = (1. - dx)*(1. - dy)*zd
         wijpkp  = (1. - dx)*dy*zd
         wipjk   = dx*(1. - dy)*(1. - zd)
         wipjpk  = dx*dy*(1. - zd)
         wipjkp  = dx*(1. - dy)*zd
         wipjpkp = dx*dy*zd

c-----------------------------------------------------------------------
c     Interpolate U and V velocity to the particle
c-----------------------------------------------------------------------

         U1 = UVEL(:,:,kk,curtime,1)*cos(ANGLE) +
     &        VVEL(:,:,kk,curtime,1)*sin(-ANGLE)
         U2 = UVEL(:,:,kp,curtime,1)*cos(ANGLE) +
     &        VVEL(:,:,kp,curtime,1)*sin(-ANGLE)
         up = drifter_intp(U1, U2, i, ip, j, jp, wijk, wijpk,
     &                    wijkp, wijpkp, wipjk, wipjpk, wipjkp, wipjpkp)

         U1 = VVEL(:,:,kk,curtime,1)*cos(ANGLE) -
     &        UVEL(:,:,kk,curtime,1)*sin(-ANGLE)
         U2 = VVEL(:,:,kp,curtime,1)*cos(ANGLE) -
     &        UVEL(:,:,kp,curtime,1)*sin(-ANGLE)
         vp = drifter_intp(U1, U2, i, ip, j, jp, wijk, wijpk,
     &                    wijkp, wijpkp, wipjk, wipjpk, wipjkp, wipjpkp)

#if drifter_particles != 2
c-----------------------------------------------------------------------
c     Now do W differently because it is located at cell bottom
c     Note that the horizontal weights are the same as for U,V
c     since WVEL is calculated in advu (still cell-bottom, though).
c     Also, the vertical weights are different--they are simply
c     zz since the vertical index for the drifters is based on
c     the W-grid.
c-----------------------------------------------------------------------

         if(k.eq.0) then
            U1 = DHU
         else
            U1 = WVEL(:,:,k)
         endif
         if(k.ge.km) then
            U2 = 0.  !  should not happen
         else
            U2 = WVEL(:,:,k + 1)
         endif

c-----------------------------------------------------------------------
c     Compute the tri-linear interpolation coefficients
c-----------------------------------------------------------------------

         wijk    = (1. - dx)*(1. - dy)*(1. - zz)
         wijpk   = (1. - dx)*dy*(1. - zz)
         wijkp   = (1. - dx)*(1. - dy)*zz
         wijpkp  = (1. - dx)*dy*zz
         wipjk   = dx*(1. - dy)*(1. - zz)
         wipjpk  = dx*dy*(1. - zz)
         wipjkp  = dx*(1. - dy)*zz
         wipjpkp = dx*dy*zz

c-----------------------------------------------------------------------
c     Interpolate W velocity to the particle
c-----------------------------------------------------------------------

         wp = drifter_intp(U1, U2, i, ip, j, jp, wijk, wijpk,
     &                   wijkp,wijpkp, wipjk, wipjpk, wipjkp, wipjpkp)
#else
c-----------------------------------------------------------------------
c     Drifters are drogued so set W = 0
c-----------------------------------------------------------------------
         wp = c0
#endif

c-----------------------------------------------------------------------
c     Now do Temperature and Salinity.  The horizontal weights need
c     to be modified due to the staggered grid.  Vertical remains
c     the same as with U,V.
c-----------------------------------------------------------------------

         i = int(drifter_ijk(1,np) + 0.5)
         j = int(drifter_ijk(2,np) + 0.5)
         if(j.gt.jmt) j = jmt

         dx = drifter_ijk(1,np) - i + 0.5
         dy = drifter_ijk(2,np) - j + 0.5

         if(i.gt.imt) then
            i = 1
            dx = drifter_ijk(1,np) - imt + 0.5
         endif

         ip = i + 1
         if(ip.gt.imt) ip = 1
         jp = j + 1
         if(jp.gt.jmt) jp = j

         wijk    = (1. - dx)*(1. - dy)*(1. - zd)
         wijpk   = (1. - dx)*dy*(1. - zd)
         wijkp   = (1. - dx)*(1. - dy)*zd
         wijpkp  = (1. - dx)*dy*zd
         wipjk   = dx*(1. - dy)*(1. - zd)
         wipjpk  = dx*dy*(1. - zd)
         wipjkp  = dx*(1. - dy)*zd
         wipjpkp = dx*dy*zd

c-----------------------------------------------------------------------
c     Interpolate Temperature and Salinity to the particle
c-----------------------------------------------------------------------

         U1 = TRACER(:,:,kk,1,curtime,1)
         U2 = TRACER(:,:,kp,1,curtime,1)
         tempp = drifter_intp(U1, U2, i, ip, j, jp, wijk, wijpk,
     &                    wijkp, wijpkp, wipjk, wipjpk, wipjkp, wipjpkp)

         U1 = TRACER(:,:,kk,2,curtime,1)
         U2 = TRACER(:,:,kp,2,curtime,1)
         saltp = drifter_intp(U1, U2, i, ip, j, jp, wijk, wijpk,
     &                    wijkp, wijpkp, wipjk, wipjpk, wipjkp, wipjpkp)

         saltp = saltp*1000.  !  convert to sigma units

c-----------------------------------------------------------------------
c     Write data to out_drifters file
c-----------------------------------------------------------------------

         write(37,'(i5,8e13.5)')np,drifter_long,drifter_lat,drifter_z,
     &                         up,vp,wp,tempp,saltp

      enddo

      end subroutine drifter_prop

c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c     end file drifter_prop.F
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#endif
