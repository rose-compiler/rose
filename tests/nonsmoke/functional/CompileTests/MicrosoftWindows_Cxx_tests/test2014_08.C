// __declspec(dllimport) class X {} varX;
__declspec(dllimport) int a;

#if 1
void foo()
   {
  // __declspec(dllimport) class Y {} varY;
     __declspec(dllimport) int b;
   }
#endif
