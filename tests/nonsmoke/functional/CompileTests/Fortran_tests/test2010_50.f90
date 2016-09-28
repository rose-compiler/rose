! Dan,
! Here is a blocking bug when I try to compile NPB's BT benchmark.
! It looks like the 'include' statement causes problem for the name stack.
! Commenting it out will let ROSE handle the input code correctly.
! Leo
!
! Error: name stack is empty when handling variable:block_size
! roseCompiler: ../../../../sourcetree/src/frontend/OpenFortranParser_SAGE_Connection/fortran_support.C:1836: 
!    SgVariableSymbol* trace_back_through_parent_scopes_lookup_variable_symbol(const SgName&, SgScopeStatement*): 
!    Assertion (*(FortranParserState::getCurrentNameStack())).empty() == false failed.
! Aborted
!

program BT
     implicit none

     include 'test2010_50.h'
!    integer problem_size
!    parameter (problem_size=12)

     integer           BLOCK_SIZE
     parameter        (BLOCK_SIZE=5)
end

