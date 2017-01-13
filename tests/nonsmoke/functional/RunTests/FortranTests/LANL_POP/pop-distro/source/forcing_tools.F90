!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module forcing_tools

!BOP
! !MODULE: forcing_tools
! !DESCRIPTION:
!  Contains common variables, parameters and routines that are used by
!  all of the individual forcing modules.
!
! !REVISION HISTORY:
!  CVS:$Id: forcing_tools.F90,v 1.10 2003/01/10 00:11:25 jfd Exp $
!  CVS:$Name: POP_2_0_1 $
!
! !USES:

   use kinds_mod
   use blocks
   use domain
   use constants
   use io
   use io_types
   use grid
   use time_management
   use exit_mod

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: find_interp_time,      &
             get_forcing_filename,  &
             find_forcing_times,    &
             interpolate_forcing,   &
             update_forcing_data,   &
             echo_forcing_options

! !PUBLIC DATA MEMBERS:

   real (r8), parameter, public :: &
      never    =  1.e20_r8,        &! value to signify never
      always   = -1.e20_r8          ! value to signify always

   !*** common data shared by forcing routines


!EOP
!BOC
!-----------------------------------------------------------------------
!
!  internal module variables
!
!-----------------------------------------------------------------------

   real (r8), dimension(12) :: &
      thour00_midmonth,        &
      thour00_endmonth

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: find_interp_time
! !INTERFACE:

 subroutine find_interp_time(forcing_interp_inc, forcing_interp_next)

! !DESCRIPTION:
!  Finds the next time at which temporal interpolation is needed
!  in the forcing.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), intent(in) :: &
      forcing_interp_inc    ! increment (hours) for intepolation time

! !OUTPUT PARAMETERS:

   real (r8), intent(out) :: &
      forcing_interp_next    ! next interpolation time

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: nstart, n

   real (r8) :: forcing_time_test

!-----------------------------------------------------------------------
!
!  find the first interpolation time that is greater than the current
!  time, which then becomes the next interpolation time.
!
!-----------------------------------------------------------------------

   nstart = int(thour00/forcing_interp_inc) - 2

   do n = nstart, nstart + 5
      forcing_time_test = (n + p5)*forcing_interp_inc
      if (forcing_time_test > thour00) exit
   enddo

   forcing_interp_next = forcing_time_test

!-----------------------------------------------------------------------
!EOC

 end subroutine find_interp_time

!***********************************************************************
!BOP
! !IROUTINE: get_forcing_filename
! !INTERFACE:

 subroutine get_forcing_filename(forcing_filename, forcing_infile, &
                                 forcing_time, forcing_data_inc)

! !DESCRIPTION:
!  Finds the name of an n-hour file given the forcing time needed.
!    Files can be labeled by the date of the beginning of the
!    forcing interval or by the date of the middle in the forcing
!    interval (which corresponds to the value of the variable
!    forcing\_time) depending on the value of
!    filename\_at\_begin\_of\_interval (T or F).
!
!  File names are asumed to be of the form:
!    fileroot.year.day.hour (fileroot.yyyy.ddd.hh)
!    where fileroot is specified by namelist input, year is
!    4 digits, day is 3 digits, and hour is 2 digits.
!    for example, Jan 1 of year 22 with a root of 'ws' would be
!    ws.0022.001.00 (labeled by beginning of interval)
!    ws.0022.001.12 (labeled by middle of 1-day interval)
!
!  Remember, forcing times are relative to 01-01-0000 so results
!    may not be what you expect.  for example, if the forcing
!    increment is 2 days, then even numbered years will have
!    mid-interval-referenced files that are on even numbered
!    days, and odd numbered years will reference odd numbered
!    days (assuming no leap years).
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), intent(in) :: &
      forcing_time,         &! time at begin of forcing interval
      forcing_data_inc       ! increment in hours to read data

   character (*), intent(in) :: &
      forcing_infile         ! root filename

! !OUTPUT PARAMETERS:

   character (char_len), intent(out) :: &
      forcing_filename       ! output forcing filename

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      file_year,         &! year for file name
      file_day,          &! day  for file name
      file_hour,         &! hour for file name
      n,                 &! dummy loop index
      days_in_prev_year, &! days in previous year
      cindx               ! index for character strings

   real (r8) ::          &
      file_time,         &!
      file_time00         !

   character (len = 5) :: file_cyear  ! character version of the
   character (len = 4) :: file_cday   ! above integer fields
   character (len = 3) :: file_chour

   logical (log_kind) ::  &
      filename_at_begin_of_interval ! flag for position of data file
                                    ! in forcing interval (begin,middle)

!-----------------------------------------------------------------------
!
!  filename_at_begin_of_interval = .true. for filenames defined at
!     the beginning of the forcing interval.
!  filename_at_begin_of_interval = .false. for filenames defined at
!     the middle of the forcing interval.
!
!-----------------------------------------------------------------------

   filename_at_begin_of_interval = .false.

!-----------------------------------------------------------------------
!
!  find the time associated with the beginning or middle of the
!  forcing interval.
!
!-----------------------------------------------------------------------

   if (filename_at_begin_of_interval) then
      file_time = forcing_time - p5*forcing_data_inc + eps
   else  !  filename defined at forcing time (middle of forcing interval)
      file_time = forcing_time + eps
   endif

!-----------------------------------------------------------------------
!
!  if the file time is negative, find the first appropriate
!  positive value and use it.
!
!-----------------------------------------------------------------------

   if (file_time < c0 ) then
      if (my_task == master_task) then
         write(stdout,'(a52)') &
            'WARNING: apparently need a forcing file earlier than'
         write(stdout,'(a54)') &
            '01-01-0000 will attempt to find first appropriate file'
      endif
      do n = 1,10
         if (filename_at_begin_of_interval) then
            file_time = forcing_time + (n-p5)*forcing_data_inc + eps
         else
            file_time = forcing_time + n*forcing_data_inc + eps
         endif
         if(file_time >= c0 ) exit
      enddo
   endif
   if (file_time < c0 ) then
      call exit_POP(sigAbort,'get_forcing_filename confused')
   endif

!-----------------------------------------------------------------------
!
!  convert file time to time relative to the beginning of the year.
!  then find the day and hour corresponding to that time.
!
!  note that the use of 'eps' above is important here due to the
!     'int' and 'nint' functions, especially if file_time00 < 0.
!
!-----------------------------------------------------------------------

   thour00_begin_this_year = thour00 - &
                             (elapsed_days_this_year + frac_day)*24.0_r8
   file_time00 = file_time - thour00_begin_this_year
   file_day  = int(abs(file_time00)/24.0_r8)
   file_hour = nint(abs(file_time00) - 24*file_day)

!-----------------------------------------------------------------------
!
!  if file_time00 < 0, then the time is in the previous year.
!
!  if file_time00 > the number of hours in a year, then the time is
!  in the next year.
!
!-----------------------------------------------------------------------

   if (file_time00 < c0 )then
      file_hour = 24 - file_hour  !  counting backwards
      file_year = iyear - 1
      days_in_prev_year = days_in_norm_year
      if (allow_leapyear .and. mod(file_year,4) == 0) then ! check if previous
         if (mod(file_year,100) /= 0) &                    ! year was leapyear
            days_in_prev_year = days_in_leap_year
         if (mod(file_year,400) == 0) &
            days_in_prev_year = days_in_leap_year
      endif
      file_day = days_in_prev_year - file_day

   elseif (file_time00 > 24*days_in_year) then
      file_day = file_day - days_in_year + 1
      file_year = iyear + 1
   else
      file_day = file_day + 1  !  day counting starts at 1, not 0
      file_year = iyear
   endif

!-----------------------------------------------------------------------
!
!  write year, day, and hour into character variables and use them
!  to create filename.
!
!-----------------------------------------------------------------------

   write(file_chour,'(i3)') 100   + file_hour
   write(file_cday, '(i4)') 1000  + file_day
   write(file_cyear,'(i5)') 10000 + file_year

   forcing_filename = char_blank
   cindx = len_trim(forcing_infile)
   forcing_filename(1:cindx) = forcing_infile(1:cindx)
   cindx = cindx + 1
   forcing_filename(cindx:cindx) = '.'
   cindx = cindx + 1
   forcing_filename(cindx:cindx+3) = file_cyear(2:5)
   cindx = cindx + 4
   forcing_filename(cindx:cindx) = '.'
   cindx = cindx + 1
   forcing_filename(cindx:cindx+2) = file_cday(2:4)
   cindx = cindx + 3
   forcing_filename(cindx:cindx) = '.'
   cindx = cindx + 1
   forcing_filename(cindx:cindx+1) = file_chour(2:3)

!-----------------------------------------------------------------------
!EOC

 end subroutine get_forcing_filename

!***********************************************************************
!BOP
! !IROUTINE: find_forcing_times
! !INTERFACE:

 subroutine find_forcing_times(forcing_time, forcing_data_inc,         &
                            forcing_interp_type, forcing_time_next,    &
                            forcing_time_min_loc, forcing_data_update, &
                            forcing_data_type)

! !DESCRIPTION:
!  Find the times associated with forcing data needed for
!  interpolation given the current time and time increment
!  between forcing data fields.  forcing times are assumed
!  to be centered;  for example, with daily forcing, the value
!  is assumed to occur at noon (hour = 12).
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (char_len), intent(in) :: &
      forcing_data_type,    &! frequency of forcing data
      forcing_interp_type    ! type of interpolation to be used

   real (r8), intent(in) :: &
      forcing_data_inc       ! time increment between forcing data

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(12), intent(inout) :: &
      forcing_time

! !OUTPUT PARAMETERS:

   integer (int_kind), intent(out) :: &
      forcing_time_min_loc  ! index to first location (in time)
                            ! of forcing data array

   real (r8), intent(out) :: &
      forcing_time_next,     &! next time for updating forcing
      forcing_data_update     ! next time to update forcing data

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      nstart, n,         &
      second, third, fourth

   real (r8) :: &
      forcing_time_test, &
      forcing_time_min

!-----------------------------------------------------------------------
!
!  set mid_month and end_month times for monthly options.
!
!-----------------------------------------------------------------------

   if (forcing_data_type == 'monthly-calendar') then
      thour00_midmonth = thour00_midmonth_calendar
      thour00_endmonth = thour00_endmonth_calendar
   else if (forcing_data_type == 'monthly-equal') then
      thour00_midmonth = thour00_midmonth_equal
      thour00_endmonth = thour00_endmonth_equal
   endif

!-----------------------------------------------------------------------
!
!  for 'monthly-equal' and 'monthly-calendar' set the forcing times to
!     be the mid-month times plus the time at the beginning of the year.
!     then find the first forcing time greater than the current time
!     and exit.  note that the loop will not exit if the current time
!     is in the second half of December in which case n will have the
!     value 13.
!  for 'n-hour' forcing, start at a time that is definitely less
!     than the first forcing time greater than the current time and
!     just increment the test time by the forcing increment until we
!     find a forcing time greater than the current time.
!
!-----------------------------------------------------------------------

   select case (forcing_data_type)

   case ('monthly-equal','monthly-calendar')

      forcing_time(1:12) = thour00_begin_this_year + &
                           thour00_midmonth(1:12)
      do n = 1,12
         if (forcing_time(n) > thour00) exit
      enddo

   case ('n-hour')

      nstart = int(thour00/forcing_data_inc) - 3
      do n = nstart, nstart + 6
         forcing_time_test = (n - p5)*forcing_data_inc
         if (forcing_time_test > thour00) exit
      enddo

   end select

!-----------------------------------------------------------------------
!
!  Set forcing times depending on the type of temporal interpolation.
!  Also set forcing_time_min_loc, which is the temporal index of the
!  minimum forcing time to be used for the interpolation on the forcing
!  data arrays.  for 'monthly' data, forcing_time_min_loc varies between
!  1 and 12.  For example, if a 4point interpolation is required and
!  the months that are needed are 3,4,5,6 then forcing_time_min_loc = 3.
!  For 'n-hour' data, forcing_time_min_loc = 1 always because
!  initially the data is read chronologically 1 file at a time.
!  Also set the times for the next interpolation and next time
!  that new data will be needed.
!
!-----------------------------------------------------------------------

   select case(forcing_interp_type)

   case ('nearest')  !  select nearest forcing time

      select case (forcing_data_type)

      case ('monthly-equal','monthly-calendar')

         do n = 1,12
            if (thour00 <= &
                thour00_begin_this_year + thour00_endmonth(n)) exit
         enddo
         if (n == 13) call exit_POP(sigAbort, &
            'Error finding current month in find_forcing_times')

         forcing_time_min_loc = n
         forcing_time_min_loc = mod(forcing_time_min_loc,12)
         if (forcing_time_min_loc <= 0) &
            forcing_time_min_loc = forcing_time_min_loc + 12
         if (n == 12) then
            forcing_time_next = forcing_time(1) + hours_in_year ! next Jan
         else
            forcing_time_next = forcing_time(forcing_time_min_loc + 1)
         endif
         forcing_data_update = thour00_begin_this_year + &
                               thour00_endmonth(forcing_time_min_loc)

      case ('n-hour')

         if ((forcing_time_test-thour00) <= p5*forcing_data_inc ) then
            forcing_time_min = (n - p5)*forcing_data_inc
         else
            forcing_time_min = (n - 1.5_r8)*forcing_data_inc
         endif
         forcing_time_min_loc = 1
         forcing_time(forcing_time_min_loc) = forcing_time_min
         forcing_time_next   = forcing_time(forcing_time_min_loc) + &
                               forcing_data_inc
         forcing_data_update = forcing_time(forcing_time_min_loc) + &
                               p5*forcing_data_inc

      end select

   case ('linear')  !  linear interpolation

      select case(forcing_data_type)

      case ('monthly-equal','monthly-calendar')
         forcing_time_min_loc = n - 1
         forcing_time_min_loc = mod(forcing_time_min_loc,12)
         if (forcing_time_min_loc <= 0) &
            forcing_time_min_loc = forcing_time_min_loc + 12
         second = mod(forcing_time_min_loc+1,12)
         if (second == 0) second = 12
         if (n == 1) then
            forcing_time(forcing_time_min_loc) = &
            forcing_time(forcing_time_min_loc) - hours_in_year ! previous Dec
         elseif (n >= 3) then
            forcing_time(1:n-2) = forcing_time(1:n-2) + hours_in_year
         endif
         if (n == 12) then
            forcing_time_next = forcing_time(1) ! next Jan
         else
            forcing_time_next = forcing_time(second+1) ! next month
         endif

      case ('n-hour')
         forcing_time_min_loc = 1
         second = 2
         forcing_time(forcing_time_min_loc) = (n - 1.5_r8)* &
                                              forcing_data_inc
         forcing_time(second) = forcing_time(forcing_time_min_loc) + &
                                forcing_data_inc
         forcing_time_next    = forcing_time(forcing_time_min_loc) + &
                                c2*forcing_data_inc

      end select

      forcing_data_update = forcing_time(second)

   case ('4point')  !  4-point interpolation

      select case(forcing_data_type)

      case ('monthly-equal','monthly-calendar')

         forcing_time_min_loc = n - 2
         forcing_time_min_loc = mod(forcing_time_min_loc,12)
         if (forcing_time_min_loc <= 0) &
            forcing_time_min_loc = forcing_time_min_loc + 12
         second = mod(forcing_time_min_loc+1,12)
         third  = mod(forcing_time_min_loc+2,12)
         fourth = mod(forcing_time_min_loc+3,12)
         if (second == 0) second = 12
         if (third  == 0) third  = 12
         if (fourth == 0) fourth = 12
         if (n == 1) then
            forcing_time(forcing_time_min_loc) = &
            forcing_time(forcing_time_min_loc) - hours_in_year ! previous Nov
            forcing_time(second)               = &
            forcing_time(second)               - hours_in_year ! previous Dec
         else if (n == 2) then
            forcing_time(forcing_time_min_loc) = &
            forcing_time(forcing_time_min_loc) - hours_in_year ! previous Dec
         else if (n >= 4) then
            forcing_time(1:n-3) = forcing_time(1:n-3) + hours_in_year
         endif
         if (n == 11) then
            forcing_time_next = forcing_time(1)        ! next Jan
         else
            forcing_time_next = forcing_time(fourth+1) ! next month
         endif

      case ('n-hour')

         forcing_time_min_loc = 1
         second = 2
         third  = 3
         fourth = 4
         forcing_time(forcing_time_min_loc) = (n - 2.5_r8)* &
                                              forcing_data_inc
         forcing_time(second) = forcing_time(forcing_time_min_loc) + &
                                forcing_data_inc
         forcing_time(third)  = forcing_time(forcing_time_min_loc) + &
                                c2*forcing_data_inc
         forcing_time(fourth) = forcing_time(forcing_time_min_loc) + &
                                c3*forcing_data_inc
         forcing_time_next    = forcing_time(forcing_time_min_loc) + &
                                c4*forcing_data_inc

      end select

      forcing_data_update = forcing_time(third)

   end select

!-----------------------------------------------------------------------
!EOC

 end subroutine find_forcing_times

!***********************************************************************
!BOC
! !IROUTINE: interpolate_forcing
! !INTERFACE:

   subroutine interpolate_forcing(INTERP, FIELD, forcing_time,         &
         forcing_interp_type, forcing_time_min_loc,                    &
         forcing_interp_freq, forcing_interp_inc, forcing_interp_next, &
         forcing_interp_last, nsteps_run_check)

! !DESCRIPTION:
!  Temporally interpolates field based on input interpolation options
!  and indices.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (char_len), intent(in) :: &
      forcing_interp_type,  &! type of interpolation to use
      forcing_interp_freq    ! frequency for interpolating forcing

   integer (int_kind), intent(in) :: &
      nsteps_run_check, &! check for handling first time step
      forcing_time_min_loc ! first location of data to interpolate

   real (r8), dimension(:,:,:,:,:), intent(in) :: &
      FIELD         !  forcing field to be interpolated

   real (r8), intent(in) :: &
      forcing_interp_inc,   &! forcing increment
      forcing_interp_next

   real (r8), dimension(12), intent(in) :: &
      forcing_time  ! times at which forcing located for interpolation

! !INPUT/OUTPUT PARAMETERS:

   real (r8), intent(inout) :: &
      forcing_interp_last      ! time when last interpolation done

! !OUTPUT PARAMETERS:

   real (r8), dimension(:,:,:,:), intent(out) :: &
     INTERP    !  result of interpolation

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      k,n,               &! loop indices
      iblock,            &! dummy block index
      second, third,     &! time indices for data to be interpolated
      fourth,            &!
      ntime               ! number of time slices in data array

   real (r8) :: &
      weight     ! linear interpolation weight

!-----------------------------------------------------------------------
!
!  if interpolation is done every timestep, then interpolate to
!     the current time.
!  if interpolation is done every n-hours, then interpolate to
!     the current interpolation time (which is >= current time)
!     or if it is the first step of the current sub-run, the value
!     of forcing_interp_last was read in from the restart file.
!
!-----------------------------------------------------------------------

   ntime = size(FIELD, dim=5)

   if (forcing_interp_freq == 'every-timestep') then
      forcing_interp_last = thour00
   else if (forcing_interp_freq == 'n-hour') then
      if (nsteps_run_check /= 0) forcing_interp_last = &
                                 forcing_interp_next
   else
      call exit_POP(sigAbort,'Error in interpolate_forcing')
   endif

   select case(forcing_interp_type)

   case ('nearest')

      INTERP =  FIELD(:,:,:,:,forcing_time_min_loc)

   case ('linear')

      second = mod(forcing_time_min_loc+1,ntime)
      if (second == 0) second = ntime

      weight = (forcing_time(second) - forcing_interp_last)/ &
               (forcing_time(second) - &
                forcing_time(forcing_time_min_loc))

      !*** 3d interior forcing and surface forcing have different
      !*** order of indices so must be treated differently

      if (size(FIELD,dim=3) == km) then ! 3d interior forcing

         !$OMP PARALLEL DO
         do iblock=1,nblocks_clinic
            INTERP(:,:,:,iblock) = &
                    weight *FIELD(:,:,:,iblock,forcing_time_min_loc) + &
              (c1 - weight)*FIELD(:,:,:,iblock,second)
         end do
         !$OMP END PARALLEL DO

      else  ! 2d surface forcing with 1 or more fields

         !$OMP PARALLEL DO PRIVATE(iblock, n)
         do iblock=1,nblocks_clinic
         do n=1,size(FIELD,dim=4)
            INTERP(:,:,iblock,n) = &
                   weight *FIELD(:,:,iblock,n,forcing_time_min_loc) + &
             (c1 - weight)*FIELD(:,:,iblock,n,second)
         end do
         end do
         !$OMP END PARALLEL DO

      endif

   case ('4point')

      second = mod(forcing_time_min_loc+1,ntime)
      third  = mod(forcing_time_min_loc+2,ntime)
      fourth = mod(forcing_time_min_loc+3,ntime)
      if (second == 0) second = ntime
      if (third  == 0) third  = ntime
      if (fourth == 0) fourth = ntime

      !*** 3d interior forcing and surface forcing have different
      !*** order of indices so must be treated differently

      if (size(FIELD,dim=3) == km) then ! 3d interior forcing

         !$OMP PARALLEL DO PRIVATE(iblock, k)
         do iblock=1,nblocks_clinic
         do k=1,km

            call interp_4pt(INTERP(:,:,k,iblock),               &
                      FIELD(:,:,k,iblock,forcing_time_min_loc), &
                      FIELD(:,:,k,iblock,second),               &
                      FIELD(:,:,k,iblock,third),                &
                      FIELD(:,:,k,iblock,fourth),               &
                      forcing_time(forcing_time_min_loc),       &
                      forcing_time(second),forcing_time(third), &
                      forcing_time(fourth),forcing_interp_last)

         end do
         end do
         !$OMP END PARALLEL DO

      else  ! 2d surface forcing with 1 or more fields

         !$OMP PARALLEL DO PRIVATE(iblock, n)
         do iblock=1,nblocks_clinic
         do n=1,size(FIELD,dim=4)
            call interp_4pt(INTERP(:,:,iblock,n),               &
                      FIELD(:,:,iblock,n,forcing_time_min_loc), &
                      FIELD(:,:,iblock,n,second),               &
                      FIELD(:,:,iblock,n,third),                &
                      FIELD(:,:,iblock,n,fourth),               &
                      forcing_time(forcing_time_min_loc),       &
                      forcing_time(second),forcing_time(third), &
                      forcing_time(fourth),forcing_interp_last)

         end do
         end do
         !$OMP END PARALLEL DO
      endif

   end select

!-----------------------------------------------------------------------
!EOC

 end subroutine interpolate_forcing

!***********************************************************************
!BOP
! !IROUTINE:
! !INTERFACE:

 subroutine update_forcing_data(forcing_time, forcing_time_min_loc,  &
                            forcing_interp_type, forcing_data_next,  &
                            forcing_data_update, forcing_data_type,  &
                            forcing_data_inc, FIELD,                 &
                            forcing_data_rescale,                    &
                            forcing_data_label, forcing_data_names,  &
                            forcing_bndy_loc, forcing_bndy_type,     &
                            forcing_infile, forcing_infile_fmt)

! !DESCRIPTION:
!  Updates the data to be used in the next temporal interpolation.
!  If data is monthly this is a shuffling of indices since
!  all 12 months are in memory.  if data is n-hour, new data
!  needs to be read in as well as shuffling of indices.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      forcing_infile,           &! file containing forcing data
      forcing_infile_fmt,       &! format (bin or netcdf) for input file
      forcing_data_type,        &! type of forcing data
      forcing_interp_type,      &! interpolation to use for forcing
      forcing_data_label         ! description of forcing

   character (char_len), dimension(:), intent(in) :: &
      forcing_data_names         ! short names for input forcing fields

   integer (int_kind), dimension(:), intent(in) :: &
      forcing_bndy_loc,         &! location and field type for ghost
      forcing_bndy_type          !    cell updates

   real (r8), dimension(20), intent(in) :: &
      forcing_data_rescale       ! factors for converting to model units

   real (r8), intent(in) :: &
      forcing_data_inc       ! time increment between forcing data

! !INPUT/OUTPUT PARAMETERS:

   integer (int_kind), intent(inout) :: &
      forcing_time_min_loc  ! first temporal index for interpolation

   real (r8), dimension(12), intent(inout) :: &
      forcing_time          !

   real (r8), intent(inout) :: &
      forcing_data_next,       &! time next data required
      forcing_data_update       ! time data to be updated

   real (r8), dimension(:,:,:,:,:), intent(inout) :: &
      FIELD   !  forcing field to be read if necessary

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) ::    &
      n,                    &! dummy index
      forcing_time_max_loc   ! time index for last forcing time

   character (char_len) :: &
      forcing_filename     ! name of forcing file

   type (datafile) :: &
      in_forcing_file  ! data file type for input forcing file

   type (io_field_desc) :: &
      forcing_field    ! io type for input forcing field

   type (io_dim) :: &
      i_dim, j_dim, &! dimension descriptors for horiz dims
      k_dim          ! dimension descriptor  for vertical levels

!-----------------------------------------------------------------------
!
!  determine data by forcing type
!
!-----------------------------------------------------------------------

   select case(forcing_data_type)

!-----------------------------------------------------------------------
!
!  for monthly forcing, all twelve months are stored - simply compute
!  proper indices into forcing data array
!
!-----------------------------------------------------------------------

   case ('monthly-equal','monthly-calendar')

      !*** compute first forcing time

      forcing_time(forcing_time_min_loc) = &
      forcing_time(forcing_time_min_loc) + hours_in_year
      forcing_time_min_loc = forcing_time_min_loc + 1
      if (forcing_time_min_loc > 12) forcing_time_min_loc = 1

      !*** compute times for other times for use in interpolation

      select case(forcing_interp_type)

      case ('nearest')
         forcing_time_max_loc = forcing_time_min_loc
         forcing_data_update  = forcing_data_update + &
                        24.0_r8*days_in_month(forcing_time_max_loc)

      case ('linear')
         forcing_time_max_loc = mod(forcing_time_min_loc + 1,12)
         if (forcing_time_max_loc == 0) forcing_time_max_loc = 12
         forcing_data_update = forcing_time(forcing_time_max_loc)

      case ('4point')
         forcing_time_max_loc = mod(forcing_time_min_loc + 3,12)
         if (forcing_time_max_loc == 0) forcing_time_max_loc = 12
         n = forcing_time_max_loc - 1
         if (n == 0) n = 12
         forcing_data_update = forcing_time(n)

      end select

      !*** compute next forcing time

      forcing_time(forcing_time_max_loc) = forcing_data_next
      n = mod(forcing_time_max_loc + 1, 12)
      if (n == 0) n = 12
      forcing_data_next = forcing_time(n)

!-----------------------------------------------------------------------
!
!  for n-hour option, read in required forcing data from forcing file
!
!-----------------------------------------------------------------------

   case ('n-hour')

      call get_forcing_filename(forcing_filename,  forcing_infile, &
                                forcing_data_next, forcing_data_inc)

      in_forcing_file = construct_file(forcing_infile_fmt,             &
                                    full_name=trim(forcing_filename),  &
                                    record_length=rec_type_dbl,        &
                                    recl_words=nx_global*ny_global)

      call data_set(in_forcing_file, 'open_read')

      i_dim = construct_io_dim('i',nx_global)
      j_dim = construct_io_dim('j',ny_global)

      if (size(FIELD,dim=3) == km) then  !  3d interior forcing field

         k_dim = construct_io_dim('k',km)

         forcing_field = construct_io_field(                     &
                         trim(forcing_data_names(1)),            &
                         i_dim, j_dim, dim3=k_dim,               &
                         field_loc = forcing_bndy_loc(1),        &
                         field_type = forcing_bndy_type(1),      &
                         d3d_array=FIELD(:,:,:,:,forcing_time_min_loc))

         call data_set (in_forcing_file, 'define', forcing_field)
         call data_set (in_forcing_file, 'read',   forcing_field)
         call destroy_io_field(forcing_field)

         !*** renormalize if necessary to compensate for different
         !*** units.

         if (forcing_data_rescale(1) /= c1)     &
            FIELD(:,:,:,:,forcing_time_min_loc) = &
            FIELD(:,:,:,:,forcing_time_min_loc)*forcing_data_rescale(1)

      else  ! forcing field(s) for surface forcing

         do n = 1,size(FIELD,dim=4)

            forcing_field = construct_io_field(                  &
                         trim(forcing_data_names(n)),            &
                         i_dim, j_dim,                           &
                         field_loc = forcing_bndy_loc(n),        &
                         field_type = forcing_bndy_type(n),      &
                         d2d_array=FIELD(:,:,:,n,forcing_time_min_loc))

            call data_set (in_forcing_file, 'define', forcing_field)
            call data_set (in_forcing_file, 'read',   forcing_field)
            call destroy_io_field(forcing_field)

            !*** renormalize if necessary to compensate for different
            !*** units.

            if (forcing_data_rescale(n) /= c1)     &
               FIELD(:,:,:,n,forcing_time_min_loc) = &
               FIELD(:,:,:,n,forcing_time_min_loc)*forcing_data_rescale(n)

         enddo
      endif

      call data_set (in_forcing_file, 'close')
      call destroy_file(in_forcing_file)

      if (my_task == master_task) then
         write(stdout,blank_fmt)
         write(stdout,'(a9,f12.3,1x,a,a12,a)') 'tday00 = ', tday00, &
            trim(forcing_data_label),' file read: ',                &
            trim(forcing_filename)
      endif

      forcing_time(forcing_time_min_loc) = forcing_data_next

      forcing_time_min_loc = forcing_time_min_loc + 1
      if (forcing_time_min_loc > size(FIELD,dim=5)) &
          forcing_time_min_loc = 1
      forcing_data_next = forcing_data_next + forcing_data_inc
      forcing_data_update = forcing_data_update + forcing_data_inc

!-----------------------------------------------------------------------

   end select

!-----------------------------------------------------------------------
!EOC

 end subroutine update_forcing_data

!***********************************************************************
!BOP
! !IROUTINE: echo_forcing_options
! !INTERFACE:

 subroutine echo_forcing_options(forcing_data_type,   &
                                 forcing_formulation, &
                                 forcing_data_inc,    &
                                 forcing_interp_freq, &
                                 forcing_interp_type, &
                                 forcing_interp_inc,  &
                                 forcing_label)

! !DESCRIPTION:
!  Writes out forcing options to stdout (or log file).
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), intent(in) :: &
      forcing_data_inc,     &
      forcing_interp_inc

   character (char_len), intent(in) :: &
      forcing_data_type,    &
      forcing_interp_freq,  &
      forcing_interp_type,  &
      forcing_label,        &
      forcing_formulation

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  echo forcing selections to stdout. only write from master task.
!
!-----------------------------------------------------------------------

   if (my_task == master_task) then

!-----------------------------------------------------------------------
!
!     no forcing
!
!-----------------------------------------------------------------------

      if (forcing_data_type == 'none') then

         write(stdout,'(a3,a)') 'No ', trim(forcing_label)

!-----------------------------------------------------------------------
!
!     if there is some kind of forcing, write out the type (eg, annual)
!       and the formulation (eg, restoring).
!     then write increment between forcing data if appropriate.
!
!-----------------------------------------------------------------------

      else

         if (len_trim(forcing_formulation) == 0) then
            ! Work around current Linux runtime error
            write(stdout,'(a,a1,a)') trim(forcing_label), ':', &
                                     trim(forcing_data_type)
         else
            write(stdout,'(a,a1,a,a1,a)') trim(forcing_label),    ':', &
                                          trim(forcing_data_type),' ', &
                                          trim(forcing_formulation)
         end if

         if (forcing_data_type == 'n-hour') then
            write(stdout,'(a,a29,f7.3)') trim(forcing_label),      &
                                  ' Data Time Increment (days): ', &
                                  forcing_data_inc/24.0_r8
         endif

!-----------------------------------------------------------------------
!
!        write the frequency that the forcing is updated.
!
!-----------------------------------------------------------------------

         select case(forcing_interp_freq)

         case ('never')
            write(stdout,'(a,a27)') trim(forcing_label), &
                                    ' data is never interpolated'

         case ('n-hour')
            write(stdout,'(a,a25,f7.3,a5)') trim(forcing_label), &
                        ' data interpolated every ',             &
                        forcing_interp_inc/24.0_r8,' days'

         case ('every-timestep')
            write(stdout,'(a,a33)') trim(forcing_label), &
                                    ' data interpolated every timestep'

         end select

!-----------------------------------------------------------------------
!
!        write order of interpolation if appropriate.
!
!-----------------------------------------------------------------------

         if (forcing_data_type /= 'none'     .and. &
             forcing_data_type /= 'analytic' .and. &
             forcing_data_type /= 'annual')        &
            write(stdout,'(a,a21,a)') trim(forcing_label), &
              ' Interpolation type: ',trim(forcing_interp_type)

      endif ! forcing type /= none

      write(stdout,blank_fmt)

!-----------------------------------------------------------------------
!
!  end of output
!
!-----------------------------------------------------------------------

   endif ! master task

!-----------------------------------------------------------------------
!EOC

 end subroutine echo_forcing_options

!***********************************************************************
!BOP
! !IROUTINE: interp_4pt
! !INTERFACE:

 subroutine interp_4pt(INTERP,  DATA1,  DATA2,  DATA3, DATA4, &
                       tdata1, tdata2, tdata3, tdata4, time)

! !DESCRIPTION:
!  Interpolates data at four consecutive (in time) points to a
!  particular time using a 3rd degree polynomial fit.  This is an
!  implementation of Nevilles algorithm as described in Numerical
!  Recipes.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(in) :: &
      DATA1, DATA2, DATA3, DATA4   ! data for each grid point in
                                   ! a block for 4 consecutive
                                   ! instants in time

   real (r8), intent(in) :: &
      time,              &! time for which interpolated data required
      tdata1, tdata2,    &! time points at which above data is located
      tdata3, tdata4

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(out) :: &
      INTERP              ! data interpolated to input time

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   real (r8), dimension(nx_block,ny_block) :: &
      WORK1,WORK2,WORK3,WORK4  ! local work space

!-----------------------------------------------------------------------

   WORK1 = c0
   WORK2 = c0
   WORK3 = c0
   WORK4 = c0
   call det(WORK1,DATA1,DATA2,tdata1,tdata2,time) ! P12
   call det(WORK2,DATA2,DATA3,tdata2,tdata3,time) ! P23
   call det(WORK3,DATA3,DATA4,tdata3,tdata4,time) ! P34

   call det(WORK4,WORK1,WORK2,tdata1,tdata3,time) ! P123
   call det(WORK1,WORK2,WORK3,tdata2,tdata4,time) ! P234

   call det(INTERP,WORK4,WORK1,tdata1,tdata4,time) ! P1234

!-----------------------------------------------------------------------
!EOC

 end subroutine interp_4pt

!***********************************************************************
!BOP
! !IROUTINE: det
! !INTERFACE:

 subroutine det(C,A,B,y,z,x)

! !DESCRIPTION:
!  Determinant routine used by Nevilles algorithm in the 4-point
!  time interpolation.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), intent(in) :: &
      y,z,x

   real (r8), dimension(nx_block,ny_block), intent(in) :: &
      A,B

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), intent(out) :: &
      C       ! output determinant

!-----------------------------------------------------------------------

   C = ( A*(z-x) - B*(y-x) )/(z-y)

!-----------------------------------------------------------------------
!EOC

 end subroutine det

!***********************************************************************

 end module forcing_tools

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
