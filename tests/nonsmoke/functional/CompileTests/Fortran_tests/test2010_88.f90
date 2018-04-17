! Bug report from Rice: 02-use-iso_c_binding.f90
! Trying to 'use' the module ISO_C_BINDING fails, saying it can't find
! the appropriate .rmod file.

program p
  use, intrinsic :: iso_c_binding ! produces error messages and assertion failure
end program
