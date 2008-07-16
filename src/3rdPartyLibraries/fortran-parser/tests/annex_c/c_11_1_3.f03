program q
  real(kind=1) :: c
  
contains
  subroutine r
    real(kind = kind(c)) :: D   ! Invalid declaration
                                ! See below
    real(kind=2) :: c
  end subroutine r
end program q
