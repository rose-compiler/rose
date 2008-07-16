module m
  type t(k)
     integer, kind :: k
     real(k), pointer :: vector(:) => NULL()
     contains
       final :: finalize_t1s, finalize_t1v, finalize_t2e
    end type t
  contains
    subroutine finalize_t1s(x)
      type(t(kind(0.0))) x
      if (associated(x%vector)) deallocate(x%vector)
    end subroutine finalize_t1s
    subroutine finalize_t1v(x)
      type(t(kind(0.0))) x(:)
      do i=lbound(x,1), ubound(x,1)
         if(associated(x(i)%vector)) deallocate(x(i)%vector)
      end do
    end subroutine finalize_t1v
    elemental subroutine finalize_t2e(x)
      type(t(kind(0.0d0))),intent(inout) :: x
      if(associated(x%vector)) deallocate(x%vector)
    end subroutine finalize_t2e
  end module m

subroutine example(n)
  use m
  type(t(kind(0.0))) a,b(10),c(n,2)
  type(t(kind(0.0d0))) d(n,n)
  ! Returning from this subroutine will effectively do
  !    call finalize_t1s(a)
  !    call finalize_t1v(b)
  !    call finalize_t2e(d)
  ! No final subroutine will be called for variable C because the user 
  ! omitted to define a suitable specific procedure for it.
end subroutine example

