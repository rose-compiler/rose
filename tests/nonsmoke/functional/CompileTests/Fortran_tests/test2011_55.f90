module a_11_55
  integer longName
end module a_11_55

module b_11_55
  integer longName
end module b_11_55

module a1_11_55
  use a_11_55, only: s => longName
  interface a1Foo
     module procedure bar
  end interface
contains 
  subroutine bar()
    s=s+1
  end subroutine bar
end module a1_11_55

module b1_11_55
  use b_11_55, only: s => longName
  interface b1Foo
     module procedure foo
  end interface
contains
  subroutine foo()
    s=s+1
  end subroutine foo
end module b1_11_55

program multRename
  use a_11_55, only: aL=>longName
  use b_11_55, only: bL=>longName
  use a1_11_55
  use b1_11_55
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
