! Rice test
! kind type is lost in translation
program lost_kind_type_1
implicit none
  integer, parameter :: i8  = selected_int_kind(16)
  
  call g(1_i8)  ! translated as call g(1), causing a compile error due to type mismatch.

contains

  subroutine g(r)
  implicit none
    integer(kind = i8) :: r
  end subroutine
  
end program
