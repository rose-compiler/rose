module mpi
 ! This results in the function non-defining declaration in the nearest module scope (SgClassDefinition) (not the global scope).
 ! And the symbol table fixed up as expected.
   EXTERNAL MPI_WTIME

 ! Here the function's return type is fixed up using this information.
 ! However, this is constructed in the AST as a variable declaration which is not correct since it is not a variable declaration.
   REAL*8 MPI_WTIME

end module

