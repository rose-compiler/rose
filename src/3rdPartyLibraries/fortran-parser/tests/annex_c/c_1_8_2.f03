module m
  type t1
     real a,b
  end type t1
  type, extends(t1) :: t2
     real, pointer :: c(:), d(:)
  contains
    final :: t2f
  end type 
  type, extends(t2) :: t3
     real, pointer :: e
  contains
    final :: t3f
  end type

contains
  subroutine t2f(x) ! Finalizer for type(t2)'s extra components
    type(t2) :: x
    if(associated(x%c)) deallocate(x%c)
    if(associated(x%d)) deallocate(x%d)
  end subroutine t2f
  subroutine t3f(y) ! Finalizer for type(t3)'s extra components
    type(t3) :: y
    if (associated(y%e)) deallocate(y%e)
  end subroutine t3f
end module 

subroutine example
  use m
  type(t1) x1
  type(t2) x2
  type(t3) x3
  
! Returning from this subroutine will effectively do
!     ! Nothing to x1; it is not finalizable
!     call t2f(x2)
!     call t3f(x3)
!     call t2f(x3%t2)
end subroutine example

   
