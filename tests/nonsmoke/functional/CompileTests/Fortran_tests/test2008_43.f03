module test2008_43_temp
   type real_num
   real :: x
   end type

   interface operator (.add.)
   module procedure real_add
   end interface

   contains
   function real_add(a,b)
   type(real_num) :: real_add
   type(real_num), intent(in) :: a,b
   real_add%x = a%x+b%x
   end function real_add

end module

program main
 ! This is a F03 feature
   use test2008_43_temp , operator(.plus.) => operator(.add.)

   type(real_num) :: a,b,c

 ! This is a F03 feature
 ! c = a.plus.b
end program

