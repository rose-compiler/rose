 subroutine extract_attrib_file_char(att_name)

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

!   type (datafile), intent(in) :: &
!      file_descr       ! data file descriptor

! !OUTPUT PARAMETERS:

!   character (*), intent(out) :: &
!      att_value        ! value of attribute to be extracted

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

!  integer :: n   ! loop index

   logical :: att_exists  ! attribute already defined

!-----------------------------------------------------------------------
!
!  first check standard attributes
!
!-----------------------------------------------------------------------

!   att_exists = .false.

   select case(trim(att_name))
   case('full_name','FULL_NAME')
!   case('FULL_NAME')
      att_exists = .true.
!      att_value = file_descr%full_name

   case('data_format','DATA_FORMAT')
      att_exists = .true.
!      att_value = file_descr%data_format

   end select

 end subroutine extract_attrib_file_char
