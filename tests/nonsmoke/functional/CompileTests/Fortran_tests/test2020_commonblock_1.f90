module badparse
contains
subroutine badsub
! OK 1
implicit real(a-h,o-z)
!real a ! if this line here, a is OK
common/com/a(10) ! BAD 2 outputs a instead of a(10). If previous line uncommented, OK

real b(10)
common/com2/b
end subroutine badsub
end module badparse
