
typedef float* xType;

class foo
   {
     public:
          typedef int xType;
          float x;
          void prod ();

       // Error: this looses its global name qualifier ("::" on the xType)
          ::xType foobar();
   };

// build the global version of variable "x" to be a pointer 
// (so that if "::" is dropped we will generate an error)
float *x;

void prod (int x);

xType foobar(int x);

void foo::prod ()
   {
  // Error if global scope name qualification is dropped in generated code!
     ::prod (0);

  // Error if global scope name qualification is dropped in generated code!
     ::x = &x;

  // Error if global scope name qualification is dropped in generated code!
     float *y1 = foobar();
     float *y2 = (::xType) foobar();
     float *y3 = static_cast< ::xType >(foobar());

     float *y4 = ::x;

     ::xType z = &x;
   }
