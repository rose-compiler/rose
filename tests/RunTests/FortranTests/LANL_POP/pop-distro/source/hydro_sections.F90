!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module hydro_sections

!BOP
! !MODULE: hydro_sections
!
! !DESCRIPTION:
!  This module computes data along hydrographic sections to compare
!  with cruise data.
!  NOTE: currently does not work.  routines appended below are from old
!  CM-5 version and must be re-done.
!
! !REVISION HISTORY:
!  CVS:$Id: hydro_sections.F90,v 1.4 2002/04/16 15:22:56 pwjones Exp $
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
! !IROUTINE: init_hydro_sections
! !INTERFACE:

 subroutine init_hydro_sections

! !DESCRIPTION:
!  Initializes all variables to be used for hydrographic sections.
!
! !REVISION HISTORY:
!  same as module

!EOP
!BOC
!-----------------------------------------------------------------------
!-----------------------------------------------------------------------
!EOC

 end subroutine init_hydro_sections

!***********************************************************************

 end module hydro_sections

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#ifdef hydrographic_stations

      integer num_cruises_max, num_stations_max, num_cruises
      parameter ( num_cruises_max = 100, num_stations_max = 300)
      integer nnbr_stations, num_fields
      parameter ( nnbr_stations = 4 , num_fields = 5)
      integer station_nday(num_stations_max,num_cruises_max)
      integer num_stations(num_cruises_max)
      TYPE station_xy(num_stations_max,num_cruises_max,2)
      character*80 cruise_file(num_cruises_max)

      common/hydro_stations_scalar/  num_cruises, station_nday
     &  , num_stations, station_xy
      common/hydro_stations_char/ cruise_file 

      integer, dimension(num_stations_max, num_cruises_max
     &                  , nnbr_stations, 2) :: ADDR_STATIONS
      TYPE, dimension(num_stations_max, num_cruises_max
     &                  , nnbr_stations) :: DIST_STATIONS

      common/hydro_stations_array/ ADDR_STATIONS, DIST_STATIONS 

      integer num_slices_max, num_columns_max, num_slices
      parameter ( num_slices_max = 1, num_columns_max = 500)
      integer nnbr_columns, mum_fields
      parameter ( nnbr_columns = 1, mum_fields = 5 )
      integer num_columns(num_slices_max)
      TYPE column_xy(num_columns_max,num_slices_max,2)
      character*80 slice_file(num_slices_max)

      common/hydro_columns_scalar/  num_slices , num_columns, column_xy
      common/hydro_columns_char/ slice_file 

      integer, dimension(num_columns_max, num_slices_max
     &                  , nnbr_columns, 2) :: ADDR_COLUMNS
      TYPE, dimension(num_columns_max, num_slices_max
     &                  , nnbr_columns) :: DIST_COLUMNS

      common/hydro_columns_array/ ADDR_COLUMNS, DIST_COLUMNS 

c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c     begin file station.F
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

      subroutine init_stations
c-----------------------------------------------------------------------
c     Initialize everything necessary for writing data for comparison
c     with hydrographic sections.
c-----------------------------------------------------------------------

      use grid
      use time_management

      implicit none

      integer day_sum(12)
      data day_sum / 0, 31, 59, 90, 120, 151, 181, 212,
     &               243, 273, 304, 334 /

      integer syear, smonth, sday, start_year, cruise, station, nlen
      integer lenc, i, i_long_max
      external lenc
      real lat,lon,long_max

      character*80 temp_file

      real pos_stations(num_stations_max,2)
      integer, dimension(num_stations_max,nnbr_stations,2) :: ADDR_TEMP
      real, dimension(num_stations_max,nnbr_stations) :: DIST_TEMP

c****************************************************************

c-----------------------------------------------------------------------
c     Read in names of the cruise files used to get times and positions.
c-----------------------------------------------------------------------

      open(31,file = in_cruises, status = 'old')

      num_cruises = 0
 10   continue

      if(num_cruises .gt. num_cruises_max) then
         write(*,*)' '
         write(*,*)' Too many cruise files listed in file ',in_cruises
         write(*,*)' Increase the value of num_cruises_max'
         stop
      endif

c-----------------------------------------------------------------------
c     Read in_cruises file until an error (or end-of-file) occurs
c     so the user does not have to specify the number of files to
c     be read in.  The file name goes into the cruise_file array.
c     Also note that if we encounter 'none' anywhere,
c     then no files are read in.
c-----------------------------------------------------------------------

      read(31,'(a)',err = 20) temp_file
      if(temp_file .eq. 'none') then
         write(*,*)' '
         write(*,*)' No cruise files will be read'
         num_cruises = 0
         goto 99
      endif
      num_cruises = num_cruises + 1
      nlen = lenc(temp_file)
      cruise_file(num_cruises) = temp_file(1:nlen)

      goto 10

 20   continue

      write(*,*)' '
      write(*,"(' Attempting to read in ',i4,' cruise files')")
     &      num_cruises
      close(31)

c-----------------------------------------------------------------------
c     Now read in data from each cruise file.  Give warning if there are
c     more stations on a given cruise than allocated space.
c
c     We assume that the run started on Jan 1 of the year 'start_year'.
c     With this assumption, the array 'station_nday' gives the model
c     day on which the data should be gathered.  If start_year = -999,
c     then we want all sections to be done this year.  that is, the
c     specific year is not important, but the time of year is.
c-----------------------------------------------------------------------

      start_year = 85
      write(*,*)' '
      do cruise = 1,num_cruises

         temp_file = '/home/ocean/data/cruises/'/ /cruise_file(cruise)
         open(31,file = temp_file, status = 'old')
         write(*,*)' Reading file: ',cruise_file(cruise)
         read(31,*)num_stations(cruise)

         if(num_stations(cruise) .gt. num_stations_max) then
            write(*,*)' '
            write(*,*)' *** Warning ***'
            write(*,*)' More stations on this cruise than allowed'
            write(*,"(' Proceeding with first ',i4,' stations')")
     &                 num_stations_max
            num_stations(cruise) = num_stations_max
         endif
           
         do station = 1,num_stations(cruise)
            read(31,11)lat,lon,syear,smonth,sday
            station_xy(station,cruise,1) = lon/radian
            station_xy(station,cruise,2) = lat/radian

            if(start_year .eq. -999) then
              station_nday(station,cruise) =
     &           tyear*365 + day_sum(smonth) + sday + 1
            else
              station_nday(station,cruise) =
     &           (syear - start_year)*365 + day_sum(smonth) + sday + 1
            endif
         enddo

         close(31)

      enddo

 11   format(f7.3,1x,f8.3,3(1x,i2),1x,3(1x,i2),1x,a1,1x,a)

c-----------------------------------------------------------------------
c     Create output files if necessary.
c
c     First check to see if the output file already exists (from a
c     previous portion of the run, for example).  This is done by
c     checking to see if the number of stations for the particular cruise
c     is on the first line of the file.  If it is, then the file is left
c     alone.  Otherwise, it writes the number of stations on the first
c     line.
c-----------------------------------------------------------------------

      write(*,*)' '
      do cruise = 1,num_cruises
         nlen = lenc(cruise_file(cruise))
         temp_file = cruise_file(cruise)(1:nlen)/ /'.model'
         open(31,file = temp_file, status = 'unknown')
         read(31,*,err = 30)nlen
         goto 40
 30      continue
         close(31)
         write(*,*)' Creating file ',temp_file
         open(31,file = temp_file, status = 'unknown')
         write(31,*)num_stations(cruise)
 40      continue
         close(31)
      enddo

c-----------------------------------------------------------------------
c     Compute the neighbor addresses for each station.
c-----------------------------------------------------------------------

      do cruise = 1,num_cruises
         do station = 1,num_stations(cruise)
            pos_stations(station,1) = station_xy(station,cruise,1)
            pos_stations(station,2) = station_xy(station,cruise,2)
         enddo

         call gather_set(ADDR_TEMP, nnbr_stations, num_stations(cruise),
     &    num_stations_max, TLAT, TLONG, CALCT, pos_stations, DIST_TEMP)

         ADDR_STATIONS(:,cruise,:,:) = ADDR_TEMP
         DIST_STATIONS(:,cruise,:  ) = DIST_TEMP

      enddo

 99   continue

      return
      end subroutine init_stations

c**************************************************
      subroutine data_stations

c-----------------------------------------------------------------------
c     Gather neighboring data and dump to file for all stations that
c     are due for such action on model day 'iday'
c-----------------------------------------------------------------------

      use grid
      use time_management
      use prognostic

      implicit none

      integer cruise, station, nlen, lenc, k, n, nf
      external lenc
      character*80 temp_file

      integer, dimension(1,nnbr_stations,2) :: ADDR_TEMP
      real, dimension(1,nnbr_stations,km) :: WORK1, WORK2, WORK3
      real, dimension(1,nnbr_stations,km,num_fields) :: FIELDS_STATIONS
      real, dimension(imt,jmt,km) :: FIELD
c*********************************************

c-----------------------------------------------------------------------
c     Loop over all stations and check to see where data is needed.
c     Then append data to the appropriate file.
c
c     Note that we are dumping U,V using T-point indices.
c-----------------------------------------------------------------------

      do cruise = 1,num_cruises
         do station = 1,num_stations(cruise)

            if(station_nday(station,cruise).eq.iday) then
               ADDR_TEMP(1,:,:) = ADDR_STATIONS(station,cruise,:,:)
               WORK1 = c0  ! initialize

               FIELD = TRACER(:,:,:,1)
               call gather(WORK1, FIELD, ADDR_TEMP, nnbr_stations, 1)
               FIELDS_STATIONS(:,:,:,1) = WORK1

               FIELD = TRACER(:,:,:,2)
               call gather(WORK1, FIELD, ADDR_TEMP, nnbr_stations, 1)
               FIELDS_STATIONS(:,:,:,2) = WORK1

               FIELD = U
               call gather(WORK1, FIELD, ADDR_TEMP, nnbr_stations, 1)

               FIELD = V
               call gather(WORK2, FIELD, ADDR_TEMP, nnbr_stations, 1)

               do k = 1,km
                  FIELD(:,:,k) = ANGLE  !  redundant but easy
               enddo
               call gather(WORK3, FIELD, ADDR_TEMP, nnbr_stations, 1)

c-----------------------------------------------------------------------
c     Rotate horizontal velocity vector to lat-long grid.
c-----------------------------------------------------------------------

               FIELDS_STATIONS(:,:,:,3) = 
     &            WORK1*cos(WORK3) + WORK2*sin(-WORK3)
               FIELDS_STATIONS(:,:,:,4) = 
     &            WORK2*cos(WORK3) - WORK1*sin(-WORK3)

               call wcalc(FIELD,U,V,this_block)  !  FIELD contains W
               call gather(WORK1, FIELD, ADDR_TEMP, nnbr_stations, 1)
               FIELDS_STATIONS(:,:,:,5) = WORK1

               nlen = lenc(cruise_file(cruise))
               temp_file = cruise_file(cruise)(1:nlen)/ /'.model'
               open(31,file=temp_file, status='unknown',access='append')
               write(*,*)' '
               write(*,*)' Appending file: ',temp_file

c-----------------------------------------------------------------------
c     First write long, lat, time and number of neighbors of station data.  
c     Then loop over neighboring points, first dumping the distance
c     from the model point to the true station point, then dumping data
c     at all depths.
c-----------------------------------------------------------------------

               write(31,'(3f12.3,1x,i4)')
     &            station_xy(station,cruise,1)*radian,
     &            station_xy(station,cruise,2)*radian,
     &            tday, nnbr_stations

               do n = 1,nnbr_stations
                  write(31,*)DIST_STATIONS(station,cruise,n)
                  do k = 1,km
                     write(31,'(5e15.6)')
     &                  (FIELDS_STATIONS(1,n,k,nf),nf=1,num_fields)
                  enddo
               enddo

               close(31)
            endif
         enddo


      enddo

      end subroutine data_stations
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c     end file station.F
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c     begin file slices.F
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

      subroutine init_slices
c-----------------------------------------------------------------------
c     Initialize everything necessary for writing data for comparison
c     with hydrographic sections.
c-----------------------------------------------------------------------

      use grid
      use time_management

      implicit none

      integer slice, column, nlen
      integer lenc, i, i_long_max
      external lenc
      real lat,lon,long_max

      character*80 temp_file, in_slices

      double precision pos_columns(num_columns_max,2)
      integer, dimension(num_columns_max,nnbr_columns,2) :: ADDR_TEMP
      double precision, 
     &   dimension(num_columns_max,nnbr_columns) :: DIST_TEMP

c****************************************************************

c-----------------------------------------------------------------------
c     Read in names of the slice files used to get times and positions.
c-----------------------------------------------------------------------

      in_slices = 'in_slices.dat'
      open(31,file = in_slices, status = 'old')

      num_slices = 0
 10   continue

      if(num_slices .gt. num_slices_max) then
         write(*,*)' '
         write(*,*)' Too many slice files listed in file ',in_slices
         write(*,*)' Increase the value of num_slices_max'
         stop
      endif

c-----------------------------------------------------------------------
c     Read in_slices file until an error (or end-of-file) occurs
c     so the user does not have to specify the number of files to
c     be read in.  The file name goes into the slice_file array.
c     Also note that if we encounter 'none' anywhere,
c     then no files are read in.
c-----------------------------------------------------------------------

      read(31,'(a)',err = 20) temp_file
      if(temp_file .eq. 'none') then
         write(*,*)' '
         write(*,*)' No slice files will be read'
         num_slices = 0
         goto 99
      endif
      num_slices = num_slices + 1
      nlen = lenc(temp_file)
      slice_file(num_slices) = temp_file(1:nlen)

      goto 10

 20   continue

      write(*,*)' '
      write(*,"(' Attempting to read in ',i4,' slice files')")
     &      num_slices
      close(31)

c-----------------------------------------------------------------------
c     Now read in data from each slice file.  Give warning if there are
c     more columns on a given slice than allocated space.
c
c     We assume that the run started on Jan 1 of the year 'start_year'.
c     With this assumption, the array 'column_nday' gives the model
c     day on which the data should be gathered.  If start_year = -999,
c     then we want all sections to be done this year.  that is, the
c     specific year is not important, but the time of year is.
c-----------------------------------------------------------------------

      write(*,*)' '
      do slice = 1,num_slices

         temp_file = '/home/ocean/data/slices/'/ /slice_file(slice)
         open(31,file = temp_file, status = 'old')
         write(*,*)' Reading file: ',slice_file(slice)
         read(31,*)num_columns(slice)

         if(num_columns(slice) .gt. num_columns_max) then
            write(*,*)' '
            write(*,*)' *** Warning ***'
            write(*,*)' More columns on this slice than allowed'
            write(*,'(i4)')' Proceeding with first ',num_columns_max,
     &                  ' columns '
            num_columns(slice) = num_columns_max
         endif
           
         do column = 1,num_columns(slice)
            read(31,*)lat,lon
            column_xy(column,slice,1) = lon/radian
            column_xy(column,slice,2) = lat/radian
         enddo

         close(31)

      enddo

c-----------------------------------------------------------------------
c     Compute the neighbor addresses for each columns.
c-----------------------------------------------------------------------

      do slice = 1,num_slices
         do column = 1,num_columns(slice)
            pos_columns(column,1) = column_xy(column,slice,1)
            pos_columns(column,2) = column_xy(column,slice,2)
         enddo

         call gather_set(ADDR_TEMP, nnbr_columns, num_columns(slice),
     &    num_columns_max, TLAT, TLONG, CALCT, pos_columns, DIST_TEMP)

         ADDR_COLUMNS(:,slice,:,:) = ADDR_TEMP
         DIST_COLUMNS(:,slice,:  ) = DIST_TEMP

      enddo

 99   continue

      return
      end subroutine init_slices

c**************************************************
      subroutine data_slices

c-----------------------------------------------------------------------
c     Gather neighboring data and dump to file for all slices that
c     are due for such action on model day 'iday'
c-----------------------------------------------------------------------

      use grid
      use time_management
      use prognostic

      implicit none

      integer slice, column, nlen, lenc, k, n, nf, iml, imr, iostat, nu
      external lenc
      character*80 temp_file, out_slices
      character*10 cday

      integer, dimension(num_columns_max,nnbr_columns,2) :: ADDR_TEMP
      double precision, dimension(num_columns_max,nnbr_columns,km) 
     &    :: WORK1, WORK2, WORK3
      real, dimension(num_columns_max,nnbr_columns,km,mum_fields) 
     &    :: FIELDS_COLUMNS
      double precision, dimension(imt,jmt,km) :: FIELD
      integer, dimension(512) :: ITEMP
      real, dimension(num_columns_max,nnbr_columns) :: DIST_TEMP
      real, dimension(num_columns_max,2) :: XY_TEMP

c*********************************************

c-----------------------------------------------------------------------
c     Loop over all slices.
c
c     Note that we are dumping U,V using T-point indices.
c-----------------------------------------------------------------------

      out_slices = '/sda/sda2/maltrud/Sixth_Barnier/slices/'

      do slice = 1,num_slices

         ADDR_TEMP(:,:,:) = ADDR_COLUMNS(:,slice,:,:)
         WORK1 = c0  ! initialize

         FIELD = TRACER(:,:,:,1)
         call gather(WORK1, FIELD, ADDR_TEMP, nnbr_columns
     &              , num_columns(slice))
         FIELDS_COLUMNS(:,:,:,1) = WORK1

         FIELD = TRACER(:,:,:,2)
         call gather(WORK1, FIELD, ADDR_TEMP, nnbr_columns
     &              , num_columns(slice))
         FIELDS_COLUMNS(:,:,:,2) = WORK1

         FIELD = U
         call gather(WORK1, FIELD, ADDR_TEMP, nnbr_columns
     &              , num_columns(slice))

         FIELD = V
         call gather(WORK2, FIELD, ADDR_TEMP, nnbr_columns
     &              , num_columns(slice))

#if polar_grid

         FIELDS_COLUMNS(:,:,:,3) = WORK1
         FIELDS_COLUMNS(:,:,:,4) = WORK2

#else

         do k = 1,km
            FIELD(:,:,k) = ANGLE  !  redundant but easy
         enddo
         call gather(WORK3, FIELD, ADDR_TEMP, nnbr_columns
     &              , num_columns(slice))

c-----------------------------------------------------------------------
c     Rotate horizontal velocity vector to lat-long grid for non-polar grids.
c-----------------------------------------------------------------------

         FIELDS_COLUMNS(:,:,:,3) = WORK1*cos(WORK3) + WORK2*sin(-WORK3)
         FIELDS_COLUMNS(:,:,:,4) = WORK2*cos(WORK3) - WORK1*sin(-WORK3)

#endif

         call wcalc(FIELD,U,V,this_block)  !  FIELD contains W
         call gather(WORK1, FIELD, ADDR_TEMP, nnbr_columns
     &              , num_columns(slice))
         FIELDS_COLUMNS(:,:,:,5) = WORK1

         write (cday,'(i10)') iday + 1000000000  !  allows 2.7 million years
         nlen = lenc(slice_file(slice))
         iml = 10 - movie_digits + 1
         imr = lenc(out_slices)
         temp_file = out_slices(1:imr)/ /slice_file(slice)(1:nlen)
     &     / /'.'/ /cday(iml:10)

         nu = 31
         call CMF_file_open(nu,temp_file,iostat)
         write(*,*)' '
         write(*,*)' Writing file: ',temp_file

         ITEMP(1) = num_columns_max
         ITEMP(2) = num_columns(slice)
         ITEMP(3) = nnbr_columns
         ITEMP(4) = km

         DIST_TEMP = -999.
         do k = 1,nnbr_columns
            do n = 1,num_columns(slice)
               DIST_TEMP(n,k) = DIST_COLUMNS(n,slice,k)
            enddo
         enddo

         XY_TEMP = -999.
         do n = 1,num_columns(slice)
            XY_TEMP(n,1) = column_xy(n,slice,1)
            XY_TEMP(n,2) = column_xy(n,slice,2)
         enddo

         call CMF_cm_array_to_file(nu,ITEMP,iostat)
         if (iostat.eq.-1) then
          if(my_task.eq.master_task) 
     &       write(*,*) ' i/o error writing ',ITEMP
          call write_status(iostat)
          stop
         endif

         call CMF_cm_array_to_file(nu,DIST_TEMP,iostat)
         if (iostat.eq.-1) then
          if(my_task.eq.master_task) 
     &       write(*,*) ' i/o error writing ',DIST_TEMP
          call write_status(iostat)
          stop
         endif

         call CMF_cm_array_to_file(nu,XY_TEMP,iostat)
         if (iostat.eq.-1) then
          if(my_task.eq.master_task) 
     &       write(*,*) ' i/o error writing ',XY_TEMP
          call write_status(iostat)
          stop
         endif

         call CMF_cm_array_to_file(nu,FIELDS_COLUMNS,iostat)
         if (iostat.eq.-1) then
          if(my_task.eq.master_task) 
     &       write(*,*) ' i/o error writing ',FIELDS_COLUMNS
          call write_status(iostat)
          stop
         endif

         call CMF_file_close(nu,temp_file,iostat)

      enddo

      end subroutine data_slices
c|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
c     end file slices.F
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

