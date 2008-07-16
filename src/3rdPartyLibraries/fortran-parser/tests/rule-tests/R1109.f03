! Test use-stmt
!      use-stmt  is  USE [ [, module-nature ] :: ] module-name [, rename-list ]
!                or  USE [ [, module-nature ] :: ] module-name, 
!                      ONLY : [ only-list ]
!
!      module-nature  is  INTRINSIC
!                     or  NON_INTRINSIC
!
!      rename  is  local-name => use-name
!              or  OPERATOR(local-defined-operator) =>
!                    OPERATOR(use-defined-operator)
!
!      only  is  generic-spec
!            or  only-use-name
!            or  rename
!
!      only-use-name  is  use-name
!
!      local-defined-operator  is  defined-unary-op
!                              or  defined-binary-op
!
!      use-defined-operator  is  defined-unary-op
!                            or  defined-binary-op
!
! Not tested here: generic-spec, only-use-name, local-name, use-name,  
! defined-binary-op, and defined-unary-op.
!
! Note: defined-binary-op and defined-unary-op are ambiguous to the grammar 
! and are both matched as T_DEFINED_OP in the lexer.

! Test with none of the optional parts
use a

! Include optional module nature
use, intrinsic :: iso_c_binding
use, non_intrinsic :: my_mod

! Include optional rename-list
use a, b=>c, d=>e
use a, operator(.myop.)=>operator(.yourop.), integer => real, &
     b => c, operator(.myotherop.) =>operator(.yourotherop.)

! Include optional only clause
use a, only: b, c=>d

end

