! { dg-do compile }
! { dg-options "-std=legacy" }
!
! PR fortran/34557
!
! Substrings with space before '(' were not properly parsed.
!
   implicit none

 ! Correct in the AST, but unparses as: "character(len=2) :: A(2,2),B(2),C" (not clear how to make this a compiler error).
   character :: A(2,2)*2, B(2)*3, C*5

 ! Not clear if it is correct in the AST, but also unparses incorrectly to "equivalence ( B(1),2:3,C(3:5) )" (not clear how to make this a compiler error).
   equivalence (A (2,1) (1:1), B (1) (2:3), C (3:5))
end
