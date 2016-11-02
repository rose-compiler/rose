j = 3
call sub (j)
write (*,*) j ! Writes 3
contains

subroutine sub (i)
integer(c_int), value :: i
end subroutine sub
end
