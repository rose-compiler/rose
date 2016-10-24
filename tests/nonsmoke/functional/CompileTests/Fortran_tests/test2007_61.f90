subroutine solve(x,y,z)
    integer, optional :: x,y,z
    x = 1
    return
end subroutine solve

subroutine foo ()
#if OS_MACOSX
    interface
    subroutine solve(x,y,z)
        integer :: x,y,z
    end subroutine solve
    end interface
#endif
    integer :: a, b, c
  ! solve(a,z=0)
   a = 0
   call solve(a,b,c)
end subroutine foo
