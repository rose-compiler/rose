! semantically bad, but still parsable
interface bad6 ! this interface is invalid !
   subroutine s6a(x,y)
     class(pear) :: x,y
   end subroutine s6a
   subroutine s6b(x,y)
     class(fruit) :: x
     class(bosc) :: y
   end subroutine s6b
end interface bad6

end

