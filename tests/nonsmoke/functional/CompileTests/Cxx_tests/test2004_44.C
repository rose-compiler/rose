// This demonstrate a bug in g++

#define DEMO_BUG 1

class A 
   {
     public:
          A() {};
          A(const A & a) {};
          A(int x) {};
   };

// object to use in copy constructor, note that "A globalObject();" can't be
// used since it would be confused with a function prototype declaration.
A globalObject;

class B
   {
  // Data member to use in preinitialization list (without a parameter)
     int integer;
     A object;

     public:
#if DEMO_BUG
         // Example calling default constructor
            B() : integer(42), object() {};
         // Example calling copy constructor
            B(int x) : integer(x), object((const class A &)globalObject) {};
         // DQ (9/10/2005): Added previous case
            B(long x) : integer(x), object(globalObject) {};
         // Example calling another constructor
            B(int x, int y) : integer(x), object(y) {};
#else
//          B(){};
#endif
   };

// B object1;

void foo ()
   {
#if DEMO_BUG
     B object2;
#else
     B object2;
#endif

  // We need this line for g++ to generate the error
     const B & objectBref = object2;
   }
