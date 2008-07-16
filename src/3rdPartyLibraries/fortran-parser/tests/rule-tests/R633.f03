! Test nullify-stmt, which is:
!      nullify-stmt  is  NULLIFY ( pointer-object-list )
!
! Tested separetly is: pointer-object-list (R634).
nullify(a)
nullify(a%b)

end
