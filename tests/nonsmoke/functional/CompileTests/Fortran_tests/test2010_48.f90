

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module io_netcdf

!BOP
! !MODULE: io_netcdf
! !DESCRIPTION:
!  This module provides a generic input/output interface
!  for writing arrays in netCDF format.
!
! !REVISION HISTORY:
!  CVS:$Id: io_netcdf.F90,v 1.18 2002/12/20 10:21:09 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: open_read_netcdf,    &
             open_netcdf,         &
             close_netcdf,        &
             define_field_netcdf, &
             read_field_netcdf,   &
             write_field_netcdf

! !USES:

   integer, parameter, public :: i4 = selected_int_kind(6)
   integer, parameter, public :: char_len = selected_int_kind(6)
   integer, parameter, public :: r4 = selected_int_kind(6)
   integer, parameter, public :: r8 = selected_int_kind(6)
   integer, parameter, public :: log_kind = selected_int_kind(6)
   integer, parameter, public :: int_kind = selected_int_kind(6)

   integer (int_kind), public :: &
      MPI_COMM_OCN,             &! MPI communicator for ocn comms
      mpi_dbl,                  &! MPI type for dbl_kind
      my_task,                  &! MPI task number for this task
      master_task,              &! task number of master task
      cpl_task                   ! task number of coupler master task

   integer (int_kind), parameter, public :: &
      mpitag_bndy_2d        = 1,    &! MPI tags for various
      mpitag_gs             = 1000   ! communication patterns

   character (char_len), public ::  &
      char_blank          ! empty character string

  !
  ! error codes:
  !
  integer, parameter, public :: &
    nf90_noerr        = 0,      &
    nf90_ebadid       = -33,    &
    nf90_eexist       = -35,    &
    nf90_einval       = -36,    &
    nf90_eperm        = -37,    &
    nf90_enotindefine = -38,    &
    nf90_eindefine    = -39,    &
    nf90_einvalcoords = -40,    &
    nf90_emaxdims     = -41,    &
    nf90_enameinuse   = -42,    &
    nf90_enotatt      = -43,    &
    nf90_emaxatts     = -44,    &
    nf90_ebadtype     = -45,    &
    nf90_ebaddim      = -46,    &
    nf90_eunlimpos    = -47,    &
    nf90_emaxvars     = -48,    &
    nf90_enotvar      = -49,    &
    nf90_eglobal      = -50,    &
    nf90_enotnc       = -51,    &
    nf90_ests         = -52,    &
    nf90_emaxname     = -53,    &
    nf90_eunlimit     = -54,    &
    nf90_enorecvars   = -55,    &
    nf90_echar        = -56,    &
    nf90_eedge        = -57,    &
    nf90_estride      = -58,    &
    nf90_ebadname     = -59,    &
    nf90_erange       = -60,    &
    nf90_enomem       = -61,    &
    nf90_evarsize     = -62,    &
    nf90_edimsize     = -63,    &
    nf90_etrunc       = -64

  integer, parameter, public :: &
    nf90_max_name = 1, &
    nf90_max_var_dims = 1

  integer, parameter, public :: &
    nf90_byte   = 1,            &
    nf90_int1   = nf90_byte,    &
    nf90_char   = 2,            &
    nf90_short  = 3,            &
    nf90_int2   = nf90_short,   &
    nf90_int    = 4,            &
    nf90_int4   = nf90_int,     &
    nf90_float  = 5,            &
    nf90_real   = nf90_float,   &
    nf90_real4  = nf90_float,   &
    nf90_double = 6,            &
    nf90_real8  = nf90_double

   integer (int_kind), parameter, public :: &
      sigExit  =  0,    &! signal for normal exit
      sigAbort = -1      ! signal for aborting (exit due to error)

   real (r4), parameter, public :: &
      undefined = -12345._r4


   type, public :: io_dim
      integer(i4) ::  id
      integer(i4) :: length  ! 1 to n, but 0 means unlimited
      integer(i4) :: start, stop, stride  ! For slicing and dicing
      character(char_len)        :: name
   end type

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

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  module variables
!
!-----------------------------------------------------------------------


!EOC
!***********************************************************************

 contains


  ! ----- 
  ! Variable definitions and inquiry
  ! ----- 
  function nf90_def_var_Scalar(ncid, name, xtype, varid)
    integer,               intent( in) :: ncid
    character (len = *),   intent( in) :: name
    integer,               intent( in) :: xtype
    integer,               intent(out) :: varid
    integer                            :: nf90_def_var_Scalar
    
    ! Dummy - shouldn't get used
    integer, dimension(1) :: dimids
    
!    nf90_def_var_Scalar = nf_def_var(ncid, name, xtype, 0, dimids, varid)
  end function nf90_def_var_Scalar
  ! ----- 
  function nf90_def_var_oneDim(ncid, name, xtype, dimids, varid)
    integer,               intent( in) :: ncid
    character (len = *),   intent( in) :: name
    integer,               intent( in) :: xtype
    integer,               intent( in) :: dimids
    integer,               intent(out) :: varid
    integer                            :: nf90_def_var_oneDim
    
    integer, dimension(1) :: dimidsA
    dimidsA(1) = dimids
!    nf90_def_var_oneDim = nf_def_var(ncid, name, xtype, 1, dimidsA, varid)
  end function nf90_def_var_oneDim
  ! ----- 
  function nf90_def_var_ManyDims(ncid, name, xtype, dimids, varid)
    integer,               intent( in) :: ncid
    character (len = *),   intent( in) :: name
    integer,               intent( in) :: xtype
    integer, dimension(:), intent( in) :: dimids
    integer,               intent(out) :: varid
    integer                            :: nf90_def_var_ManyDims
    
!    nf90_def_var_ManyDims = nf_def_var(ncid, name, xtype, size(dimids), dimids, varid)
  end function nf90_def_var_ManyDims
  ! ----- 
  function nf90_inq_varid(ncid, name, varid)
    integer,             intent( in) :: ncid
    character (len = *), intent( in) :: name
    integer,             intent(out) :: varid
    integer                          :: nf90_inq_varid
    
!    nf90_inq_varid = nf_inq_varid(ncid, name, varid)
  end function nf90_inq_varid
  ! ----- 
  function nf90_inquire_variable(ncid, varid, name, xtype, ndims, dimids, nAtts)
    integer,                         intent( in) :: ncid, varid
    character (len = *),   optional, intent(out) :: name
    integer,               optional, intent(out) :: xtype, ndims 
    integer, dimension(:), optional, intent(out) :: dimids
    integer,               optional, intent(out) :: nAtts
    integer                                      :: nf90_inquire_variable
    
    ! Local variables
    character (len = nf90_max_name)       :: varName
    integer                               :: externalType, numDimensions
    integer, dimension(nf90_max_var_dims) :: dimensionIDs
    integer                               :: numAttributes
    
!    nf90_inquire_variable = nf_inq_var(ncid, varid, varName, externalType, &
!                                       numDimensions, dimensionIDs, numAttributes)
    if (nf90_inquire_variable == nf90_noerr) then
        if(present(name))   name                   = trim(varName)
        if(present(xtype))  xtype                  = externalType
        if(present(ndims))  ndims                  = numDimensions
        if(present(dimids)) then
            if (size(dimids) .ge. numDimensions) then
                dimids(:numDimensions) = dimensionIDs(:numDimensions)
            else
                nf90_inquire_variable = nf90_einval
            endif
        endif
        if(present(nAtts))  nAtts                  = numAttributes
    endif
  end function nf90_inquire_variable
  ! ----- 
  function nf90_rename_var(ncid, varid, newname)
    integer,             intent( in) :: ncid, varid
    character (len = *), intent( in) :: newname
    integer                          :: nf90_rename_var
    
!   nf90_rename_var = nf_rename_var(ncid, varid, newname)
  end function nf90_rename_var
  ! ----- 




 subroutine open_read_netcdf()

 end subroutine open_read_netcdf

!***********************************************************************
!BOP
! !IROUTINE: open_netcdf
! !INTERFACE:

 subroutine open_netcdf()

 end subroutine open_netcdf

!***********************************************************************
!BOP
! !IROUTINE: close_netcdf
! !INTERFACE:

 subroutine close_netcdf()

 end subroutine close_netcdf

!***********************************************************************
!BOP
! !IROUTINE: define_field_netcdf
! !INTERFACE:

 subroutine define_field_netcdf(data_file, io_field)

! !DESCRIPTION:
!  This routine defines an io field for a netCDF file.
!  When reading a file, the define routine will attempt to fill an 
!  io field structure with meta-data information from the netCDF file.
!  When writing a file, it calls the appropriate netCDF routines
!  to define all the field attributes and assign a field id.
!
! !REVISION HISTORY:
!  same as module

! !INPUT/OUTPUT PARAMETERS:

   type (datafile), intent (inout)  :: &
      data_file       ! data file in which field contained

   type (io_field_desc), intent (inout) :: &
      io_field        ! field descriptor for this field

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
      att_name        ! attribute name

   integer (i4) :: &
      iostat,      &! status flag for netCDF calls
      ncid,        &! file id for netcdf file
      varid,       &! variable id for field
      ndims,       &! number of dimensions
      dimid,       &! dimension id
      n,           &! loop index
      ncount,      &! num additional attributes
      nsize,       &! length of character strings
      itype,       &! netCDF data type
      num_atts,    &! number of variable attributes
      att_ival,    &! temp for integer attribute
      ncvals,      &! counter for number of character attributes
      nlvals,      &! counter for number of logical   attributes
      nivals,      &! counter for number of integer   attributes
      nrvals,      &! counter for number of real      attributes
      ndvals        ! counter for number of double    attributes

   logical (log_kind) ::    &
      att_lval      ! temp for logical attribute

   real (r4) ::    &
      att_rval      ! temp for real attribute

   real (r4) ::    &
      att_dval      ! temp for double attribute

   logical (log_kind) :: &
      define_error       ! error flag

!-----------------------------------------------------------------------
!
!  make sure file has been opened
!
!-----------------------------------------------------------------------

   ncid = data_file%id(1)
   define_error = .false.

   if (data_file%id(1) <= 0) then
      define_error = .true.
   endif

!  call broadcast_scalar(define_error, master_task)
   if (define_error) &
     call exit_POP(sigAbort, &
                   'attempt to define field without opening file first')

!-----------------------------------------------------------------------
!
!  for input files, get the variable id and determine number of field
!  attributes
!
!-----------------------------------------------------------------------

   if (data_file%readonly) then
      if (my_task == master_task) then
         iostat = NF90_INQ_VARID(ncid, trim(io_field%short_name), &
                                 io_field%id)
         call check(iostat)
      endif
      call broadcast_scalar(iostat, master_task)
      if (iostat /= nf90_noerr) &
         call exit_POP(sigAbort, &
                   'Error finding field in netCDF input file')

      call broadcast_scalar(io_field%id, master_task)

      if (my_task == master_task) then
         iostat = NF90_Inquire_Variable(ncid,io_field%id,nAtts=num_atts)
         call check(iostat)
      endif
      call broadcast_scalar(iostat, master_task)
      if (iostat /= nf90_noerr) &
         call exit_POP(sigAbort, &
                   'Error getting attribute count for netCDF field')

      call broadcast_scalar(num_atts, master_task)

      !***
      !*** for each attribute, define standard attributes or add
      !*** attribute to io_field
      !***

      do n=1,num_atts

         !***
         !*** get attribute name
         !***

         att_name = char_blank
!         if (my_task == master_task) then
!            iostat = nf90_inq_attname(ncid, io_field%id, n, att_name)
!         endif

         call broadcast_scalar(iostat, master_task)
         if (iostat /= nf90_noerr) &
            call exit_POP(sigAbort, &
                   'error getting netCDF field attribute name')
   
         call broadcast_scalar(att_name, master_task)

         !***
         !*** check to see if name matches any of the standard field
         !*** attributes
         !***

         select case(trim(att_name))

         case('long_name')

            io_field%long_name = char_blank

            if (my_task == master_task) then
!               iostat = nf90_inquire_attribute(ncid, io_field%id, &
!                                               'long_name', len=nsize)

               if (iostat == nf90_noerr) then
!                  iostat = nf90_get_att(ncid, io_field%id, 'long_name',&
!                                        io_field%long_name(1:nsize))
                  call check(iostat)
               endif
            endif

            call broadcast_scalar(iostat, master_task)
            if (iostat /= nf90_noerr) then
               call exit_POP(sigAbort, &
                   'Error reading long_name from netCDF file')
            endif

            call broadcast_scalar(io_field%long_name, master_task)

         case('units')

            io_field%units = char_blank

            if (my_task == master_task) then
!               iostat = nf90_inquire_attribute(ncid, io_field%id, &
!                                               'units', len=nsize)

               if (iostat == nf90_noerr) then
!                  iostat = nf90_get_att(ncid, io_field%id, 'units', &
!                                        io_field%units(1:nsize))
                  call check(iostat)
               endif
            endif

            call broadcast_scalar(iostat, master_task)
            if (iostat /= nf90_noerr) then
               call exit_POP(sigAbort, &
                   'Error reading units from netCDF file')
            endif

            call broadcast_scalar(io_field%units, master_task)

         case('grid_loc')

            io_field%grid_loc = '    '

            if (my_task == master_task) then
!               iostat = nf90_get_att(ncid, io_field%id, 'grid_loc', &
!                                     io_field%grid_loc)
               call check(iostat)
            endif

            call broadcast_scalar(iostat, master_task)
            if (iostat /= nf90_noerr) then
               call exit_POP(sigAbort, &
                   'Error reading grid_loc from netCDF file')
            endif

            call broadcast_scalar(io_field%grid_loc, master_task)

         case('missing_value')

            if (my_task == master_task) then
!               iostat = nf90_get_att(ncid, io_field%id, &
!                                     'missing_value',   &
!                                     io_field%missing_value)
               call check(iostat)
            endif

            call broadcast_scalar(iostat, master_task)
            if (iostat /= nf90_noerr) then
               call exit_POP(sigAbort, &
                   'Error reading missing_value from netCDF file')
            endif

            call broadcast_scalar(io_field%missing_value, master_task)

         case('valid_range')

            if (my_task == master_task) then
!               iostat = nf90_get_att(ncid, io_field%id, &
!                                     'valid_range',   &
!                                     io_field%valid_range)
               call check(iostat)
            endif

            call broadcast_scalar(iostat, master_task)
            if (iostat /= nf90_noerr) then
               call exit_POP(sigAbort, &
                   'Error reading valid_range from netCDF file')
            endif

            call broadcast_array(io_field%valid_range, master_task)


         case default

            !***
            !*** if does not match any of the standard file attributes
            !*** add the attribute to the datafile
            !***

            if (my_task == master_task) then
!               iostat = nf90_Inquire_Attribute(ncid, io_field%id, &
!                                               trim(att_name),    &
!                                               xtype = itype,     &
!                                               len = nsize) 
            endif
   
            call broadcast_scalar(iostat, master_task)
            if (iostat /= nf90_noerr) then
               call exit_POP(sigAbort, &
                   'Error reading netCDF file attribute')
            endif
   
            call broadcast_scalar(itype, master_task)

            select case (itype)

            case (NF90_CHAR)
               work_line = char_blank
               call broadcast_scalar(nsize, master_task)
               if (my_task == master_task) then
!                  iostat = nf90_get_att(ncid, io_field%id, &
!                                        trim(att_name),    &
!                                        work_line(1:nsize))
               endif
               call broadcast_scalar(iostat, master_task)
               if (iostat /= nf90_noerr) then
                  call exit_POP(sigAbort, &
                                'Error reading netCDF file attribute')
               endif

               call broadcast_scalar(work_line, master_task)
               call add_attrib_io_field(io_field, trim(att_name), &
                                                  trim(work_line))

            case (NF90_INT) !*** both integer and logical attributes
               if (my_task == master_task) then
!                  iostat = nf90_get_att(ncid, io_field%id, &
!                                        trim(att_name), att_ival)
               endif
               call broadcast_scalar(iostat, master_task)
               if (iostat /= nf90_noerr) then
                  call exit_POP(sigAbort, &
                                'Error reading netCDF file attribute')
               endif
   
               call broadcast_scalar(att_ival, master_task)
               if (att_name(1:4) == 'LOG_') then !*** attribute logical
                  work_line = att_name
                  work_line(1:4) = '    '
                  att_name = adjustl(work_line)

                  if (att_ival == 1) then
                     att_lval = .true.
                  else
                     att_lval = .false.
                  endif
                  call add_attrib_file(data_file, trim(att_name), &
                                                  att_lval)

               else
                  call add_attrib_file(data_file, trim(att_name), &
                                                  att_ival)
               endif

            case (NF90_FLOAT)
               if (my_task == master_task) then
!                  iostat = nf90_get_att(ncid, io_field%id, &
!                                        trim(att_name), att_rval)
               endif
               call broadcast_scalar(iostat, master_task)
               if (iostat /= nf90_noerr) then
                  call exit_POP(sigAbort, &
                                'Error reading netCDF file attribute')
               endif

               call broadcast_scalar(att_rval, master_task)
               call add_attrib_io_field(io_field, trim(att_name), &
                                                  att_rval)

            case (NF90_DOUBLE)
               if (my_task == master_task) then
!                  iostat = nf90_get_att(ncid, io_field%id, &
!                                        trim(att_name), att_dval)
               endif
               call broadcast_scalar(iostat, master_task)
               if (iostat /= nf90_noerr) then
                  call exit_POP(sigAbort, &
                                'Error reading netCDF file attribute')
               endif
   
               call broadcast_scalar(att_dval, master_task)
               call add_attrib_io_field(io_field, trim(att_name), &
                                                  att_dval)
   
            end select

         end select

      end do ! num_atts

!-----------------------------------------------------------------------
!
!  for output files, need to define everything
!  make sure file is in define mode
!
!-----------------------------------------------------------------------

   else ! output file

      if (.not. data_file%ldefine) &
        call exit_POP(sigAbort, &
                      'attempt to define field but not in define mode')

!-----------------------------------------------------------------------
!
!     define the dimensions
!
!-----------------------------------------------------------------------

! DQ (9/12/2010): Comment this out as a test.
!#if 0
      ndims = io_field%nfield_dims
      if (my_task == master_task) then
         do n = 1,ndims
            dimid = 0

            !*** check to see whether already defined

!            iostat = NF90_INQ_DIMID(ncid=ncid,                         &
!                                 name=trim(io_field%field_dim(n)%name),&
!                                 dimid=dimid)

            if (iostat /= NF90_NOERR) then ! dimension not yet defined
!               iostat = NF90_DEF_DIM (ncid=ncid,                    &
!                             name=trim(io_field%field_dim(n)%name), &
!                             len=io_field%field_dim(n)%length,      &
!                             dimid=io_field%field_dim(n)%id)
!
! DQ (9/12/2010): Comment this out as a test.
!            else
!               io_field%field_dim(n)%id = dimid
!
            end if
         end do

!-----------------------------------------------------------------------
!
!        now define the field
!
!-----------------------------------------------------------------------

         !*** check to see whether field of this name already defined.

         iostat = NF90_INQ_VARID(ncid, trim(io_field%short_name), varid)

         if (iostat /= NF90_NOERR) then ! variable was not yet defined

            if (associated (io_field%field_r_2d).or. &
                associated (io_field%field_r_3d)) then
!               iostat = NF90_DEF_VAR (ncid=ncid,                       &
!                                      name=trim(io_field%short_name),  &
!                                      xtype=NF90_FLOAT,                &
!                    dimids=(/ (io_field%field_dim(n)%id, n=1,ndims) /),&
!                                      varid=io_field%id)

            else if (associated (io_field%field_d_2d).or. &
                     associated (io_field%field_d_3d)) then
!               iostat = NF90_DEF_VAR (ncid=ncid,                      &
!                                      name=trim(io_field%short_name), &
!                                      xtype=NF90_DOUBLE,              &
!                   dimids=(/ (io_field%field_dim(n)%id, n=1,ndims) /),&
!                                      varid=io_field%id)
            else if (associated (io_field%field_i_2d).or. &
                     associated (io_field%field_i_3d)) then
!               iostat = NF90_DEF_VAR (ncid=ncid,                      &
!                                      name=trim(io_field%short_name), &
!                                      xtype=NF90_INT,                 &
!                   dimids=(/ (io_field%field_dim(n)%id, n=1,ndims) /),&
!                                      varid=io_field%id)
            else
               define_error = .true.
            end if
            call check(iostat)
            if (iostat /= nf90_noerr) define_error = .true.
            varid = io_field%id
         else ! Variable was previously defined, OK to use it
            io_field%id = varid
         end if
      end if ! master task
!#endif

      call broadcast_scalar(define_error, master_task)
      if (define_error) call exit_POP(sigAbort, &
                                      'Error defining netCDF field')

!-----------------------------------------------------------------------
!
!     Now define the field attributes
!
!-----------------------------------------------------------------------

      if (my_task == master_task) then

         !*** long_name

         if (io_field%long_name /= char_blank) then
!            iostat = NF90_INQUIRE_ATTRIBUTE(ncid=NCID, varid=varid, &
!                                            name='long_name')
            if (iostat /= NF90_NOERR) then ! attrib probably not defined
!               iostat = NF90_PUT_ATT(ncid=NCID, varid=varid, &
!                                     name='long_name',       &
!                                     values=trim(io_field%long_name))
               call check(iostat)
               if (iostat /= NF90_NOERR) define_error = .true.
            end if
         endif

         !*** units

         if (io_field%units /= char_blank) then
!            iostat = NF90_INQUIRE_ATTRIBUTE(ncid=NCID, varid=varid, &
!                                            name='units')
            if (iostat /= NF90_NOERR) then ! attrib probably not defined
!               iostat = NF90_PUT_ATT(ncid=NCID, varid=varid, &
!                                     name='units',           &
!                                     values=trim(io_field%units))
               call check(iostat)
               if (iostat /= NF90_NOERR) define_error = .true.
            end if
         endif

         !*** grid_loc
 
         if (io_field%grid_loc /= '    ') then
!            iostat = NF90_INQUIRE_ATTRIBUTE(ncid=NCID, varid=varid, &
!                                            name='grid_loc')
            if (iostat /= NF90_NOERR) then ! attrib probably not defined
!               iostat = NF90_PUT_ATT(ncid=NCID, varid=varid, &
!                                     name='grid_loc',        &
!                                     values=io_field%grid_loc)
               call check(iostat)
               if (iostat /= NF90_NOERR) define_error = .true.
            end if
         endif

         !*** missing_value

         if (io_field%missing_value == undefined) then
!            iostat = NF90_INQUIRE_ATTRIBUTE(ncid=NCID, varid=varid, &
!                                            name='missing_value')
            if (iostat /= NF90_NOERR) then ! attrib probably not defined
!               iostat = NF90_PUT_ATT(ncid=NCID, varid=varid, &
!                                     name='missing_value',   &
!                                     values=io_field%missing_value)
               call check(iostat)
               if (iostat /= NF90_NOERR) define_error = .true.
            end if
         endif

         !*** valid_range(1:2)

         if (any(io_field%valid_range /= undefined)) then
!            iostat = NF90_INQUIRE_ATTRIBUTE(ncid=NCID, varid=varid, &
!                                            name='valid_range')
            if (iostat /= NF90_NOERR) then ! attrib probably not yet defined
!               iostat = NF90_PUT_ATT(ncid=NCID, varid=varid, &
!                                     name='valid_range',       &
!                                     values=io_field%valid_range(:))
               call check(iostat)
               if (iostat /= NF90_NOERR) define_error = .true.
            end if
         endif

         !*** additional attributes if defined

         ncvals = 0
         nlvals = 0
         nivals = 0
         nrvals = 0
         ndvals = 0
         if (associated(io_field%add_attrib_cval)) &
            ncvals = size(io_field%add_attrib_cval)
         if (associated(io_field%add_attrib_lval)) &
            nlvals = size(io_field%add_attrib_lval)
         if (associated(io_field%add_attrib_ival)) &
            nivals = size(io_field%add_attrib_ival)
         if (associated(io_field%add_attrib_rval)) &
            nrvals = size(io_field%add_attrib_rval)
         if (associated(io_field%add_attrib_dval)) &
            ndvals = size(io_field%add_attrib_dval)

         do n=1,ncvals
!            iostat = NF90_PUT_ATT(ncid=NCID, varid=varid,             &
!                         name=trim(io_field%add_attrib_cname(n)), &
!                         values=trim(io_field%add_attrib_cval(n)))
            call check(iostat)
            if (iostat /= NF90_NOERR) define_error = .true.
         end do

         do n=1,nlvals
            work_line = 'LOG_'/&
                               &/trim(io_field%add_attrib_lname(n))
!            iostat = NF90_PUT_ATT(ncid=NCID, varid=varid,             &
!                         name=trim(work_line),                        &
!                         values=io_field%add_attrib_ival(n))
            call check(iostat)
            if (iostat /= NF90_NOERR) define_error = .true.
         end do

         do n=1,nivals
!            iostat = NF90_PUT_ATT(ncid=NCID, varid=varid,             &
!                         name=trim(io_field%add_attrib_iname(n)),     &
!                         values=io_field%add_attrib_ival(n))
            call check(iostat)
            if (iostat /= NF90_NOERR) define_error = .true.
         end do

         do n=1,nrvals
!            iostat = NF90_PUT_ATT(ncid=NCID, varid=varid,             &
!                         name=trim(io_field%add_attrib_rname(n)),     &
!                         values=io_field%add_attrib_rval(n))
            call check(iostat)
            if (iostat /= NF90_NOERR) define_error = .true.
         end do

         do n=1,ndvals
!            iostat = NF90_PUT_ATT(ncid=NCID, varid=varid,             &
!                         name=trim(io_field%add_attrib_dname(n)),     &
!                         values=io_field%add_attrib_dval(n))
            call check(iostat)
            if (iostat /= NF90_NOERR) define_error = .true.
         end do

      endif ! master_task

      call broadcast_scalar(define_error, master_task)
      if (define_error) call exit_POP(sigAbort, &
                        'Error adding attributes to field')

   endif ! input/output file

!-----------------------------------------------------------------------
!EOC

 end subroutine define_field_netcdf



 subroutine write_field_netcdf(data_file, io_field)

! !DESCRIPTION:
!  This routine writes a field to a netCDF data file.
!
! !REVISION HISTORY:
!  same as module

! !INPUT/OUTPUT PARAMETERS:

   type (datafile), intent (inout)  :: &
      data_file             ! file to which field will be written

   type (io_field_desc), intent (inout) :: &
      io_field              ! field to write to file

 end subroutine write_field_netcdf

!***********************************************************************
!BOP
! !IROUTINE: read_field_netcdf
! !INTERFACE:

 subroutine read_field_netcdf(data_file, io_field)

! !DESCRIPTION:
!  This routine reads a field from a netcdf input file.
!
! !REVISION HISTORY:
!  same as module

! !INPUT/OUTPUT PARAMETERS:

   type (datafile), intent (inout)  :: &
      data_file              ! file from which to read field

   type (io_field_desc), intent (inout) :: &
      io_field               ! field to be read

 end subroutine read_field_netcdf

!***********************************************************************
!BOP
! !IROUTINE:  check
! !INTERFACE:

 subroutine check(status)

! !DESCRIPTION:
!  This exception handler subroutine can be used to check error status
!  after a netcdf call.  It prints out a text message assigned to
!  an error code but does not exit because this routine is typically
!  only called from a single process.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (i4), intent (in) ::  &
      status                     ! status returned by netCDF call

 end subroutine check

!***********************************************************************

 end module io_netcdf

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


