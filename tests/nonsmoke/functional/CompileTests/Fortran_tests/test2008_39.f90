! DQ (11/10/2008): Note module names use file name prefix to for use of 
! unique names and avoid race conditions in mod file generation.

! Simple tests symbol handling with use statement.
module test2008_39_A
   integer i_var
   integer j_var
   integer k_var
end module

module test2008_39_B
   use test2008_39_A
   integer a_var
   integer b_var
end module

function foo_1()
   use test2008_39_A

 ! i_var will reference A::i_var
   i_var = 0

 ! x will be built using implicit rules.
   x = 1
end function

function foo_2()
   use test2008_39_A, only : i_var,j_var

 ! i_var will reference A::i_var
   i_var = 0

 ! x will be built using implicit rules.
   x = 1
end function

function foo_3()
   use test2008_39_A, only : x => i_var, y => j_var

   integer i_var

 ! x will reference the local i_var (since "only" option was used in the "use" statement)
   i_var = 0

 ! x will reference A::i_var (using C++ style name qualification which does not exits in Fortran 90)
   x = 1
end function

function foo_4()
   use test2008_39_A, x => i_var, y => j_var

 ! i_var will reference A::i_var
   i_var = 0

 ! x is a reference to A::i_var
   x = 1
end function

function foo_5()
   use test2008_39_B, x => i_var, y => j_var

 ! i_var will be built using implicit rules, since x => i_var removes i_var from the local scope.
   i_var = 0

 ! x will reference A::i_var (using C++ style name qualification which does not exits in Fortran 90)
   x = 1
end function

END
