subroutine foo(A,B)
   ! declare fewer variable explicitly
   ! Note also tht the type referenced in the variable declaration for 
   ! variable "A" is "integer" and should be "array of integer"
     integer :: A,C,E,F
     dimension A(100),B(2),C(3),D(4),E(6),F(7)
end subroutine foo
