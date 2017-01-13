! Rice test
! A 3-dimensional component of a derived type is unparsed incorrectly.

module dim_3_component
  type t
    integer, dimension(:,:,:), pointer :: a  ! =>  INTEGER, DIMENSION(:), POINTER :: a = (/:,:/)
  end type
end module
