!! R533 contiguous-stmt
!     is  CONTIGUOUS [ :: ] object-name-list

subroutine contiguous_example(A, B)
  real :: A(:), B(:)
  contiguous :: A, B
end

