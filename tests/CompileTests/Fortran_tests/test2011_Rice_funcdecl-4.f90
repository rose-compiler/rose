! Rice test
!
module module_bits_8

contains

!-------------------------------------------------------------------------------
! result type in type declaration
!-------------------------------------------------------------------------------
  integer function number_of_bits8(i) result(num)
    implicit none
    integer :: i, itmp, num 
    num = 0
    itmp = i
    do while (itmp .gt. 0)
       num = num + 1
       itmp = ishft(itmp, -1)
    end do
  end function number_of_bits8

end module



program test_module_bits_8
use module_bits_8
integer :: error

error = 0
if (4 .ne. number_of_bits8(14)) then
   error = error + 1
endif

if (0 .ne. error) then
  write(*, '(A)') "Test failed"
else
  write(*, '(A)') "Test passed"
endif

end program
