type(t) function a()
! use m1, only: hh
! type(t) :: X
  type t
    logical :: b
  end type t

! DQ commented out as test
  a%b = .true.
end function a
