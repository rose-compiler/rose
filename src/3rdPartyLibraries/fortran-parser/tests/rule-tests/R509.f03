
! Testing language binding spec.
integer, BIND(C) :: i
integer, bind ( c , name = "cint" ) :: i
end
