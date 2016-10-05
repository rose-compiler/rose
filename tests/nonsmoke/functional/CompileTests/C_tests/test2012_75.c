
// Location recorded for un-named parameter is take from the defining declaration on line #10
static void foobar( int abcdefg );



// The location of the named function parameter below is used for that of the un-named parameter above.
// This causes comments to be incorrectly woven into the AST and unparsed incorrectly.  This in turn
#include "test2012_75.h"
// causes the gene4ated code to not compile properly (effects grep fts.c file).
static void foobar( int xyz )
   {
   }

