! Bug report from Rice: 01-empty-string-constant.f90
! ROSE's unparser fails an assertion on an empty character string constant.
! It doesn't matter whether single or double quotes are used.

program p
  character(*) :: c1 = ""  ! produces assertion failure in testTranslator
  character(*) :: c2 = ''  ! ditto
end program
