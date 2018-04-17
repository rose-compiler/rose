BLOCK DATA abc
    IMPLICIT NONE
    REAL :: X, Y
END BLOCK DATA

subroutine foo(i)
end subroutine

subroutine foobar()
    integer :: i, j

    if (i < 1) then
       j = 1
    end if
end subroutine

subroutine foolish(a)
    integer :: i, j

    if (i < 1) then
        j = 0
    else
       i = 4
    end if

end subroutine

10 integer function alt_main()
      implicit none
      integer :: a,b,c
      a = b + c * 2
      a = b - 1
      a = b * 1
      a = b / 1
      alt_main = a
      return
20 end function alt_main

PROGRAM Triangle_Area

  IMPLICIT NONE

  TYPE triangle
     REAL :: a
  END TYPE triangle

  TYPE(triangle) :: t

END PROGRAM Triangle_Area

