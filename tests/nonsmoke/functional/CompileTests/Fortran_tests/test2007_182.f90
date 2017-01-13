Program main
 ! Declare "foo" as a function (it't return type is not interpreted to be integer)
 ! dimension foo(5)
 ! Declare the integer type variables
   integer i,j
   integer foo

 ! This should make "foo" a function or a data block
   external foo

 ! Now use the function
   i = foo(i)

 ! But the use of the same statement a second time is an error
   j = foo(i)
end
