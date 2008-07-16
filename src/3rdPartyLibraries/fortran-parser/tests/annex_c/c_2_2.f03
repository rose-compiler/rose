program iter
  real, dimension(1000,1000), target :: a, b
  real, dimension (:, :), pointer :: in, out, swap
  
  ! Read values into A
  
  in => A    ! Associate IN with target A
  out => B   ! Associate OUT with target B
  
  iter:do
     ! Apply transformation of IN values to produce OUT
     
     if(converged) exit iter
     ! Swap IN and OUT
     swap => in; in => out; out => swap
  end do iter

end program iter

  
