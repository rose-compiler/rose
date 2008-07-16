! Testing subroutine-stmt, R1232
11 SUBROUTINE foo
end subroutine
subroutine boo(a,b)
end subroutine
PURE subroutine coo()
end subroutine
subroutine doo() BIND(C)
end subroutine
end
