subroutine where_example
    real :: a(50)

  ! a(1::3) = 0 ! This does not unparse correctly!
  ! a(1:) = 0
    a(1:) = 0
    a(1) = 0

end subroutine where_example
