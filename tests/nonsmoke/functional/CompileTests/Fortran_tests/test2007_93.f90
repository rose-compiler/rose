! Demonstrates that R836 is called instead of R834 (not clear when one is called vs. the other!)
subroutine example
    real :: b

! do statement with numeric label and a string label, using a labled do
 10 MY_DO: do 20 i=1,100
       b = 0
 20 end do MY_DO

end subroutine example
