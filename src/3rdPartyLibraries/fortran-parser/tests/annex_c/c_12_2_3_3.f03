subroutine transition (n, ising, iterations, p)
  logical ising(n, n, n), flips(n, n, n)
  integer ones(n, n, n), count (n, n, n)
  real threshold (n, n, n), p(6)

  do i = 1, iterations
     ones = 0
     where (ising) ones = 1
     count = cshift(ones, -1, 1) + cshift(ones, 1, 1) &
          + cshift(ones, -1, 2) + cshift(ones, 1, 2) &
          + cshift(ones, -1, 3) + cshift(ones, 1, 3)
     where (.not. ising) count = 6 - count
     threshold = 1.0
     where (count == 4) threshold = p(4)
     where (count == 5) threshold = p(5)
     where (count == 6) threshold = p(6)
     flips = rand(n) <= threshold
     where(flips) ising = .not. ising
  end do

contains
  function rand(n)
    real rand(n,n,n)
    call random_number(harvest = rand)
    return
  end function rand
end 
