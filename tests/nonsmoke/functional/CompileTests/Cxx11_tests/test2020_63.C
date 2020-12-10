
class base_12
   {
     public:
          typedef int integer;
          class nested_class
             {
               public:
                    struct nested_nested_class
                       {
                         integer i;
                       };

                    integer i;
             };

          integer i;
   };

struct X : public ::base_12, public base_12::nested_class
   {
     double d;
   };

void foobar()
   {
     X x;

     x.base_12::i;
     x.base_12::nested_class::i;
   }


