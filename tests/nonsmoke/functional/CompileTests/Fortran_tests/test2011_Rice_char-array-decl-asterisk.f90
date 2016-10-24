! Rice test
! If a character array is declared with length given after the dimension,
! its dimension is lost in translation.

program char_array_decl_asterisk
  character   c(3)*4  ! becomes character(len=4) :: c
  c(2) = 'abcd'
end program
