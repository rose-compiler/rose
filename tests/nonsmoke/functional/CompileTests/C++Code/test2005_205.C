
enum numbers { null };

void foobar_number(numbers n);

namespace X {
void foobar_number();
void foo()
   {
     float* null = 0;

  // This will be constant propagated to be 0
     double global_null_value = ::null;

     foobar_number(::null);
   }

}
