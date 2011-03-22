module m1
  integer :: hh
  type t
    real :: r
  end type t
end module m1

module m2
  type t
    integer :: k
  end type t
end module m2

module m3
contains
  type(t) function func()
    use m2
    func%k = 77
  end function func
end module m3

type(t) function a()
  use m1, only: hh
  type t2
    integer :: j
  end type t2
  type t
    logical :: b
  end type t

! DQ commented out as test
  a%b = .true.
end function a

type(t) function b()
  use m1, only: hh
  use m2
  use m3

! DQ commented out as test
  b = func ()
  b%k = 5
end function b
