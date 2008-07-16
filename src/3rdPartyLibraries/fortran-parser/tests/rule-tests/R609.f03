! Test substring, which is:
!      substring  is  parent-string ( substring-range )
!
! Tested separately are: R610 (parent-string) and R611 (substring-range).
!
! substring tested as part of a designator, which can be part of a primary, 
! so tested as a rhs to an assignment-stmt.
a = my_string(2:3)

end

