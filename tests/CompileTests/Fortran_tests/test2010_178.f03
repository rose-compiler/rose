! { dg-do compile }
! Tests the fix for PR34431 in which function TYPEs that were
! USE associated would cause an error.
!
! Contributed by Tobias Burnus <burnus@gcc.gnu.org>
!
module m1test2010_178
  integer :: hh
  type t
    real :: r
  end type t
end module m1test2010_178

type(t) function a()
  use m1test2010_178, only: hh
  type t
    logical :: b
  end type t

! DQ commented out as test
  a%b = .true.
end function a
