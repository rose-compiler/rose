! Rice test
! A 2-dimensional component of a derived type is unparsed incorrectly.

module dim_2_component
  type t
    integer, dimension(:,:), pointer :: a  ! =>  INTEGER, DIMENSION(:), POINTER :: a = (/:/)
  end type
end module
