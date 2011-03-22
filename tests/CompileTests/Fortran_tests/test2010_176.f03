module kinds
  implicit none
  type t
     integer :: i
  end type t
end module kinds

! We don't see the type(t) until after we declare the use of the "kinds" module
type(t) function func()
  use kinds
  func%i = 5
end function func

end
