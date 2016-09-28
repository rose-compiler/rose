! Pointer to array of real
  real, pointer, dimension(:) :: g
! Array of pointer to real (not unparsed correctly)
! -- unparsed as: real, dimension(:) :: h
  real, dimension(:), pointer :: h
end
