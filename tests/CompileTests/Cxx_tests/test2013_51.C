// Test of un-named namepsace...found in BoxLib.

using namespace std;

// namespace X
// namespace Y

namespace X 
   {
     int x;
   }

// namespace X {}

// namespace {}


namespace Y
   {
     using namespace X;
   }

namespace Y
   {
     using namespace X;
   }

using namespace X;
using namespace Y;
