! https://raw.githubusercontent.com/OpenACC/openacc-interoperability-examples/master/saxpy_openacc_f.f90
module saxpy_mod
  contains
  subroutine saxpy(n, a, x, y)
    integer :: n
    real    :: a, x(:), y(:)
    !$acc parallel deviceptr(x,y)
    y(:) = y(:) + a * x(:)
    !$acc end parallel
  end subroutine
end module
