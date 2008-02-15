// here is a funny one:
// f1 returns a pointer to a function member f2 which returns an integer
// EDG doesn't parse it

// Another interesting point is that f2 is called before it is declared in the 
// class. So this test the handling of this point in the EDG/Sage III translation.

class A;

typedef bool (A::*g)(int);

class A 
   {
     public:
          g f1(char c)
             {
               return &A::f2;
             }

          bool f2 (int i)
             {}

          void f3()
             {
               bool b;
               char c;
               int i;
               (this->*f1(c))(i);
             }

       // bool (A::*pf)(int);
   };
