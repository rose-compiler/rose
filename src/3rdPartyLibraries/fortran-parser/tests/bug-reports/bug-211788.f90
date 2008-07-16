! Test for bug report 211788 on bugs.eclipse.org
! The following
!
!     DATA FLUX / -1.0 /
!
! doesn't produce a negative sign token.  I don't believe that data_stmt_value
! produces either a T_PLUS or a T_MINUS token.  T_ASTERISK in data_stmt_value is
! not sent to actions either.

DATA FLUX / -1.0 /
data flan /10*-1/
end
