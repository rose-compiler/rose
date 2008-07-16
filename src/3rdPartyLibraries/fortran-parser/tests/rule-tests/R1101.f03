! Test main-program
!      main-program  is  [ program-stmt ]
!                          [ specification-part ]
!                          [ execution-part ]
!                          [ internal-subprogram-part ]
!                        end-program-stmt
!
!      program-stmt  is  PROGRAM program-name
!
!      end-program-stmt  is  END [ PROGRAM [ program-name ] ]
!
! Not tested here: specification-part, execution-part, and 
! internal-subprogram-part.

! None of the optional parts
end

! Only include the optional PROGRAM with the required END.
end program

! To include the optional name in end-program-stmt requires (by C1102) 
! the program-stmt with the optional name.
program my_program
end program my_program

! Add an optional specification-part
program my_prog
  real a
end program my_prog

! Add an optional execution-part
program my_prog
  i = 1
end program my_prog

! Add an optional internal-subprogram-part
program my_prog
contains
  subroutine sub
  end subroutine sub
end program my_prog

! Add all of the optionals together.
program my_prog
  real i
  i = 1
contains
  subroutine sub
  end subroutine sub
end program my_prog
