
namespace X
{
struct B
   {
     void foo();
   };
}

struct D : X::B
   {
     void foo(int i);
   };

namespace X
{
void foobar(D *p)
   {
     p->B::foo();
   }
}

