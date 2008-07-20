	subroutine n0
      type mytype
         integer i
         real x
      end type mytype

   type (mytype) con

 ! Note that the typename is required in the syntax.
	parameter (con = mytype(1,1.2))

 ! This can not be a normal function, from this we can conclude that the 
 ! IR should not be building a function to represent the initializer.
 ! parameter (con = mytype(1,cos(1.2)))

   end 
