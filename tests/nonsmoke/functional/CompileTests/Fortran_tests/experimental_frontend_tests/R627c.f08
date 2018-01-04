!! R627 alloc-opt
!    or MOLD = source-expr
!   - F2008 addition
!   - MOLD= in ALLOCATE statement can give polymorphic variable the shape,
!     type, and type parameters
!
program alloc_opt
  type Base
    integer :: val
  end type Base
  type, extends(Base) :: Derived
    integer :: dval
  end type Derived

  type(Derived) :: old(10,20)
  type(Base), allocatable :: new(:,:)

  allocate (new, MOLD=old)

end program
