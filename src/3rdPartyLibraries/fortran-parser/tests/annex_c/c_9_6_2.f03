integer large(100,100)
call sub(large)

call sub()
contains
  subroutine sub(arg)
    integer, target, optional :: arg(100,100)
    integer, pointer, dimension(:,:) :: parg
    if (present(arg)) then
       parg => arg
    else
       allocate(parg(100,100))
       parg = 0
    end if
    ! Code with lots of references to parg
    if (.not. present(arg)) deallocate(parg)
  end subroutine sub
end

