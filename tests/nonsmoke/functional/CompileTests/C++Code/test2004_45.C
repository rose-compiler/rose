class A
   {
     public:
          static void foobar ();
   };

#if 0
class B
   {
     public:
          A* integerPointer;
          void foo ();
   };

void
B::foo()
   {
     integerPointer->foobar();
   }
#endif

int main()
   {
     A* a;
     a->foobar();
   }
