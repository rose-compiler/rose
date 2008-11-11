! Simple tests symbol handling with use statement.
module test2008_57_A
   integer i_var

   interface operator (.add.)
      module procedure real_add
   end interface

   contains
      function real_add(a,b)
         integer :: real_add
         integer, intent(in) :: a,b
         real_add = 1
      end function real_add

end module

function foo()
   use test2008_57_A, only : operator(.add.)

   integer a,b,c

 ! Make a call to the function from module "A"
   a = b .add. c

end function
