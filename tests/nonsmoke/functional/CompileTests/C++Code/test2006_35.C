// DQ (2/7/2006): This was previously demonstrated in test2004_135.C
// but the relavant part was commented out.

// This example demonstrates a bug in ROSE when nested 
// namespaces are used and a namespace alias is formed
// using a qualified namespace.

namespace G
   {
     namespace H
        {
          class I{};
        }
   }

// class G::H::I {};

// namespace aliasing can only build an alias in the scope of the namespace alias declaration
namespace GG = G;

  // DQ (11/19/2004): Temporarily commented out since this is a demonstrated bug now that we qualify everything!
#if 1
namespace GH = G::H;
#endif
