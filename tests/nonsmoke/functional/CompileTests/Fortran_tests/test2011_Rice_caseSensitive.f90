! ROSE does not desensitise the case for derived type name
program caseSensitiveBug

   type Mytype 
     integer i
   end type

   ! ROSE cannot identify mytype as Mytype
   type (mytype) :: X

   X%i = 5
  
end 
