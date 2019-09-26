
class base_12
   {
     public:
          typedef int integer;
          class nested_class
             {
               public:
                    static int ctor;
                    struct nested_nested_class
                       {
                         static int ctor;
                         integer i;
                         nested_nested_class(int ii = 'n') : i(ii) { ++ctor; }
                       };
                    integer i;
                    nested_class(int ii) : i(ii) { ++ctor; }
             };
          static int ctor;
          integer i;
          base_12(int ii) : i(ii) { ++ctor; }
   };

int base_12::ctor = 0;
int base_12::nested_class::ctor = 0;
int base_12::nested_class::nested_nested_class::ctor = 0;


struct X_ : public ::base_12, public base_12::nested_class
   {
     double d;
  // Original code:     X_(int ii) : base_12(ii+1),   ::base_12::nested_class(ii-1), d(-ii) { }
  // Unparse as: inline X_(int ii) : base_12(ii + 1), nested_class(ii - 1), d((-ii)) { }
  // BUG: nested_class() must be unparsed as base_12::nested_class().
     X_(int ii) : base_12(ii+1), ::base_12::nested_class(ii-1), d(-ii) { }
   };

void foobar()
   {
     base_12::ctor = 0;
     base_12::nested_class::ctor = 0;
     X_ x(15);
   }


