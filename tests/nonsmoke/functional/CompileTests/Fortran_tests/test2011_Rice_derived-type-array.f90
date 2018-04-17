! Rice test
! When a derived type contains an array component, and a data reference
! indexes an array of that derived type and then selects the result's
! array component, the data reference is incorrectly translated.

program derived_type_array

  type :: t
    integer :: c(2)
    integer :: d
  end type

  type(t) :: q(100)

  q(1) % c = (/ 5, 7 /)  ! data reference becomes 'q % c(1)'
  q(1) % d = 0        ! ok
  
end program 
