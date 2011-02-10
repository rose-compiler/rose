module a_11_54
  integer longName
end module a_11_54

module b_11_54
  integer longName
end module b_11_54

module a1_11_54
  use a_11_54, only: s => longName
  private :: foo
  public :: a1Foo
  interface a1Foo
     module procedure foo
  end interface
contains 
  subroutine foo()
    s=s+1
  end subroutine foo
end module a1_11_54

module b1_11_54
  use b_11_54, only: s => longName
  private :: foo
  public :: b1Foo
  interface b1Foo
     module procedure foo
  end interface
contains
  subroutine foo()
    s=s+1
  end subroutine foo
end module b1_11_54

program multRename
  use a_11_54, only: aL=>longName
  use b_11_54, only: bL=>longName
  use a1_11_54
  use b1_11_54
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
