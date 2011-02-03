module a_11_57
  integer longName
end module a_11_57

module b_11_57
  use a_11_57, only: s => longName
  interface bFoo
     module procedure foo
  end interface
contains 
  subroutine foo()
    s=s+1
  end subroutine foo
end module b_11_57

program multRename
  use a_11_57
  use b_11_57
  longName=1
  call bFoo()
  if (longName==2) then 
     print *, 'OK'
  else 
     print *, 'FAILED'
  end if
end program multRename
