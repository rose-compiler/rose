!! R502 attr-spec
!                 is  access-spec 
!                 or  CONTIGUOUS
!                 or  ...

subroutine contiguous_example(A, B, C)
  real, CONTIGUOUS :: A(:), B(:)
  real, contiguous :: C(:,:)
end

