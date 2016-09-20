! More meaningful use of value attribute, I think.

j = 3
call sub (j)
write (*,*) j ! Writes 3
contains

subroutine sub (i)
integer, value :: I
i = 4
write (*,*) i ! Writes 4
end subroutine sub
end
