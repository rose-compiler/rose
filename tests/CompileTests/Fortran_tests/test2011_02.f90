  type :: t
    integer :: c(1)
    integer :: d
  end type

  type(t) :: q(100)

! q(1) % c = (/ 0 /)  ! data reference becomes 'q % c(1)'
  q(1) % c = 0  ! data reference becomes 'q % c(1)'
end
