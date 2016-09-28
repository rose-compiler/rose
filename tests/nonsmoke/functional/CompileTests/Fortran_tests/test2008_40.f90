! Simple tests symbol handling with use statement.
module test2008_40_A
   integer i_var
   integer j_var
   integer k_var
end module

function foo_3()
   use test2008_40_A, only : x => i_var, y => j_var

 ! Note that this is legal code
 ! use A, only :

   integer i_var

 ! x will reference the local i_var (since "only" option was used in the "use" statement)
   i_var = 0

 ! x will reference A::i_var (using C++ style name qualification which does not exits in Fortran 90)
   x = 1
end function


