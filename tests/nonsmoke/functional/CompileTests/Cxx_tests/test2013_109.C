#if 1
   #include <vector>
#else
namespace std
   {
     template <typename T>
     class vector
        {
          T* basePointer;

          public:
               T & operator[](int n) { return *(basePointer + n); }
        };
   }
#endif

int z = 0;

class X
   {
     public:
         void set( std::vector<double> & data )
             {
                  {
                    int a;
                    data[z] = 0.0;
                 // int b;
                  }

                  // int x;
                  // return;
             }
      // int y;
   };

// int abc;

