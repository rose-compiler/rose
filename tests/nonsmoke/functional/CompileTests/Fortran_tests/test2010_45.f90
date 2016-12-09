   implicit none
 ! Separating the declarations will work fine in ROSE. 
 ! integer, parameter :: x = 42
 ! integer, parameter :: y = x

 ! This is valid Fortran and now works in ROSE.
   integer, parameter :: x = 42, y = x

 ! Fortunately this is not valid Fortran.
 ! integer, parameter :: a = b, b = 7
end
