// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"

int main( int argc, char * argv[] )
   {
  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);

  // Generate a PDF file for interactive exploration of the AST.
     generatePDF ( *project );

     return 0;
   }

