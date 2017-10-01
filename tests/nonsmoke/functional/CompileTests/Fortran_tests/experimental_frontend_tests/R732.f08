!! R732 assignment-stmt
!    is variable = expr
!   - F2008 addition
!   -  intrinsic assignment to an allocatable polymorphic variable is allowed
!
program alloc_opt
  type Base
    integer :: val
  end type Base
  type, extends(Base) :: Derived
    integer :: dval
  end type Derived

  type(Derived) :: old
  type(Derived), allocatable :: new

  old%val  = 1
  old%dval = 3

  allocate (new)
  new = old

end program
