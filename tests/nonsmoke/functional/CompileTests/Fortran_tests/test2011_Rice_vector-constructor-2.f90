! Rice test
! vector constructor with implied do loop is mistranslated
module test_vector_constructor_2
contains
 subroutine g(v)
 integer :: v(:)
 end subroutine g
end module

program vector_constructor_2
use test_vector_constructor_2
  call g(v=(/ (i, i=1,4) /)) ! mistranslated as g(v=((i,i = 1, 4)))
end program
