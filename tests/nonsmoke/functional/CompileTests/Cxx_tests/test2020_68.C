// Note that only the first non-defining declaration needs to be specified as extern "C"
// And it does appear to be marked as such in the ROSE AST.
extern "C" void foobar();

// extern "C" void foobar() {}
void foobar() {}
