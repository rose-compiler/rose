! derived-type-3d-component.f90
! A 2-dimensional component of a derived type is unparsed incorrectly.

program p
  type t
    integer, dimension(:,:), pointer :: a  ! =>  INTEGER, DIMENSION(:), POINTER :: a = (/:/)
  end type

  type(t) :: x
end program
