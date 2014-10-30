// here is a funny one:
// f1 returns a pointer to a function member f2 which returns an integer
// EDG doesn't parse it

class A;

typedef bool (A::*g)(int);

class A 
   {
     public:
#if 0
       // This test code works fine if the member function has already been seen (easy case).
          bool f2 (int i)
             {}
#endif
          g f1(char c)
             {
            // Unless seen previously (see commented out block above), this is the first reference to this function.
               return &A::f2;
             }
#if 1
       // This is the first definition of the function that was previously referenced (harder case).
          bool f2 (int i)
             {}
#endif
#if 1
          void f3()
             {
               bool b;
               char c;
               int i;
               (this->*f1(c))(i);
             }
#endif
   };

