// This should be a single variable declaration, but instead it is two separate declarations.
// this is OK here, but a bug when it is handled this way inside of a for loop initialization.
// See test2012_106.C.
// class A0 {public: int foo (int x) {return x;}} x2;
#if 1
#if 0
class A0 { int x; };
A0 x2;
#else
class A0 { int x; } x2;
#endif
#endif

#if 0
void foo (int x)
   {
  // This should be a single variable declaration, but instead it is two seperate declarations.
  // this is OK here, but a bug when it is handled this way inside of a for loop initialization.
  // See test2012_106.C.
// #if 1
//   class A0 {public: int foo (int x) {return x;}} x2;
// #else
     for (class A0 {public: int foo (int x) {return x;}} x2; x2.foo (0); x) {}
// #endif
   }
#endif
