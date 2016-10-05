/*
   Bug contributed by Yarden, IBM.
   This bug demonstrates the importance of the "struct" keyword in C
   and how it is optional in C++.
*/

struct a 
   {
     int x;
     float y;
     double z;
   };

void foo()
   {
#if __cplusplus
  // This works for C++, but not for C
     int size1 = sizeof(a);
#endif

  // This is generally required for C (but gcc allows dropping "struct")
     int size2 = sizeof(struct a);

  // Test the use of sizeof with an expression.
     int size3 = sizeof(26);
   }
