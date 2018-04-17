/*
compiling the following code with ROSE:
class MyError {
    static char *ErrorMsg[];
};

char *MyError::ErrorMsg[] = {
#include "sgnodee.hpp"
"User error number clash",
""
};
where "sgnodee.hpp" is:
         "EOF expected",
gives the following error:
In file included from rose_AgDefaultError.C:9:
../../../../ROSE/src/midend/astProcessing/sgnodee.hpp:1: error: parse error
   before string constant

Andreas
*/


class MyError
   {
     static char *ErrorMsg[];
   };

// DQ (8/14/2006): This include file will be placed into the initializer but will then redundantly represented!
char *MyError::ErrorMsg[] = 
   {
#include "sgnodee.hpp"
"User error number clash",
""
   };


