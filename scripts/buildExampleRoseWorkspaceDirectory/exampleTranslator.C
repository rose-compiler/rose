/******************************************************/
/*                Example Translator                  */
/******************************************************/

#include "rose.h"

int 
main (int argc, char *argv[])
   {
   /* build AST */
     SgProject* project = frontend(argc, argv);

  // make sure AST is well formed (optional)
     AstTests::runAllTests(project);

  // Put any analysis or transformation of the AST here...

     return backend(project);
   }
