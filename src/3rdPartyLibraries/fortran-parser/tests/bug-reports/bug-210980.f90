!
! bugs.eclipse.org bug # 210980
!
! T_CONTAINS was inlined for a contains-stmt.  Thus a contains statement can't
! have a label as it currently stands.  The contains-stmt needs to be put back as
! a statement rather than just an inlined token.
!

module foo

10 contains

subroutine boo
20 continue
end subroutine boo

end module foo
