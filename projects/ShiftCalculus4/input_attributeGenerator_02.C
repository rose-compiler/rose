// Include the API specification for DSL attributes so that we can more easily generate instantiations.
// This step could perhaps be automated, but it is not important at this point.
#include "dslAttribute.h"

// Example DSL API specification
template < typename T >
class NVRAMptr
   {
     private:
          T* internal_ptr;

     public:
          NVRAMptr(T* s);
          NVRAMptr operator* (const NVRAMptr & X) const;
   };

