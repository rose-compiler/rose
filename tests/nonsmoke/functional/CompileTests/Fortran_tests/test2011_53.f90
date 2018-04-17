! front end problem with multiple renames
! now use the renamed variables

module a_11_53
  integer longName
end module a_11_53

module b_11_53
  use a_11_53, only: s => longName
  interface bFoo
     module procedure foo
  end interface
contains 
  subroutine foo()
    s=s+1
  end subroutine foo
end module b_11_53

module c_11_53
  use a_11_53, only: s => longName
  interface cBar
     module procedure bar
  end interface
contains
  subroutine bar()
    use b_11_53
    s=s+1
    call bFoo()
  end subroutine bar
end module c_11_53

program multRename
  use a_11_53
  use c_11_53
  longName=1
  call cBar()
  if (longName==3) then 
     print *, 'OK'
  else 
     print *, 'FAILED'
  end if
end program multRename
