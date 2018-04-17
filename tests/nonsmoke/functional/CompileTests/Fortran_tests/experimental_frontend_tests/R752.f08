!! R752 forall-header
!    is ( [ type-spec :: ] forall-triplet-spec-list [, scalar-mask-expr] )
!
!   - F2008 addition
!   - FORALL index can have its type and kind explicitly declared within the construct
!

forall (integer(kind=4) :: ii = 1:10, jj = 1:10)
end forall

end
