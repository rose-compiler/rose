!! R733 pointer-assignment-stmt
!   - F2008 addition
!   - a pointer function reference can denote a variable in any variable
!     definition context
!
subroutine pointer_function_ref(proc_ptr)
  INTERFACE
     SUBROUTINE int_arg_sub(i)
       integer :: i
     END SUBROUTINE
  END INTERFACE

  procedure(int_arg_sub), pointer :: proc_ptr

  proc_ptr => internal_proc

contains

  subroutine internal_proc(i)
    integer :: i
    i = 13
  end subroutine

end subroutine
