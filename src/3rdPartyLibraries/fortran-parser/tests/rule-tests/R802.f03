! Test if-construct
!      if-construct  is  if-then-stmt
!                          block
!                          [ else-if-smt 
!                              block ] ...
!                          [ else-stmt 
!                              block ]
!                        end-if-stmt
!
! Also tests rules R803 (if-then-stmt), R804 (else-if-stmt), R805 (else-stmt), 
! R806 (end-if-stmt), and if-construct-name.  Does not test 
! scalar-logical-expr.
if(x .eq. y) then
   x = 1
else if(x .lt. y) then
   x = 0
else
   x = -1
end if

if(x .eq. y) then
   x = 1
else
   x = -1
end if

if(x .eq. y) then
   x = 1
end if

my_if: if(x .eq. y) then
   x = 1
else if(x .lt. y) then my_if
   x = 0
else my_if
   x = -1
end if my_if

end

