! S04-character-decl-asterisk-length.f90
! A character type declaration with a length selector of the form
! '* (xxx)' causes the front end to fail an assertion.

program p
 ! This builds an array of character.
 ! character a (3)  ! this works correctly

 ! This builds a string of length 3.
   character c*(3)  ! assertion failure: expression stack empty
end program
