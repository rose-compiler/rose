
enum numbers { null };

void foobar_number(numbers n);

numbers zero = null;

namespace X
   {
     void foobar_number();
     void foo()
        {
          float* null = 0;

       // This will be constant propagated to be 0, but the values 
       // used in the generated code will be the enum constants 
       // which require global qualification.
          double global_null_value = ::null;

          foobar_number(::null);
        }
   }
