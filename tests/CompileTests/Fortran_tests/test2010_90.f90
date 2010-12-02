! Bug report from Rice: 04-designator-with-substring-range.f90
! The "hasSubstringRange" case of 'c_action_designator' is not implemented.

program p
  character(10) :: c(10)
  c(9)(1:2) = "xy" ! produces error message & subsequent assertion failure
end program
