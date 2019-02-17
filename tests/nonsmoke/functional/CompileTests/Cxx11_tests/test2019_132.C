namespace N
   {
     enum class color : long;
   }

// BUG: The CPP directive will be unparsed in the namespace redundantly.
#if 1
// Typedef base type struct should reference the one in the namespace N.
// Should be unparsed as: "N::color" instead of "color".
typedef enum class N::color : long { RED } new_type;
#endif

void foobar()
   {
     new_type x = N::color::RED;
   }


