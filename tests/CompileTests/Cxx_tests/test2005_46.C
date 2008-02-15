/* Example exceptionProblem.C from Andreas */

/*
   This preprocessor if-directive is not true in g++, but triggers as
   true in ROSE.
*/

#include <cmath>

#if !(defined(test1) || defined(test2))
// || defined(gpp_Cplusplus))
inline float  abs      (float  r)          { return ( (r >= 0.0) ? r : -r ); }
#endif

int main(){

return 1;
}
