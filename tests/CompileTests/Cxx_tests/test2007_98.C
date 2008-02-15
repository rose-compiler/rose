/*
Compiling the following code:
__asm__ (
".text\n\t"
"ret\n"
);

gives the following error:

The case of an asm declaration may have more operands not yet saved in the AST
lt-identityTranslator:
/home/andreas/REPOSITORY-SRC/ROSE/June-29a-Unsafe/NEW_ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:33480:
SgAsmStmt* sage_gen_asm_decl(): Assertion false failed. 
*/

__asm__ (
".text\n\t"
"ret\n"
);

