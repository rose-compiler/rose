// This is just a test code for viewing the AST
// from different language constructs.

int array[10];

#pragma STDC FENV_ACCESS ON
/* xxx */
void foobar()
   {
     array[0] = 0;
   }


#if 0
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
#endif
