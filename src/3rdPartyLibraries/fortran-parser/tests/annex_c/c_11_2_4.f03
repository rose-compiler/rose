! semantically bad, but still parsable..
interface bad4 ! this interface is invalid!
   subroutine s4a(w,x,y,z)
     real :: w,y
     integer :: x,z
   end subroutine s4a
   subroutine s4b(x,w,z,y)
     real :: x,y
     integer :: w,z
   end subroutine s4b
end interface bad4

end

