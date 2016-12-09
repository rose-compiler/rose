subroutine where_example
    real :: a(50)

    a(1:2:3) = 0
    a(1:2)   = 0
    a(1::3)  = 0
    a(::3)   = 0 
    a(:2)    = 0
    a(1:)    = 0 
    a(:)     = 0
    a(1)     = 0 ! scalar indexing

end subroutine where_example

