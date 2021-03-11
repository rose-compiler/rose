   subroutine bsrch_interval2d_vec(x)
! test
     implicit none
     integer x
!$acc wait(1) if (gpu%elem_set_active)
!$acc end data
   end subroutine bsrch_interval2d_vec
