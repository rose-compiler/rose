! Simple tests symbol handling with use statement.
module A
   integer i_var
   integer j_var
   integer k_var
end module

module B
   use A
   integer a_var
   integer b_var
end module

function foo_1()
   use A

 ! i_var will reference A::i_var
   i_var = 0

 ! x will be built using implicit rules.
   x = 1
end function

function foo_2()
   use A, only : i_var,j_var

 ! i_var will reference A::i_var
   i_var = 0

 ! x will be built using implicit rules.
   x = 1
end function

function foo_3()
   use A, only : x => i_var, y => j_var

   integer i_var

 ! x will reference the local i_var (since "only" option was used in the "use" statement)
   i_var = 0

 ! x will reference A::i_var (using C++ style name qualification which does not exits in Fortran 90)
   x = 1
end function

function foo_4()
   use A, x => i_var, y => j_var

 ! i_var will reference A::i_var
   i_var = 0

 ! x is a reference to A::i_var
   x = 1
end function

function foo_5()
   use B, x => i_var, y => j_var

 ! i_var will be built using implicit rules, since x => i_var removes i_var from the local scope.
   i_var = 0

 ! x will reference A::i_var (using C++ style name qualification which does not exits in Fortran 90)
   x = 1
end function

END
