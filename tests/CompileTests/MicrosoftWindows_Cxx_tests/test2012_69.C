// Example of curret error: 
//      rose_test2005_134.C:24: error: too few template-parameter-lists

template <typename T> 
class Y
   {
     public:
          typedef T scalar;
          Y()
             {
               T x;
             }
   };

class X
   {
     void foo ()
        {
          Y<int> y;
       // int x = foobar<int>();
          for (int i = 0; i != 10; i++)
             {
             }
        }
};

