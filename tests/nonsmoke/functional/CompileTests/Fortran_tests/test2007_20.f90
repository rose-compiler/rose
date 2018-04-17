subroutine foo()
   integer :: array_A
   integer :: array_C

 ! The dimension statement is not output since the dimension attribute 
 ! is used in the declaration for each array. Not clear if this is a 
 ! problem, I think it is semantically equivalent code (though not 
 ! source-to-source equivalent).
   dimension array_A(2,3), array_C(4,5)

end subroutine


