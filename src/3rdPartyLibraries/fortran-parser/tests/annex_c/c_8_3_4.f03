program global_work
  call configure_arrays     ! Perform the appropriate allocations
  call compute              ! Use the arrays in computations
end program global_work
module work_arrays          ! An example set of work arrays
  integer n
  real, allocatable, save :: a(:), b(:,:), c(:,:,:)
end module work_arrays
subroutine configure_arrays ! Process to set up work arrays
  use work_arrays
  read (*,*) N
  allocate (a(n), b(n,n), c(n,n,2*n))
end subroutine configure_arrays
subroutine compute
  use work_arrays
  ! ... Computations involving arrays A, B, and C
end subroutine compute

  
