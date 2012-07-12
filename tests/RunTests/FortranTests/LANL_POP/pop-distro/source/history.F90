!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module history

!BOP
! !MODULE: history
! !DESCRIPTION:
!  This module contains fields and routines necessary for writing 
!  snapshot history file output.
!
! !REVISION HISTORY:
!  CVS:$Id: history.F90,v 1.16 2003/12/23 22:11:40 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $
!
! !USES:

   use kinds_mod
   use domain
   use constants
   use prognostic
   use grid
   use io
   use broadcast
   use time_management
   use forcing
   use forcing_shf
   use exit_mod

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_history, &
             write_history

! !PUBLIC DATA MEMBERS:

   logical (log_kind), public :: &
      lhist_on      = .false.    ! hist file output wanted

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  hist field descriptor data type and array of such types
!
!-----------------------------------------------------------------------

   type :: hist_field_desc
      character(char_len)     :: short_name     ! short name for field
      character(char_len)     :: long_name      ! long descriptive name
      character(char_len)     :: units          ! units
      character(4)            :: grid_loc       ! location in grid
      real (r4)               :: missing_value  ! value on land pts
      real (r4), dimension(2) :: valid_range    ! min/max
      integer (i4)            :: ndims          ! num dims (2 or 3)
      logical (log_kind)      :: requested      ! true if requested
   end type

   integer (int_kind), parameter :: &
      max_avail_hist_fields = 50     ! limit on available fields - can
                                     !   be pushed as high as necessary

   integer (int_kind) ::           &
      num_avail_hist_fields = 0,   &! current number of defined fields
      num_requested_hist_fields     ! number of fields requested

   type (hist_field_desc), dimension(max_avail_hist_fields) :: &
      avail_hist_fields

!-----------------------------------------------------------------------
!
!  other module variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      history_flag,      &  ! time flag for writing history files
      history_freq_iopt, &  ! frequency option for writing history
      history_freq          ! frequency of history output

   logical (log_kind) :: &
      lhistory_on  ! history file output wanted

   character (char_len) :: &
      history_outfile,     &! root filename for history output
      history_fmt           ! format (nc or bin) for writing

   !***
   !*** available history fields
   !***

   integer (int_kind) ::   &! history field ids
      hist_id_shgt,        &! id for surface height
      hist_id_suf,         &! id for surface momentum flux in U dir
      hist_id_svf,         &! id for surface momentum flux in V dir
      hist_id_shf,         &! id for surface heat flux
      hist_id_sfwf,        &! id for surface fresh water flux
      hist_id_solar,       &! id for short-wave solar flux
      hist_id_uvel,        &! id for U velocity
      hist_id_vvel,        &! id for V Velocity
      hist_id_temp,        &! id for potential temperature
      hist_id_salt,        &! id for salinity
      hist_id_ubtrop,      &! id for barotropic U velocity
      hist_id_vbtrop        ! id for barotropic V velocity

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: write_history
! !INTERFACE:

 subroutine write_history

! !DESCRIPTION:
!  This routine writes snapshots of requested fields to a file.
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

   type (datafile) ::      &
      hist_file_desc        ! I/O file descriptor

   character (char_len) :: &
      file_suffix,         &! suffix to append to root filename 
      hist_string           ! string defining history of file

   logical (log_kind) :: &
      lhistory_write     ! true if time to write a file

   character (8) :: &
      date_created   ! string with (real) date this file created

   character (10) :: &
      time_created   ! string with (real) date this file created

   type (io_field_desc), dimension(:), allocatable :: &
      hist_fields

   type (io_dim) :: &
      i_dim, j_dim, &! dimension descriptors for horiz dims
      k_dim          ! dimension descriptor  for vertical levels

   real (r4), dimension(nx_block,ny_block,max_blocks_clinic) :: &
      WORK2D        ! temp for holding 2d fields

   real (r4), dimension(nx_block,ny_block,km,max_blocks_clinic) :: &
      WORK3D        ! temp for holding 3d fields

   integer (int_kind) :: &
      nfield,            &! dummy field index
      iblock              ! dummy block index

!-----------------------------------------------------------------------
!
!  check to see whether it is time to write files
!
!-----------------------------------------------------------------------

   lhistory_write = .false.
   if (lhistory_on) then
      lhistory_write = check_time_flag(history_flag)
   endif

!-----------------------------------------------------------------------
!
!  write history files if it is time
!
!-----------------------------------------------------------------------

   if (lhistory_write) then

      !*** create file suffix

      file_suffix = char_blank
      call create_suffix_hist(file_suffix)

!-----------------------------------------------------------------------
!
!     create data file descriptor
!
!-----------------------------------------------------------------------

      call date_and_time(date=date_created, time=time_created)
      hist_string = char_blank
      write(hist_string,'(a23,a8,1x,a10)') & 
         'POP HIST file created: ',date_created,time_created

      hist_file_desc = construct_file(history_fmt,                     &
                                   root_name  = trim(history_outfile), &
                                   file_suffix= trim(file_suffix),     &
                                   title      ='POP HIST file',        &
                                   conventions='POP HIST conventions', &
                                   history    = trim(hist_string),     &
                                   record_length = rec_type_real,        &
                                    recl_words=nx_global*ny_global)

!-----------------------------------------------------------------------
!
!     add scalar fields to file as file attributes
!
!-----------------------------------------------------------------------

      call add_attrib_file(hist_file_desc, 'nsteps_total', nsteps_total)
      call add_attrib_file(hist_file_desc, 'tday'        , tday)
      call add_attrib_file(hist_file_desc, 'iyear'       , iyear)
      call add_attrib_file(hist_file_desc, 'imonth'      , imonth)
      call add_attrib_file(hist_file_desc, 'iday'        , iday)

!-----------------------------------------------------------------------
!
!     open output file and define dimensions
!
!-----------------------------------------------------------------------

      call data_set (hist_file_desc, 'open')

      i_dim = construct_io_dim('i',nx_global)
      j_dim = construct_io_dim('j',ny_global)
      k_dim = construct_io_dim('k',km)

!-----------------------------------------------------------------------
!
!     write fields to file - this requires two phases
!     in this first phase, we define all the fields to be written
!
!-----------------------------------------------------------------------
 
      allocate(hist_fields(num_avail_hist_fields))

      do nfield = 1,num_avail_hist_fields  ! check all available fields

         if (avail_hist_fields(nfield)%requested) then 

            !*** construct io_field descriptors for each field

            if (avail_hist_fields(nfield)%ndims == 2) then

               hist_fields(nfield) = construct_io_field(               &
                              avail_hist_fields(nfield)%short_name,    &
                              i_dim, j_dim,                            &
                    long_name=avail_hist_fields(nfield)%long_name,     &
                    units    =avail_hist_fields(nfield)%units    ,     &
                    grid_loc =avail_hist_fields(nfield)%grid_loc ,     &
                missing_value=avail_hist_fields(nfield)%missing_value, &
                  valid_range=avail_hist_fields(nfield)%valid_range,   &
                   r2d_array =WORK2D)

            else if (avail_hist_fields(nfield)%ndims == 3) then

               hist_fields(nfield) = construct_io_field(               &
                              avail_hist_fields(nfield)%short_name,    &
                              i_dim, j_dim, dim3=k_dim,                &
                    long_name=avail_hist_fields(nfield)%long_name,     &
                    units    =avail_hist_fields(nfield)%units    ,     &
                    grid_loc =avail_hist_fields(nfield)%grid_loc ,     &
                missing_value=avail_hist_fields(nfield)%missing_value, &
                  valid_range=avail_hist_fields(nfield)%valid_range,   &
                   r3d_array =WORK3D)

            endif

            call data_set (hist_file_desc,'define',hist_fields(nfield))
         endif
      end do

!-----------------------------------------------------------------------
!
!     write fields to file 
!     in this second phase, we actually write the data
!
!-----------------------------------------------------------------------
 
      do nfield = 1,num_avail_hist_fields  ! check all available fields

         if (avail_hist_fields(nfield)%requested) then 

            !$OMP PARALLEL DO
            do iblock=1,nblocks_clinic
               if (nfield == hist_id_shgt) then
                  WORK2D(:,:,iblock) = PSURF(:,:,curtime,iblock)/grav
               else if (nfield == hist_id_suf) then
                  WORK2D(:,:,iblock) = SMF(:,:,1,iblock)
               else if (nfield == hist_id_svf) then
                  WORK2D(:,:,iblock) = SMF(:,:,2,iblock)
               else if (nfield == hist_id_shf) then
                  !*** convert to W/m2
                  WORK2D(:,:,iblock) = STF(:,:,1,iblock)/hflux_factor
               else if (nfield == hist_id_sfwf) then
                  !*** convert to m/year
                  WORK2D(:,:,iblock) = STF(:,:,2,iblock)* &
                                seconds_in_year/c1000/salinity_factor
               else if (nfield == hist_id_solar) then
                  !*** convert to W/m2
                  WORK2D(:,:,iblock) = SHF_QSW(:,:,iblock)/hflux_factor
               else if (nfield == hist_id_uvel) then
                  WORK3D(:,:,:,iblock) = UVEL(:,:,:,curtime,iblock) 
               else if (nfield == hist_id_vvel) then
                  WORK3D(:,:,:,iblock) = VVEL(:,:,:,curtime,iblock) 
               else if (nfield == hist_id_temp) then
                  WORK3D(:,:,:,iblock) = TRACER(:,:,:,1,curtime,iblock) 
               else if (nfield == hist_id_salt) then
                  WORK3D(:,:,:,iblock) = TRACER(:,:,:,2,curtime,iblock) 
               else if (nfield == hist_id_ubtrop) then
                  WORK2D(:,:,iblock) = UBTROP(:,:,curtime,iblock)
               else if (nfield == hist_id_vbtrop) then
                  WORK2D(:,:,iblock) = VBTROP(:,:,curtime,iblock)
               endif
            end do !block loop
            !$OMP END PARALLEL DO

            call data_set (hist_file_desc,'write',hist_fields(nfield))
            call destroy_io_field(hist_fields(nfield))
         endif
      end do

      deallocate(hist_fields)
      call data_set (hist_file_desc, 'close')

      if (my_task == master_task) then
         write(stdout,blank_fmt)
         write(stdout,*) 'Wrote file: ', trim(hist_file_desc%full_name)
      endif

!-----------------------------------------------------------------------
!
!     get rid of file descriptor
!
!-----------------------------------------------------------------------

      call destroy_file(hist_file_desc)

   endif ! time to do history file

!-----------------------------------------------------------------------
!EOC

 end subroutine write_history

!***********************************************************************
!BOP
! !IROUTINE: init_history
! !INTERFACE:

 subroutine init_history

! !DESCRIPTION:
!  Initializes history output choices from input files.
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
      nu,                &! unit for contents input file
      nfield,            &! dummy for field number
      n,                 &! dummy for position in character string
      nml_error           ! error flag for namelist i/o

   character (char_len) :: &
      history_freq_opt,    &! choice for frequency of history output
      history_contents,    &! filename for choosing fields for output
      char_temp             ! temporary for manipulating fields

   namelist /history_nml/ history_freq_opt, history_freq, &
                          history_outfile, history_contents, history_fmt

!-----------------------------------------------------------------------
!
!  read history file output frequency and filenames from namelist
!
!-----------------------------------------------------------------------

   history_freq_iopt = freq_opt_never
   history_freq      = 100000
   history_outfile   = 'h'
   history_contents  = 'unknown_history_contents'
   history_fmt       = 'bin'

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=history_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading history_nml')
   endif

   if (my_task == master_task) then
      write(stdout,delim_fmt)
      write(stdout,blank_fmt)
      write(stdout,'(a23)') ' History output options'
      write(stdout,blank_fmt)
      write(stdout,delim_fmt)

      select case (trim(history_freq_opt))
      case ('never')
         history_freq_iopt = freq_opt_never
         write(stdout,'(a23)') 'History output disabled'
      case ('nyear')
         history_freq_iopt = freq_opt_nyear
         write(stdout,'(a21,i6,a8)') 'History output every ', &
                                     history_freq, ' year(s)'
      case ('nmonth')
         history_freq_iopt = freq_opt_nmonth
         write(stdout,'(a21,i6,a9)') 'History output every ', &
                                     history_freq, ' month(s)'
      case ('nday')
         history_freq_iopt = freq_opt_nday
         write(stdout,'(a21,i6,a7)') 'History output every ', &
                                     history_freq, ' day(s)'
      case ('nhour')
         history_freq_iopt = freq_opt_nhour
         write(stdout,'(a21,i6,a8)') 'History output every ', &
                                     history_freq, ' hour(s)'
      case ('nsecond')
         history_freq_iopt = freq_opt_nsecond
         write(stdout,'(a21,i6,a8)') 'History output every ', &
                                     history_freq, ' seconds'
      case ('nstep')
         history_freq_iopt = freq_opt_nstep
         write(stdout,'(a21,i6,a6)') 'History output every ', &
                                     history_freq, ' steps'
      case default
         history_freq_iopt = -1000
      end select
   endif

   call broadcast_scalar(history_freq_iopt, master_task)
   call broadcast_scalar(history_freq,      master_task)

   if (history_freq_iopt == -1000) then
      call exit_POP(sigAbort, &
         'ERROR: unknown option for history file freq')
   else if (history_freq_iopt == freq_opt_never) then
      lhistory_on = .false.
   else
      lhistory_on = .true.
   endif

   if (lhistory_on) then
      call broadcast_scalar(history_outfile,  master_task)
      call broadcast_scalar(history_contents, master_task)
      call broadcast_scalar(history_fmt,      master_task)
      if (my_task == master_task) write(stdout,'(a24,a)') &
         ' History output format: ',trim(history_fmt)
   endif

   history_flag = init_time_flag('history',default=.false.,    &
                                 freq_opt = history_freq_iopt, &
                                 freq     = history_freq)

!-----------------------------------------------------------------------
!
!  if history output turned on, define available history fields
!
!-----------------------------------------------------------------------

   if (lhistory_on) then
      call define_hist_field(hist_id_shgt, 'SHGT', 2,          &
                             long_name = 'Sea surface height', &
                             units     = 'cm',                 &
                             grid_loc  = '2110',               &
                             missing_value = undefined)

      call define_hist_field(hist_id_suf, 'SUF', 2,                 &
                             long_name = 'Surface U velocity flux', &
                             units     = 'cm2/s2',                  &
                             grid_loc  = '2220',                    &
                             missing_value = undefined)

      call define_hist_field(hist_id_svf, 'SVF', 2,                 &
                             long_name = 'Surface V velocity flux', &
                             units     = 'cm2/s2',                  &
                             grid_loc  = '2220',                    &
                             missing_value = undefined)

      call define_hist_field(hist_id_shf, 'SHF', 2,           &
                             long_name = 'Surface heat flux', &
                             units     = 'W/cm2',             &
                             grid_loc  = '2110',              &
                             missing_value = undefined)

      call define_hist_field(hist_id_sfwf, 'SFWF', 2,                &
                             long_name = 'Surface fresh water flux', &
                             units     = 'm/yr',                     &
                             grid_loc  = '2110',                     &
                             missing_value = undefined)

      call define_hist_field(hist_id_solar, 'SOLAR', 2,             &
                             long_name = 'Surface solar heat flux', &
                             units     = 'W/cm2',                   &
                             grid_loc  = '2110',                    &
                             missing_value = undefined)

      call define_hist_field(hist_id_uvel, 'UVEL', 3,   &
                             long_name = 'U velocity',  &
                             units     = 'cm/s',       &
                             grid_loc  = '3221',        &
                             missing_value = undefined)

      call define_hist_field(hist_id_vvel, 'VVEL', 3,   &
                             long_name = 'V velocity',  &
                             units     = 'cm/s',       &
                             grid_loc  = '3221',        &
                             missing_value = undefined)

      call define_hist_field(hist_id_temp, 'TEMP', 3,             &
                             long_name = 'Potential temperature', &
                             units     = 'deg C',                 &
                             grid_loc  = '3111',                  &
                             missing_value = undefined)

      call define_hist_field(hist_id_salt, 'SALT', 3,  &
                             long_name = 'Salinity',   &
                             units     = 'g/g',        &
                             grid_loc  = '3111',       &
                             missing_value = undefined)

      call define_hist_field(hist_id_ubtrop, 'UBTROP', 2,         &
                             long_name = 'barotropic U velocity', &
                             units     = 'cm/s',                  &
                             grid_loc  = '2220',                  &
                             missing_value = undefined)

      call define_hist_field(hist_id_vbtrop, 'VBTROP', 2,         &
                             long_name = 'barotropic V velocity', &
                             units     = 'cm/s',                  &
                             grid_loc  = '2220',                  &
                             missing_value = undefined)

!-----------------------------------------------------------------------
!
!     read contents file to determine which fields to dump
!
!-----------------------------------------------------------------------

      call get_unit(nu)

      if (my_task == master_task) then
         open(nu, file=history_contents, status='old')
         read(nu,*) num_requested_hist_fields
      endif

      call broadcast_scalar(num_requested_hist_fields, master_task)

      do nfield=1,num_requested_hist_fields
         if (my_task == master_task) then
            read(nu,'(a80)') char_temp
         endif
         call broadcast_scalar(char_temp, master_task)

         char_temp = adjustl(char_temp)
         n = index(char_temp,' ') - 1
   
         call request_hist_field(char_temp(1:n))
      end do

      close(nu)
      call release_unit(nu)

   endif ! lhist_on

!-----------------------------------------------------------------------
!EOC

 end subroutine init_history

!***********************************************************************
!BOP
! !IROUTINE: define_hist_field
! !INTERFACE:

 subroutine define_hist_field(id, short_name, ndims, long_name, units, &
                              grid_loc, missing_value, valid_range)

! !DESCRIPTION:
!  Initializes description of an available field and returns location
!  in the available fields array for use in later hist calls.
!
! !REVISION HISTORY:
!  same as module

! !OUTPUT PARAMETERS:

   integer (int_kind), intent(out) :: &
      id                ! location in avail_fields array for use in
                        ! later hist routines

! !INPUT PARAMETERS:

   character(*), intent(in) :: &
      short_name               ! short name for field

   integer (i4), intent(in) :: &
      ndims                    ! number of dims (2 or 3) of hist field

   character(*), intent(in), optional :: &
      long_name,              &! long descriptive name for field
      units                    ! physical units for field

   character(4), intent(in), optional :: &
      grid_loc                 ! location in grid (in 4-digit code)

   real (r4), intent(in), optional :: &
      missing_value            ! value on land pts

   real (r4), dimension(2), intent(in), optional :: &
      valid_range              ! min/max

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  increment the number of defined fields and make sure it does not
!  exceed the maximum
!  return the id as the current number
!
!-----------------------------------------------------------------------

   num_avail_hist_fields = num_avail_hist_fields + 1
   if (num_avail_hist_fields > max_avail_hist_fields) then
      call exit_POP(sigAbort,'hist: defined hist fields > max allowed')
   endif
 
   id = num_avail_hist_fields

!-----------------------------------------------------------------------
!
!  now fill the field descriptor
!
!-----------------------------------------------------------------------

   avail_hist_fields(id)%ndims      = ndims
   avail_hist_fields(id)%short_name = short_name
   avail_hist_fields(id)%requested  = .false.

   if (present(long_name)) then
      avail_hist_fields(id)%long_name = long_name
   else
      avail_hist_fields(id)%long_name = char_blank
   endif

   if (present(units)) then
      avail_hist_fields(id)%units = units
   else
      avail_hist_fields(id)%units = char_blank
   endif

   if (present(grid_loc)) then
      avail_hist_fields(id)%grid_loc = grid_loc
   else
      avail_hist_fields(id)%grid_loc = '    '
   endif

   if (present(missing_value)) then
      avail_hist_fields(id)%missing_value = missing_value
   else
      avail_hist_fields(id)%missing_value = undefined
   endif

   if (present(valid_range)) then
      avail_hist_fields(id)%valid_range = valid_range
   else
      avail_hist_fields(id)%valid_range = undefined
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine define_hist_field

!***********************************************************************
!BOP
! !IROUTINE: request_hist_field
! !INTERFACE:

 subroutine request_hist_field(short_name)

! !DESCRIPTION:
!  This field marks an available field as requested and computes
!  the location in the hist buffer array.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      short_name                ! the short name of the field

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      n,                 &! loop index
      id                  ! location of field in avail_fields array

!-----------------------------------------------------------------------
!
!  search for field with same name
!
!-----------------------------------------------------------------------

   id = 0
   srch_loop: do n=1,num_avail_hist_fields
      if (trim(avail_hist_fields(n)%short_name) == short_name) then
         id = n
         exit srch_loop
      endif
   end do srch_loop

   if (id == 0) then
      if (my_task == master_task) &
         write(stdout,*) 'Requested hist field: ', trim(short_name)
      call exit_POP(sigAbort,'hist: requested field unknown')
   endif

!-----------------------------------------------------------------------
!
!  mark the field as requested
!
!-----------------------------------------------------------------------

   avail_hist_fields(id)%requested = .true.

!-----------------------------------------------------------------------
!EOC

 end subroutine request_hist_field

!***********************************************************************
!BOP
! !IROUTINE: hist_requested
! !INTERFACE:

 function hist_requested(id)

! !DESCRIPTION:
!  This function determines whether an available (defined) hist field
!  has been requested by a user (through the input contents file) and 
!  returns true if it has.  Note that if hist has been turned off, 
!  the function will always return false.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
      id                   ! id returned by the define function which
                           !   gives the location of the field

! !OUTPUT PARAMETERS:

   logical (log_kind) :: &
      hist_requested     ! result of checking whether the field has
                         !   been requested

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  check the buffer location - if zero, the field has not been
!  requested
!
!-----------------------------------------------------------------------

   if (id < 1 .or. id > num_avail_hist_fields) then
      call exit_POP(sigAbort,'hist_requested: invalid hist id')
   endif

   hist_requested = avail_hist_fields(id)%requested

!-----------------------------------------------------------------------
!EOC

 end function hist_requested

!***********************************************************************
!BOP
! !IROUTINE: create_suffix_hist
! !INTERFACE:

 subroutine create_suffix_hist(file_suffix)

! !DESCRIPTION:
!  Creates suffix to append to history file name based on output
!  frequency option.
!
! !REVISION HISTORY:
!  same as module

! !OUTPUT PARAMETERS:

   character (*), intent(out) :: &
      file_suffix                ! suffix to append to root filename

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) ::  &
      cindx1, cindx2      ! indices into character strings

   character (char_len) :: &
      char_temp            ! temp character space (for removing spaces)

   character (10) :: &
      cdate          ! character string to hold date

!-----------------------------------------------------------------------
!
!  start by putting runid in suffix
!
!-----------------------------------------------------------------------

   cindx2 = len_trim(runid) + 1
   file_suffix(1:cindx2) = trim(runid)/&
                                       &/'.'
   cindx1 = cindx2 + 1

!-----------------------------------------------------------------------
!
!  determine time portion of suffix from frequency option
!
!-----------------------------------------------------------------------

   cdate = '          '
   call time_stamp('now', date_string = cdate)

   select case (history_freq_iopt)
   case (freq_opt_nyear, freq_opt_nmonth, freq_opt_nday)
      !*** use date as time suffix

      cindx2 = cindx1 + len_trim(cdate)
      file_suffix(cindx1:cindx2) = trim(cdate)

   case (freq_opt_nhour)
      cindx2 = cindx1 + len_trim(cdate)
      file_suffix(cindx1:cindx2) = trim(cdate)
      cindx1 = cindx2 + 1
      cindx2 = cindx1 + 1
      write (file_suffix(cindx1:cindx2),'(a1,i2)') ':',ihour

   case (freq_opt_nsecond)
      char_temp = char_blank
      write (char_temp,'(i10)') nint(tsecond)
      char_temp = adjustl(char_temp)
      cindx2 = len_trim(char_temp) + cindx1
      file_suffix(cindx1:cindx2) = trim(char_temp)

   case (freq_opt_nstep)
      char_temp = char_blank
      write (char_temp,'(i10)') nsteps_total
      char_temp = adjustl(char_temp)
      cindx2 = len_trim(char_temp) + cindx1
      file_suffix(cindx1:cindx2) = trim(char_temp)

   case default
   end select
 
!-----------------------------------------------------------------------
!EOC

 end subroutine create_suffix_hist

!***********************************************************************

 end module history

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
