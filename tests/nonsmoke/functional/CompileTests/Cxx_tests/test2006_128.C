#if 0
The following code produces a compilation error when run through identityTranslator:

void x(const char*, char []);

The generated output is:

extern void x(const char *,char ()[]);

which has an extra pair of parentheses.  The first parameter is necessary to exhibit the bug, and changing it to "char*" or "const char" makes things work correctly.
-- Jeremiah Willcock 
#endif

void x(const char*, char []);
