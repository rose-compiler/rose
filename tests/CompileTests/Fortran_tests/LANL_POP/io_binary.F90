!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module io_binary

!BOP
! !MODULE: io_binary
! !DESCRIPTION:
!  This module provides a binary parallel input/output interface
!  for writing fields.
!
! !REVISION HISTORY:
!  CVS:$Id: io_binary.F90,v 1.18 2002/12/20 10:21:09 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod
   use domain_size
   use domain
   use constants
   use boundary
   use communicate
   use broadcast
   use gather_scatter
   use exit_mod
   use io_types

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: open_read_binary,    &
             open_binary,         &
             close_binary,        &
             define_field_binary, &
             read_field_binary,   &
             write_field_binary

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  module variables
!
!-----------------------------------------------------------------------

   character (1), parameter :: &
      attrib_separator=':'  ! character separating attrib name and value

!-----------------------------------------------------------------------
!
!  binary interfaces for i/o routines
!
!-----------------------------------------------------------------------

   interface write_array
      module procedure write_int_2d,   &
                       write_real4_2d, &
                       write_real8_2d, &
                       write_int_3d,   &
                       write_real4_3d, &
                       write_real8_3d
   end interface

   interface read_array
     module procedure read_int_2d,   &
                      read_real4_2d, &
                      read_real8_2d, &
                      read_int_3d,   &
                      read_real4_3d, &
                      read_real8_3d
   end interface

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: open_read_binary
! !INTERFACE:

 subroutine open_read_binary(data_file)

! !DESCRIPTION:
!  This routine opens a binary data file (and header file if it
!  exists) for reading.  It also reads global file attributes.
!
! !REVISION HISTORY:
!  same as module

! !INPUT/OUTPUT PARAMETERS:

   type (datafile), intent (inout)  :: &
      data_file       ! file to be opened

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   character (80) :: &
      path,          &! filename to read
      work_line,     &! temporary to use for parsing file lines
      comp_line,     &! temporary to use for parsing file lines
      data_type,     &! temporary to use for data type
      att_name        ! attribute name

   integer (i4) ::  &
      n,            &! loop index
      att_ival,     &! value for integer attribute
      hdr_error,    &! error for reading header file
      cindx1,cindx2  ! indices for unpacking character strings

   logical (log_kind) ::     &
      att_lval       ! value for logical attribute

   real (r4) ::     &
      att_rval       ! value for real attribute

   real (r8) ::     &
      att_dval       ! value for double attribute

   logical (log_kind) :: &
      header_exists,     &! flag to check existence of header file
      lmatch              ! flag to use for attribute search

!-----------------------------------------------------------------------
!
!  set the readonly flag in the data file descriptor
!
!-----------------------------------------------------------------------

   data_file%readonly = .true.

!-----------------------------------------------------------------------
!
!  if unit not assigned already, assign a unit (id) to this file
!
!-----------------------------------------------------------------------

   if (data_file%id(1) == 0) then
     call get_unit(data_file%id(1))
     call get_unit(data_file%id(2))
   endif

!-----------------------------------------------------------------------
!
!  check to see whether a header file exists
!    if so, open it
!    if not, set the unit to -unit so it can be released later
!
!-----------------------------------------------------------------------

   header_exists = .false.

   if (my_task == master_task) then
      path = trim(data_file%full_name)/&
             &/'.hdr'

! DQ (9/12/2010): Commented out incomplete handling of inqure support.
!     inquire(file=path, exist=header_exists)

      if (header_exists) then
         open (unit=data_file%id(2), file=path, status='old')
      else
         write(stdout,*) 'WARNING: Input header file does not exist'
         write(stdout,*) 'for file: ',trim(path)
         write(stdout,*) &
                  'Assuming fields will be read in sequential order'
      endif
   endif

   call broadcast_scalar(header_exists, master_task)

   if (.not. header_exists) then
      data_file%id(2) = -data_file%id(2)
      data_file%current_record = 1
   endif

!-----------------------------------------------------------------------
!
!  now open data file
!
!-----------------------------------------------------------------------

   if (my_task < data_file%num_iotasks) then
      path = trim(data_file%full_name)
      open (unit=data_file%id(1),action='read',status='unknown', &
            file=trim(path), form='unformatted',access='direct', &
            recl=data_file%record_length)
   endif

!-----------------------------------------------------------------------
!
!  if the header file exists read file attributes from header file
!
!-----------------------------------------------------------------------

   if (header_exists) then

      work_line = char_blank
      hdr_error = 0

      if (my_task == master_task) then
         !*** first read until hit beginning of global attribute
         !*** definition section

         do while (hdr_error == 0 .and. work_line(1:7) /= '&GLOBAL')
            read(data_file%id(2),'(a80)',iostat=hdr_error) comp_line
            work_line = adjustl(comp_line)
         end do
      end if

      !*** now read until hit end of global attribute definition

      att_loop: do while (hdr_error == 0)

         work_line = char_blank
         if (my_task == master_task) then
            read(data_file%id(2),'(a80)',iostat=hdr_error) comp_line
            work_line = adjustl(comp_line)
         endif

         call broadcast_scalar(work_line, master_task)
         call broadcast_scalar(hdr_error, master_task)

         if (work_line(1:1) == '/') exit att_loop

         !*** find location of separator in the string
         !*** to determine name of attribute and extract
         !*** attribute name from beginning of string

         cindx1 = index(work_line,attrib_separator)
         att_name = char_blank
         att_name(1:cindx1-1) = work_line(1:cindx1-1)

         comp_line = work_line
         do n=1,cindx1
            comp_line(n:n) = ' '
         end do
         work_line = adjustl(comp_line)

         !*** now find location of separator in the string
         !*** to determine data type of attribute and extract
         !*** data type from beginning of string

         cindx1 = index(work_line,attrib_separator)
         data_type = char_blank
         data_type(1:cindx1-1) = work_line(1:cindx1-1)

         comp_line = work_line
         do n=1,cindx1
            comp_line(n:n) = ' '
         end do
         work_line = adjustl(comp_line)

         !*** check for standard file attributes

         select case(trim(att_name))
         case ('title','TITLE')
            data_file%title = trim(work_line)
         case ('history','HISTORY')
            data_file%history = trim(work_line)
         case ('conventions','CONVENTIONS')
            data_file%conventions = trim(work_line)
         case default

            !*** if additional attributes exist, add them as
            !*** additional attributes
            !*** note that if they are already defined, the
            !*** call to add_attrib will overwrite value

            select case (trim(data_type))
            case('char','CHAR')
               call add_attrib_file(data_file, trim(att_name), &
                                               trim(work_line))
            case('log','LOG','logical','LOGICAL')
               read(work_line,*) att_lval 
               call add_attrib_file(data_file, trim(att_name), att_lval)
            case('int','INT','i4','I4')
               read(work_line,*) att_ival 
               call add_attrib_file(data_file, trim(att_name), att_ival)
            case('r4','R4','REAL','real','float','FLOAT')
               read(work_line,*) att_rval 
               call add_attrib_file(data_file, trim(att_name), att_rval)
            case('r8','R8','dbl','DBL','double','DOUBLE')
               read(work_line,*) att_dval 
               call add_attrib_file(data_file, trim(att_name), att_dval)
            end select

         end select
      end do att_loop

   endif ! header exists

!-----------------------------------------------------------------------
!EOC

 end subroutine open_read_binary

!***********************************************************************
!BOP
! !IROUTINE: open_binary
! !INTERFACE:

 subroutine open_binary(data_file)

! !DESCRIPTION:
!  This routine opens a binary data file and header file for writing
!  and writes global file attributes to the header file.
!
! !REVISION HISTORY:
!  same as module

! !INPUT/OUTPUT PARAMETERS:

   type (datafile), intent (inout)  :: &
      data_file              ! file to open

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   character (255) :: path

   character (char_len) :: &
      work_line  ! temp space for manipulating strings 

   character (5), parameter :: &
      hdr_fmt = '(a80)'

   integer (i4) ::    &
      n,              &! loop index
      cindx1, cindx2, &! indices for character strings
      hdr_unit         ! unit number for header file

!-----------------------------------------------------------------------
!
!  assign units to file if not assigned already
!
!-----------------------------------------------------------------------

   if (data_file%id(1) == 0) then
      call get_unit(data_file%id(1))
      call get_unit(data_file%id(2))
   endif

!-----------------------------------------------------------------------
!
!  open data and header files for writing
!
!-----------------------------------------------------------------------

   !*** open header file from one task only

   if (my_task == master_task) then
      path = trim(data_file%full_name)/&
                                       &/'.hdr'
      open (unit=data_file%id(2), file=path, status='unknown')
      hdr_unit = data_file%id(2)
   end if

   path = trim(data_file%full_name)

   !*** open data file from all io tasks

   if (my_task < data_file%num_iotasks) then
      open(data_file%id(1), file=path, access='direct',      &
           form='unformatted', recl=data_file%record_length, &
           status='unknown')
   endif

   !*** initialize record number

   data_file%current_record = 1

!-----------------------------------------------------------------------
!
!  write attributes to header file
!
!-----------------------------------------------------------------------

   if (my_task == master_task) then

      work_line = char_blank
      work_line(1:7) = '&GLOBAL'
      write(hdr_unit,hdr_fmt) work_line

      work_line = char_blank
      work_line(1:5) = 'title'
      work_line(6:6) = attrib_separator
      work_line(7:10) = 'char'
      work_line(11:11) = attrib_separator
      write(work_line(12:),*) trim(data_file%title)
      write(hdr_unit,hdr_fmt) work_line

      work_line = char_blank
      work_line(1:7) = 'history'
      work_line(8:8) = attrib_separator
      work_line(9:12) = 'char'
      work_line(13:13) = attrib_separator
      write(work_line(14:),*) trim(data_file%history)
      write(hdr_unit,hdr_fmt) work_line

      work_line = char_blank
      work_line(1:11) = 'conventions'
      work_line(12:12) = attrib_separator
      work_line(11:14) = 'char'
      work_line(15:15) = attrib_separator
      write(work_line(16:),*) trim(data_file%conventions)
      write(hdr_unit,hdr_fmt) work_line

      !*** if additional attributes are defined in the
      !*** file definition, write these as well

      if (associated(data_file%add_attrib_cval)) then
         do n=1,size(data_file%add_attrib_cval)

            work_line = char_blank
            cindx1 = 1
            cindx2 = len_trim(data_file%add_attrib_cname(n))
            work_line(cindx1:cindx2) = trim(data_file%add_attrib_cname(n))
            cindx1 = cindx2 + 1
            cindx2 = cindx1
            work_line(cindx1:cindx2) = attrib_separator
            cindx1 = cindx2 + 1
            cindx2 = cindx1 + 3
            work_line(cindx1:cindx2) = 'char'
            cindx1 = cindx2 + 1
            cindx2 = cindx1
            work_line(cindx1:cindx2) = attrib_separator
            cindx1 = cindx2 + 1
            write(work_line(cindx1:),*) trim(data_file%add_attrib_cval(n))
            write(hdr_unit,hdr_fmt) work_line

         end do
      endif ! cval

      if (associated(data_file%add_attrib_lval)) then
         do n=1,size(data_file%add_attrib_lval)

            work_line = char_blank
            cindx1 = 1
            cindx2 = len_trim(data_file%add_attrib_lname(n))
            work_line(cindx1:cindx2)=trim(data_file%add_attrib_lname(n))
            cindx1 = cindx2 + 1
            cindx2 = cindx1
            work_line(cindx1:cindx2) = attrib_separator
            cindx1 = cindx2 + 1
            cindx2 = cindx1 + 2
            work_line(cindx1:cindx2) = 'log'
            cindx1 = cindx2 + 1
            cindx2 = cindx1
            work_line(cindx1:cindx2) = attrib_separator
            cindx1 = cindx2 + 1
            write(work_line(cindx1:),*) data_file%add_attrib_lval(n)
            write(hdr_unit,hdr_fmt) work_line

         end do
      endif ! lval

      if (associated(data_file%add_attrib_ival)) then
         do n=1,size(data_file%add_attrib_ival)

            work_line = char_blank
            cindx1 = 1
            cindx2 = len_trim(data_file%add_attrib_iname(n))
            work_line(cindx1:cindx2)=trim(data_file%add_attrib_iname(n))
            cindx1 = cindx2 + 1
            cindx2 = cindx1
            work_line(cindx1:cindx2) = attrib_separator
            cindx1 = cindx2 + 1
            cindx2 = cindx1 + 2
            work_line(cindx1:cindx2) = 'int'
            cindx1 = cindx2 + 1
            cindx2 = cindx1
            work_line(cindx1:cindx2) = attrib_separator
            cindx1 = cindx2 + 1
            write(work_line(cindx1:),*) data_file%add_attrib_ival(n)
            write(hdr_unit,hdr_fmt) work_line

         end do
      endif ! ival

      if (associated(data_file%add_attrib_rval)) then
         do n=1,size(data_file%add_attrib_rval)

            work_line = char_blank
            cindx1 = 1
            cindx2 = len_trim(data_file%add_attrib_rname(n))
            work_line(cindx1:cindx2) = &
                                trim(data_file%add_attrib_rname(n))
            cindx1 = cindx2 + 1
            cindx2 = cindx1
            work_line(cindx1:cindx2) = attrib_separator
            cindx1 = cindx2 + 1
            cindx2 = cindx1 + 1
            work_line(cindx1:cindx2) = 'r4'
            cindx1 = cindx2 + 1
            cindx2 = cindx1
            work_line(cindx1:cindx2) = attrib_separator
            cindx1 = cindx2 + 1
            write(work_line(cindx1:),*) data_file%add_attrib_rval(n)
            write(hdr_unit,hdr_fmt) work_line

         end do
      endif ! rval

      if (associated(data_file%add_attrib_dval)) then
         do n=1,size(data_file%add_attrib_dval)

            work_line = char_blank
            cindx1 = 1
            cindx2 = len_trim(data_file%add_attrib_dname(n))
            work_line(cindx1:cindx2)=trim(data_file%add_attrib_dname(n))
            cindx1 = cindx2 + 1
            cindx2 = cindx1
            work_line(cindx1:cindx2) = attrib_separator
            cindx1 = cindx2 + 1
            cindx2 = cindx1 + 1
            work_line(cindx1:cindx2) = 'r8'
            cindx1 = cindx2 + 1
            cindx2 = cindx1
            work_line(cindx1:cindx2) = attrib_separator
            cindx1 = cindx2 + 1
            write(work_line(cindx1:),*) data_file%add_attrib_dval(n)
            write(hdr_unit,hdr_fmt) work_line

         end do
      endif ! dval

   endif ! master task

!-----------------------------------------------------------------------
!EOC

 end subroutine open_binary

!***********************************************************************
!BOP
! !IROUTINE: close_binary
! !INTERFACE:

 subroutine close_binary(data_file)

! !DESCRIPTION:
!  This routine closes an open binary data file and the associated
!  header file if it exists.
!
! !REVISION HISTORY:
!  same as module

! !INPUT/OUTPUT PARAMETERS:

   type (datafile), intent (inout)  :: data_file

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  close a data file
!
!-----------------------------------------------------------------------

   if (my_task < data_file%num_iotasks) close(data_file%id(1))

   if (my_task == master_task .and. data_file%id(2) > 0) &
      close(data_file%id(2))

!-----------------------------------------------------------------------
!EOC

 end subroutine close_binary

!***********************************************************************
!BOP
! !IROUTINE: define_field_binary
! !INTERFACE:

 subroutine define_field_binary(data_file, io_field)

! !DESCRIPTION:
!  This routine defines an io field.  When writing a file, this means
!  computing the starting record number for the field and then writing
!  the field information to the header file.  When reading a file,
!  the define routine will attempt to fill an io field with information
!  from the header file; if a header file does not exist, it will leave
!  the io field mostly undefined and will rely on an input record from
!  the read routine to locate the field in the file (this latter is
!  provided for compatibility with forcing and grid files).
!
! !REVISION HISTORY:
!  same as module

! !INPUT/OUTPUT PARAMETERS:

   type (datafile), intent (inout)  :: &
      data_file

   type (io_field_desc), intent (inout) :: &
      io_field

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   character (80) :: &
      work_line,     &! workspace for manipulating input string
      comp_line,     &! comparison string
      att_name,      &! attribute name string
      ctype           ! attribute data type

   integer (i4) ::  &
      n,            &! loop index
      att_ival,     &! temp value for integer attribute
      hdr_error,    &! io error status for header file
      cindx1,       &! character string index
      cindx2,       &! character string index
      unit           ! unit for header file

   logical (log_kind) :: &
      att_lval   ! temp value for logical attribute

   real (r4) :: &
      att_rval   ! temp value for real attribute

   real (r8) :: &
      att_dval   ! temp value for double attribute

!-----------------------------------------------------------------------
!
!  for output files, set id to current record number and write
!  field attributes to header file.
!
!-----------------------------------------------------------------------

   if (.not. data_file%readonly) then

      !*** define id as current record

      io_field%id = data_file%current_record

      !*** increment record counter for next field

      if (io_field%nfield_dims == 2) then

         data_file%current_record = data_file%current_record + 1

      else if (io_field%nfield_dims == 3) then

         if (associated(io_field%field_i_3d)) then
            n = size(io_field%field_i_3d,dim=3)
         else if (associated(io_field%field_r_3d)) then
            n = size(io_field%field_r_3d,dim=3)
         else if (associated(io_field%field_d_3d)) then
            n = size(io_field%field_d_3d,dim=3)
         endif

         data_file%current_record = data_file%current_record + n

      endif

      !***
      !*** now write attributes to header file
      !***

      unit = data_file%id(2)

      if (my_task == master_task) then

         !*** first write variable name header

         write(unit,'(a1,a)') '&',trim(io_field%short_name)

         !*** write standard attributes to header file

         if (io_field%long_name /= char_blank)                           &
            write(unit,'(a9,a1,a4,a1,a)') 'long_name', attrib_separator, &
                                          'char',      attrib_separator, &
                                          trim(io_field%long_name)

         if (io_field%units /= char_blank)                           &
            write(unit,'(a5,a1,a4,a1,a)') 'units', attrib_separator, &
                                          'char' , attrib_separator, &
                                          trim(io_field%units)

         if (io_field%grid_loc /= '    ')                                &
            write(unit,'(a8,a1,a4,a1,a4)') 'grid_loc', attrib_separator, &
                                           'char'    , attrib_separator, &
                                           io_field%grid_loc

         if (io_field%missing_value /= undefined) then
            work_line = char_blank
            work_line( 1:13) = 'missing_value'
            work_line(14:14) = attrib_separator
            work_line(15:16) = 'r8'
            work_line(17:17) = attrib_separator
            write(work_line(18:),*) io_field%missing_value
            write(unit,'(a)') trim(work_line)
         endif

         if (any(io_field%valid_range(:) /= undefined)) then
            work_line = char_blank
            work_line( 1:11) = 'valid_range'
            work_line(12:12) = attrib_separator
            work_line(13:14) = 'r8'
            work_line(15:15) = attrib_separator
            write(work_line(16:),*) io_field%valid_range(:)
            write(unit,'(a)') trim(work_line)
         endif

         work_line = char_blank
         work_line(1:2) = 'id'
         work_line(3:3) = attrib_separator
         work_line(4:6) = 'int'
         work_line(7:7) = attrib_separator
         write(work_line(8:),*) io_field%id
         write(unit,'(a)') trim(work_line)

         !**** number of dimensions and dimension info

         work_line = char_blank
         work_line( 1:11) = 'nfield_dims'
         work_line(12:12) = attrib_separator
         work_line(13:15) = 'int'
         work_line(16:16) = attrib_separator
         write(work_line(17:),*) io_field%nfield_dims
         write(unit,'(a)') trim(work_line)

         !**** not sure what to do about dimension info

         ! write field_dim stuff

         !*** write additional attributes to header file

         if (associated(io_field%add_attrib_cval)) then
            do n=1,size(io_field%add_attrib_cval)

               work_line = char_blank
               cindx1 = len_trim(io_field%add_attrib_cname(n))
               cindx2 = cindx1 + 7
               work_line(1:cindx1) = trim(io_field%add_attrib_cname(n))
               work_line(cindx1+1:cindx1+1) = attrib_separator 
               work_line(cindx1+2:cindx2-2) = 'char'
               work_line(cindx2-1:cindx2-1) = attrib_separator 
               cindx1 = cindx2 + len_trim(io_field%add_attrib_cval(n))
               work_line(cindx2:cindx2) = trim(io_field%add_attrib_cval(n))
               write(unit,'(a)') trim(work_line)
            end do
         endif

         if (associated(io_field%add_attrib_lval)) then
            do n=1,size(io_field%add_attrib_lval)
   
               work_line = char_blank
               cindx1 = len_trim(io_field%add_attrib_lname(n))
               cindx2 = cindx1 + 6
               work_line(1:cindx1) = trim(io_field%add_attrib_lname(n))
               work_line(cindx1+1:cindx1+1) = attrib_separator 
               work_line(cindx1+2:cindx2-2) = 'log'
               work_line(cindx2-1:cindx2-1) = attrib_separator 
               write(work_line(cindx2:),*) io_field%add_attrib_lval(n)
               write(unit,'(a)') trim(work_line)
            end do
         endif

         if (associated(io_field%add_attrib_ival)) then
            do n=1,size(io_field%add_attrib_ival)
   
               work_line = char_blank
               cindx1 = len_trim(io_field%add_attrib_iname(n))
               cindx2 = cindx1 + 6
               work_line(1:cindx1) = trim(io_field%add_attrib_iname(n))
               work_line(cindx1+1:cindx1+1) = attrib_separator 
               work_line(cindx1+2:cindx2-2) = 'int'
               work_line(cindx2-1:cindx2-1) = attrib_separator 
               write(work_line(cindx2:),*) io_field%add_attrib_ival(n)
               write(unit,'(a)') trim(work_line)
            end do
         endif

         if (associated(io_field%add_attrib_rval)) then
            do n=1,size(io_field%add_attrib_rval)
   
               work_line = char_blank
               cindx1 = len_trim(io_field%add_attrib_rname(n))
               cindx2 = cindx1 + 5
               work_line(1:cindx1) = trim(io_field%add_attrib_rname(n))
               work_line(cindx1+1:cindx1+1) = attrib_separator 
               work_line(cindx1+2:cindx2-2) = 'r4'
               work_line(cindx2-1:cindx2-1) = attrib_separator 
               write(work_line(cindx2:),*) io_field%add_attrib_rval(n)
               write(unit,'(a)') trim(work_line)
            end do
         endif

         if (associated(io_field%add_attrib_dval)) then
            do n=1,size(io_field%add_attrib_dval)
   
               work_line = char_blank
               cindx1 = len_trim(io_field%add_attrib_dname(n))
               cindx2 = cindx1 + 5
               work_line(1:cindx1) = trim(io_field%add_attrib_dname(n))
               work_line(cindx1+1:cindx1+1) = attrib_separator 
               work_line(cindx1+2:cindx2-2) = 'r8'
               work_line(cindx2-1:cindx2-1) = attrib_separator 
               write(work_line(cindx2:),*) io_field%add_attrib_dval(n)
               write(unit,'(a)') work_line
               write(unit,'(a)') trim(work_line)
            end do
         endif

         !*** end variable attribute section with a delimiter

         write(unit,'(a1)') '/'

      endif ! master task

!-----------------------------------------------------------------------
!
!  attempt to define an io field from an input header file
!
!-----------------------------------------------------------------------

   else ! this is an input file

      unit = data_file%id(2)
      if (unit <= 0) then  ! no header file, assume fields are defined 
                           ! in the order they exist in input file
                           ! set id as current record and increment

         io_field%id = data_file%current_record

         if (associated(io_field%field_i_2d)) then
            data_file%current_record = data_file%current_record + 1
         else if (associated(io_field%field_i_3d)) then
            data_file%current_record = data_file%current_record + &
                                       size(io_field%field_i_3d, dim=3)
         else if (associated(io_field%field_r_2d)) then
            data_file%current_record = data_file%current_record + 1
         else if (associated(io_field%field_r_3d)) then
            data_file%current_record = data_file%current_record + &
                                       size(io_field%field_r_3d, dim=3)
         else if (associated(io_field%field_d_2d)) then
            data_file%current_record = data_file%current_record + 1
         else if (associated(io_field%field_d_3d)) then
            data_file%current_record = data_file%current_record + &
                                       size(io_field%field_d_3d, dim=3)
         else
            call exit_POP(sigAbort, &
             'define: No known binary field descriptor associated')
         end if

         
      else  ! header exists: read all attributes from header file
         
         if (my_task == master_task) then
            hdr_error = 0
            rewind (unit)
            cindx1 = len_trim(io_field%short_name) + 1
            comp_line(1:1) = '&'
            comp_line(2:cindx1) = trim(io_field%short_name)

            srch_loop: do while (hdr_error == 0) ! look for field in file

               read(unit,'(a80)',iostat=hdr_error) work_line
               work_line = adjustl(work_line)
               if (work_line(1:cindx1) == comp_line(1:cindx1)) &
                  exit srch_loop
            end do srch_loop
         endif ! master_task

         call broadcast_scalar(hdr_error, master_task)
         if (hdr_error /= 0) call exit_POP(sigAbort,&
            'could not find field in binary header file')

         !*** found io_field definition - now extract field attributes

         att_loop: do

            if (my_task == master_task) then
               read(unit,'(a80)',iostat=hdr_error) work_line
               work_line = adjustl(work_line)
            endif

            call broadcast_scalar(hdr_error, master_task)
            call broadcast_scalar(work_line, master_task)

            !*** exit if error reading or reached end of definition

            if (hdr_error /= 0 .or. work_line(1:1) == '/') &
               exit att_loop

            !*** extract attribute name

            cindx1 = index(work_line,attrib_separator)
            att_name = char_blank
            att_name(1:cindx1-1) = work_line(1:cindx1-1)

            !*** strip attribute name from input line

            comp_line = work_line
            do n=1,cindx1
               comp_line(n:n) = ' '
            end do
            work_line = adjustl(comp_line)

            !*** extract attribute data type

            cindx1 = index(work_line,attrib_separator)
            ctype = char_blank
            ctype(1:cindx1-1) = work_line(1:cindx1-1)

            !*** strip data type from input line

            comp_line = work_line
            do n=1,cindx1
               comp_line(n:n) = ' '
            end do
            work_line = adjustl(comp_line)

            !*** if this attribute matches a standard attribute
            !*** set attribute value.  otherwise, define an
            !*** additional attribute

            select case (trim(att_name))

            case('long_name','LONG_NAME')
               io_field%long_name = char_blank
               io_field%long_name = trim(work_line)

            case('units','UNITS')
               io_field%units = char_blank
               io_field%units = trim(work_line)

            case('grid_loc','GRID_LOC')
               io_field%grid_loc = char_blank
               io_field%grid_loc = trim(work_line)

            case('missing_value','MISSING_VALUE')
               read(work_line,*) io_field%missing_value

            case('valid_range','VALID_RANGE')
               read(work_line,*) io_field%valid_range(:)

            case('id','ID')
               read(work_line,*) io_field%id

            case('field_dim','FIELD_DIM')
               !**** do not know what to do about dimensions yet

            case('nfield_dims','NFIELD_DIMS')
               read(work_line,*) io_field%nfield_dims

            case default

               !*** add attribute to io field

               select case (ctype)
               case ('char','CHAR','character','CHARACTER')
                  call add_attrib_io_field(io_field, trim(att_name), &
                                                     trim(work_line))

               case ('log','LOG','logical','LOGICAL')
                  read(work_line,*) att_lval
                  call add_attrib_io_field(io_field, trim(att_name), &
                                                     att_lval)

               case ('int','INT','i4','I4','integer','INTEGER')
                  read(work_line,*) att_ival
                  call add_attrib_io_field(io_field, trim(att_name), &
                                                     att_ival)

               case ('r4','R4','real','REAL','float','FLOAT')
                  read(work_line,*) att_rval
                  call add_attrib_io_field(io_field, trim(att_name), &
                                                     att_rval)

               case ('r8','R8','dbl','DBL','double','DOUBLE')
                  read(work_line,*) att_dval
                  call add_attrib_io_field(io_field, trim(att_name), &
                                                     att_dval)

               case default
                  call exit_POP(sigAbort, &
                                'define_io_field: unknown data type')
               end select

            end select ! att_name

         end do att_loop

         if (hdr_error /= 0) call exit_POP(sigAbort, &
            'define_io_field: error reading attribute from header')

      endif ! unit > 0 (header exists)

   endif !readonly

!-----------------------------------------------------------------------
!EOC

 end subroutine define_field_binary

!***********************************************************************
!BOP
! !IROUTINE: write_field_binary
! !INTERFACE:

 subroutine write_field_binary(data_file, io_field)

! !INPUT PARAMETERS:

   type (datafile), intent (in)  :: &
      data_file             ! file to which data will be written

   type (io_field_desc), intent (in) :: &
      io_field              ! field to be written

! !DESCRIPTION:
!  This routine writes a binary field to the data file.
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

   integer (i4) :: &
      record        ! starting record for writing array

!-----------------------------------------------------------------------
!
!  extract the starting record from the io field
!
!-----------------------------------------------------------------------

   record = io_field%id

!-----------------------------------------------------------------------
!
!  write the io field
!
!-----------------------------------------------------------------------

   if (associated(io_field%field_i_2d)) then
      call write_array(data_file,io_field%field_i_2d,record)
   else if (associated(io_field%field_i_3d)) then
      call write_array(data_file,io_field%field_i_3d,record)
   else if (associated(io_field%field_r_2d)) then
      call write_array(data_file,io_field%field_r_2d,record)
   else if (associated(io_field%field_r_3d)) then
      call write_array(data_file,io_field%field_r_3d,record)
   else if (associated(io_field%field_d_2d)) then
      call write_array(data_file,io_field%field_d_2d,record)
   else if (associated(io_field%field_d_3d)) then
      call write_array(data_file,io_field%field_d_3d,record)
   else
      call exit_POP(sigAbort, &
                   'write: No known binary field descriptor associated')
   end if

!-----------------------------------------------------------------------
!EOC

 end subroutine write_field_binary

!***********************************************************************
!BOP
! !IROUTINE: read_field_binary
! !INTERFACE:

 subroutine read_field_binary(data_file, io_field)

! !DESCRIPTION:
!  This routine reads a field from a binary input file.
!
! !REVISION HISTORY:
!  same as module

! !INPUT/OUTPUT PARAMETERS:

   type (datafile), intent (inout)  :: &
      data_file             ! file from which to read data

   type (io_field_desc), intent (inout) :: &
      io_field              ! data to be read

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  if the field location parameters undefined, assume center scalar
!
!-----------------------------------------------------------------------

   if (io_field%field_loc == field_loc_unknown) then
      io_field%field_loc  = field_loc_center
      io_field%field_type = field_type_scalar
   endif

!-----------------------------------------------------------------------
!
!  read an io field
!
!-----------------------------------------------------------------------

   if (associated(io_field%field_i_2d)) then
      call read_array(data_file,io_field%field_i_2d,io_field%id, &
                                io_field%field_loc, io_field%field_type)
   else if (associated(io_field%field_i_3d)) then
      call read_array(data_file,io_field%field_i_3d,io_field%id, &
                                io_field%field_loc, io_field%field_type)
   else if (associated(io_field%field_r_2d)) then
      call read_array(data_file,io_field%field_r_2d,io_field%id, &
                                io_field%field_loc, io_field%field_type)
   else if (associated(io_field%field_r_3d)) then
      call read_array(data_file,io_field%field_r_3d,io_field%id, &
                                io_field%field_loc, io_field%field_type)
   else if (associated(io_field%field_d_2d)) then
      call read_array(data_file,io_field%field_d_2d,io_field%id, &
                                io_field%field_loc, io_field%field_type)
   else if (associated(io_field%field_d_3d)) then
      call read_array(data_file,io_field%field_d_3d,io_field%id, &
                                io_field%field_loc, io_field%field_type)
   else
      call exit_POP(sigAbort,'read_field: field not associated')
   end if

!-----------------------------------------------------------------------
!EOC

 end subroutine read_field_binary

!***********************************************************************
!BOP
! !IROUTINE: read_int_2d
! !INTERFACE:

 subroutine read_int_2d(data_file, INT2D, start_record, &
                        field_loc, field_type)

! !DESCRIPTION:
!  Reads a 2-d horizontal slice of integers from a binary file
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   type (datafile), intent(in) :: &
      data_file                   ! info about data file

   integer (i4), intent(in) :: &
      start_record,            &! starting record of field in file
      field_loc,               &! loc of field on horiz grid
      field_type                ! type of field (scalar, vector, angle)

! !INPUT/OUTPUT PARAMETERS:

   integer (i4), dimension(:,:,:), intent(inout) :: &
      INT2D                        ! array to be read

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4), dimension(:,:), allocatable ::  &
      IOBUFI                ! local global-sized buffer

!-----------------------------------------------------------------------
!
!  read in global 2-d slice from one processor
!
!-----------------------------------------------------------------------

   if (my_task == master_task) then
      allocate(IOBUFI(nx_global,ny_global))
      read(data_file%id(1),rec=start_record) IOBUFI
   endif

!-----------------------------------------------------------------------
!
!  send chunks to processors who own them
!
!-----------------------------------------------------------------------

   call scatter_global(INT2D, IOBUFI, master_task, distrb_clinic, &
                       field_loc, field_type)

   if (my_task == master_task) deallocate(IOBUFI)

!-----------------------------------------------------------------------
!EOC

 end subroutine read_int_2d

!***********************************************************************
!BOP
! !IROUTINE: read_real4_2d
! !INTERFACE:

 subroutine read_real4_2d(data_file, REAL2D, start_record, &
                          field_loc, field_type)

! !DESCRIPTION:
!  Reads a 2-d horizontal slice of reals from a binary file
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   type (datafile), intent(in) :: &
      data_file                   ! info about data file

   integer (i4), intent(in) :: &
      start_record,            &! starting record of field in file
      field_loc,               &! loc of field on horiz grid
      field_type                ! type of field (scalar, vector, angle)

! !INPUT/OUTPUT PARAMETERS:

   real (r4), dimension(:,:,:), intent(inout) :: &
      REAL2D                    ! array to be read

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   real (r4), dimension(:,:), allocatable ::  &
      IOBUFR                ! local global-sized buffer

!-----------------------------------------------------------------------
!
!  read in global 2-d slice from one processor
!
!-----------------------------------------------------------------------

   if (my_task == master_task) then
      allocate(IOBUFR(nx_global,ny_global))
      read(data_file%id(1),rec=start_record) IOBUFR
   endif

!-----------------------------------------------------------------------
!
!  send chunks to processors who own them
!
!-----------------------------------------------------------------------

   call scatter_global(REAL2D, IOBUFR, master_task, distrb_clinic, &
                       field_loc, field_type)

   if (my_task == master_task) deallocate(IOBUFR)

!-----------------------------------------------------------------------
!EOC

 end subroutine read_real4_2d

!***********************************************************************
!BOP
! !IROUTINE: read_real8_2d
! !INTERFACE:

 subroutine read_real8_2d(data_file, DBL2D, start_record, &
                          field_loc, field_type)

! !DESCRIPTION:
!  Reads a 2-d horizontal slice of 64-bit reals from a binary file
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   type (datafile), intent(in) :: &
      data_file                   ! info about data file

   integer (i4), intent(in) :: &
      start_record,            &! starting record of field in file
      field_loc,               &! loc of field on horiz grid
      field_type                ! type of field (scalar, vector, angle)

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(:,:,:), intent(inout) :: &
      DBL2D                        ! array to be read

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   real (r8), dimension(:,:), allocatable ::  &
      IOBUFD                ! local global-sized buffer

!-----------------------------------------------------------------------
!
!  read in global 2-d slice from one processor
!
!-----------------------------------------------------------------------

   if (my_task == master_task) then
      allocate(IOBUFD(nx_global,ny_global))
      read(data_file%id(1),rec=start_record) IOBUFD
   endif

!-----------------------------------------------------------------------
!
!  send chunks to processors who own them
!
!-----------------------------------------------------------------------

   call scatter_global(DBL2D, IOBUFD, master_task, distrb_clinic, &
                       field_loc, field_type)

   if (my_task == master_task) deallocate(IOBUFD)

!-----------------------------------------------------------------------
!EOC

 end subroutine read_real8_2d

!***********************************************************************
!BOP
! !IROUTINE: read_int_3d
! !INTERFACE:

 subroutine read_int_3d(data_file, INT3D, start_record, &
                        field_loc, field_type)

! !DESCRIPTION:
!  Reads a 3-d integer array from a binary file
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   type (datafile), intent(in) :: &
      data_file                   ! info on input data file

   integer (i4), intent(in) :: &
      start_record,            &! starting record of field in file
      field_loc,               &! loc of field on horiz grid
      field_type                ! type of field (scalar, vector, angle)

! !INPUT/OUTPUT PARAMETERS:

   integer (i4), dimension(:,:,:,:), intent(inout) ::  &
      INT3D  ! array to be read

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) k,n,krecs,klvl,nz

   integer (i4), dimension(:,:), allocatable ::  &
      IOBUFI                ! global-sized array buffer

!-----------------------------------------------------------------------
!
!  determine the number of records each i/o process must read to
!  get all the records
!
!-----------------------------------------------------------------------

   nz = size(INT3D, DIM=3)

   if (mod(nz,data_file%num_iotasks) == 0) then
      krecs = nz/data_file%num_iotasks
   else
      krecs = nz/data_file%num_iotasks + 1
   endif

   if (my_task < data_file%num_iotasks) &
      allocate(IOBUFI(nx_global,ny_global))

!-----------------------------------------------------------------------
!
!  each i/o process reads a horizontal slab from a record and sends
!  the data to processors who own it. read and distribute num_iotasks
!  records at a time to keep the messages from getting mixed up.
!
!-----------------------------------------------------------------------

   do k=1,krecs

      if (my_task < data_file%num_iotasks) then
         klvl = (k-1)*data_file%num_iotasks + my_task + 1
         if (klvl <= nz) then
            read(data_file%id(1), rec=start_record+klvl-1) IOBUFI
         endif
      endif

      do n=1,data_file%num_iotasks
         klvl = (k-1)*data_file%num_iotasks + n
         if (klvl <= nz) then
            call scatter_global(INT3D(:,:,klvl,:), IOBUFI, n-1, &
                                distrb_clinic, field_loc, field_type)
         endif
      end do

   end do

   if (my_task < data_file%num_iotasks) deallocate(IOBUFI)

!-----------------------------------------------------------------------
!EOC

 end subroutine read_int_3d

!***********************************************************************
!BOP
! !IROUTINE: read_real4_3d
! !INTERFACE:

 subroutine read_real4_3d(data_file, REAL3D, start_record, &
                          field_loc, field_type)

! !DESCRIPTION:
!  Reads a 3-d real array from a binary file
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   type (datafile), intent(in) :: &
      data_file                   ! info on input data file

   integer (i4), intent(in) :: &
      start_record,            &! starting record of field in file
      field_loc,               &! loc of field on horiz grid
      field_type                ! type of field (scalar, vector, angle)

! !INPUT/OUTPUT PARAMETERS:

   real (r4), dimension(:,:,:,:), intent(inout) ::  &
      REAL3D  ! array to be read

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) k,n,krecs,klvl,nz

   real (r4), dimension(:,:), allocatable ::  &
      IOBUFR                ! global-sized array buffer

!-----------------------------------------------------------------------
!
!  determine the number of records each i/o process must read to
!  get all the records
!
!-----------------------------------------------------------------------

   nz = size(REAL3D, DIM=3)

   if (mod(nz,data_file%num_iotasks) == 0) then
      krecs = nz/data_file%num_iotasks
   else
      krecs = nz/data_file%num_iotasks + 1
   endif

   if (my_task < data_file%num_iotasks) &
      allocate(IOBUFR(nx_global,ny_global))

!-----------------------------------------------------------------------
!
!  each i/o process reads a horizontal slab from a record and sends
!  the data to processors who own it. read and distribute num_iotasks
!  records at a time to keep the messages from getting mixed up.
!
!-----------------------------------------------------------------------

   do k=1,krecs

      if (my_task < data_file%num_iotasks) then
         klvl = (k-1)*data_file%num_iotasks + my_task + 1
         if (klvl <= nz) then
            read(data_file%id(1), rec=start_record+klvl-1) IOBUFR
         endif
      endif

      do n=1,data_file%num_iotasks
         klvl = (k-1)*data_file%num_iotasks + n
         if (klvl <= nz) then
            call scatter_global(REAL3D(:,:,klvl,:), IOBUFR, n-1, &
                                distrb_clinic, field_loc, field_type)
         endif
      end do

   end do

   if (my_task < data_file%num_iotasks) deallocate(IOBUFR)

!-----------------------------------------------------------------------
!EOC

 end subroutine read_real4_3d

!***********************************************************************
!BOP
! !IROUTINE: read_real8_3d
! !INTERFACE:

 subroutine read_real8_3d(data_file, DBL3D, start_record, &
                          field_loc, field_type)

! !DESCRIPTION:
!  Reads a 3-d 64-bit real array from a binary file
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   type (datafile), intent(in) :: &
      data_file                   ! info on input data file

   integer (i4), intent(in) :: &
      start_record,            &! starting record of field in file
      field_loc,               &! loc of field on horiz grid
      field_type                ! type of field (scalar, vector, angle)

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(:,:,:,:), intent(inout) ::  &
      DBL3D  ! array to be read

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) k,n,krecs,klvl,nz

   real (r8), dimension(:,:), allocatable ::  &
      IOBUFD                ! global-sized array buffer

!-----------------------------------------------------------------------
!
!  determine the number of records each i/o process must read to
!  get all the records
!
!-----------------------------------------------------------------------

   nz = size(DBL3D, DIM=3)

   if (mod(nz,data_file%num_iotasks) == 0) then
      krecs = nz/data_file%num_iotasks
   else
      krecs = nz/data_file%num_iotasks + 1
   endif

   if (my_task < data_file%num_iotasks) &
      allocate(IOBUFD(nx_global,ny_global))

!-----------------------------------------------------------------------
!
!  each i/o process reads a horizontal slab from a record and sends
!  the data to processors who own it. read and distribute num_iotasks
!  records at a time to keep the messages from getting mixed up.
!
!-----------------------------------------------------------------------

   do k=1,krecs

      if (my_task < data_file%num_iotasks) then
         klvl = (k-1)*data_file%num_iotasks + my_task + 1
         if (klvl <= nz) then
            read(data_file%id(1), rec=start_record+klvl-1) IOBUFD
         endif
      endif

      do n=1,data_file%num_iotasks
         klvl = (k-1)*data_file%num_iotasks + n
         if (klvl <= nz) then
            call scatter_global(DBL3D(:,:,klvl,:), IOBUFD, n-1, &
                                distrb_clinic, field_loc, field_type)
         endif
      end do

   end do

   if (my_task < data_file%num_iotasks) deallocate(IOBUFD)

!-----------------------------------------------------------------------
!EOC

 end subroutine read_real8_3d

!***********************************************************************
!BOP
! !IROUTINE: write_int_2d
! !INTERFACE:

 subroutine write_int_2d(data_file,INT2D,start_record)

! !DESCRIPTION:
!  Writes a 2-d slab of integers to a binary file.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   type (datafile), intent(in) :: &
      data_file                   ! file information

   integer (i4), intent(in) ::  &
      start_record              ! starting record of array in file

   integer (i4), dimension(:,:,:), intent(in) ::  &
      INT2D                 ! array to be written

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4), dimension(:,:), allocatable ::  &
      IOBUFI                ! local global-sized buffer

!-----------------------------------------------------------------------
!
!  receive chunks from processors who own them
!
!-----------------------------------------------------------------------

   if (my_task == master_task) allocate(IOBUFI(nx_global,ny_global))

   call gather_global(IOBUFI, INT2D, master_task, distrb_clinic)

!-----------------------------------------------------------------------
!
!  write global 2-d slice from one processor
!
!-----------------------------------------------------------------------

   if (my_task == master_task) then
      write(data_file%id(1),rec=start_record) IOBUFI
      deallocate(IOBUFI)
   endif

!-----------------------------------------------------------------------
!EOC
 end subroutine write_int_2d

!***********************************************************************
!BOP
! !IROUTINE: write_real4_2d
! !INTERFACE:

 subroutine write_real4_2d(data_file,REAL2D,start_record)

! !DESCRIPTION:
!  Writes a 2-d slab of reals to a binary file.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   type (datafile), intent(in) :: &
      data_file                   ! file information

   integer (i4), intent(in) ::  &
      start_record              ! starting record of array in file

   real (r4), dimension(:,:,:), intent(in) ::  &
      REAL2D                 ! array to be written

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   real (r4), dimension(:,:), allocatable ::  &
      IOBUFR                ! local global-sized buffer

!-----------------------------------------------------------------------
!
!  receive chunks from processors who own them
!
!-----------------------------------------------------------------------

   if (my_task == master_task) allocate(IOBUFR(nx_global,ny_global))
   call gather_global(IOBUFR, REAL2D, master_task, distrb_clinic)

!-----------------------------------------------------------------------
!
!  write global 2-d slice from one processor
!
!-----------------------------------------------------------------------

   if (my_task == master_task) then
      write(data_file%id(1),rec=start_record) IOBUFR
      deallocate(IOBUFR)
   endif

!-----------------------------------------------------------------------
!EOC
 end subroutine write_real4_2d

!***********************************************************************
!BOP
! !IROUTINE: write_real8_2d
! !INTERFACE:

 subroutine write_real8_2d(data_file,DBL2D,start_record)

! !DESCRIPTION:
!  Writes a 2-d slab of doubles to a binary file.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   type (datafile), intent(in) :: &
      data_file                   ! file information

   integer (i4), intent(in) ::  &
      start_record              ! starting record of array in file

   real (r8), dimension(:,:,:), intent(in) ::  &
      DBL2D                 ! array to be written

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   real (r8), dimension(:,:), allocatable ::  &
      IOBUFD                ! local global-sized buffer

!-----------------------------------------------------------------------
!
!  receive chunks from processors who own them
!
!-----------------------------------------------------------------------

   if (my_task == master_task) allocate(IOBUFD(nx_global,ny_global))
   call gather_global(IOBUFD, DBL2D, master_task, distrb_clinic)

!-----------------------------------------------------------------------
!
!  write global 2-d slice from one processor
!
!-----------------------------------------------------------------------

   if (my_task == master_task) then
      write(data_file%id(1),rec=start_record) IOBUFD
      deallocate(IOBUFD)
   endif

!-----------------------------------------------------------------------
!EOC
 end subroutine write_real8_2d

!***********************************************************************
!BOP
! !IROUTINE: write_int_3d
! !INTERFACE:

 subroutine write_int_3d(data_file,INT3D,start_record)

! !DESCRIPTION:
!  Writes a 3-d integer array as a series of 2-d slabs to a binary
!  output file.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   type (datafile), intent(in) :: &
      data_file                   ! info on output data file

   integer (i4), intent(in) :: &
      start_record             ! starting position of array in file

   integer (i4), dimension(:,:,:,:), intent(in) :: &
      INT3D                      ! array to be written

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) ::  &
      k, n,   &  ! dummy counters
      krecs,  &  ! number of records each iotask must write
      klvl,   &  ! k index corresponding to current record
      nz         ! size of 3rd dimension of 3-d array to be written

   integer (i4), dimension(:,:), allocatable ::  &
      IOBUFI                ! global-sized buffer array

!-----------------------------------------------------------------------
!
!  determine the number of records each i/o process must write to
!  get all the records
!
!-----------------------------------------------------------------------

   nz = size(INT3D, DIM=3)

   if (mod(nz,data_file%num_iotasks) == 0) then
      krecs = nz/data_file%num_iotasks
   else
      krecs = nz/data_file%num_iotasks + 1
   endif

   if (my_task < data_file%num_iotasks) &
      allocate(IOBUFI(nx_global,ny_global))

!-----------------------------------------------------------------------
!
!  gather and write num_iotasks records at a time to keep the
!  messages from getting mixed up.
!
!-----------------------------------------------------------------------

   do k=1,krecs

!-----------------------------------------------------------------------
!
!     gather a global slice on each iotask
!
!-----------------------------------------------------------------------

      do n=1,data_file%num_iotasks
         klvl = (k-1)*data_file%num_iotasks + n
         if (klvl <= nz) then
            call gather_global(IOBUFI, INT3D(:,:,klvl,:), &
                               n-1, distrb_clinic)
         endif
      end do

!-----------------------------------------------------------------------
!
!     iotasks wait for the gather to be complete and write global
!     slabs to the file
!
!-----------------------------------------------------------------------

      if (my_task < data_file%num_iotasks) then
         klvl = (k-1)*data_file%num_iotasks + my_task + 1
         if (klvl <= nz) then
            write(data_file%id(1), rec=start_record+klvl-1) IOBUFI
         endif
      endif

   end do

!-----------------------------------------------------------------------

   if (my_task < data_file%num_iotasks) deallocate(IOBUFI)

!-----------------------------------------------------------------------
!EOC

 end subroutine write_int_3d

!***********************************************************************
!BOP
! !IROUTINE: write_real4_3d
! !INTERFACE:

 subroutine write_real4_3d(data_file,REAL3D,start_record)

! !DESCRIPTION:
!  Writes a 3-d real array as a series of 2-d slabs to a binary
!  output file.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   type (datafile), intent(in) :: &
      data_file                   ! info on output data file

   integer (i4), intent(in) :: &
      start_record             ! starting position of array in file

   real (r4), dimension(:,:,:,:), intent(in) :: &
      REAL3D                      ! array to be written

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) ::  &
      k, n,   &  ! dummy counters
      krecs,  &  ! number of records each iotask must write
      klvl,   &  ! k index corresponding to current record
      nz         ! size of 3rd dimension of 3-d array to be written

   real (r4), dimension(:,:), allocatable ::  &
      IOBUFR                ! global-sized buffer array

!-----------------------------------------------------------------------
!
!  determine the number of records each i/o process must write to
!  get all the records
!
!-----------------------------------------------------------------------

   nz = size(REAL3D, DIM=3)

   if (mod(nz,data_file%num_iotasks) == 0) then
      krecs = nz/data_file%num_iotasks
   else
      krecs = nz/data_file%num_iotasks + 1
   endif

   if (my_task < data_file%num_iotasks) &
      allocate(IOBUFR(nx_global,ny_global))

!-----------------------------------------------------------------------
!
!  gather and write num_iotasks records at a time to keep the
!  messages from getting mixed up.
!
!-----------------------------------------------------------------------

   do k=1,krecs

!-----------------------------------------------------------------------
!
!     gather a global slice on each iotask
!
!-----------------------------------------------------------------------

      do n=1,data_file%num_iotasks
         klvl = (k-1)*data_file%num_iotasks + n
         if (klvl <= nz) then
            call gather_global(IOBUFR, REAL3D(:,:,klvl,:), &
                               n-1, distrb_clinic)
         endif
      end do

!-----------------------------------------------------------------------
!
!     iotasks wait for the gather to be complete and write global
!     slabs to the file
!
!-----------------------------------------------------------------------

      if (my_task < data_file%num_iotasks) then
         klvl = (k-1)*data_file%num_iotasks + my_task + 1
         if (klvl <= nz) then
            write(data_file%id(1), rec=start_record+klvl-1) IOBUFR
         endif
      endif

   end do

!-----------------------------------------------------------------------

   if (my_task < data_file%num_iotasks) deallocate(IOBUFR)

!-----------------------------------------------------------------------
!EOC

 end subroutine write_real4_3d

!***********************************************************************
!BOP
! !IROUTINE: write_real8_3d
! !INTERFACE:

 subroutine write_real8_3d(data_file,DBL3D,start_record)

! !DESCRIPTION:
!  Writes a 3-d 64-bit real array as a series of 2-d slabs to a binary
!  output file.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   type (datafile), intent(in) :: &
      data_file                   ! info on output data file

   integer (i4), intent(in) :: &
      start_record             ! starting position of array in file

   real (r8), dimension(:,:,:,:), intent(in) :: &
      DBL3D                      ! array to be written

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (i4) ::  &
      k, n,   &  ! dummy counters
      krecs,  &  ! number of records each iotask must write
      klvl,   &  ! k index corresponding to current record
      nz         ! size of 3rd dimension of 3-d array to be written

   real (r8), dimension(:,:), allocatable ::  &
      IOBUFD                ! global-sized buffer array

!-----------------------------------------------------------------------
!
!  determine the number of records each i/o process must write to
!  get all the records
!
!-----------------------------------------------------------------------

   nz = size(DBL3D, DIM=3)

   if (mod(nz,data_file%num_iotasks) == 0) then
      krecs = nz/data_file%num_iotasks
   else
      krecs = nz/data_file%num_iotasks + 1
   endif

   if (my_task < data_file%num_iotasks) &
      allocate(IOBUFD(nx_global,ny_global))

!-----------------------------------------------------------------------
!
!  gather and write num_iotasks records at a time to keep the
!  messages from getting mixed up.
!
!-----------------------------------------------------------------------

   do k=1,krecs

!-----------------------------------------------------------------------
!
!     gather a global slice on each iotask
!
!-----------------------------------------------------------------------

      do n=1,data_file%num_iotasks
         klvl = (k-1)*data_file%num_iotasks + n
         if (klvl <= nz) then
            call gather_global(IOBUFD, DBL3D(:,:,klvl,:), &
                               n-1, distrb_clinic)
         endif
      end do

!-----------------------------------------------------------------------
!
!     iotasks wait for the gather to be complete and write global
!     slabs to the file
!
!-----------------------------------------------------------------------

      if (my_task < data_file%num_iotasks) then
         klvl = (k-1)*data_file%num_iotasks + my_task + 1
         if (klvl <= nz) then
            write(data_file%id(1), rec=start_record+klvl-1) IOBUFD
         endif
      endif

   end do

!-----------------------------------------------------------------------

   if (my_task < data_file%num_iotasks) deallocate(IOBUFD)

!-----------------------------------------------------------------------
!EOC

 end subroutine write_real8_3d

!***********************************************************************

 end module io_binary

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
