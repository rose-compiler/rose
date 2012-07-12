!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module movie

!BOP
! !MODULE: movie
! !DESCRIPTION:
!  This module contains fields and routines necessary for writing 
!  movie file output.
!
! !REVISION HISTORY:
!  CVS:$Id: movie.F90,v 1.18 2003/12/23 22:11:40 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $
!
! !USES:

   use kinds_mod
   use blocks
   use domain
   use constants
   use prognostic
   use grid
   use io
   use broadcast
   use time_management
   use forcing
   use exit_mod
   use operators

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_movie, &
             write_movie

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  movie field descriptor data type and array of such types
!
!-----------------------------------------------------------------------

   type :: movie_field_desc
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
      max_avail_movie_fields = 50    ! limit on available fields - can
                                     !   be pushed as high as necessary

   integer (int_kind) ::           &
      num_avail_movie_fields = 0,  &! current number of defined fields
      num_requested_movie_fields    ! number of fields requested

   type (movie_field_desc), dimension(max_avail_movie_fields) :: &
      avail_movie_fields

!-----------------------------------------------------------------------
!
!  other module variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      movie_flag,        &! time flag for writing movie files
      movie_freq_iopt,   &! frequency option for writing movie
      movie_freq          ! frequency of movie output

   logical (log_kind) :: &
      lmovie_on           ! movie file output wanted

   character (char_len) :: &
      movie_outfile,       &! root filename for movie output
      movie_fmt             ! format (nc or bin) for writing

   !***
   !*** available movie fields
   !***

   integer (int_kind) ::   &! movie field ids
      movie_id_shgt,       &! id for surface height
      movie_id_utrans,     &! id for surface momentum flux in U dir
      movie_id_vtrans,     &! id for surface momentum flux in V dir
      movie_id_temp1_2,    &! id for surface heat flux
      movie_id_salt1_2,    &! id for surface fresh water flux
      movie_id_temp6,      &! id for potential temperature
      movie_id_salt6,      &! id for salinity
      movie_id_vort         ! id for barotropic V velocity

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: write_movie
! !INTERFACE:

 subroutine write_movie

! !DESCRIPTION:
!  This routine writes requested fields to a file for making
!  movies.  It is very much like history file output, but is
!  supplied for more frequent output of a smaller amount of data.
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

   type (block) :: &
      this_block   ! block info for local block

   type (datafile) ::      &
      movie_file_desc        ! I/O file descriptor

   character (char_len) :: &
      file_suffix,         &! suffix to append to root filename 
      hist_string           ! string defining history of file

   logical (log_kind) :: &
      lmovie_write     ! true if time to write a file

   character (8) :: &
      date_created   ! string with (real) date this file created

   character (10) :: &
      time_created   ! string with (real) date this file created

   type (io_field_desc), dimension(:), allocatable :: &
      movie_fields

   type (io_dim) :: &
      i_dim, j_dim, k_dim  ! dimension descriptors

   real (r4), dimension(nx_block,ny_block,max_blocks_clinic) :: &
      WORK2D        ! temp for holding 2d fields

   real (r4), dimension(nx_block,ny_block,km,max_blocks_clinic) :: &
      WORK3D        ! temp for holding 3d fields

   real (r8), dimension(nx_block,ny_block) :: &
      WORK          ! full precision temp for intermediate results

   integer (int_kind) :: &
      k,                 &! vertical level index
      nfield,            &! dummy field index
      iblock              ! dummy block index

!-----------------------------------------------------------------------
!
!  check to see whether it is time to write files
!
!-----------------------------------------------------------------------

   lmovie_write = .false.
   if (lmovie_on) then
      lmovie_write = check_time_flag(movie_flag)
   endif

!-----------------------------------------------------------------------
!
!  write movie files if it is time
!
!-----------------------------------------------------------------------

   if (lmovie_write) then

      !*** create file suffix

      file_suffix = char_blank
      call create_suffix_movie(file_suffix)

!-----------------------------------------------------------------------
!
!     create data file descriptor
!
!-----------------------------------------------------------------------

      call date_and_time(date=date_created, time=time_created)
      hist_string = char_blank
      write(hist_string,'(a24,a8,1x,a10)') & 
         'POP MOVIE file created: ',date_created,time_created

      movie_file_desc = construct_file(movie_fmt,                      &
                                   root_name  = trim(movie_outfile),   &
                                   file_suffix= trim(file_suffix),     &
                                   title      ='POP MOVIE file',       &
                                   conventions='POP MOVIE conventions',&
                                   history    = trim(hist_string),     &
                                   record_length = rec_type_real,      &
                                   recl_words=nx_global*ny_global)

!-----------------------------------------------------------------------
!
!     add scalar fields to file as file attributes
!
!-----------------------------------------------------------------------

      call add_attrib_file(movie_file_desc, 'nsteps_total',nsteps_total)
      call add_attrib_file(movie_file_desc, 'tday'        ,tday)
      call add_attrib_file(movie_file_desc, 'iyear'       ,iyear)
      call add_attrib_file(movie_file_desc, 'imonth'      ,imonth)
      call add_attrib_file(movie_file_desc, 'iday'        ,iday)

!-----------------------------------------------------------------------
!
!     open output file and define dimensions
!
!-----------------------------------------------------------------------

      call data_set (movie_file_desc, 'open')

      i_dim = construct_io_dim('i',nx_global)
      j_dim = construct_io_dim('j',ny_global)
      k_dim = construct_io_dim('k',km)

!-----------------------------------------------------------------------
!
!     write fields to file - this requires two phases
!     in this first phase, we define all the fields to be written
!
!-----------------------------------------------------------------------
 
      allocate(movie_fields(num_avail_movie_fields))

      do nfield = 1,num_avail_movie_fields  ! check all available fields

         if (avail_movie_fields(nfield)%requested) then 

            !*** construct io_field descriptors for each field

            if (avail_movie_fields(nfield)%ndims == 2) then

               movie_fields(nfield) = construct_io_field(              &
                             avail_movie_fields(nfield)%short_name,    &
                             i_dim, j_dim,                             &
                   long_name=avail_movie_fields(nfield)%long_name,     &
                   units    =avail_movie_fields(nfield)%units    ,     &
                   grid_loc =avail_movie_fields(nfield)%grid_loc ,     &
               missing_value=avail_movie_fields(nfield)%missing_value, &
                 valid_range=avail_movie_fields(nfield)%valid_range,   &
                  r2d_array =WORK2D)

            else if (avail_movie_fields(nfield)%ndims == 3) then

               movie_fields(nfield) = construct_io_field(              &
                             avail_movie_fields(nfield)%short_name,    &
                             i_dim, j_dim, dim3=k_dim,                 &
                   long_name=avail_movie_fields(nfield)%long_name,     &
                   units    =avail_movie_fields(nfield)%units    ,     &
                   grid_loc =avail_movie_fields(nfield)%grid_loc ,     &
               missing_value=avail_movie_fields(nfield)%missing_value, &
                 valid_range=avail_movie_fields(nfield)%valid_range,   &
                  r3d_array =WORK3D)

            endif

            call data_set(movie_file_desc,'define',movie_fields(nfield))
         endif
      end do

!-----------------------------------------------------------------------
!
!     write fields to file 
!     in this second phase, we actually write the data
!
!-----------------------------------------------------------------------
 
      do nfield = 1,num_avail_movie_fields  ! check all available fields

         if (avail_movie_fields(nfield)%requested) then 

            !$OMP PARALLEL DO &
            !$OMP   PRIVATE(iblock, k, this_block, WORK)
            do iblock=1,nblocks_clinic
               this_block = get_block(blocks_clinic(iblock),iblock)  

               if (nfield == movie_id_shgt) then 
                  !*** surface height
                  WORK2D(:,:,iblock) = PSURF(:,:,curtime,iblock)/grav

               else if (nfield == movie_id_utrans) then
                  !*** U transport
                  call ugrid_to_tgrid(WORK, HU(:,:,iblock)*       &
                                      UBTROP(:,:,curtime,iblock), &
                                      iblock)
                  WORK2D(:,:,iblock) = WORK

               else if (nfield == movie_id_vtrans) then
                  !*** V transport
                  call ugrid_to_tgrid(WORK, HU(:,:,iblock)*       &
                                      VBTROP(:,:,curtime,iblock), &
                                      iblock)
                  WORK2D(:,:,iblock) = WORK

               else if (nfield == movie_id_temp1_2) then
                  !*** surface temperature
                  WORK2D(:,:,iblock) = p5* &
                                    (TRACER(:,:,1,1,curtime,iblock) + & 
                                     TRACER(:,:,2,1,curtime,iblock))

               else if (nfield == movie_id_salt1_2) then
                  !*** surface salinity
                  WORK2D(:,:,iblock) = p5* &
                                    (TRACER(:,:,1,2,curtime,iblock) + & 
                                     TRACER(:,:,2,2,curtime,iblock))

               else if (nfield == movie_id_temp6) then
                  !*** temperature at level 6
                  WORK2D(:,:,iblock) = TRACER(:,:,6,1,curtime,iblock)

               else if (nfield == movie_id_salt6) then
                  !*** salinity at level 6
                  WORK2D(:,:,iblock) = TRACER(:,:,6,2,curtime,iblock)

               else if (nfield == movie_id_vort) then
                  !*** relative vorticity at sfc
                  k = 1 ! Index for surface level
                  call zcurl(k,WORK, UVEL(:,:,k,curtime,iblock), &
                                     VVEL(:,:,k,curtime,iblock), &
                                     this_block)
                  WORK2D(:,:,iblock) = WORK*TAREA_R(:,:,iblock)

               endif

            end do !block loop
            !$OMP END PARALLEL DO

            call data_set (movie_file_desc,'write',movie_fields(nfield))
            call destroy_io_field(movie_fields(nfield))
         endif
      end do

      deallocate(movie_fields)
      call data_set (movie_file_desc, 'close')

      if (my_task == master_task) then
         write(stdout,blank_fmt)
         write(stdout,*) 'Wrote file: ', trim(movie_file_desc%full_name)
      endif

!-----------------------------------------------------------------------
!
!     get rid of file descriptor
!
!-----------------------------------------------------------------------

      call destroy_file(movie_file_desc)

   endif ! time to do movie file

!-----------------------------------------------------------------------
!EOC

 end subroutine write_movie

!***********************************************************************
!BOP
! !IROUTINE: init_movie
! !INTERFACE:

 subroutine init_movie

! !DESCRIPTION:
!  Initializes movie output choices from input files.
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
      movie_freq_opt,    &! choice for frequency of movie output
      movie_contents,    &! filename for choosing fields for output
      char_temp           ! temporary for manipulating fields

   namelist /movie_nml/ movie_freq_opt, movie_freq, &
                        movie_outfile, movie_contents, movie_fmt

!-----------------------------------------------------------------------
!
!  read movie file output frequency and filenames from namelist
!
!-----------------------------------------------------------------------

   movie_freq_iopt = freq_opt_never
   movie_freq      = 100000
   movie_outfile   = 'm'
   movie_contents  = 'unknown_movie_contents'
   movie_fmt       = 'bin'

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=movie_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading movie_nml')
   endif

   if (my_task == master_task) then
      write(stdout,delim_fmt)
      write(stdout,blank_fmt)
      write(stdout,'(a23)') ' Movie output options'
      write(stdout,blank_fmt)
      write(stdout,delim_fmt)

      select case (trim(movie_freq_opt))
      case ('never')
         movie_freq_iopt = freq_opt_never
         write(stdout,'(a23)') 'Movie output disabled'
      case ('nyear')
         movie_freq_iopt = freq_opt_nyear
         write(stdout,'(a21,i6,a8)') 'Movie output every ', &
                                      movie_freq, ' year(s)'
      case ('nmonth')
         movie_freq_iopt = freq_opt_nmonth
         write(stdout,'(a21,i6,a9)') 'Movie output every ', &
                                      movie_freq, ' month(s)'
      case ('nday')
         movie_freq_iopt = freq_opt_nday
         write(stdout,'(a21,i6,a7)') 'Movie output every ', &
                                      movie_freq, ' day(s)'
      case ('nhour')
         movie_freq_iopt = freq_opt_nhour
         write(stdout,'(a21,i6,a8)') 'Movie output every ', &
                                      movie_freq, ' hour(s)'
      case ('nsecond')
         movie_freq_iopt = freq_opt_nsecond
         write(stdout,'(a21,i6,a8)') 'Movie output every ', &
                                      movie_freq, ' seconds'
      case ('nstep')
         movie_freq_iopt = freq_opt_nstep
         write(stdout,'(a21,i6,a6)') 'Movie output every ', &
                                      movie_freq, ' steps'
      case default
         movie_freq_iopt = -1000
      end select
   endif

   call broadcast_scalar(movie_freq_iopt, master_task)
   call broadcast_scalar(movie_freq,      master_task)

   if (movie_freq_iopt == -1000) then
      call exit_POP(sigAbort, &
         'ERROR: unknown option for movie file freq')
   else if (movie_freq_iopt == freq_opt_never) then
      lmovie_on = .false.
   else
      lmovie_on = .true.
   endif

   if (lmovie_on) then
      call broadcast_scalar(movie_outfile,  master_task)
      call broadcast_scalar(movie_contents, master_task)
      call broadcast_scalar(movie_fmt,      master_task)
      if (my_task == master_task) write(stdout,'(a24,a)') &
                   ' Movie output format: ',trim(movie_fmt)
   endif

   movie_flag = init_time_flag('movie',default=.false.,    &
                                freq_opt = movie_freq_iopt, &
                                freq     = movie_freq)

!-----------------------------------------------------------------------
!
!  if movie output turned on, define available movie fields
!
!-----------------------------------------------------------------------

   if (lmovie_on) then
      call define_movie_field(movie_id_shgt, 'SHGT', 2,        &
                             long_name = 'Sea surface height', &
                             units     = 'cm',                 &
                             grid_loc  = '2110',               &
                             missing_value = undefined)

      call define_movie_field(movie_id_utrans, 'UTRANS', 2,         &
                  long_name = 'Vertically-intergrated U transport', &
                             units     = 'cm2/s',                   &
                             grid_loc  = '2220',                    &
                             missing_value = undefined)

      call define_movie_field(movie_id_vtrans, 'VTRANS', 2,         &
                  long_name = 'Vertically-intergrated V transport', &
                             units     = 'cm2/s',                   &
                             grid_loc  = '2220',                    &
                             missing_value = undefined)

      call define_movie_field(movie_id_temp1_2, 'TEMP1_2', 2,       &
         long_name = 'Potential temperature averaged of levels 1-2',&
                             units     = 'deg C',                   &
                             grid_loc  = '2110',                    &
                             missing_value = undefined)

      call define_movie_field(movie_id_salt1_2, 'SALT1_2', 2,       &
                     long_name = 'Salinity averaged of levels 1-2', &
                             units     = 'g/g (msu)',               &
                             grid_loc  = '2110',                    &
                             missing_value = undefined)

      call define_movie_field(movie_id_temp6, 'TEMP6', 2,             &
                      long_name = 'Potential temperature at level 6', &
                             units     = 'deg C',                     &
                             grid_loc  = '2110',                      &
                             missing_value = undefined)

      call define_movie_field(movie_id_salt6, 'SALT6', 2,         &
                             long_name = 'Salinity at level 6',   &
                             units     = 'g/g',                   &
                             grid_loc  = '2110',                  &
                             missing_value = undefined)

      call define_movie_field(movie_id_vort, 'VORT', 2,               &
                             long_name = 'Surface relative vorticity',&
                             units     = '1/s',                       &
                             grid_loc  = '2110',                      &
                             missing_value = undefined)

!-----------------------------------------------------------------------
!
!     read contents file to determine which fields to dump
!
!-----------------------------------------------------------------------

      call get_unit(nu)

      if (my_task == master_task) then
         open(nu, file=movie_contents, status='old')
         read(nu,*) num_requested_movie_fields
      endif

      call broadcast_scalar(num_requested_movie_fields, master_task)

      do nfield=1,num_requested_movie_fields
         if (my_task == master_task) then
            read(nu,'(a80)') char_temp
         endif
         call broadcast_scalar(char_temp, master_task)

         char_temp = adjustl(char_temp)
         n = index(char_temp,' ') - 1
   
         call request_movie_field(char_temp(1:n))
      end do

      close(nu)
      call release_unit(nu)

   endif ! lmovie_on

!-----------------------------------------------------------------------
!EOC

 end subroutine init_movie

!***********************************************************************
!BOP
! !IROUTINE: define_movie_field
! !INTERFACE:

 subroutine define_movie_field(id, short_name, ndims, &
                               long_name, units,      &
                               grid_loc, missing_value, valid_range)

! !DESCRIPTION:
!  Initializes description of an available field and returns location
!  in the available fields array for use in later movie calls.
!
! !REVISION HISTORY:
!  same as module

! !OUTPUT PARAMETERS:

   integer (int_kind), intent(out) :: &
      id                ! location in avail_fields array for use in
                        ! later movie routines

! !INPUT PARAMETERS:

   character(*), intent(in) :: &
      short_name               ! short name for field

   integer (i4), intent(in) :: &
      ndims                    ! number of dims (2 or 3) of movie field

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

   num_avail_movie_fields = num_avail_movie_fields + 1
   if (num_avail_movie_fields > max_avail_movie_fields) then
      call exit_POP(sigAbort, &
                    'movie: defined movie fields > max allowed')
   endif
 
   id = num_avail_movie_fields

!-----------------------------------------------------------------------
!
!  now fill the field descriptor
!
!-----------------------------------------------------------------------

   avail_movie_fields(id)%ndims      = ndims
   avail_movie_fields(id)%short_name = short_name
   avail_movie_fields(id)%requested  = .false.

   if (present(long_name)) then
      avail_movie_fields(id)%long_name = long_name
   else
      avail_movie_fields(id)%long_name = char_blank
   endif

   if (present(units)) then
      avail_movie_fields(id)%units = units
   else
      avail_movie_fields(id)%units = char_blank
   endif

   if (present(grid_loc)) then
      avail_movie_fields(id)%grid_loc = grid_loc
   else
      avail_movie_fields(id)%grid_loc = '    '
   endif

   if (present(missing_value)) then
      avail_movie_fields(id)%missing_value = missing_value
   else
      avail_movie_fields(id)%missing_value = undefined
   endif

   if (present(valid_range)) then
      avail_movie_fields(id)%valid_range = valid_range
   else
      avail_movie_fields(id)%valid_range = undefined
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine define_movie_field

!***********************************************************************
!BOP
! !IROUTINE: request_movie_field
! !INTERFACE:

 subroutine request_movie_field(short_name)

! !DESCRIPTION:
!  This field marks an available field as requested and computes
!  the location in the movie buffer array.
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
   srch_loop: do n=1,num_avail_movie_fields
      if (trim(avail_movie_fields(n)%short_name) == short_name) then
         id = n
         exit srch_loop
      endif
   end do srch_loop

   if (id == 0) then
      if (my_task == master_task) &
         write(stdout,*) 'Requested movie field: ', trim(short_name)
      call exit_POP(sigAbort,'movie: requested field unknown')
   endif

!-----------------------------------------------------------------------
!
!  mark the field as requested
!
!-----------------------------------------------------------------------

   avail_movie_fields(id)%requested = .true.

!-----------------------------------------------------------------------
!EOC

 end subroutine request_movie_field

!***********************************************************************
!BOP
! !IROUTINE: movie_requested
! !INTERFACE:

 function movie_requested(id)

! !DESCRIPTION:
!  This function determines whether an available (defined) movie field
!  has been requested by a user (through the input contents file) and 
!  returns true if it has.  Note that if movie has been turned off, 
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
      movie_requested     ! result of checking whether the field has
                         !   been requested

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  check the buffer location - if zero, the field has not been
!  requested
!
!-----------------------------------------------------------------------

   if (id < 1 .or. id > num_avail_movie_fields) then
      call exit_POP(sigAbort,'movie_requested: invalid movie id')
   endif

   movie_requested = avail_movie_fields(id)%requested

!-----------------------------------------------------------------------
!EOC

 end function movie_requested

!***********************************************************************
!BOP
! !IROUTINE: create_suffix_movie
! !INTERFACE:

 subroutine create_suffix_movie(file_suffix)

! !DESCRIPTION:
!  Creates suffix to append to movie file name based on output
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

   select case (movie_freq_iopt)
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

 end subroutine create_suffix_movie

!***********************************************************************

 end module movie

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
