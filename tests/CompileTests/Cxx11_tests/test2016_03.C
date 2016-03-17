#include <initializer_list>
class X
   {
     public:
          X (std::initializer_list<int> list);
   };

// C++11 Initialization List syntax was not implemented:
// This was unparsed as: class X x(((42),1UL));
X x = {42};

