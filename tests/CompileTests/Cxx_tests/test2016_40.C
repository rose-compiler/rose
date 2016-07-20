
// This test code demonstrates the use of a private type in the template argument list.

#include <map>

// This works fine
// std::map<int, int>::iterator it;

#if 1
class foo
   {
     public:
       // This works fine
       // std::map<int, int>::iterator it;
#if 1
void doSomething()
   {
  // This fails
     std::map<int, int>::iterator it;
   }
#endif
   };
#endif

#if 1
void doSomething()
   {
  // This works fine
  // std::map<int, int>::iterator it;
   }
#endif
