! Test procedure-declaration-stmt
!      procedure-declaration-stmt  is  PROCEDURE ( [ proc-interface ] ) 
!                                        [ [ , proc-attr-spec ] ... :: ] 
!                                        proc-decl-list
!
!      proc-interface  is  interface-name
!                      or  declaration-type-spec
!
!      proc-attr-spec  is  access-spec
!                      or  proc-language-binding-spec
!                      or  INTENT ( intent-spec )
!                      or  OPTIONAL
!                      or  POINTER
!                      or  SAVE
!
!      proc-decl  is  procedure-entity-name [ => null-init ]
!
!      interface-name  is  name
!
! Not tested here: name, declaration-type-spec, access-spec, 
! proc-language-binding-spec, intent-spec, procedure-entity-name, and 
! null-init.

! Test with no optional parts.
procedure() a
procedure() a, b

! Test with optional proc-interface.
procedure(c) a
procedure(c) a, b
procedure(integer(4)) a

! Test with optional ::
procedure(c) :: a

! Test with optional proc-attr-spec
procedure(c), bind(c), public, pointer :: a, b

! Test with optional null-init
procedure(c), pointer :: func_ptr=>null(), b

end

