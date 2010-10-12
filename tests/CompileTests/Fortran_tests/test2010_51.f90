! Adding function name = partit_size to the global scope (even thouhg we have not seen the definition yet)
! Adding function name = partit_size to the global scope (even thouhg we have not seen the definition yet)
! roseCompiler: ../../../../sourcetree/src/frontend/OpenFortranParser_SAGE_Connection/FortranParserActionROSE.C:6629:
!    void c_action_common_block_object(Token_t*, ofp_bool): Assertion `variableSymbol != __null' failed.
! Aborted

! The problem here is that "naa" is declared after the common block declaration.
! Thus the variable can't be found (variableSymbol is NULL).

program cg
     implicit none
   ! common / partit_size  /    naa
     common /partit_size/    naa
     integer                    naa
end
