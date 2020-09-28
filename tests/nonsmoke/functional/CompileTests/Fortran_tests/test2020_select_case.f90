program test
    INTEGER :: x
    x=2
    SELECT CASE (x)
        case (:3)
            print *, ":3, PASS"
        case (4)
            print *, "4"
        case (5:6)
            print *, "5:6"
        case (7,8)
            print *, "7,8"
        case (90:100, 200: )
            print *, "90:100, 200:"
    END SELECT
end program test
