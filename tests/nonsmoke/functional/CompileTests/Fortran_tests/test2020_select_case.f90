program test
    INTEGER :: x 
    x=2  
    SELECT CASE (x) 
        case (1:3)    
            print *, "PASS"
    END SELECT
end program test
