
namespace A
   {
     int a;

  // BUG: The symbol for b is not also place in namespace A using a SgAliasSymbol.
     inline namespace B
        {
          int b;
        }
   }

