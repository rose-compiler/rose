! ROSE used to incorrectly parse a POINTER attribute if it followed a DIMENSION attribute.
!
program Rice_pointer_after_dimension

  real, dimension(:), pointer :: p  ! POINTER will be missing from unparsed output

end program
