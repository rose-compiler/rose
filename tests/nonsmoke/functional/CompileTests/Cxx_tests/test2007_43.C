/*
The following code (extracted from mpi2c++):

namespace N {
  class A;
  class B {friend class N::A;};
}

unparses into the incorrect code (cleaned up):

namespace N {
  class A;
  class B {
    private: friend class  ;
  };
}

Is this a known bug?  If not, could you please add it to your tests? Thank you,

-- Jeremiah Willcock 
*/

namespace N
   {
     class A;
     class B {friend class N::A;};
   }
