! P02-derived-type-array.f90
! When a derived type contains an array component, and a data reference
! indexes an array of that derived type and then selects the result's
! array component, the data reference is incorrectly translated.

program p

  type :: t
    integer :: c(1)
    integer :: d
  end type

  type(t) :: q(100)

  q(1) % c = (/ 0 /)  ! data reference becomes 'q % c(1)'
  q(1) % d = 0        ! ok
  
end program 
