class A 
    { 
      public: 
           template<typename T> 
           void foobar() 
              { 
                int abVar; 
#pragma rose_outline
                abVar = 42; 
              } 
    };
