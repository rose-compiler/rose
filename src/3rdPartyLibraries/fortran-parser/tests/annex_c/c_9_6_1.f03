real, pointer :: pbest
real, target :: b(10000)
call best(pbest, b)   ! Upon return pbest is associated with the "best" 
                      ! element of B
contains
  subroutine best(p, a)
    real, pointer, intent(out) :: p
    real, target, intent(in) :: a(:)
    ! find the "best" element of a(i)
    p => a(i)
    return
  end subroutine best
end
  
