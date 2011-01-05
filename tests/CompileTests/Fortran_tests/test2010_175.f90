! This test causes OFP to terminate without warning so that that it
! does not notify ROSE and it appears to work, but fails to unparse 
! the function (and thus does not even cause an error in compiling 
! the generated code).

! See test2010_176.f90 for a version that demonstrates another error.

implicit none
type t
   integer :: i
end type t

type(t) function func()
  func%i = 5
end function func

end
