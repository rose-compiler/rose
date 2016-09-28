subroutine foo(i)
   INTEGER :: a,b

   MY_NAME: DO I=0,1
      a = b
   end do MY_NAME

 ! Example of code using named if loops
   MY_IF:   IF (a.GT.b) THEN
   END IF MY_IF

   IF (a.EQ.b) a=b
   IF (c.GT.d) a=b

end subroutine

