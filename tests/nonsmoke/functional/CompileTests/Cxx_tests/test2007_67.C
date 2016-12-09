#if 0
when compiling the attached file in ROSE I get the following error:

Error: virtual function
SgDeclarationStatement::get_symbol_from_symbol_table() called on base
class SgDeclarationStatement
lt-identityTranslator: Cxx_Grammar.C:42797: virtual SgSymbol*
SgDeclarationStatement::get_symbol_from_symbol_table() const:
Assertion false failed.
Aborted (core dumped) 
#endif

// This is a C99 specific bug, and works fine in C++.

void  alarm()
{
    enum {abort, scan} why = scan;

    while (why != abort)
    {
            why = abort; 
    }
}  

