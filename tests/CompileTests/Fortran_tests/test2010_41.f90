 function global_sum_dbl(X, MASK)

   real , dimension(:,:,:), intent(in) :: X ! array to be summed

   real , dimension(size(X,dim=1), &
                              size(X,dim=2), &
                              size(X,dim=3)), intent(in), optional :: &
      MASK                 ! real multiplicative mask

 end function global_sum_dbl
