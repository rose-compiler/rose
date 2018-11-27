!! R832 if-construct
!    is  if-then-stmt
!              block
!          [ else-if-smt
!              block ] ...
!          [ else-stmt
!              block ]
!         end-if-stmt
!
! Also tests rules R841 (if-then-stmt), R842 (else-if-stmt), R843 (else-stmt),
! R844 (end-if-stmt), and if-construct-name.  Does not test
! scalar-logical-expr.

10 if(x .eq. y) then
   x = 1
11 else if(x .lt. y) then
   x = 0
12 else
   x = -1
13 end if

if(x .eq. y) then
   x = 1
else
   x = -1
end if

if(x .eq. y) then
   x = 1
end if

my_if: if(x .eq. y) then
   x = 0
else if(x .lt. y) then my_if
   if (.false.) x = 1
else if(x .gt. y) then
   x = 2
   y = 13
else my_if
end if my_if

end
