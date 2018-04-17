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

! !USES:

   use kinds_mod
   use domain_size
   use domain
   use constants
   use communicate
   use boundary
   use broadcast
   use gather_scatter
   use exit_mod

#if 1
   use io_types
   use netcdf
#endif

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
#if 0
   public :: nf90_inq_dimid

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
#endif

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

 subroutine open_read_netcdf()
 end subroutine open_read_netcdf

 subroutine open_netcdf()
 end subroutine open_netcdf

 subroutine close_netcdf()
 end subroutine close_netcdf

#if 0
  function nf90_inq_dimid(ncid, name, dimid)
    integer,             intent( in) :: ncid
    character (len = *), intent( in) :: name
    integer,             intent(out) :: dimid
    integer                          :: nf90_inq_dimid

    nf90_inq_dimid = 0
  end function nf90_inq_dimid
#endif

!***********************************************************************
!BOP
! !IROUTINE: define_field_netcdf
! !INTERFACE:

! subroutine define_field_netcdf(data_file, io_field)
 subroutine define_field_netcdf(io_field)

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

!   type (datafile), intent (inout)  :: data_file       ! data file in which field contained

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

   integer :: nf90_noerr

#if 0
! DQ ((9/12/2010): Comment this out as a test.
            iostat = NF90_INQ_DIMID(ncid=ncid,                         &
                                 name=trim(io_field%field_dim(n)%name),&
                                 dimid=dimid)
#endif

#if 1
! DQ ((9/12/2010): Comment this out as a test.
            iostat = NF90_INQ_DIMID(ncid,att_name,dimid)
            iostat = NF90_INQ_DIMID(ncid,io_field%field_dim(n)%name,dimid)
#endif

!-----------------------------------------------------------------------
!
!  make sure file has been opened
!
!-----------------------------------------------------------------------

!-----------------------------------------------------------------------
!
!  for input files, get the variable id and determine number of field
!  attributes
!
!-----------------------------------------------------------------------

!   if (data_file%readonly) then
!   else ! output file
!      if (my_task == master_task) then
!         do n = 1,ndims
            if (iostat /= NF90_NOERR) then ! dimension not yet defined
            else
               io_field%field_dim(n)%id = dimid
            end if
!         end do

!         if (iostat /= NF90_NOERR) then ! variable was not yet defined
!         end if
!      end if ! master task

!   endif ! input/output file

!-----------------------------------------------------------------------
!EOC

 end subroutine define_field_netcdf

 subroutine write_field_netcdf()

 end subroutine write_field_netcdf

 subroutine read_field_netcdf()

 end subroutine read_field_netcdf

 subroutine check(status)

   integer (i4), intent (in) ::  status  ! status returned by netCDF call

 end subroutine check

!***********************************************************************

 end module io_netcdf

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
