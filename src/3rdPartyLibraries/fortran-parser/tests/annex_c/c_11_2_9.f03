! semantically bad, but should be parsable
interface bad9 ! this interface is invalid !
   ! despite the fact that it is unambiguous !
   subroutine s9a(x)
     real :: x
   end subroutine s9a
   subroutine s9b(x)
     interface
        function x(a)
          real :: x,a
        end function x
     end interface
   end subroutine s9b
   subroutine s9c(x) 
     interface
        function x(a)
          real :: x
          integer :: a
        end function x
     end interface
   end subroutine s9c
end interface bad9

end

