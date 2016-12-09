!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module io_types

!BOP
!
! !MODULE:  io_types
!
! !DESCRIPTION:
!  This module contains the declarations for all required io
!  data types and several operators for those data types. It also 
!  contains several global parameters used by various io operations
!  and an io unit manager.
!
! !REVISION HISTORY:
!  CVS:$Id: io_types.F90,v 1.19 2003/12/23 22:11:40 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod
   use constants
   use communicate
   use broadcast
   use exit_mod

   implicit none
   private
   save

! !PUBLIC TYPES:

   ! Generic IO field descriptor

   type, public :: io_field_desc
      character(char_len)                         :: short_name
      character(char_len)                         :: long_name
      character(char_len)                         :: units
      character(4)                                :: grid_loc
      real(r4)                                    :: missing_value
      real(r4), dimension(2)                      :: valid_range
      integer(i4)                                 :: id
      integer(i4)                                 :: nfield_dims
      integer(i4)                                 :: field_loc
      integer(i4)                                 :: field_type
      type (io_dim), dimension(:), pointer        :: field_dim
      character(char_len), dimension(:), pointer  :: add_attrib_cname
      character(char_len), dimension(:), pointer  :: add_attrib_cval
      character(char_len), dimension(:), pointer  :: add_attrib_lname
      logical  (log_kind), dimension(:), pointer  :: add_attrib_lval
      character(char_len), dimension(:), pointer  :: add_attrib_iname
      integer  (i4),       dimension(:), pointer  :: add_attrib_ival
      character(char_len), dimension(:), pointer  :: add_attrib_rname
      real     (r4),       dimension(:), pointer  :: add_attrib_rval
      character(char_len), dimension(:), pointer  :: add_attrib_dname
      real     (r8),       dimension(:), pointer  :: add_attrib_dval
      !   Only one of these next nine pointers can be associated.
      !   The others must be nullified.  For convenience in
      !   initialization, these declarations are the last listed
      !   in this type.
      integer(i4), dimension(:,:,:), pointer      :: field_i_2d
      integer(i4), dimension(:,:,:,:), pointer    :: field_i_3d
      real(r4), dimension(:,:,:), pointer         :: field_r_2d
      real(r4), dimension(:,:,:,:), pointer       :: field_r_3d
      real(r8), dimension(:,:,:), pointer         :: field_d_2d
      real(r8), dimension(:,:,:,:), pointer       :: field_d_3d
   end type

   type, public :: io_dim
      integer(i4) ::  id
      integer(i4) :: length  ! 1 to n, but 0 means unlimited
      integer(i4) :: start, stop, stride  ! For slicing and dicing
      character(char_len)        :: name
   end type

   ! Generic data file descriptor

   type, public :: datafile
      character(char_len)                        :: full_name
      character(char_len)                        :: data_format ! .bin or
                                                                ! .nc
      character(char_len)                        :: root_name
      character(char_len)                        :: file_suffix
      integer(i4), dimension (2)                 ::  id ! LUN (binary) or
                                                        ! NCID (netcdf)
      character(char_len)                        :: title
      character(char_len)                        :: history
      character(char_len)                        :: conventions
      character(char_len), dimension(:), pointer :: add_attrib_cname
      character(char_len), dimension(:), pointer :: add_attrib_cval
      character(char_len), dimension(:), pointer :: add_attrib_lname
      logical  (log_kind), dimension(:), pointer :: add_attrib_lval
      character(char_len), dimension(:), pointer :: add_attrib_iname
      integer  (i4),       dimension(:), pointer :: add_attrib_ival
      character(char_len), dimension(:), pointer :: add_attrib_rname
      real     (r4),       dimension(:), pointer :: add_attrib_rval
      character(char_len), dimension(:), pointer :: add_attrib_dname
      real     (r8),       dimension(:), pointer :: add_attrib_dval
      integer(i4)                                :: num_iotasks
      integer(i4)                                :: record_length
      integer(i4)                                :: current_record ! bin
      logical(log_kind)                          :: readonly
      logical(log_kind)                          :: ldefine
   end type

! !PUBLIC MEMBER FUNCTIONS:

   public :: get_unit,                &
             release_unit,            &
             construct_file,          &
             destroy_file,            &
             add_attrib_file,         &
             extract_attrib_file,     &
             construct_io_field,      &
             destroy_io_field,        &
             construct_io_dim,        &
             add_attrib_io_field,     &
             extract_attrib_io_field, &
             init_io

! !PUBLIC DATA MEMBERS:

   integer (i4), parameter, public :: &
      nml_in    = 10,         &! reserved unit for namelist input
      stdin     =  5,         &! reserved unit for standard input
      stdout    =  6,         &! reserved unit for standard output
      stderr    =  6           ! reserved unit for standard error

   integer (i4), parameter, public :: &
      rec_type_int  = -1,     &! ids to use for inquiring the
      rec_type_real = -2,     &! record length to use for binary files
      rec_type_dbl  = -3       !

   character (6), parameter, public :: &
      nml_filename = 'pop_in'  ! namelist input file name

   integer (i4), public :: &
      num_iotasks   ! num of procs to use for parallel io
                    ! if output format is 'netcdf'.  Then it is 1.

   logical (log_kind), public :: &
      luse_pointer_files   ! use files to point to location of restarts

   character (char_len), public :: &
      pointer_filename     ! filename to use for pointer files
!EOP
!BOC
!-----------------------------------------------------------------------
!
!  io unit manager variables
!
!-----------------------------------------------------------------------

   integer (i4), parameter, private :: &
      max_units = 99           ! maximum number of open units

   logical (log_kind), dimension(max_units), private :: &
      in_use                ! flag=.true. if unit currently open

!-----------------------------------------------------------------------
!
!  other module variables
!
!-----------------------------------------------------------------------

   logical (log_kind), private :: &
      lredirect_stdout          ! redirect stdout to log file

   character (char_len), private :: &
      log_filename          ! root name for log file

!-----------------------------------------------------------------------
!
!  generic interface definitions
!
!-----------------------------------------------------------------------

   interface add_attrib_file
      module procedure add_attrib_file_char, &
                       add_attrib_file_log,  &
                       add_attrib_file_int,  &
                       add_attrib_file_real, &
                       add_attrib_file_dbl
   end interface 

   interface extract_attrib_file
      module procedure extract_attrib_file_char, &
                       extract_attrib_file_log,  &
                       extract_attrib_file_int,  &
                       extract_attrib_file_real, &
                       extract_attrib_file_dbl
   end interface 

   interface add_attrib_io_field
      module procedure add_attrib_io_field_char, &
                       add_attrib_io_field_log,  &
                       add_attrib_io_field_int,  &
                       add_attrib_io_field_real, &
                       add_attrib_io_field_dbl
   end interface 

   interface extract_attrib_io_field
      module procedure extract_attrib_io_field_char, &
                       extract_attrib_io_field_log,  &
                       extract_attrib_io_field_int,  &
                       extract_attrib_io_field_real, &
                       extract_attrib_io_field_dbl
   end interface 

!EOC
!***********************************************************************

contains

!***********************************************************************
!BOP
! !IROUTINE: construct_file
! !INTERFACE:

 function construct_file (              &
      data_format,                      &
   ! Optional arguments begin here
      full_name,                        &
      root_name,                        &
      file_suffix,                      &
      id,                               &
      title,                            &
      history,                          &
      conventions,                      &
      record_length,                    &
      recl_words,                       &
      current_record,                   &
      input_num_iotasks                 &
    )                                   &
    result (descriptor)

! !DESCRIPTION:
!  This routine constructs a file descriptor for use in io routines.
!  It fills in every field to guarantee that the descriptor
!  will contain no illegal field values upon exit.  The data format
!  is required together with either a full name or root name.
!  If full name is supplied, that name will be used in all file
!  operations.  If root name is supplied, the full name will be
!  constructed using rootname.suffix.dataformat.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character(*), intent(in) :: &
      data_format               ! bin (binary) or nc (netCDF)

   character(*), intent(in), optional :: &
      full_name,               &! name (and path) for file
      root_name,               &! file name root
      file_suffix,             &! suffix (eg model day, step)
      title,                   &! descriptive name for the file
      history,                 &! file history string
      conventions               ! conventions for file

   integer (i4), dimension (2), intent(in), optional :: &
      id                        ! unit numbers for binary file & hdr
                                ! netCDF id for netCDF file

   integer (i4), intent(in), optional :: &
      input_num_iotasks,     &! to override default iotasks
      record_length,         &! record length type for binary files
      recl_words,            &! num words per record for binary files
      current_record          ! current record number in binary file

! !OUTPUT PARAMETERS:

   type (datafile) :: descriptor  ! data file descriptor

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) ::       &
      n,                 &! loop index
      io_record_length    ! actual record length

   integer (i4), dimension(:), allocatable :: work_i
   real (r4), dimension(:), allocatable :: work_r
   real (r8), dimension(:), allocatable :: work_d

!-----------------------------------------------------------------------
!
!  define data format
!
!-----------------------------------------------------------------------

   descriptor%data_format = char_blank
   descriptor%data_format = trim(data_format)

!-----------------------------------------------------------------------
!
!  file naming stuff
!  if full name is present, use that as filename
!  if root name and suffix supplied, construct file name from those
!
!-----------------------------------------------------------------------

   if (present(full_name)) then
      descriptor%full_name = char_blank
      descriptor%full_name = trim(full_name)

   else if (present(root_name)) then
      descriptor%root_name = char_blank
      if (present(root_name)) then
         descriptor%root_name = trim(root_name)
      else
         descriptor%root_name = 'root'
      end if
      descriptor%file_suffix = char_blank
      if (present(file_suffix)) then
         descriptor%file_suffix = trim(file_suffix)/&
                                  &/'.'/&
                                  &/trim(data_format)
      else
         descriptor%file_suffix = trim(data_format)
      end if

      descriptor%full_name = char_blank
      descriptor%full_name = trim(descriptor%root_name)/&
                           &/'.'/&
                           &/trim(descriptor%file_suffix)

   else
      call exit_POP(sigAbort, &
                    'construct_file: can not determine file name')
   endif

!-----------------------------------------------------------------------
!
!  parameters specific to binary files
!
!-----------------------------------------------------------------------

   if (descriptor%data_format=='bin') then

      !*** assign unit numbers as file id
      !*** if not assigned as input, assign later during file open

      if (present(id)) then
         descriptor%id = id
      else
         descriptor%id = 0
      endif

      !*** number of io tasks for parallel io

      if (present(input_num_iotasks)) then
         !*** override namelist input - useful if you need
         !*** to serial i/o for a particular file in binary mode
         descriptor%num_iotasks = input_num_iotasks
      else
         descriptor%num_iotasks = num_iotasks ! namelist input
      endif

      !*** compute record length

      if (present(record_length)) then
         if (.not. present(recl_words)) &
            call exit_POP(sigAbort, &
                    'construct_file: must supply recl_words')
         select case (record_length)
         case (rec_type_int)
            allocate (work_i(recl_words))
            inquire (iolength=io_record_length) work_i
            deallocate (work_i)
         case (rec_type_real)
            allocate (work_r(recl_words))
            inquire (iolength=io_record_length) work_r
            deallocate (work_r)
         case (rec_type_dbl)
            allocate (work_d(recl_words))
            inquire (iolength=io_record_length) work_d
            deallocate (work_d)
         case default
            io_record_length = 0
         end select
      else
         io_record_length = 0
      endif
      descriptor%record_length = io_record_length

      !*** initialize first record
      if (present(current_record)) then
         descriptor%current_record = current_record
      else
         descriptor%current_record = 1
      endif

!-----------------------------------------------------------------------
!
!  parameters specific to netCDF files
!
!-----------------------------------------------------------------------
   else

      !*** set id if already known, otherwise defined during file open

      if (present(id)) then
         descriptor%id = id
      else
         descriptor%id = 0 ! to be defined during open
      endif

      descriptor%num_iotasks = 1    ! netCDF can only do serial i/o
      descriptor%record_length  = 0 ! not used for netCDF
      descriptor%current_record = 0 ! not used for netCDF
   endif

!-----------------------------------------------------------------------
!
!  general descriptive info for files
!
!-----------------------------------------------------------------------

   descriptor%title = char_blank
   if (present(title)) then
      descriptor%title = trim(title)
   else
      descriptor%title = 'none'
   endif

   descriptor%history = char_blank
   if (present(history)) then
      descriptor%history = trim(history)
   else
      descriptor%history = 'none'
   endif

   descriptor%conventions = char_blank
   if (present(conventions)) then
      descriptor%conventions = trim(conventions)
   else
      descriptor%conventions = 'none'
   endif

!-----------------------------------------------------------------------
!
!  initialize global file attributes - these must be set separately
!  in the routine add_attrib_file.
!
!-----------------------------------------------------------------------

   descriptor%readonly = .false. ! reset later if necessary

   nullify (descriptor%add_attrib_cname)
   nullify (descriptor%add_attrib_cval)
   nullify (descriptor%add_attrib_lname)
   nullify (descriptor%add_attrib_lval)
   nullify (descriptor%add_attrib_iname)
   nullify (descriptor%add_attrib_ival)
   nullify (descriptor%add_attrib_rname)
   nullify (descriptor%add_attrib_rval)
   nullify (descriptor%add_attrib_dname)
   nullify (descriptor%add_attrib_dval)

!-----------------------------------------------------------------------
!EOC

 end function construct_file

!***********************************************************************
!BOP
! !IROUTINE: add_attrib_file_char
! !INTERFACE:

 subroutine add_attrib_file_char(file_descr, att_name, att_value)

! !DESCRIPTION:
!  This routine adds a global file attribute to an io file.  This
!  particular instantiation adds a character attribute, but is aliased
!  to the generic routine name add\_attrib\_file.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      att_name,       &! name  of attribute to be added
      att_value        ! value of attribute to be added

! !INPUT/OUTPUT PARAMETERS:

   type (datafile), intent(inout) :: &
      file_descr       ! data file descriptor

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) :: &
      n,           &! loop index
      num_attribs   ! current number of attributes defined

   character (char_len), dimension(:), allocatable :: &
      name_tmp,            &! temp space for resizing attrib name  array
      val_tmp               ! temp space for resizing attrib value array

   logical (log_kind) :: &
      att_exists         ! attribute already defined

!-----------------------------------------------------------------------
!
!  if this is the first attribute, allocate space and set the attribute
!
!-----------------------------------------------------------------------

   if (.not. associated(file_descr%add_attrib_cval)) then

      allocate(file_descr%add_attrib_cval(1), &
               file_descr%add_attrib_cname(1))

      file_descr%add_attrib_cval (1) = char_blank
      file_descr%add_attrib_cname(1) = char_blank
      file_descr%add_attrib_cval (1) = trim(att_value)
      file_descr%add_attrib_cname(1) = trim(att_name)

!-----------------------------------------------------------------------
!
!  if not the first, see if it already exists and over-write value
!  if does not exist, resize the attribute array and store the 
!  attributes
!
!-----------------------------------------------------------------------

   else

      att_exists = .false.
      num_attribs = size(file_descr%add_attrib_cval(:))
      att_search: do n=1,num_attribs
         if (trim(file_descr%add_attrib_cname(n)) == trim(att_name)) then
            file_descr%add_attrib_cval(n) = char_blank
            file_descr%add_attrib_cval(n) = trim(att_value)
            att_exists = .true.
            exit att_search
         endif
      end do att_search

      if (.not. att_exists) then

         allocate(name_tmp(num_attribs), val_tmp(num_attribs))
         name_tmp(:) = file_descr%add_attrib_cname(:)
         val_tmp (:) = file_descr%add_attrib_cval (:)

         deallocate(file_descr%add_attrib_cname)
         deallocate(file_descr%add_attrib_cval )
         num_attribs = num_attribs + 1
         allocate(file_descr%add_attrib_cname(num_attribs), &
                  file_descr%add_attrib_cval (num_attribs))

         file_descr%add_attrib_cname(:) = char_blank
         file_descr%add_attrib_cval (:) = char_blank
         do n=1,num_attribs-1
            file_descr%add_attrib_cname(n) = trim(name_tmp(n))
            file_descr%add_attrib_cval (n) = trim( val_tmp(n))
         end do
         file_descr%add_attrib_cname(num_attribs) = trim(att_name)
         file_descr%add_attrib_cval (num_attribs) = trim(att_value)

         deallocate(name_tmp,val_tmp)
      endif

   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine add_attrib_file_char

!***********************************************************************
!BOP
! !IROUTINE: add_attrib_file_log
! !INTERFACE:

 subroutine add_attrib_file_log(file_descr, att_name, att_value)

! !DESCRIPTION:
!  This routine adds a global file attribute to an io file.  This
!  particular instantiation adds a logical attribute, but is aliased
!  to the generic routine name add\_attrib\_file.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      att_name         ! name  of attribute to be added

   logical (log_kind), intent(in) :: &
      att_value        ! value of attribute to be added

! !INPUT/OUTPUT PARAMETERS:

   type (datafile), intent(inout) :: &
      file_descr       ! data file descriptor

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) :: &
      n,           &! loop index
      num_attribs   ! current number of attributes defined

   character (char_len), dimension(:), allocatable :: &
      name_tmp              ! temp space for resizing attrib name  array

   logical (log_kind), dimension(:), allocatable :: &
      val_tmp               ! temp space for resizing attrib value array

   logical (log_kind) :: &
      att_exists         ! attribute already defined

!-----------------------------------------------------------------------
!
!  if this is the first attribute, allocate space and set the attribute
!
!-----------------------------------------------------------------------

   if (.not. associated(file_descr%add_attrib_lval)) then

      allocate(file_descr%add_attrib_lval(1), &
               file_descr%add_attrib_lname(1))

      file_descr%add_attrib_lval (1) = att_value
      file_descr%add_attrib_lname(1) = char_blank
      file_descr%add_attrib_lname(1) = trim(att_name)

!-----------------------------------------------------------------------
!
!  if not the first, see if it already exists and over-write value
!  if does not exist, resize the attribute array and store the 
!  attributes
!
!-----------------------------------------------------------------------

   else

      att_exists = .false.
      num_attribs = size(file_descr%add_attrib_lval(:))
      att_search: do n=1,num_attribs
         if (trim(file_descr%add_attrib_lname(n)) == trim(att_name)) then
            file_descr%add_attrib_lval(n) = att_value
            att_exists = .true.
            exit att_search
         endif
      end do att_search

      if (.not. att_exists) then

         allocate(name_tmp(num_attribs), val_tmp(num_attribs))

         name_tmp(:) = file_descr%add_attrib_lname(:)
         val_tmp (:) = file_descr%add_attrib_lval (:)

         deallocate(file_descr%add_attrib_lname, &
                    file_descr%add_attrib_lval )

         num_attribs = num_attribs + 1

         allocate(file_descr%add_attrib_lname(num_attribs), &
                  file_descr%add_attrib_lval (num_attribs))

         file_descr%add_attrib_lname(:) = char_blank
         do n=1,num_attribs-1
            file_descr%add_attrib_lname(n) = trim(name_tmp(n))
            file_descr%add_attrib_lval (n) =       val_tmp(n)
         end do
         file_descr%add_attrib_lname(num_attribs) = trim(att_name)
         file_descr%add_attrib_lval (num_attribs) = att_value

         deallocate(name_tmp,val_tmp)
      endif

   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine add_attrib_file_log

!***********************************************************************
!BOP
! !IROUTINE: add_attrib_file_int
! !INTERFACE:

 subroutine add_attrib_file_int(file_descr, att_name, att_value)

! !DESCRIPTION:
!  This routine adds a global file attribute to an io file.  This
!  particular instantiation adds an integer attribute, but is aliased
!  to the generic routine name add\_attrib\_file.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      att_name         ! name  of attribute to be added

   integer (i4), intent(in) :: &
      att_value        ! value of attribute to be added

! !INPUT/OUTPUT PARAMETERS:

   type (datafile), intent(inout) :: &
      file_descr       ! data file descriptor

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) :: &
      n,           &! loop index
      num_attribs   ! current number of attributes defined

   character (char_len), dimension(:), allocatable :: &
      name_tmp              ! temp space for resizing attrib name  array

   integer (i4), dimension(:), allocatable :: &
      val_tmp               ! temp space for resizing attrib value array

   logical (log_kind) :: &
      att_exists         ! attribute already defined

!-----------------------------------------------------------------------
!
!  if this is the first attribute, allocate space and set the attribute
!
!-----------------------------------------------------------------------

   if (.not. associated(file_descr%add_attrib_ival)) then

      allocate(file_descr%add_attrib_ival(1), &
               file_descr%add_attrib_iname(1))

      file_descr%add_attrib_ival (1) = att_value
      file_descr%add_attrib_iname(1) = char_blank
      file_descr%add_attrib_iname(1) = trim(att_name)

!-----------------------------------------------------------------------
!
!  if not the first, see if it already exists and over-write value
!  if does not exist, resize the attribute array and store the 
!  attributes
!
!-----------------------------------------------------------------------

   else

      att_exists = .false.
      num_attribs = size(file_descr%add_attrib_ival(:))
      att_search: do n=1,num_attribs
         if (trim(file_descr%add_attrib_iname(n)) == trim(att_name)) then
            file_descr%add_attrib_ival(n) = att_value
            att_exists = .true.
            exit att_search
         endif
      end do att_search

      if (.not. att_exists) then

         allocate(name_tmp(num_attribs), val_tmp(num_attribs))

         name_tmp(:) = file_descr%add_attrib_iname(:)
         val_tmp (:) = file_descr%add_attrib_ival (:)

         deallocate(file_descr%add_attrib_iname, &
                    file_descr%add_attrib_ival )

         num_attribs = num_attribs + 1

         allocate(file_descr%add_attrib_iname(num_attribs), &
                  file_descr%add_attrib_ival (num_attribs))

         file_descr%add_attrib_iname(:) = char_blank
         do n=1,num_attribs-1
            file_descr%add_attrib_iname(n) = trim(name_tmp(n))
            file_descr%add_attrib_ival (n) =       val_tmp(n)
         end do
         file_descr%add_attrib_iname(num_attribs) = trim(att_name)
         file_descr%add_attrib_ival (num_attribs) = att_value

         deallocate(name_tmp,val_tmp)
      endif

   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine add_attrib_file_int

!***********************************************************************
!BOP
! !IROUTINE: add_attrib_file_real
! !INTERFACE:

 subroutine add_attrib_file_real(file_descr, att_name, att_value)

! !DESCRIPTION:
!  This routine adds a global file attribute to an io file.  This
!  particular instantiation adds a real attribute, but is aliased
!  to the generic routine name add\_attrib\_file.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      att_name         ! name  of attribute to be added

   real (r4), intent(in) :: &
      att_value        ! value of attribute to be added

! !INPUT/OUTPUT PARAMETERS:

   type (datafile), intent(inout) :: &
      file_descr       ! data file descriptor

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) :: &
      n,           &! loop index
      num_attribs   ! current number of attributes defined

   character (char_len), dimension(:), allocatable :: &
      name_tmp              ! temp space for resizing attrib name  array

   real (r4), dimension(:), allocatable :: &
      val_tmp               ! temp space for resizing attrib value array

   logical (log_kind) :: &
      att_exists         ! attribute already defined

!-----------------------------------------------------------------------
!
!  if this is the first attribute, allocate space and set the attribute
!
!-----------------------------------------------------------------------

   if (.not. associated(file_descr%add_attrib_rval)) then

      allocate(file_descr%add_attrib_rval(1), &
               file_descr%add_attrib_rname(1))

      file_descr%add_attrib_rval (1) = att_value
      file_descr%add_attrib_rname(1) = char_blank
      file_descr%add_attrib_rname(1) = trim(att_name)

!-----------------------------------------------------------------------
!
!  if not the first, see if it already exists and over-write value
!  if does not exist, resize the attribute array and store the 
!  attributes
!
!-----------------------------------------------------------------------

   else

      att_exists = .false.
      num_attribs = size(file_descr%add_attrib_rval(:))
      att_search: do n=1,num_attribs
         if (trim(file_descr%add_attrib_rname(n)) == trim(att_name)) then
            file_descr%add_attrib_rval(n) = att_value
            att_exists = .true.
            exit att_search
         endif
      end do att_search

      if (.not. att_exists) then

         allocate(name_tmp(num_attribs), val_tmp(num_attribs))

         name_tmp(:) = file_descr%add_attrib_rname(:)
         val_tmp (:) = file_descr%add_attrib_rval (:)

         deallocate(file_descr%add_attrib_rname, &
                    file_descr%add_attrib_rval )

         num_attribs = num_attribs + 1

         allocate(file_descr%add_attrib_rname(num_attribs), &
                  file_descr%add_attrib_rval (num_attribs))

         file_descr%add_attrib_rname(:) = char_blank
         do n=1,num_attribs-1
            file_descr%add_attrib_rname(n) = trim(name_tmp(n))
            file_descr%add_attrib_rval (n) =       val_tmp(n)
         end do
         file_descr%add_attrib_rname(num_attribs) = trim(att_name)
         file_descr%add_attrib_rval (num_attribs) = att_value

         deallocate(name_tmp,val_tmp)
      endif

   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine add_attrib_file_real

!***********************************************************************
!BOP
! !IROUTINE: add_attrib_file_dbl
! !INTERFACE:

 subroutine add_attrib_file_dbl(file_descr, att_name, att_value)

! !DESCRIPTION:
!  This routine adds a global file attribute to an io file.  This
!  particular instantiation adds a double precision attribute, but is 
!  aliased to the generic routine name add\_attrib\_file.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      att_name         ! name  of attribute to be added

   real (r8), intent(in) :: &
      att_value        ! value of attribute to be added

! !INPUT/OUTPUT PARAMETERS:

   type (datafile), intent(inout) :: &
      file_descr       ! data file descriptor

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) :: &
      n,           &! loop index
      num_attribs   ! current number of attributes defined

   character (char_len), dimension(:), allocatable :: &
      name_tmp              ! temp space for resizing attrib name  array

   real (r8), dimension(:), allocatable :: &
      val_tmp               ! temp space for resizing attrib value array

   logical (log_kind) :: &
      att_exists         ! attribute already defined

!-----------------------------------------------------------------------
!
!  if this is the first attribute, allocate space and set the attribute
!
!-----------------------------------------------------------------------

   if (.not. associated(file_descr%add_attrib_dval)) then

      allocate(file_descr%add_attrib_dval(1), &
               file_descr%add_attrib_dname(1))

      file_descr%add_attrib_dval (1) = att_value
      file_descr%add_attrib_dname(1) = char_blank
      file_descr%add_attrib_dname(1) = trim(att_name)

!-----------------------------------------------------------------------
!
!  if not the first, see if it already exists and over-write value
!  if does not exist, resize the attribute array and store the 
!  attributes
!
!-----------------------------------------------------------------------

   else

      att_exists = .false.
      num_attribs = size(file_descr%add_attrib_dval(:))
      att_search: do n=1,num_attribs
         if (trim(file_descr%add_attrib_dname(n)) == trim(att_name)) then
            file_descr%add_attrib_dval(n) = att_value
            att_exists = .true.
            exit att_search
         endif
      end do att_search

      if (.not. att_exists) then

         allocate(name_tmp(num_attribs), val_tmp(num_attribs))

         name_tmp(:) = file_descr%add_attrib_dname(:)
         val_tmp (:) = file_descr%add_attrib_dval (:)

         deallocate(file_descr%add_attrib_dname, &
                    file_descr%add_attrib_dval )

         num_attribs = num_attribs + 1

         allocate(file_descr%add_attrib_dname(num_attribs), &
                  file_descr%add_attrib_dval (num_attribs))

         file_descr%add_attrib_dname(:) = char_blank
         do n=1,num_attribs-1
            file_descr%add_attrib_dname(n) = trim(name_tmp(n))
            file_descr%add_attrib_dval (n) =       val_tmp(n)
         end do
         file_descr%add_attrib_dname(num_attribs) = trim(att_name)
         file_descr%add_attrib_dval (num_attribs) = att_value

         deallocate(name_tmp,val_tmp)
      endif

   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine add_attrib_file_dbl

!***********************************************************************
!BOP
! !IROUTINE: extract_attrib_file_char
! !INTERFACE:

 subroutine extract_attrib_file_char(file_descr, att_name, att_value)

! !DESCRIPTION:
!  This routine extracts a global file attribute from an io file
!  descriptor based on the attribute name.  This particular 
!  instantiation extracts a character attribute, but is aliased
!  to the generic routine name extract\_attrib\_file.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      att_name         ! name  of attribute to be extracted

   type (datafile), intent(in) :: &
      file_descr       ! data file descriptor

! !OUTPUT PARAMETERS:

   character (*), intent(out) :: &
      att_value        ! value of attribute to be extracted

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) :: &
      n             ! loop index

   logical (log_kind) :: &
      att_exists         ! attribute already defined

!-----------------------------------------------------------------------
!
!  first check standard attributes
!
!-----------------------------------------------------------------------

   att_exists = .false.

   select case(trim(att_name))
   case('full_name','FULL_NAME')
      att_exists = .true.
      att_value = file_descr%full_name

   case('data_format','DATA_FORMAT')
      att_exists = .true.
      att_value = file_descr%data_format

   case('root_name','ROOT_NAME')
      att_exists = .true.
      att_value = file_descr%root_name

   case('file_suffix','FILE_SUFFIX')
      att_exists = .true.
      att_value = file_descr%file_suffix

   case('title','TITLE')
      att_exists = .true.
      att_value = file_descr%title

   case('history','HISTORY')
      att_exists = .true.
      att_value = file_descr%history

   case('conventions','CONVENTIONS')
      att_exists = .true.
      att_value = file_descr%conventions

   end select

!-----------------------------------------------------------------------
!
!  if attribute array exists, search for attribute
!
!-----------------------------------------------------------------------

   if (.not. att_exists .and. &
       associated(file_descr%add_attrib_cval)) then

      att_search: do n=1,size(file_descr%add_attrib_cval)

         if (trim(file_descr%add_attrib_cname(n))==trim(att_name)) then

            !*** found the attribute - assign the value

            att_value = file_descr%add_attrib_cval(n)
            att_exists = .true.
            exit att_search
         endif
      end do att_search
   endif

!-----------------------------------------------------------------------
!
!  if attribute not found, exit with an error
!
!-----------------------------------------------------------------------

   if (.not. att_exists) then
      if (my_task == master_task) then
         write(stdout,*) 'Attribute name: ',trim(att_name)
      endif
      call exit_POP(sigAbort,'Unknown file attribute')
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine extract_attrib_file_char

!***********************************************************************
!BOP
! !IROUTINE: extract_attrib_file_log
! !INTERFACE:

 subroutine extract_attrib_file_log(file_descr, att_name, att_value)

! !DESCRIPTION:
!  This routine extracts a global file attribute from an io file
!  descriptor based on the attribute name.  This particular 
!  instantiation extracts a logical attribute, but is aliased
!  to the generic routine name extract\_attrib\_file.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      att_name         ! name  of attribute to be extracted

   type (datafile), intent(in) :: &
      file_descr       ! data file descriptor

! !OUTPUT PARAMETERS:

   logical (log_kind), intent(out) :: &
      att_value        ! value of attribute to be extracted

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) :: &
      n             ! loop index

   logical (log_kind) :: &
      att_exists         ! attribute already defined

!-----------------------------------------------------------------------
!
!  first check standard attributes
!
!-----------------------------------------------------------------------

   att_exists = .false.

   select case(trim(att_name))
   case('readonly','READONLY')
      att_exists = .true.
      att_value = file_descr%readonly

   case('ldefine','LDEFINE')
      att_exists = .true.
      att_value = file_descr%ldefine

   end select

!-----------------------------------------------------------------------
!
!  if attribute array exists, search for attribute
!
!-----------------------------------------------------------------------

   if (.not. att_exists .and. &
       associated(file_descr%add_attrib_lval)) then

      att_search: do n=1,size(file_descr%add_attrib_lval)

         if (trim(file_descr%add_attrib_lname(n))==trim(att_name)) then

            !*** found the attribute - assign the value

            att_value = file_descr%add_attrib_lval(n)
            att_exists = .true.
            exit att_search
         endif
      end do att_search
   endif

!-----------------------------------------------------------------------
!
!  if attribute not found, exit with an error
!
!-----------------------------------------------------------------------

   if (.not. att_exists) then
      if (my_task == master_task) then
         write(stdout,*) 'Attribute name: ',trim(att_name)
      endif
      call exit_POP(sigAbort,'Unknown file attribute')
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine extract_attrib_file_log

!***********************************************************************
!BOP
! !IROUTINE: extract_attrib_file_int
! !INTERFACE:

 subroutine extract_attrib_file_int(file_descr, att_name, att_value)

! !DESCRIPTION:
!  This routine extracts a global file attribute from an io file
!  descriptor based on the attribute name.  This particular 
!  instantiation extracts an integer attribute, but is aliased
!  to the generic routine name extract\_attrib\_file.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      att_name         ! name  of attribute to be extracted

   type (datafile), intent(in) :: &
      file_descr       ! data file descriptor

! !OUTPUT PARAMETERS:

   integer (i4), intent(out) :: &
      att_value        ! value of attribute to be extracted

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) :: &
      n             ! loop index

   logical (log_kind) :: &
      att_exists         ! attribute already defined

!-----------------------------------------------------------------------
!
!  first check standard attributes
!
!-----------------------------------------------------------------------

   att_exists = .false.

   select case(trim(att_name))
   case('id','ID')
      att_exists = .true.
      att_value = file_descr%id(1)

   case('num_iotasks','NUM_IOTASKS')
      att_exists = .true.
      att_value = file_descr%num_iotasks

   case('record_length','RECORD_LENGTH')
      att_exists = .true.
      att_value = file_descr%record_length

   case('current_record','CURRENT_RECORD')
      att_exists = .true.
      att_value = file_descr%current_record

   end select

!-----------------------------------------------------------------------
!
!  if attribute array exists, search for attribute
!
!-----------------------------------------------------------------------

   if (.not. att_exists .and. &
       associated(file_descr%add_attrib_ival)) then

      att_search: do n=1,size(file_descr%add_attrib_ival)

         if (trim(file_descr%add_attrib_iname(n))==trim(att_name)) then

            !*** found the attribute - assign the value

            att_value = file_descr%add_attrib_ival(n)
            att_exists = .true.
            exit att_search
         endif
      end do att_search
   endif

!-----------------------------------------------------------------------
!
!  if attribute not found, exit with an error
!
!-----------------------------------------------------------------------

   if (.not. att_exists) then
      if (my_task == master_task) then
         write(stdout,*) 'Attribute name: ',trim(att_name)
      endif
      call exit_POP(sigAbort,'Unknown file attribute')
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine extract_attrib_file_int

!***********************************************************************
!BOP
! !IROUTINE: extract_attrib_file_real
! !INTERFACE:

 subroutine extract_attrib_file_real(file_descr, att_name, att_value)

! !DESCRIPTION:
!  This routine extracts a global file attribute from an io file
!  descriptor based on the attribute name.  This particular 
!  instantiation extracts a real attribute, but is aliased
!  to the generic routine name extract\_attrib\_file.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      att_name         ! name  of attribute to be extracted

   type (datafile), intent(in) :: &
      file_descr       ! data file descriptor

! !OUTPUT PARAMETERS:

   real (r4), intent(out) :: &
      att_value        ! value of attribute to be extracted

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) :: &
      n             ! loop index

   logical (log_kind) :: &
      att_exists         ! attribute already defined

!-----------------------------------------------------------------------
!
!  first check standard attributes
!
!-----------------------------------------------------------------------

   att_exists = .false.

   !*** no standard real attributes

!-----------------------------------------------------------------------
!
!  if attribute array exists, search for attribute
!
!-----------------------------------------------------------------------

   if (.not. att_exists .and. &
       associated(file_descr%add_attrib_rval)) then

      att_search: do n=1,size(file_descr%add_attrib_rval)

         if (trim(file_descr%add_attrib_rname(n))==trim(att_name)) then

            !*** found the attribute - assign the value

            att_value = file_descr%add_attrib_rval(n)
            att_exists = .true.
            exit att_search
         endif
      end do att_search
   endif

!-----------------------------------------------------------------------
!
!  if attribute not found, exit with an error
!
!-----------------------------------------------------------------------

   if (.not. att_exists) then
      if (my_task == master_task) then
         write(stdout,*) 'Attribute name: ',trim(att_name)
      endif
      call exit_POP(sigAbort,'Unknown file attribute')
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine extract_attrib_file_real

!***********************************************************************
!BOP
! !IROUTINE: extract_attrib_file_dbl
! !INTERFACE:

 subroutine extract_attrib_file_dbl(file_descr, att_name, att_value)

! !DESCRIPTION:
!  This routine extracts a global file attribute from an io file
!  descriptor based on the attribute name.  This particular 
!  instantiation extracts a double precision attribute, but is aliased
!  to the generic routine name extract\_attrib\_file.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      att_name         ! name  of attribute to be extracted

   type (datafile), intent(in) :: &
      file_descr       ! data file descriptor

! !OUTPUT PARAMETERS:

   real (r8), intent(out) :: &
      att_value        ! value of attribute to be extracted

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) :: &
      n             ! loop index

   logical (log_kind) :: &
      att_exists         ! attribute already defined

!-----------------------------------------------------------------------
!
!  first check standard attributes
!
!-----------------------------------------------------------------------

   att_exists = .false.

   !*** no standard real attributes

!-----------------------------------------------------------------------
!
!  if attribute array exists, search for attribute
!
!-----------------------------------------------------------------------

   if (.not. att_exists .and. &
       associated(file_descr%add_attrib_dval)) then

      att_search: do n=1,size(file_descr%add_attrib_dval)

         if (trim(file_descr%add_attrib_dname(n))==trim(att_name)) then

            !*** found the attribute - assign the value

            att_value = file_descr%add_attrib_dval(n)
            att_exists = .true.
            exit att_search
         endif
      end do att_search
   endif

!-----------------------------------------------------------------------
!
!  if attribute not found, exit with an error
!
!-----------------------------------------------------------------------

   if (.not. att_exists) then
      if (my_task == master_task) then
         write(stdout,*) 'Attribute name: ',trim(att_name)
      endif
      call exit_POP(sigAbort,'Unknown file attribute')
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine extract_attrib_file_dbl

!***********************************************************************
!BOP
! !IROUTINE: destroy_file
! !INTERFACE:

 subroutine destroy_file(descriptor)

! !DESCRIPTION:
!  This routine destroys a file descriptor in order to free up units
!  and memory.
!
! !REVISION HISTORY:
!  same as module

! !INPUT/OUTPUT PARAMETERS:

   type (datafile), intent(inout) :: descriptor  ! data file descriptor

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  release units if they have been assigned
!
!-----------------------------------------------------------------------

   if (descriptor%data_format == 'bin') then
      call release_unit(descriptor%id(1))
      !*** if a header file was absent, the sign of the unit was changed
      call release_unit(abs(descriptor%id(2)))
   else
      descriptor%id = 0
   endif

!-----------------------------------------------------------------------
!
!  clear data fields
!
!-----------------------------------------------------------------------

   descriptor%full_name          = char_blank
   descriptor%data_format        = char_blank
   descriptor%root_name          = char_blank
   descriptor%file_suffix        = char_blank
   descriptor%num_iotasks        = 0
   descriptor%record_length      = 0
   descriptor%current_record     = 0
   descriptor%title              = char_blank
   descriptor%history            = char_blank
   descriptor%conventions        = char_blank
   descriptor%readonly           = .false. ! reset later if necessary

!-----------------------------------------------------------------------
!
!  free up memory in additional attribute fields
!
!-----------------------------------------------------------------------

   if (associated(descriptor%add_attrib_cname)) &
      deallocate(descriptor%add_attrib_cname)

   if (associated(descriptor%add_attrib_cval)) &
      deallocate(descriptor%add_attrib_cval)

   if (associated(descriptor%add_attrib_lname)) &
      deallocate(descriptor%add_attrib_lname)

   if (associated(descriptor%add_attrib_lval)) &
      deallocate(descriptor%add_attrib_lval)

   if (associated(descriptor%add_attrib_iname)) &
      deallocate(descriptor%add_attrib_iname)

   if (associated(descriptor%add_attrib_ival)) &
      deallocate(descriptor%add_attrib_ival)

   if (associated(descriptor%add_attrib_rname)) &
      deallocate(descriptor%add_attrib_rname)

   if (associated(descriptor%add_attrib_rval)) &
      deallocate(descriptor%add_attrib_rval)

   if (associated(descriptor%add_attrib_dname)) &
      deallocate(descriptor%add_attrib_dname)

   if (associated(descriptor%add_attrib_dval)) &
      deallocate(descriptor%add_attrib_dval)

   nullify (descriptor%add_attrib_cname)
   nullify (descriptor%add_attrib_cval)
   nullify (descriptor%add_attrib_lname)
   nullify (descriptor%add_attrib_lval)
   nullify (descriptor%add_attrib_iname)
   nullify (descriptor%add_attrib_ival)
   nullify (descriptor%add_attrib_rname)
   nullify (descriptor%add_attrib_rval)
   nullify (descriptor%add_attrib_dname)
   nullify (descriptor%add_attrib_dval)

!-----------------------------------------------------------------------
!EOC

 end subroutine destroy_file

!***********************************************************************
!BOP
! !IROUTINE: construct_io_field
! !INTERFACE:

 function construct_io_field (  &
       short_name,       &
       dim1, dim2,       &
       dim3,             &
       long_name,        &
       units,            &
       grid_loc,         &
       missing_value,    &
       valid_range,      &
       field_loc,        &
       field_type,       &
       i2d_array,        &
       i3d_array,        &
       r2d_array,        &
       r3d_array,        &
       d2d_array,        &
       d3d_array)        &
    result (descriptor)

! !DESCRIPTION:
!  Constructs a new io\_field descriptor for a field which will
!  be read/written.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character(*), intent(in) :: &
      short_name                ! short (one word) name for field

   type (io_dim), intent(in) :: &
      dim1, dim2                ! dimension data types for dims 1,2

   type (io_dim), intent(in), optional :: &
      dim3                      ! dimension data type for 3rd dim

   character(*), intent(in), optional :: &
      long_name,               &! longer descriptive name for field
      units                     ! units for field

   character(4), intent(in), optional :: &
      grid_loc                  ! position of field in staggered grid

   real (r4), intent(in), optional :: &
      missing_value             ! value for missing points (eg land)

   real (r4), intent(in), dimension(2), optional :: &
      valid_range               ! valid range (min,max) for field

   integer (i4), intent(in), optional :: &  ! for ghost cell updates
      field_loc,               &! staggering location
      field_type                ! field type (scalar,vector,angle)

   !***
   !*** one (and only one) of these must be present
   !*** the extra dimension on 2d,3d arrays corresponds to block index
   !***

   integer (i4), dimension(:,:,:),   intent(in), optional, target :: &
                                                           i2d_array
   integer (i4), dimension(:,:,:,:), intent(in), optional, target :: &
                                                           i3d_array
   real    (r4), dimension(:,:,:),   intent(in), optional, target :: &
                                                           r2d_array
   real    (r4), dimension(:,:,:,:), intent(in), optional, target :: &
                                                           r3d_array
   real    (r8), dimension(:,:,:),   intent(in), optional, target :: &
                                                           d2d_array
   real    (r8), dimension(:,:,:,:), intent(in), optional, target :: &
                                                           d3d_array

! !OUTPUT PARAMETERS:

   type (io_field_desc) :: descriptor ! descriptor to be created

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  variables to describe the field
!
!-----------------------------------------------------------------------

   descriptor%short_name = char_blank
   descriptor%short_name = trim(short_name)

   descriptor%long_name = char_blank
   if (present(long_name)) then
      descriptor%long_name = trim(long_name)
   endif

   descriptor%units = char_blank
   if (present(units)) then
      descriptor%units = trim(units)
   endif

   descriptor%grid_loc = '    '
   if (present(grid_loc)) then
      descriptor%grid_loc = grid_loc
   endif

   descriptor%missing_value = undefined
   if (present(missing_value)) then
      descriptor%missing_value = missing_value
   endif

   descriptor%valid_range = undefined
   if (present(valid_range)) then
      descriptor%valid_range = valid_range
   endif

   descriptor%field_loc = field_loc_unknown
   if (present(field_loc)) then
      descriptor%field_loc = field_loc
   endif

   descriptor%field_type = field_type_unknown
   if (present(field_type)) then
      descriptor%field_type = field_type
   endif

!-----------------------------------------------------------------------
!
!  initialize id and dimension info (primarily for netCDF files)
!
!-----------------------------------------------------------------------

   descriptor%id = 0

   if (present(i3d_array) .or. present(r3d_array) .or. &
                               present(d3d_array)) then
      descriptor%nfield_dims = 3
      allocate (descriptor%field_dim(1:3))
      descriptor%field_dim(1) = dim1
      descriptor%field_dim(2) = dim2
      if (present(dim3)) then
         descriptor%field_dim(3) = dim3
      else
         call exit_POP(sigAbort, &
                       'construct_io_field: must supply 3d dim')
      endif

   else if (present(i2d_array) .or. present(r2d_array) .or. &
                                    present(d2d_array)) then
      descriptor%nfield_dims = 2
      allocate (descriptor%field_dim(1:2))
      descriptor%field_dim(1) = dim1
      descriptor%field_dim(2) = dim2
   else
      descriptor%nfield_dims = 1
      allocate (descriptor%field_dim(1))
      descriptor%field_dim(1) = dim1
   end if

   nullify (descriptor%field_i_2d)
   nullify (descriptor%field_i_3d)

   nullify (descriptor%field_r_2d)
   nullify (descriptor%field_r_3d)

   nullify (descriptor%field_d_2d)
   nullify (descriptor%field_d_3d)

   if      (present(r2d_array)) then
      descriptor%field_r_2d => r2d_array
   else if (present(r3d_array)) then
      descriptor%field_r_3d => r3d_array
   else if (present(d2d_array)) then
      descriptor%field_d_2d => d2d_array
   else if (present(d3d_array)) then
      descriptor%field_d_3d => d3d_array
   else if (present(i2d_array)) then
      descriptor%field_i_2d => i2d_array
   else if (present(i3d_array)) then
      descriptor%field_i_3d => i3d_array
   else
      call exit_POP(sigAbort, &
                    'construct_io_field: must supply data array')
   end if

!-----------------------------------------------------------------------
!
!  nullify additional field attributes
!
!-----------------------------------------------------------------------

   nullify (descriptor%add_attrib_cname)
   nullify (descriptor%add_attrib_cval)
   nullify (descriptor%add_attrib_lname)
   nullify (descriptor%add_attrib_lval)
   nullify (descriptor%add_attrib_iname)
   nullify (descriptor%add_attrib_ival)
   nullify (descriptor%add_attrib_rname)
   nullify (descriptor%add_attrib_rval)
   nullify (descriptor%add_attrib_dname)
   nullify (descriptor%add_attrib_dval)

!-----------------------------------------------------------------------
!EOC

 end function construct_io_field

!***********************************************************************
!BOP
! !IROUTINE: destroy_io_field
! !INTERFACE:

 subroutine destroy_io_field (descriptor)

! !DESCRIPTION:
!  Clears all fields of an io\_field structure and dereference all
!  pointers.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   type (io_field_desc), intent(inout) :: &
      descriptor ! descriptor to be destroyed

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  clear all fields
!
!-----------------------------------------------------------------------

   descriptor%short_name = char_blank
   descriptor%long_name  = char_blank
   descriptor%units      = char_blank
   descriptor%grid_loc   = '    '

   descriptor%missing_value   = undefined
   descriptor%valid_range     = undefined
   descriptor%id              = 0
   descriptor%nfield_dims     = 2
   descriptor%field_loc       = field_loc_unknown
   descriptor%field_type      = field_type_unknown

!-----------------------------------------------------------------------
!
!  deallocate and dereference pointers
!
!-----------------------------------------------------------------------

   deallocate(descriptor%field_dim)
   nullify(descriptor%field_dim)

   nullify (descriptor%field_i_2d)
   nullify (descriptor%field_i_3d)

   nullify (descriptor%field_r_2d)
   nullify (descriptor%field_r_3d)

   nullify (descriptor%field_d_2d)
   nullify (descriptor%field_d_3d)

   if (associated(descriptor%add_attrib_cname)) then
      deallocate (descriptor%add_attrib_cname)
      deallocate (descriptor%add_attrib_cval)
   endif
   if (associated(descriptor%add_attrib_lname)) then
      deallocate (descriptor%add_attrib_lname)
      deallocate (descriptor%add_attrib_lval)
   endif
   if (associated(descriptor%add_attrib_iname)) then
      deallocate (descriptor%add_attrib_iname)
      deallocate (descriptor%add_attrib_ival)
   endif
   if (associated(descriptor%add_attrib_rname)) then
      deallocate (descriptor%add_attrib_rname)
      deallocate (descriptor%add_attrib_rval)
   endif
   if (associated(descriptor%add_attrib_dname)) then
      deallocate (descriptor%add_attrib_dname)
      deallocate (descriptor%add_attrib_dval)
   endif

   nullify (descriptor%add_attrib_cname)
   nullify (descriptor%add_attrib_cval)
   nullify (descriptor%add_attrib_iname)
   nullify (descriptor%add_attrib_ival)
   nullify (descriptor%add_attrib_rname)
   nullify (descriptor%add_attrib_rval)
   nullify (descriptor%add_attrib_dname)
   nullify (descriptor%add_attrib_dval)

!-----------------------------------------------------------------------
!EOC

 end subroutine destroy_io_field

!***********************************************************************
!BOP
! !IROUTINE: add_attrib_io_field_char
! !INTERFACE:

 subroutine add_attrib_io_field_char(iofield, att_name, att_value)

! !DESCRIPTION:
!  This routine adds an attribute to an io field.  This
!  particular instantiation adds a character attribute, but is aliased
!  to the generic routine name add\_attrib\_io\_field.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      att_name,       &! name  of attribute to be added
      att_value        ! value of attribute to be added

! !INPUT/OUTPUT PARAMETERS:

   type (io_field_desc), intent(inout) :: &
      iofield       ! data file descriptor

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) :: &
      n,           &! loop index
      num_attribs   ! current number of attributes defined

   character (char_len), dimension(:), allocatable :: &
      name_tmp,            &! temp space for resizing attrib name  array
      val_tmp               ! temp space for resizing attrib value array

   logical (log_kind) :: &
      att_exists         ! attribute already defined

!-----------------------------------------------------------------------
!
!  if this is the first attribute, allocate space and set the attribute
!
!-----------------------------------------------------------------------

   if (.not. associated(iofield%add_attrib_cval)) then

      allocate(iofield%add_attrib_cval(1), &
               iofield%add_attrib_cname(1))

      iofield%add_attrib_cval (1) = att_value
      iofield%add_attrib_cname(1) = att_name

!-----------------------------------------------------------------------
!
!  if not the first, see if it exists and over-write value
!  if does not exist, resize the attribute array and store the 
!  attributes
!
!-----------------------------------------------------------------------

   else

      att_exists = .false.
      num_attribs = size(iofield%add_attrib_cval(:))
      att_search: do n=1,num_attribs
         if (trim(iofield%add_attrib_cname(n)) == trim(att_name)) then
            iofield%add_attrib_cval(n) = char_blank
            iofield%add_attrib_cval(n) = trim(att_value)
            att_exists = .true.
            exit att_search
         endif
      end do att_search

      if (.not. att_exists) then
         allocate(name_tmp(num_attribs), val_tmp(num_attribs))
         name_tmp(:) = iofield%add_attrib_cname(:)
         val_tmp (:) = iofield%add_attrib_cval (:)

         deallocate(iofield%add_attrib_cname)
         deallocate(iofield%add_attrib_cval )
         num_attribs = num_attribs + 1
         allocate(iofield%add_attrib_cname(num_attribs), &
                  iofield%add_attrib_cval (num_attribs))

         iofield%add_attrib_cname(1:num_attribs-1) = name_tmp
         iofield%add_attrib_cval (1:num_attribs-1) =  val_tmp
         iofield%add_attrib_cname(num_attribs) = att_name
         iofield%add_attrib_cval (num_attribs) = att_value

         deallocate(name_tmp,val_tmp)
      endif

   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine add_attrib_io_field_char

!***********************************************************************
!BOP
! !IROUTINE: add_attrib_io_field_log
! !INTERFACE:

 subroutine add_attrib_io_field_log(iofield, att_name, att_value)

! !DESCRIPTION:
!  This routine adds a field attribute to an io field.  This
!  particular instantiation adds a logical attribute, but is aliased
!  to the generic routine name add\_attrib\_io\_field.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      att_name         ! name  of attribute to be added

   logical (log_kind), intent(in) :: &
      att_value        ! value of attribute to be added

! !INPUT/OUTPUT PARAMETERS:

   type (io_field_desc), intent(inout) :: &
      iofield       ! data file descriptor

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) :: &
      n,           &! loop index
      num_attribs   ! current number of attributes defined

   character (char_len), dimension(:), allocatable :: &
      name_tmp              ! temp space for resizing attrib name  array

   logical (log_kind), dimension(:), allocatable :: &
      val_tmp               ! temp space for resizing attrib value array

   logical (log_kind) :: &
      att_exists         ! attribute already defined

!-----------------------------------------------------------------------
!
!  if this is the first attribute, allocate space and set the attribute
!
!-----------------------------------------------------------------------

   if (.not. associated(iofield%add_attrib_lval)) then

      allocate(iofield%add_attrib_lval(1), &
               iofield%add_attrib_lname(1))

      iofield%add_attrib_lval (1) = att_value
      iofield%add_attrib_lname(1) = att_name

!-----------------------------------------------------------------------
!
!  otherwise, check to see if the attribute already is defined
!  if yes, store the value
!  if no, resize the attribute array and store the attributes
!
!-----------------------------------------------------------------------

   else

      att_exists = .false.
      num_attribs = size(iofield%add_attrib_lval(:))
      att_search: do n=1,num_attribs
         if (trim(iofield%add_attrib_lname(n)) == trim(att_name)) then
            iofield%add_attrib_lval(n) = att_value
            att_exists = .true.
            exit att_search
         endif
      end do att_search

      if (.not. att_exists) then

         allocate(name_tmp(num_attribs), val_tmp(num_attribs))

         name_tmp(:) = iofield%add_attrib_lname(:)
         val_tmp (:) = iofield%add_attrib_lval (:)

         deallocate(iofield%add_attrib_lname, &
                    iofield%add_attrib_lval )

         num_attribs = num_attribs + 1

         allocate(iofield%add_attrib_lname(num_attribs), &
                  iofield%add_attrib_lval (num_attribs))

         iofield%add_attrib_lname(1:num_attribs-1) = name_tmp
         iofield%add_attrib_lval (1:num_attribs-1) =  val_tmp
         iofield%add_attrib_lname(num_attribs) = att_name
         iofield%add_attrib_lval (num_attribs) = att_value

         deallocate(name_tmp,val_tmp)
      endif

   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine add_attrib_io_field_log

!***********************************************************************
!BOP
! !IROUTINE: add_attrib_io_field_int
! !INTERFACE:

 subroutine add_attrib_io_field_int(iofield, att_name, att_value)

! !DESCRIPTION:
!  This routine adds a field attribute to an io field.  This
!  particular instantiation adds an integer attribute, but is aliased
!  to the generic routine name add\_attrib\_io\_field.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      att_name         ! name  of attribute to be added

   integer (i4), intent(in) :: &
      att_value        ! value of attribute to be added

! !INPUT/OUTPUT PARAMETERS:

   type (io_field_desc), intent(inout) :: &
      iofield       ! data file descriptor

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) :: &
      n,           &! loop index
      num_attribs   ! current number of attributes defined

   character (char_len), dimension(:), allocatable :: &
      name_tmp              ! temp space for resizing attrib name  array

   integer (i4), dimension(:), allocatable :: &
      val_tmp               ! temp space for resizing attrib value array

   logical (log_kind) :: &
      att_exists         ! attribute already defined

!-----------------------------------------------------------------------
!
!  if this is the first attribute, allocate space and set the attribute
!
!-----------------------------------------------------------------------

   if (.not. associated(iofield%add_attrib_ival)) then

      allocate(iofield%add_attrib_ival(1), &
               iofield%add_attrib_iname(1))

      iofield%add_attrib_ival (1) = att_value
      iofield%add_attrib_iname(1) = att_name

!-----------------------------------------------------------------------
!
!  otherwise, check to see if the attribute already is defined
!  if yes, store the value
!  if no, resize the attribute array and store the attributes
!
!-----------------------------------------------------------------------

   else

      att_exists = .false.
      num_attribs = size(iofield%add_attrib_ival(:))
      att_search: do n=1,num_attribs
         if (trim(iofield%add_attrib_iname(n)) == trim(att_name)) then
            iofield%add_attrib_ival(n) = att_value
            att_exists = .true.
            exit att_search
         endif
      end do att_search

      if (.not. att_exists) then

         allocate(name_tmp(num_attribs), val_tmp(num_attribs))

         name_tmp(:) = iofield%add_attrib_iname(:)
         val_tmp (:) = iofield%add_attrib_ival (:)

         deallocate(iofield%add_attrib_iname, &
                    iofield%add_attrib_ival )

         num_attribs = num_attribs + 1

         allocate(iofield%add_attrib_iname(num_attribs), &
                  iofield%add_attrib_ival (num_attribs))

         iofield%add_attrib_iname(1:num_attribs-1) = name_tmp
         iofield%add_attrib_ival (1:num_attribs-1) =  val_tmp
         iofield%add_attrib_iname(num_attribs) = att_name
         iofield%add_attrib_ival (num_attribs) = att_value

         deallocate(name_tmp,val_tmp)
      endif

   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine add_attrib_io_field_int

!***********************************************************************
!BOP
! !IROUTINE: add_attrib_io_field_real
! !INTERFACE:

 subroutine add_attrib_io_field_real(iofield, att_name, att_value)

! !DESCRIPTION:
!  This routine adds a field attribute to an io field.  This
!  particular instantiation adds a real attribute, but is aliased
!  to the generic routine name add\_attrib\_io\_field.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      att_name         ! name  of attribute to be added

   real (r4), intent(in) :: &
      att_value        ! value of attribute to be added

! !INPUT/OUTPUT PARAMETERS:

   type (io_field_desc), intent(inout) :: &
      iofield       ! io field descriptor

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) :: &
      n,           &! loop index
      num_attribs   ! current number of attributes defined

   character (char_len), dimension(:), allocatable :: &
      name_tmp              ! temp space for resizing attrib name  array

   real (r4), dimension(:), allocatable :: &
      val_tmp               ! temp space for resizing attrib value array

   logical (log_kind) :: &
      att_exists         ! attribute already defined

!-----------------------------------------------------------------------
!
!  if this is the first attribute, allocate space and set the attribute
!
!-----------------------------------------------------------------------

   if (.not. associated(iofield%add_attrib_rval)) then

      allocate(iofield%add_attrib_rval(1), &
               iofield%add_attrib_rname(1))

      iofield%add_attrib_rval (1) = att_value
      iofield%add_attrib_rname(1) = att_name

!-----------------------------------------------------------------------
!
!  otherwise, check to see if the attribute already is defined
!  if yes, store the value
!  if no, resize the attribute array and store the attributes
!
!-----------------------------------------------------------------------

   else

      att_exists = .false.
      num_attribs = size(iofield%add_attrib_rval(:))
      att_search: do n=1,num_attribs
         if (trim(iofield%add_attrib_rname(n)) == trim(att_name)) then
            iofield%add_attrib_rval(n) = att_value
            att_exists = .true.
            exit att_search
         endif
      end do att_search

      if (.not. att_exists) then

         allocate(name_tmp(num_attribs), val_tmp(num_attribs))

         name_tmp(:) = iofield%add_attrib_rname(:)
         val_tmp (:) = iofield%add_attrib_rval (:)

         deallocate(iofield%add_attrib_rname, &
                    iofield%add_attrib_rval )

         num_attribs = num_attribs + 1

         allocate(iofield%add_attrib_rname(num_attribs), &
                  iofield%add_attrib_rval (num_attribs))

         iofield%add_attrib_rname(1:num_attribs-1) = name_tmp
         iofield%add_attrib_rval (1:num_attribs-1) =  val_tmp
         iofield%add_attrib_rname(num_attribs) = att_name
         iofield%add_attrib_rval (num_attribs) = att_value

         deallocate(name_tmp,val_tmp)
      endif

   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine add_attrib_io_field_real

!***********************************************************************
!BOP
! !IROUTINE: add_attrib_io_field_dbl
! !INTERFACE:

 subroutine add_attrib_io_field_dbl(iofield, att_name, att_value)

! !DESCRIPTION:
!  This routine adds a field attribute to an io field.  This
!  particular instantiation adds a double precision attribute, but is 
!  aliased to the generic routine name add\_attrib\_io\_field.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      att_name         ! name  of attribute to be added

   real (r8), intent(in) :: &
      att_value        ! value of attribute to be added

! !INPUT/OUTPUT PARAMETERS:

   type (io_field_desc), intent(inout) :: &
      iofield       ! io field descriptor

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) :: &
      n,           &! loop index
      num_attribs   ! current number of attributes defined

   character (char_len), dimension(:), allocatable :: &
      name_tmp              ! temp space for resizing attrib name  array

   real (r8), dimension(:), allocatable :: &
      val_tmp               ! temp space for resizing attrib value array

   logical (log_kind) :: &
      att_exists         ! attribute already defined

!-----------------------------------------------------------------------
!
!  if this is the first attribute, allocate space and set the attribute
!
!-----------------------------------------------------------------------

   if (.not. associated(iofield%add_attrib_dval)) then

      allocate(iofield%add_attrib_dval(1), &
               iofield%add_attrib_dname(1))

      iofield%add_attrib_dval (1) = att_value
      iofield%add_attrib_dname(1) = att_name

!-----------------------------------------------------------------------
!
!  otherwise, check to see if the attribute already is defined
!  if yes, store the value
!  if no, resize the attribute array and store the attributes
!
!-----------------------------------------------------------------------

   else

      att_exists = .false.
      num_attribs = size(iofield%add_attrib_dval(:))
      att_search: do n=1,num_attribs
         if (trim(iofield%add_attrib_dname(n)) == trim(att_name)) then
            iofield%add_attrib_dval(n) = att_value
            att_exists = .true.
            exit att_search
         endif
      end do att_search

      if (.not. att_exists) then

         allocate(name_tmp(num_attribs), val_tmp(num_attribs))

         name_tmp(:) = iofield%add_attrib_dname(:)
         val_tmp (:) = iofield%add_attrib_dval (:)

         deallocate(iofield%add_attrib_dname, &
                    iofield%add_attrib_dval )

         num_attribs = num_attribs + 1

         allocate(iofield%add_attrib_dname(num_attribs), &
                  iofield%add_attrib_dval (num_attribs))

         iofield%add_attrib_dname(1:num_attribs-1) = name_tmp
         iofield%add_attrib_dval (1:num_attribs-1) =  val_tmp
         iofield%add_attrib_dname(num_attribs) = att_name
         iofield%add_attrib_dval (num_attribs) = att_value

         deallocate(name_tmp,val_tmp)
      endif

   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine add_attrib_io_field_dbl

!***********************************************************************
!BOP
! !IROUTINE: extract_attrib_io_field_char
! !INTERFACE:

 subroutine extract_attrib_io_field_char(iofield, att_name, att_value)

! !DESCRIPTION:
!  This routine extracts an attribute from an io field.  This
!  particular instantiation extracts a character attribute, but is 
!  aliased to the generic routine name extract\_attrib\_io\_field.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      att_name        ! name  of attribute to be extracted

   type (io_field_desc), intent(in) :: &
      iofield       ! data file descriptor

! !OUTPUT PARAMETERS:

   character (*), intent(out) :: &
      att_value        ! value of attribute to be extracted

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) :: &
      n             ! loop index

   logical (log_kind) :: &
      att_exists         ! attribute already defined

!-----------------------------------------------------------------------
!
!  first check standard attributes
!
!-----------------------------------------------------------------------

   att_exists = .false.

   select case (trim(att_name))
   case ('short_name','SHORT_NAME')
      att_value = iofield%short_name
      att_exists = .true.

   case ('long_name','LONG_NAME')
      att_exists = .true.
      att_value = iofield%long_name

   case ('units','UNITS')
      att_exists = .true.
      att_value = iofield%units

   case ('grid_loc','GRID_LOC')
      att_exists = .true.
      att_value = iofield%grid_loc

   end select

!-----------------------------------------------------------------------
!
!  if not a standard attribute, check additional attributes
!
!-----------------------------------------------------------------------

   if (.not. att_exists .and. associated(iofield%add_attrib_cval)) then

      att_search: do n=1,size(iofield%add_attrib_cval)
         if (trim(att_name) == trim(iofield%add_attrib_cname(n))) then
            att_value = iofield%add_attrib_cval(n)
            att_exists = .true.
            exit att_search
         endif
      end do att_search
   endif

!-----------------------------------------------------------------------
!
!  if attribute not found, exit with error
!
!-----------------------------------------------------------------------

   if (.not. att_exists) then
      if (my_task == master_task) then
         write(stdout,*) 'Attribute name: ',trim(att_name)
      endif
      call exit_POP(sigAbort,'Unknown iofield attribute')
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine extract_attrib_io_field_char

!***********************************************************************
!BOP
! !IROUTINE: extract_attrib_io_field_log
! !INTERFACE:

 subroutine extract_attrib_io_field_log(iofield, att_name, att_value)

! !DESCRIPTION:
!  This routine extracts an attribute from an io field.  This
!  particular instantiation extracts a logical attribute, but is 
!  aliased to the generic routine name extract\_attrib\_io\_field.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      att_name        ! name  of attribute to be extracted

   type (io_field_desc), intent(in) :: &
      iofield       ! data file descriptor

! !OUTPUT PARAMETERS:

   logical (log_kind), intent(out) :: &
      att_value        ! value of attribute to be extracted

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) :: &
      n             ! loop index

   logical (log_kind) :: &
      att_exists         ! attribute already defined

!-----------------------------------------------------------------------
!
!  first check standard attributes
!
!-----------------------------------------------------------------------

   att_exists = .false.

   !*** no standard logical attributes

!-----------------------------------------------------------------------
!
!  if not a standard attribute, check additional attributes
!
!-----------------------------------------------------------------------

   if (.not. att_exists .and. associated(iofield%add_attrib_lval)) then

      att_search: do n=1,size(iofield%add_attrib_lval)
         if (trim(att_name) == trim(iofield%add_attrib_lname(n))) then
            att_value = iofield%add_attrib_lval(n)
            att_exists = .true.
            exit att_search
         endif
      end do att_search
   endif

!-----------------------------------------------------------------------
!
!  if attribute not found, exit with error
!
!-----------------------------------------------------------------------

   if (.not. att_exists) then
      if (my_task == master_task) then
         write(stdout,*) 'Attribute name: ',trim(att_name)
      endif
      call exit_POP(sigAbort,'Unknown iofield attribute')
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine extract_attrib_io_field_log

!***********************************************************************
!BOP
! !IROUTINE: extract_attrib_io_field_int
! !INTERFACE:

 subroutine extract_attrib_io_field_int(iofield, att_name, att_value)

! !DESCRIPTION:
!  This routine extracts an attribute from an io field.  This
!  particular instantiation extracts an integer attribute, but is 
!  aliased to the generic routine name extract\_attrib\_io\_field.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      att_name        ! name  of attribute to be extracted

   type (io_field_desc), intent(in) :: &
      iofield       ! data file descriptor

! !OUTPUT PARAMETERS:

   integer (i4), intent(out) :: &
      att_value        ! value of attribute to be extracted

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) :: &
      n             ! loop index

   logical (log_kind) :: &
      att_exists         ! attribute already defined

!-----------------------------------------------------------------------
!
!  first check standard attributes
!
!-----------------------------------------------------------------------

   att_exists = .false.

   select case (trim(att_name))
   case ('id','ID')
      att_exists = .true.
      att_value = iofield%id

   case ('nfield_dims','NFIELD_DIMS')
      att_exists = .true.
      att_value = iofield%nfield_dims

   end select

!-----------------------------------------------------------------------
!
!  if not a standard attribute, check additional attributes
!
!-----------------------------------------------------------------------

   if (.not. att_exists .and. associated(iofield%add_attrib_ival)) then

      att_search: do n=1,size(iofield%add_attrib_ival)
         if (trim(att_name) == trim(iofield%add_attrib_iname(n))) then
            att_value = iofield%add_attrib_ival(n)
            att_exists = .true.
            exit att_search
         endif
      end do att_search
   endif

!-----------------------------------------------------------------------
!
!  if attribute not found, exit with error
!
!-----------------------------------------------------------------------

   if (.not. att_exists) then
      if (my_task == master_task) then
         write(stdout,*) 'Attribute name: ',trim(att_name)
      endif
      call exit_POP(sigAbort,'Unknown iofield attribute')
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine extract_attrib_io_field_int

!***********************************************************************
!BOP
! !IROUTINE: extract_attrib_io_field_real
! !INTERFACE:

 subroutine extract_attrib_io_field_real(iofield, att_name, att_value)

! !DESCRIPTION:
!  This routine extracts an attribute from an io field.  This
!  particular instantiation extracts a real attribute, but is 
!  aliased to the generic routine name extract\_attrib\_io\_field.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      att_name        ! name  of attribute to be extracted

   type (io_field_desc), intent(in) :: &
      iofield       ! data file descriptor

! !OUTPUT PARAMETERS:

   real (r4), intent(out) :: &
      att_value        ! value of attribute to be extracted

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) :: &
      n             ! loop index

   logical (log_kind) :: &
      att_exists         ! attribute already defined

!-----------------------------------------------------------------------
!
!  first check standard attributes
!
!-----------------------------------------------------------------------

   att_exists = .false.

   select case (trim(att_name))
   case ('missing_value','MISSING_VALUE')
      att_value = iofield%missing_value
      att_exists = .true.

   case ('valid_range','VALID_RANGE')
      att_exists = .true.
      !att_value = iofield%valid_range

   end select

!-----------------------------------------------------------------------
!
!  if not a standard attribute, check additional attributes
!
!-----------------------------------------------------------------------

   if (.not. att_exists .and. associated(iofield%add_attrib_rval)) then

      att_search: do n=1,size(iofield%add_attrib_rval)
         if (trim(att_name) == trim(iofield%add_attrib_rname(n))) then
            att_value = iofield%add_attrib_rval(n)
            att_exists = .true.
            exit att_search
         endif
      end do att_search
   endif

!-----------------------------------------------------------------------
!
!  if attribute not found, exit with error
!
!-----------------------------------------------------------------------

   if (.not. att_exists) then
      if (my_task == master_task) then
         write(stdout,*) 'Attribute name: ',trim(att_name)
      endif
      call exit_POP(sigAbort,'Unknown iofield attribute')
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine extract_attrib_io_field_real

!***********************************************************************
!BOP
! !IROUTINE: extract_attrib_io_field_dbl
! !INTERFACE:

 subroutine extract_attrib_io_field_dbl(iofield, att_name, att_value)

! !DESCRIPTION:
!  This routine extracts an attribute from an io field.  This
!  particular instantiation extracts a double precision attribute, but
!  is aliased to the generic routine name extract\_attrib\_io\_field.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      att_name        ! name  of attribute to be extracted

   type (io_field_desc), intent(in) :: &
      iofield       ! data file descriptor

! !OUTPUT PARAMETERS:

   real (r8), intent(out) :: &
      att_value        ! value of attribute to be extracted

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) :: &
      n             ! loop index

   logical (log_kind) :: &
      att_exists         ! attribute already defined

!-----------------------------------------------------------------------
!
!  first check standard attributes
!
!-----------------------------------------------------------------------

   att_exists = .false.

   !*** no standard dbl attributes

!-----------------------------------------------------------------------
!
!  if not a standard attribute, check additional attributes
!
!-----------------------------------------------------------------------

   if (.not. att_exists .and. associated(iofield%add_attrib_dval)) then

      att_search: do n=1,size(iofield%add_attrib_dval)
         if (trim(att_name) == trim(iofield%add_attrib_dname(n))) then
            att_value = iofield%add_attrib_dval(n)
            att_exists = .true.
            exit att_search
         endif
      end do att_search
   endif

!-----------------------------------------------------------------------
!
!  if attribute not found, exit with error
!
!-----------------------------------------------------------------------

   if (.not. att_exists) then
      if (my_task == master_task) then
         write(stdout,*) 'Attribute name: ',trim(att_name)
      endif
      call exit_POP(sigAbort,'Unknown iofield attribute')
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine extract_attrib_io_field_dbl

!***********************************************************************
!BOP
! !IROUTINE: construct_io_dim
! !INTERFACE:

 function construct_io_dim(name, length, start, stop, stride)

! !DESCRIPTION:
!  Constructs a dimension for use in defining fields for io.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character(*), intent(in) :: &
      name     ! name of dimension

   integer(i4), intent(in) :: &
      length   ! size of dimension (1 to n, but 0 means unlimited)

   integer(i4), intent(in), optional :: &
      start, stop, stride  ! For slicing and dicing

! !OUTPUT PARAMETERS:

   type (io_dim) :: construct_io_dim

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  build data structure using input values
!
!-----------------------------------------------------------------------

   construct_io_dim%name = char_blank
   construct_io_dim%name = trim(name)

   construct_io_dim%id = 0  ! will be set later using netCDF routine
   construct_io_dim%length = length

   if (present(start)) then
      construct_io_dim%start = start
   else
      construct_io_dim%start = 1
   endif

   if (present(stop)) then
      construct_io_dim%stop = stop
   else
      construct_io_dim%stop = length
   endif

   if (present(stride)) then
      construct_io_dim%stride = stride
   else
      construct_io_dim%stride = 1
   endif

!-----------------------------------------------------------------------
!EOC

 end function construct_io_dim

!***********************************************************************
!BOP
! !IROUTINE: init_io
! !INTERFACE:

 subroutine init_io

! !DESCRIPTION:
!  This routine initializes some i/o arrays and checks the validity
!  of the i/o processor number.  It also sets up netcdf datasets.
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

   integer (i4) ::  &
      nml_error      ! namelist i/o error flag

   character (8) :: &
      logdate        ! actual date at model startup
                     !   (not simulation date)

   character (10) :: &
      logtime        ! wallclock time at model startup

   character (char_len) :: &
      char_tmp       ! temp character string for filenames

   namelist /io_nml/ num_iotasks,                          &
                     lredirect_stdout, log_filename,       &
                     luse_pointer_files, pointer_filename

!-----------------------------------------------------------------------
!
!  initialize io unit manager
!
!-----------------------------------------------------------------------

   in_use = .false.                  ! no unit in use

   in_use(stdin) = .true.           ! reserved units
   in_use(stdout) = .true.
   in_use(stderr) = .true.
   in_use(nml_in) = .true.

!-----------------------------------------------------------------------
!
!  read and define namelist inputs
!
!-----------------------------------------------------------------------

   lredirect_stdout = .false.
   log_filename = 'pop.out'
   luse_pointer_files = .false.
   pointer_filename = 'pop_pointer'
   num_iotasks = 1         ! set default num io tasks

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=io_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading io_nml')
   endif

   call broadcast_scalar(num_iotasks,        master_task)
   call broadcast_scalar(lredirect_stdout,   master_task)
   call broadcast_scalar(log_filename,       master_task)
   call broadcast_scalar(luse_pointer_files, master_task)
   call broadcast_scalar(pointer_filename,   master_task)

!-----------------------------------------------------------------------
!
!  redirect stdout to a log file if requested
!
!-----------------------------------------------------------------------

   if (lredirect_stdout .and. my_task == master_task) then
       call date_and_time(date=logdate,time=logtime)
       char_tmp = char_blank
       char_tmp = trim(log_filename)
       log_filename = char_blank

       log_filename = trim(char_tmp)/&
                                     &/'.'/&
                                     &/logdate/&
                                     &/'.'/&
                                     &/logtime

       open (stdout,file=log_filename,form='formatted',status='unknown')
   end if

!-----------------------------------------------------------------------
!EOC

 end subroutine init_io

!***********************************************************************
!BOP
! !IROUTINE: get_unit
! !INTERFACE:

 subroutine get_unit(iunit)

! !DESCRIPTION:
!  This routine returns the next available i/o unit.
!  Note that {\em all} processors must call get\_unit (even if only
!  the master task is doing the i/o) to insure that
!  the in\_use array remains synchronized.
!
! !REVISION HISTORY:
!  same as module

! !OUTPUT PARAMETERS:

   integer (i4), intent(out) :: &
      iunit                     ! next free i/o unit

!EOP
!BOC
!-----------------------------------------------------------------------

   integer (i4) :: n  ! dummy loop index

!-----------------------------------------------------------------------
!
!  find next free unit
!
!-----------------------------------------------------------------------

   srch_units: do n=1,max_units
      if (.not. in_use(n)) then   ! I found one, I found one
         iunit = n
         exit srch_units
      endif
   end do srch_units

   in_use(iunit) = .true.    ! mark iunit as being in use

!-----------------------------------------------------------------------
!EOC

 end subroutine get_unit

!***********************************************************************
!BOP
! !IROUTINE: release_unit
! !INTERFACE:

 subroutine release_unit(iunit)

! !DESCRIPTION:
!  This routine releases an i/o unit (marks it as available).
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETER:

   integer (i4), intent(in) :: &
      iunit                    ! i/o unit to be released

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  mark the unit as not in use
!
!-----------------------------------------------------------------------

   in_use(iunit) = .false.  !  that was easy...

!-----------------------------------------------------------------------
!EOC

 end subroutine release_unit

!***********************************************************************

 end module io_types

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
