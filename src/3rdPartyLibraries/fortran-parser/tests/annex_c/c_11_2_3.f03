interface good3
   subroutine s3a(w,x,y,z)
     real :: w,y
     integer :: x,z
   end subroutine s3a
   subroutine s3b(x,w,z,y)
     real :: w,z
     integer :: x,y
   end subroutine s3b
end interface good3

end

