#if 0
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
#endif


class MyError
   {
     static char *ErrorMsg[];
   };

char *MyError::ErrorMsg[] = {
#include "sgnodee.hpp"
"User error number clash",
""
};


