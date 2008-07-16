program a
  integer i, j

contains
  subroutine b
    integer i  ! Declaration of i hides 
               ! program a's declaration of i

    i = j      ! Use of variable j from program a
               ! through host association
  end subroutine b
end program a

