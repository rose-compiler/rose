// Example showing how the lex pass we use can not interpret the CPP directives to figure out
// which brace matches the extern "C" declaration.  The extern "C" declaration does not
// appear in the EDG AST and so we can't see when to use it in the AST.  The solution here is
// to unparse the code with all header files included at the top of the unparsed code.

// Note that only under this condition do we get a comment with a closing brace "}". 
// It is possible that a hack is possible to interpret this and do the write thing later, 
// this is not clear presently.  I would hate to implement such a hack!

extern "C"
{
#if 0
#include "test2001_18A.h"
}
#else
#include "test2001_18A.h"
}
#endif

void main ()
{
}
