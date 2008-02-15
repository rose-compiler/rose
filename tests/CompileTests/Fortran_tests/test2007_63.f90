subroutine where_example
    logical :: a(1,2,3,4)
    real    :: result(1,2,3,4)

  ! where(a) a(:,:,:,l) = a * ( a(:,:,:,l) * a + (a - a) )
  ! a(:,:,:,l) = a * ( a(:,:,:,l) * a + (a - a) )
  ! a(:,:,:,l) = a(:,:,:,2);
  ! a(:,:,:,l) = b * (a(:,:,:,l) * c);

  ! where(should_set_cell) result(:,:,:,l) = temperature2*(result(:,:,:,l)*temperature1 + (donor_variability - temperature4))
  ! result(:,:,:,l) = temperature2*(result(:,:,:,l)*temperature1 + (donor_variability - temperature4))
    where ( a(:,:,:,l) ) result(:,:,:,l) = temperature2*(result(:,:,:,l)*temperature1 + (donor_variability - temperature4))
!   where ( a(:,:,:,l) ) result(:,:,:,l) = 0

end subroutine where_example
