module test

real, dimension(10) :: d1
real, dimension(10) :: zz

end module

subroutine  mytest
  use test, only: d11=>d1,zz
end subroutine mytest
