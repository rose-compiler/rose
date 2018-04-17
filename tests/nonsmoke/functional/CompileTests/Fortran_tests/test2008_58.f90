! Simple tests symbol handling with use statement.
module test2008_58_A
   integer i_var

   interface operator (.mynot.)
      module procedure real_not
   end interface

   contains
      function real_not(a)
         integer :: real_not
         integer, intent(in) :: a
         real_add = 1
      end function real_not

end module

function foo()
 ! It is illegal to rename intrinsic functions.
   use test2008_58_A, only : operator(.mynot.)

   integer a,b,c

 ! Make a call to the function from module "A"
   a = .mynot. b

end function
