module testmod
implicit none
contains

FUNCTION outerproduct(a)
implicit none
real, intent(in) :: a
real :: outerproduct
    outerproduct = 0.
end FUNCTION outerproduct
end module
