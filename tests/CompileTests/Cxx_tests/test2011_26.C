

namespace Y { int x; }

namespace Z { using namespace Y; int z; } // test transitive usement of namespaces

void foo()
   {
     using Z::x; // valid variable symbol for next scope

     x = 0.0;
   }
