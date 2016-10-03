enum numbers { null };
typedef numbers local_null;

namespace X 
   {
     typedef float* local_null;
     typedef ::local_null global_null;
     void foo(global_null x = null )
        {
        }
   }
