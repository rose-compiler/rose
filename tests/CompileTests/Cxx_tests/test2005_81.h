// header file for test2005_81.C
// demonstrates bug in how specializations are output for g++

namespace std
   {
     template<typename T>
     struct X
        {
          T t;
        };

     template<typename T>
     void foobar(T t1)
        {
          T t2;
        };
   }
