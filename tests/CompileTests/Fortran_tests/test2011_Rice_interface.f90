! Rice test
! interface contents are lost
!
interface
  function f(v) result(x) ! the whole function is lost in translation
    integer x
    integer v
  end function
end interface

integer a
a = f(v=1)   ! compile error here because interface contents are lost.
END

function f(v) result(x)
  integer x
  integer v
  x = 0
end function
