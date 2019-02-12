static long h_ = 0;

struct T 
   {
     int i;
     T(int j) : i(j) { h_ = 1 + 10*h_; }
    ~T() { h_ = 2 + 10*h_; }
   };

struct S 
   {
     S(int j = T(0).i) { h_ = j + 3 + 10*h_; }
    ~S() { h_ = 4 + 10*h_; }
   };

S* foo(int n) 
   {
     return new S[n];
   }

void foobar()
   {
     S* p = foo(3);
   }
