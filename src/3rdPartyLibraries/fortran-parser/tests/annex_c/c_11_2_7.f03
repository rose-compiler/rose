interface good7
   subroutine s7a(x,y,z)
     class(pear) :: x,y,z
   end subroutine s7a
   subroutine s7b(x,z,w)
     class(fruit) :: x
     class(bosc) :: z
     class(apple), optional :: w
   end subroutine s7b
end interface good7

end

