#if 0
class X
   {
     public:
          int x;
   };
#endif

namespace X
   {
     int x;
   }

// Secondary namespace is a problem (ending position not correctly marked in source position information).
namespace X
   {
     int y;
   }

   
