#include <cstddef>

class Base_Class {};

template<typename T>
class X : Base_Class
   {
     public:
          X<T>& operator=(X* x) { return *this; }
       // X<T>& operator=(nullptr_t x) { return *this; }
          X<T>& operator=(std::nullptr_t x) { return *this; }
   };



