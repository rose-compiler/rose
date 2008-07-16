! semantically bad, but should still be parsable.
interface bad8 ! this interface is invalid !
   ! despite the fact that it is unambiguous !
   subroutine s8a(x,y,z)
     real, optional :: x
     integer :: y
     real :: z
   end subroutine s8a
   subroutine s8b(x,z,y)
     integer,optional :: x
     integer :: z
     real :: y
   end subroutine s8b
end interface bad8

end

