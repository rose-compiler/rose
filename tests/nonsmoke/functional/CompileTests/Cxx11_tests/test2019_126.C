namespace N
   {
     enum class color : long;
   }

typedef enum class N::color : long { RED } new_type;

void foobar()
   {
     new_type x = N::color::RED;
   }
