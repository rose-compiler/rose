! Sometimes it is necessary to jump out of more than the innermost DO loop. To allow this, loops 
! can be given names and then the EXIT statement can be made to refer to a particular loop. An 
! analogous situation also exists for CYCLE. The (optional) name following the EXIT or CYCLE 
! highlights which loop the statement refers to.

subroutine foo(i)
   INTEGER :: a,b

! Example of code using named do loops
    outa: DO
     inna: DO
      IF (a.GT.b) EXIT outa
      IF (a.EQ.b) CYCLE outa
      IF (c.GT.d) EXIT inna
     END DO inna
    END DO outa

end subroutine

! For example,
!   IF (a.EQ.b) CYCLE outa
! causes a jump to the first DO loop named outa.
! Likewise,
!   IF (c.GT.d) EXIT inna
! jumps to the statement after
!   END DO inna
! If the name is missing then the directive is applied, as usual, to the next outermost loop.
! The scope of a loop name is the same as that of any construct name.

