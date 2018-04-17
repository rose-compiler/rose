// This is the more complex example of a bug distilled in test2007_171.C

extern void foo();

class A
   {
     private:
          inline int foo() const
             {
               return 7;
             }

     private:
          inline int bar() const
             {
               return (this) -> foo() / 2;
             }

     public:
          inline int biz() const // note the public modifier here
             {
               int result = 0;
               for (int i = 1; i <= (this) -> foo(); i++) {
                    for (int j = 1; j <= (this) -> bar(); j++) {
                         result += (i * j);
                    }
               }
               return result;
             }

   };

void foo()
   {
     A x;

  // If this is NOT public then we will generate an error (current bug)
     x.biz();
   }
