
#include "stdio.h"

#include <list>
#include <vector>

using std::list;
using std::vector;

class X
   {
     public:

#if 0
          enum long_enum
             {
               v0  = 0x0000000000000001,
               v1  = 0x0000000000000002, 
               v2  = 0x0000000000000004, 
               v3  = 0x0000000000000008, 
               v4  = 0x0000000000000010, 
               v5  = 0x0000000010000000, 
               v6  = 0x0000000010000000, 
#if 0
               v7  = 0x0000000100000000, 
               v8  = 0x0000001000000000, 
               v9  = 0x0000010000000000, 
               v10 = 0x0000100000000000, 
               v11 = 0x0001000000000000, 
               v12 = 0x0010000000000000, 
               v13 = 0x0100000000000000, 
               v14 = 0x1000000000000000, 
#endif
               v_max
             };

          long_enum value;
#else
          X();

          enum long_enum
             {
               v0  = 0,
#if 1
               v1, 
               v2, 
               v3, 
               v4, 
               v5, 
               v6, 
               v7, 
               v8, 
               v9, 
               v10, 
               v11, 
               v12, 
               v13, 
               v14, 
#endif
               v_max
             };

          vector<bool> value;
#endif

   };

X::X ()
   {
     for (int i=0; i < v_max; i++)
        {
          value.push_back(false);
        }

     printf ("value.size() = %zu \n",value.size());
   }

int main()
{
   X a;

   list<int> intList;
   vector<bool> bitVector;

   printf ("sizeof(int) = %d \n",sizeof(int));
   printf ("sizeof(long) = %d \n",sizeof(long));
   printf ("sizeof(long long) = %d \n",sizeof(long long));
   printf ("sizeof(X) = %d \n",sizeof(X));
   printf ("sizeof(X.value) = %d \n",sizeof(a.value));

     for (int i=0; i < X::v_max/2; i++)
        {
          a.value[i] = true;
        }

     for (int i=0; i < X::v_max; i++)
        {
          printf ("X.value[%d] = %s \n",i,a.value[i] ? "true" : "false");
        }


   return 0;
}





