module m1test2010_181
  integer :: hh
  type t
    real :: r
  end type t
end module m1test2010_181

module m2test2010_181
  type t
    integer :: k
  end type t
end module m2test2010_181

module m3test2010_181
contains
  type(t) function func()
    use m2test2010_181
    func%k = 77
  end function func
end module m3test2010_181

type(t) function a()
  use m1test2010_181, only: hh
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
  use m1test2010_181, only: hh
  use m2test2010_181
  use m3test2010_181

! DQ commented out as test
  b = func ()
  b%k = 5
end function b
