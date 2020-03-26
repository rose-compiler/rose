program test
    INTEGER :: x 
    x=2  
    SELECT CASE (x) 
        case (1:3)    
            print *, "PASS"
        case (4)    
            print *, "4"
        case (7,8)    
            print *, "7,8"
        case (90:100, 200: )    
            print *, ":3, 10:100"
    END SELECT
end program test
