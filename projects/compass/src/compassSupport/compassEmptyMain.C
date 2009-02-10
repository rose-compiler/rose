#include "rose.h"
#include "compass.h"
#include <rose.h>
#include <iostream>

// This file is a simple test of the compassSupport,  compass infrastructure
// and allows compass to be built without any checkers.

int 
main(int argc, char** argv)
   {
     /* SgProject* sageProject = */ frontend(argc,argv);

     Compass::Parameters params(Compass::findParameterFile());
     Compass::PrintingOutputObject output(std::cerr);

     return 0;
   }
