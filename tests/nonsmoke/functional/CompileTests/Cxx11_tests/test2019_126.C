namespace N
   {
     enum class color : long;
   }

// Typedef base type struct should reference the one in the namespace N.
// Should be unparsed as: "N::color" instead of "color".
typedef enum class N::color : long { RED } new_type;

void foobar()
   {
     new_type x = N::color::RED;
   }
