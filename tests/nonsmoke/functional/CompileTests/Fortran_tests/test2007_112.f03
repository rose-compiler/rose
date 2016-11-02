! It is not an error reported by gfortran, but I think the "(c)" syntax is required.
! integer, bind :: somevar

integer, bind(c) :: somevar

! We can also provide an explicit binding label
integer, bind(c,NAME="some_C_var") :: some_other_var

end
