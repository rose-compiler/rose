
#if 0
struct myType
   {
     int x;
   };
#endif

#if 0
struct
   {
     int x;
   } myVar;

void foo()
   {
     myVar.x = 0;
   }
#endif

#if 1
typedef struct foo
   {
     int x;
   } mystruct;

mystruct X;
#endif

#if 0
// #include "stdio.h"

// typedef int my_size_t;

// float abcdefghij;
// double klmnopqrs;

class B
   {
#if 1
     public: double b;
#else
     public:
     protected: int a;
     public: int b;
     private: int c;
#endif
   };

#if 0
int a = 2;
int b;
int y = a + 42;
#endif

#endif


