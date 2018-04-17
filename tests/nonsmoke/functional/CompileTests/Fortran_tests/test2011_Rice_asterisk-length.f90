! Rice test
! A character type declaration with a length selector of the form
! '* (xxx)' causes the front end to fail an assertion.

program asterisk_length
  implicit none
  character b (3)  ! this works correctly
  character c*(3)  ! assertion failure: expression stack empty
end program
