! Rice test
! ROSE's unparser fails an assertion on an empty character string constant.
! It doesn't matter whether single or double quotes are used.

program empty_string_constant
  character(*), parameter :: c1 = "" , c3 = '', c4 = "Zung" ! produces assertion failure in testTranslator
  character(len = 8) :: c2 = ''  ! ditto
end program
