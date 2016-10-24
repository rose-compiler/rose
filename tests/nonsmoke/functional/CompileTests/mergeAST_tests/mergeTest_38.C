// Note that the use of __builtin_va_list causes a "void*" to be referenced in 
// the AST which will show up as a disconnected graph in graph of the whole AST.
int foobar (__builtin_va_list);
