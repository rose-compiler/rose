// This code by Erin Parker demonstrates a bug in EDG and SAGE II.
// In EDG the exception handling in not turned on.
// And in SAGE II there is some STL code that is not implemented
// (part of the move to using STL in SAGE).  It should be a simple fix.

/* try_catch_test.C
   
   Test of TRY_STMT and CATCH_STMT modifications to 
   ROSE/src/pdf_ast.C.

   Erin Parker, 23 May 2001
*/

#include<stdio.h>

double divide(double top, double bottom) 
   {
     int divide_by_zero = 1;

     if(bottom == 0.0) 
        {
          throw divide_by_zero;
        }
       else 
        {
          return top/bottom;
        }
   }

int main() 
   {
     double result;

     try 
        {
          result = divide(12.3, 0.0);
          printf("The result is %g.\n", result);
        }
  // Bug in use of named exception (does not unparse correctly)
     catch(int i)
        {
          printf("ERROR:  Divide by zero.\n\n");
        }
   }

