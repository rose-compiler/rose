program main 
     implicit none

   ! Call to function that has not yet been seen (current bug). 
     call driver()

     stop
end 

subroutine driver() 
     implicit none 

     return 
end 
