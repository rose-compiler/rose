program dynam_iter
  real, dimension(:,:), pointer :: a, b, swap ! Declare pointers

  read(*,*) n, m
  allocate(a(n,m), b(n,m)) ! Allocate target arrays
  ! Read values into A
  iter: do
     ! Apply transformation of values in A to produce values in B
     if (converged) exit iter
     ! Swap A and B
     swap => a; a => b; b => swap
  end do iter

end program dynam_iter
