namespace X 
{
   int a;
}

namespace X { int b; }

using namespace X;

void foo()
   {
     b = 0;
  // X::b = 0;
   }

// namespace X { int b; }
