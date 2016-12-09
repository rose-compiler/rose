#if 1
namespace my_namespace_X 
   {
     typedef int my_integerType;
   }
#endif

#if 1
namespace my_namespace_X 
   {
     typedef int my_floatType;
   }
#endif

int main()
   {
#if 1
     my_namespace_X::my_integerType A;
     my_namespace_X::my_floatType B;
#endif

     return 0;
   }

