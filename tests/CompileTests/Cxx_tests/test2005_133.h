#if 1

#include <vector>

#else

// Use this branch for debugging only!

namespace std
   {
     template <typename T>
     class vector
        {
          T* basePointer;

          public:
               T & operator[](int n) { return *(basePointer + n); }
               T & begin() { return *(basePointer); }
               int size() { return 0; }
        };
   }

#endif
