// Bug demonstrated in test2004_35.C
// Note namespace "std" is special and only appears as a secondary declaration
// so the symbol table handling for it needs to be a bit special.

namespace std
   {
//   int var_A;
   }

#if 0
namespace std
   {
     int var_B = var_A;
   }
#endif


