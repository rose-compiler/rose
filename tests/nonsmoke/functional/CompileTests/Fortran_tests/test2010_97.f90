! Bug report from Rice: 11-inquire-statement.f90
! An 'inquire' statement causes an assertion failure in the front end,
! no matter what sort of arguments are given for it.

program p
  inquire(unit=7)  ! causes assertion failure 'name stack not empty'
  inquire(42)      ! ditto
end program
