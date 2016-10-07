#include <initializer_list>
class X
   {
     public:
          X (std::initializer_list<int> list);
   };

X x = {42};

