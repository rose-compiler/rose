program main
  type t
    integer :: m
  end type t
contains
  type(t) function a1()
!   use m1, only: hh
!    type t2
!      integer :: j
!    end type t2
    type t
      logical :: b
    end type t

! DQ commented out as test
   a1%b = .true.
  end function a1
end program main
