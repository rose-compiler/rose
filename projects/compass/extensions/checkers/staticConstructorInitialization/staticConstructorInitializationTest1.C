// This test code demonstrates static constructor initialization of class objects.
// The order of the calls to the constructors are compiler dependent.

class A { public: A(int); };
class B { public: B(int); };

// constructor initialization calls can appear in any order
A a = 0;
B b = 0;

class C
   {
     public: 
          static A a;

          C(int);
   };

// constructor initialization calls can appear in any order
A C::a = 0;

