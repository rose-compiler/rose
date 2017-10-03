!! R448 type-bound-procedure-stmt
!    is PROCEDURE [ [ , binding-attr-list ] :: ] type-bound-proc-decl-list
!    or PROCEDURE (interface-name ), binding-attr-list :: binding-name-list
!
!   - F2008 addition
!   - multiple type-bound procedures can be declared in a single type-bound
!     procedure statement
!
module myjunk

TYPE POINT
CONTAINS
   PROCEDURE, PASS :: LENGTH => POINT_LENGTH
   procedure length => point_length
   procedure :: length => point_length
10 procedure (proc_name), non_overridable, DEFERRED :: sink, or, swim
END TYPE POINT

end module
