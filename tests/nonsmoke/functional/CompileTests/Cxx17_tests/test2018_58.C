// Refining Expression Evaluation Order for Idiomatic C++

// This C++17 feature is representing only by defining some 
// limited expression evaluation orderings, and as a result 
// there are no specific langauge examples.

// Note that this feature in C++17 still does not define the 
// order of function argument evaluation, but it does add some 
// constraintes (may not be interleaved, as I recall).

#include <map>
int main() 
   {
     std::map<int, int> m;

  // When expression is evaluated first is not well defined.
     m[0] = m.size();  // #1
   }
