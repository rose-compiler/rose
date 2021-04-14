program main
  logical :: x
  logical :: y = .true.
  logical :: z = .false.

  x = y .and. z
  x = y .or. z
  x = y .eqv. z
  x = y .neqv. z

  x = 1 < 2
  x = 1 .lt. 2
  x = 1 <= 2
  x = 1 .le. 2

  x = 1 > 2
  x = 1 .gt. 2
  x = 1 >= 2
  x = 1 .ge. 2

  x = 1 == 1
  x = 1 .eq. 1
  x = 1 /= 1
  x = 1 .ne. 1

end program main
