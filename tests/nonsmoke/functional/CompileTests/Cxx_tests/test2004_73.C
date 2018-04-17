// Test if nested base classes are properly resolved in NULL pointer initialization

// #define NULL (void* L0)
// #define NULL __null
// #define NULL ((void*) 0)
#define NULL (0L)

class A
   {
     public:
          class B
             {
             };

          B *b_ptr;

          typedef B typedefB;
   };

void foo()
   {
  // These work
     A a;
     a.b_ptr = NULL;

  // These work
     A::B *b;
     b = NULL;

  // This works
     A::B X;

  // typedef A::B innerClass;
     typedef A::typedefB innerClass;
     innerClass innerClassObject;
   }
