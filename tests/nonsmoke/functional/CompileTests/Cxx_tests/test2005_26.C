/*
   The following code fails when the header file test2005_26.h contains: "const double pi = 3.14159265;"
   The variable declaration in the header file is output in the SgBasicBlock along with the #include
   directive and this causes the multiple definition of the variable "pi".
 */

void foo()
   {
#include "test2005_26.h"
     double one = 1.0;
   }
