!
! This original test is incorrect code.
!
! Without the "interface" this code is not legal F90 code.  It may still be
! legal Fortran 77 code however, so I have left the older code as a comment.
! The older code passed gfortran 4.2, but fails with gforgran 4.4.  The 
! modification by Craig fixes the problem so it works with grogram 4.4.
!
! function my_sum(var1,var2) result(x)
! integer x
! integer var1,var2
! x = 0
! end function
! 
! integer a
! a = my_sum(var1=1,var2=2)
! END
! 
interface
function my_sum(var1,var2) result(x)
integer x
integer var1,var2
end function
end interface

integer a
a = my_sum(var1=1,var2=2)
END

function my_sum(var1,var2) result(x)
integer x
integer var1,var2
x = 0
end function
