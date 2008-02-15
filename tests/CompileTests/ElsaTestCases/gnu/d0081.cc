// dsw: g++ seems to define a variable __null that my guess is a void*
// to 0; test that we can typecheck it.
void *x = __null;
