

namespace N 
   {
     template <int v>
     struct X 
        {
          static const int value = v;
        };
   }

template <int s>
struct Y 
   {
     int array[ N::X<s>::value ];
   };

static_assert(sizeof(Y<5>) == 5 * sizeof(int),"");


