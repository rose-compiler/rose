
! Matt's bug in ac_implied_do. It turned out that the rule just 
! didn't have an action. - Bryan R.

integer :: i
integer :: a(10)

a = (/ (i,i=1,10) /)
end
