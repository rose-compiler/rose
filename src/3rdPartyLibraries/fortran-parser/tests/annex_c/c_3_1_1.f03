! this is the second example in C.3.1
type large
   integer elt(10)
   integer val
end type large

type(large) a(5)

print *, a(1)
print *, a(1)%elt(3)
print *, a(2:4)%val

end
