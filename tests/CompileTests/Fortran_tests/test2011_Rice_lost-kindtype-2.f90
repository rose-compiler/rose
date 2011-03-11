! Rice test
! kind type is lost in translation
program lost_kind_type_2
implicit none
  integer, parameter :: r8  = selected_real_kind(13)
  
  call g(1.0_r8)  ! translated as call g(1.0), causing a compile error due to type mismatch.

contains

  subroutine g(r)
    real(kind = r8) :: r
  end subroutine
  
end program
