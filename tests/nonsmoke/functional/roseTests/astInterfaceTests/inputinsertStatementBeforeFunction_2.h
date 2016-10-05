// Introduce a variable that uses a variable inserted as a transformation
// Protect with include guards so that it will compile with ROSE the first time
// but require InsertStatementAfterThisFunction_var to be defined in the generated
// code compilied using the backend compiler.
#ifndef USE_ROSE
int variable_hidden_in_header_file_2 = InsertStatementAfterThisFunction_var;
#endif
