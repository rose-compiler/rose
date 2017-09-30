!! R1211 procedure-declaration-stmt
!   - F2008 addition
!   - internal procedure can be used as an actual argument
!     or procedure pointer target
!
program internal_proc_as_actual
  INTERFACE
     SUBROUTINE proc_arg(proc)
       procedure() :: proc
     END SUBROUTINE
  END INTERFACE

  procedure(internal_proc), pointer :: proc_ptr => internal_proc

  call proc_arg(internal_proc)

contains

  subroutine internal_proc(i)
    integer :: i
    i = 13
  end subroutine

end program
