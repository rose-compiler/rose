!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

   module current_meters

!BOP
! !MODULE: current_meters
!
! !DESCRIPTION:
!  This module collects data to compare with current meter data.
!  NOTE: this module currently does not work.  old CM-5 routines
!        are appended but must be re-done.
!
! !REVISION HISTORY:
!  CVS:$Id: current_meters.F90,v 1.4 2002/04/16 15:22:53 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

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
! !IROUTINE: init_current_meters
! !INTERFACE:

 subroutine init_current_meters

! !DESCRIPTION:
!  Initializes all necessary variables for current meter diagnostics.
!
! !REVISION HISTORY:
!  same as module

!EOP
!BOC
!-----------------------------------------------------------------------
!-----------------------------------------------------------------------
!EOC

 end subroutine init_current_meters

!***********************************************************************

 end module current_meters

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#ifdef old_current_meters

      integer num_cmeters_max, num_buoys_max, num_cmeters
      parameter ( num_cmeters_max = 1, num_buoys_max = 1100)
      integer nnbr_buoys
      parameter ( nnbr_buoys = 4 )
      integer num_buoys(num_cmeters_max)
      TYPE buoy_xy(num_buoys_max,num_cmeters_max,2)
      character*80 cmeter_file(num_cmeters_max)

      common/hydro_buoys_scalar/  num_cmeters , num_buoys, buoy_xy
      common/hydro_buoys_char/ cmeter_file

      integer, dimension(num_buoys_max, num_cmeters_max
     &                  , nnbr_buoys, 2) :: ADDR_BUOYS
      TYPE, dimension(num_buoys_max, num_cmeters_max
     &                  , nnbr_buoys) :: DIST_BUOYS

      common/hydro_buoys_array/ ADDR_BUOYS, DIST_BUOYS

c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c     begin file cmeters.F
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

      subroutine init_cmeters
c-----------------------------------------------------------------------
c     Initialize everything necessary for writing data for comparison
c     with hydrographic sections.
c-----------------------------------------------------------------------

      use grid
      use io
      use time_management

      implicit none

      integer (kind=int_kind) ::
     &   nu                   ! i/o unit attached to input file

      integer syear, smonth, sday, start_year, cmeter, buoy, nlen
      integer lenc, i, num_temp
      external lenc
      real lat,lon,lat1,lon1

      character*80 temp_file

      double precision pos_buoys(num_buoys_max,2)
      integer, dimension(num_buoys_max,nnbr_buoys,2) :: ADDR_TEMP
      double precision, dimension(num_buoys_max,nnbr_buoys) :: DIST_TEMP

c****************************************************************

c-----------------------------------------------------------------------
c     Read in names of the current meter files used to get times and positions.
c-----------------------------------------------------------------------

      call get_unit(nu)
      open(nu,file = in_cmeters, status = 'old')

      num_cmeters = 0
 10   continue

      if(num_cmeters .gt. num_cmeters_max) then
         write(stdout,*)' '
         write(stdout,*)' Too many current meter files listed in file '
     &              ,in_cmeters
         write(stdout,*)' Increase the value of num_cmeters_max'
         stop
      endif

c-----------------------------------------------------------------------
c     Read in_cmeters file until an error (or end-of-file) occurs
c     so the user does not have to specify the number of files to
c     be read in.  The file name goes into the cmeter_file array.
c     Also note that if we encounter 'none' anywhere,
c     then no files are read in.
c-----------------------------------------------------------------------

      read(nu,'(a)',err = 20) temp_file
      if(temp_file .eq. 'none') then
         write(stdout,*)' '
         write(stdout,*)' No current meter files will be read'
         num_cmeters = 0
         goto 99
      endif
      num_cmeters = num_cmeters + 1
      nlen = lenc(temp_file)
      cmeter_file(num_cmeters) = temp_file(1:nlen)

      goto 10

 20   continue

      write(stdout,*)' '
      write(stdout,"(' Attempting to read in ',i4,
     &              ' current meter files')") num_cmeters
      close(nu)
      call release_unit(nu)

c-----------------------------------------------------------------------
c     Now read in data from each current meter file.  Give warning if there
c     are more buoys on a given current meter than allocated space.
c
c     Current meter files generally have several instruments at the
c     same location (different depths) so we are actually interested
c     in only one horizontal location since well dump velocity at all
c     depths.
c-----------------------------------------------------------------------

      write(stdout,*)' '
      do cmeter = 1,num_cmeters

         temp_file = '/home/ocean/data/cmeters/'/ /cmeter_file(cmeter)
         call get_unit(nu)
         open(nu,file = temp_file, status = 'old')
         write(stdout,*)' Reading file: ',cmeter_file(cmeter)
         read(nu,*)num_temp

         lat = -999.
         lon = -999.
         num_buoys(cmeter) = 0
         do buoy = 1,num_temp
            read(nu,11)lat1,lon1
            if(lat1.ne.lat .or. lon1.ne.lon) then
               if(num_buoys(cmeter).eq.num_buoys_max) then
                 write(stdout,*)' '
                 write(stdout,*)' *** Warning ***'
                 write(stdout,*)
     &            ' More buoys on this current meter file than allowed'
                 write(stdout,"(' Proceeding with first ',i4,' buoys')")
     &                     num_buoys_max
                 num_buoys(cmeter) = num_buoys_max
                 goto 22
               endif
               num_buoys(cmeter) = num_buoys(cmeter) + 1
               lat = lat1
               lon = lon1
               buoy_xy(num_buoys(cmeter),cmeter,1) = lon/radian
               buoy_xy(num_buoys(cmeter),cmeter,2) = lat/radian
            endif
         enddo

 22      continue

         close(nu)
         call release_unit(nu)

      enddo

 11   format(f7.3,1x,f8.3,3(1x,i2),1x,3(1x,i2),1x,a1,1x,a)

c-----------------------------------------------------------------------
c     Compute the neighbor addresses for each buoy.
c-----------------------------------------------------------------------

      do cmeter = 1,num_cmeters
         do buoy = 1,num_buoys(cmeter)
            pos_buoys(buoy,1) = buoy_xy(buoy,cmeter,1)
            pos_buoys(buoy,2) = buoy_xy(buoy,cmeter,2)
         enddo

         call gather_set(ADDR_TEMP, nnbr_buoys, num_buoys(cmeter),
     &    num_buoys_max, ULAT, ULONG, CALCU, pos_buoys, DIST_TEMP)

         ADDR_BUOYS(:,cmeter,:,:) = ADDR_TEMP
         DIST_BUOYS(:,cmeter,:  ) = DIST_TEMP

      enddo

 99   continue

      return
      end subroutine init_cmeters

c**************************************************
      subroutine data_cmeters

c-----------------------------------------------------------------------
c     Gather neighboring data and dump to file for all buoys.
c-----------------------------------------------------------------------

      use grid
      use time_management
      use prognostic

      implicit none

      integer cmeter, buoy, nlen, lenc, k, n, nf, nu, iostat, iml, imr
      external lenc
      character*80 temp_file
      character*10 cday

      integer, dimension(num_buoys_max,nnbr_buoys,2) :: ADDR_TEMP
      double precision, dimension(num_buoys_max,nnbr_buoys,km)
     &      :: WORK1, WORK2, WORK3
      real, dimension(num_buoys_max,nnbr_buoys,km,3)
     &      :: FIELDS_BUOYS
      double precision, dimension(imt,jmt,km) :: FIELD
      integer, dimension(512) :: ITEMP
      real, dimension(num_buoys_max,nnbr_buoys) :: DIST_TEMP
      real, dimension(num_buoys_max,2) :: XY_TEMP
c*********************************************

c-----------------------------------------------------------------------
c     Loop over all buoys and check to see where data is needed.
c     Note that U and V are in local coordinates, so we must rotate
c     them to true zonal and meridional components for non-polar grids.
c-----------------------------------------------------------------------

      do cmeter = 1,num_cmeters

         ADDR_TEMP(:,:,:) = ADDR_BUOYS(:,cmeter,:,:)
         WORK1 = c0  ! initialize

         FIELD = U
         call gather(WORK1, FIELD, ADDR_TEMP, nnbr_buoys
     &              , num_buoys(cmeter))

         FIELD = V
         call gather(WORK2, FIELD, ADDR_TEMP, nnbr_buoys
     &              , num_buoys(cmeter))

#if polar_grid

         FIELDS_BUOYS(:,:,:,1) = WORK1
         FIELDS_BUOYS(:,:,:,2) = WORK2

#else

c-----------------------------------------------------------------------
c     Rotate velocity vector to lat-long grid for non-polar grids.
c-----------------------------------------------------------------------

         do k = 1,km
            FIELD(:,:,k) = ANGLE  !  redundant but easy
         enddo
         call gather(WORK3, FIELD, ADDR_TEMP, nnbr_buoys
     &              , num_buoys(cmeter))

         FIELDS_BUOYS(:,:,:,1) = WORK1*cos(WORK3) + WORK2*sin(-WORK3)
         FIELDS_BUOYS(:,:,:,2) = WORK2*cos(WORK3) - WORK1*sin(-WORK3)

#endif

         call wcalc(FIELD,U,V,this_block)  !  FIELD contains W
         call gather(WORK1, FIELD, ADDR_TEMP, nnbr_buoys
     &              , num_buoys(cmeter))
         FIELDS_BUOYS(:,:,:,3) = WORK1

         write (cday,'(i10)') iday + 1000000000  !  allows 2.7 million years
         nlen = lenc(cmeter_file(cmeter))
         iml = 10 - movie_digits + 1
         imr = lenc(out_cmeters)
         temp_file = out_cmeters(1:imr)/ /cmeter_file(cmeter)(1:nlen)
     &     / /'.'/ /cday(iml:10)

         nu = 31
         call CMF_file_open(nu,temp_file,iostat)
         write(stdout,*)' '
         write(stdout,*)' Writing file: ',temp_file

         ITEMP(1) = num_buoys_max
         ITEMP(2) = num_buoys(cmeter)
         ITEMP(3) = nnbr_buoys
         ITEMP(4) = km

         DIST_TEMP = -999.
         do k = 1,nnbr_buoys
            do n = 1,num_buoys(cmeter)
               DIST_TEMP(n,k) = DIST_BUOYS(n,cmeter,k)
            enddo
         enddo

         XY_TEMP = -999.
         do n = 1,num_buoys(cmeter)
            XY_TEMP(n,1) = buoy_xy(n,cmeter,1)
            XY_TEMP(n,2) = buoy_xy(n,cmeter,2)
         enddo

         call CMF_cm_array_to_file(nu,ITEMP,iostat)
         if (iostat.eq.-1) then
          if(my_task.eq.master_task)
     &       write(stdout,*) ' i/o error writing ',ITEMP
          call write_status(iostat)
          stop
         endif

         call CMF_cm_array_to_file(nu,DIST_TEMP,iostat)
         if (iostat.eq.-1) then
          if(my_task.eq.master_task)
     &       write(stdout,*) ' i/o error writing ',DIST_TEMP
          call write_status(iostat)
          stop
         endif

         call CMF_cm_array_to_file(nu,XY_TEMP,iostat)
         if (iostat.eq.-1) then
          if(my_task.eq.master_task)
     &       write(stdout,*) ' i/o error writing ',XY_TEMP
          call write_status(iostat)
          stop
         endif

         call CMF_cm_array_to_file(nu,FIELDS_BUOYS,iostat)
         if (iostat.eq.-1) then
          if(my_task.eq.master_task)
     &       write(stdout,*) ' i/o error writing ',FIELDS_BUOYS
          call write_status(iostat)
          stop
         endif

         call CMF_file_close(nu,iostat)

      enddo

      end subroutine data_cmeters
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c     end file cmeters.F
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c     begin file gather.F
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

c***********************************************************************
c     this set of routines searches for and gathers the nnbr closest
c     points on an ocean grid to a given set of target points.
c
c     written by: Phil Jones, T-3, Los Alamos National Laboratory
c     date last revised: 6 December 1994
c***********************************************************************

      subroutine gather(DSTARR, SRCARR, IADDR, nnbr, nlist)

c-----------------------------------------------------------------------
c     this routine gathers nnbr values from srcarr and places the
c     result in dstarr given a list of addresses computed in a
c     previous setup routine (gather_set).
c-----------------------------------------------------------------------

      implicit none

c-----------------------------------------------------------------------
c     intent (in):
c
c       nlist        = number of search points
c
c       nnbr         = number of neighboring points to gather for each
c                      target point
c
c       SRCARR       = the ocean variable to gather
c
c       IADDR        = the list of gather addresses computed in
c                      a previous setup routine
c-----------------------------------------------------------------------

      integer nlist, nnbr, k
      integer, dimension(nlist, nnbr, 2) :: IADDR
      real, dimension(imt, jmt, km) :: SRCARR

c-----------------------------------------------------------------------
c     intent(out):
c
c       DSTARR       = values at all the neighboring ocean points
c-----------------------------------------------------------------------

      real, dimension(nlist, nnbr, km) :: DSTARR


c-----------------------------------------------------------------------
c     local variables:
c-----------------------------------------------------------------------

      integer i, n

c-----------------------------------------------------------------------
c     gather up the nnbr neighbors for all model levels.
c-----------------------------------------------------------------------

      DSTARR = 0.0
      do k = 1,km
         forall (n=1:nlist, i=1:nnbr)
     &       DSTARR(n,i,k) = SRCARR(IADDR(n,i,1),IADDR(n,i,2),k)
      enddo

c-----------------------------------------------------------------------
      return
      end subroutine gather
c***********************************************************************
c***********************************************************************
c***********************************************************************

      subroutine gather_set(IADDR, nnbr, nlist, nlmax, OLAT, OLON, OMSK,
     &                             plist, NBR_DIST)

c-----------------------------------------------------------------------
c     this subroutine sets up address arrays for the nnbr closests
c     points to a set of target points.
c-----------------------------------------------------------------------

      implicit none

c-----------------------------------------------------------------------
c     intent(in):
c
c       nlist        = number of points in list of search points
c
c       nnbr         = number of neighboring points to gather for
c                      each target point
c
c       OLAT, OLON   = array of latitudes and longitudes on the
c                      ocean grid
c
c       OMSK         = ocean mask
c
c       plist        = list of points for which neighboring ocean
c                      points are desired
c-----------------------------------------------------------------------

      integer nlist, nnbr, nlmax
      logical, dimension(imt, jmt) :: OMSK
      real, dimension(imt, jmt) :: OLAT, OLON
      real, dimension(nlmax, 2) :: plist

c-----------------------------------------------------------------------
c     intent(out):
c
c       IADDR  = the addresses of the nnbr closest ocean points to the
c                list of search points
c-----------------------------------------------------------------------

      integer, dimension(nlmax, nnbr, 2) :: IADDR
      real, dimension(nlmax, nnbr) :: NBR_DIST

c-----------------------------------------------------------------------
c     local variables:
c-----------------------------------------------------------------------

      integer i, j, n
      real *8 rlat, rlon, fac1, fac2, fac3, dmin, wttmp, dsum
      real, dimension(imt, jmt) :: OARC1, OARC2, OARC3, DIST

c-----------------------------------------------------------------------
c     set up some arc-length constants and initialize some stuff.
c-----------------------------------------------------------------------

      OARC3 = COS(OLAT)
      OARC1 = COS(OLON)*OARC3
      OARC2 = SIN(OLON)*OARC3
      OARC3 = SIN(OLAT)

      IADDR  = 0

c-----------------------------------------------------------------------
c     loop through the list of destination points.  compute the
c     arclength from this point to all the ocean grid points.
c-----------------------------------------------------------------------

      do n=1,nlist
        rlon = plist(n, 1)
        rlat = plist(n, 2)
        fac3 = COS(rlat)
        fac1 = COS(rlon)*fac3
        fac2 = SIN(rlon)*fac3
        fac3 = SIN(rlat)
        DIST = ACOS(OARC1*fac1 + OARC2*fac2 + OARC3*fac3)

c-----------------------------------------------------------------------
c       find the closest nnbr points. eliminate land points.
c-----------------------------------------------------------------------

        where (.NOT. OMSK) DIST = 10.0
        do i=1,nnbr
          IADDR(n,i,:) = MINLOC(DIST)
          NBR_DIST(n,i) = DIST(IADDR(n,i,1),IADDR(n,i,2))
          DIST(IADDR(n,i,1),IADDR(n,i,2)) = 10.0
        end do
      end do

c-----------------------------------------------------------------------

      return
      end subroutine gather_set

c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c     end file gather.F
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#endif

