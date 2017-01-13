// 7.3.4c.cc

asm("collectLookupResults i=9 i=9 j=14");

namespace A {
  int i;                         // line 6
}
namespace B {
  int i;                         // line 9
  int j;                         // line 10
  namespace C {
    namespace D {
      using namespace A;
      int j;                     // line 14
      int k;                     // line 15
      int a = i;                 // B::i hides A::i
    }
    using namespace D;
    int k = 89;                  // no problem yet              line 19
    //ERROR(1): int l = k;       // ambiguous: C::k or D::k     line 20
    int m = i;                   // B::i hides A::i             line 21
    int n = j;                   // D::j hides B::j             line 22
  }
}
