module a
  integer longName
end module a

module b
  integer longName
end module b

module a1
  use a, only: s => longName
  interface a1Foo
     module procedure bar
  end interface
contains 
  subroutine bar()
    s=s+1
  end subroutine bar
end module a1

module b1
  use b, only: s => longName
  interface b1Foo
     module procedure foo
  end interface
contains
  subroutine foo()
    s=s+1
  end subroutine foo
end module b1

program multRename
  use a, only: aL=>longName
  use b, only: bL=>longName
  use a1
  use b1
  aL=1
  bL=2
  call a1Foo()
  call b1Foo()
  if (aL==2 .and. bL == 3) then 
     print *,'OK'
  else 
     print *,'FAILED'
  endif
end program multRename
