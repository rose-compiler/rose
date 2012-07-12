
#include "rose.h"

int main( int argc, char * argv[] )
   {
     SgProject* project = frontend(argc,argv);

     project->unparse();
     
     return 0;
   }
