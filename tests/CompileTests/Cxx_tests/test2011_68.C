
// Make sure that we resolve the correct function when the names match.
namespace Z { void foo(); }

void foobar()
   {
     Z::foo();
   }

void foo();

