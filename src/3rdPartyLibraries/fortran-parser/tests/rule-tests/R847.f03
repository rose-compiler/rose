! Test arithmetic-if-stmt
!      arithmetic-if-stmt  is  IF ( scalar-numeric-expr ) label, label, label
!
! Tested here: arithmetic-if-stmt
!
! Not tested here: scalar-numeric-expr and label.
if(x) 10, 20, 30

10 print *, "hello from label 10"
20 print *, "hello from label 20"
30 print *, "hello from label 30"

end

