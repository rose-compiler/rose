! -------whole AST ------------
! omp_get_num_threads  has a SgInitializedName and a SgVariableSymbol.

! src/frontend/OpenFortranParser_SAGE_Connection/FortranParserActionROSE.C : 8485 removes the SgVariableSymbol from its SgSymbolTable.
! But  the symbol's parent pointer still points to the SgSymbolTable.
! AST consistency checking will later complain a child is not in its parent's children list.
! Leo

program  main
   implicit none
   external omp_get_num_threads
   integer omp_get_num_threads

   integer i
   i = omp_get_num_threads()
end
