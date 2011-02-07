RECURSIVE FUNCTION fact(n) RESULT (fact_n)
  IMPLICIT none
  real :: fact_n
  integer, intent (in) :: n
  select case(n)
  case(0)
     fact_n=1.0
  case(1:)
     fact_n=n*fact(n-1)
  case default
     fact_n=0.0
  end select
end function fact

program main
  if(fact(6).EQ.720) then
     print *, "OK"
  else
     print *, fact(6)," should be 720"
  end if
end program main
