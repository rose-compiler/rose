! Test part-ref, which is:
!      part-ref  is  part-name [ ( section-subscript-list ) ]
!
! part-name is a name->variable->T_IDENT.
! Tested separetly is: section-subscript-list (R619).
!
! part-ref tested as part of an assignment-stmt.
a(1:3) = 3
a = 3

end

