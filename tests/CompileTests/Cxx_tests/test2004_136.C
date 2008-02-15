
class X
   {
     public:
          static int y;
   };

int X::y = 0;

void foo ( int a = X::y )
   {
     int x = X::y;
   }

namespace Y
   {
     class X
        {
          public:
            // test handling of static variables
               static int x;
               static int y;
               static int z;

            // Can't specify storage class for class member
            // extern int a;
        };

  // Can be defined in an intermedate namespace (not just global scope)
     int X::x = 0;   
  // Can be over qualified!
     int Y::X::y = 0;   
   }

// Can be defined in the global scope using a fully qualified name
int Y::X::z = 0;

// Definition for extern variable in class nested in namespace (g++ accepts this, but EDG does not!)
// int Y::X::a = 0;
