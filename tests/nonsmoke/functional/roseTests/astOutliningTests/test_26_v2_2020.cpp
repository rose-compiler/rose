// This is from the paper copy of test_26.cpp

namespace BAN
    { 
      enum B
         { 
           xx_ENUM
         };
    }

class C
    { 
      public:
           static C & get_C(void);
    };

void foobar()
    { 
      BAN::B abvar1;
      C::get_C();
#pragma rose_outline
      BAN::xx_ENUM == abvar1;
      bool abvar2 = (BAN::xx_ENUM == abvar1);
    }
