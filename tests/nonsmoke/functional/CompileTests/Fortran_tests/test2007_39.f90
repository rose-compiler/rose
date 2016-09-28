subroutine foo()

integer :: i,j,k
! common i,j

common /namedCommonBlock/ i,j, /anotherCommonBlock/ k

end subroutine
