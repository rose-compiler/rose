class base_12
   {
     public:
          class nested_class
             {
               public:
                    int i;
             };

     int i;
   };

struct X : public base_12, public base_12::nested_class
   {
   };

void foobar()
   {
     X x;

  // x.base_12::i;

  // Original code: x.base_12::nested_class::i;
  // Unparsed as:   x.nested_class::i;
     x.base_12::nested_class::i;
   }

