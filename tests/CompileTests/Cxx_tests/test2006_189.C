// This code appears to violate the rule that SgFunctionDeclaration 
// always have a valid firstNondefiningDeclaration pointer.

/* CI and AS (061219) Guess to the problem:
   ROSE normalizes all function
   declarations into having at least one non-defining
   declaration. This is an example where the use
   of an extern function will make this
   normalization fail.
*/

//somehow it seem like the two following functions are confused
//with each other.
extern int returnSixFunction();
int z()
   {
     return 5;
   }

void f2()
   {
     int(*fptr)();	

     fptr=returnSixFunction;

     fptr();
   }

