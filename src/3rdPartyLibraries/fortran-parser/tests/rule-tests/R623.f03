! Test allocate-stmt, which is:
!      allocate-stmt  is  ALLOCATE ( [ type-spec :: ] allocation-list 
!                           [, alloc-opt-list ] )
!
! Tested separetly are: type-spec (R401) and allocation-list (R628).
allocate(a,b)
allocate(integer::a,b)
allocate(a,stat=i, errmsg=blah, source=k)

end

