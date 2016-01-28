template <int Value>
struct keyword
   {
  // This is a static template variable.
     static keyword<Value> instance;
   };

template <int Value> keyword<Value> keyword<Value>::instance = {};

void foobar()
   {
     keyword<3> ::instance;
     keyword<4> ::instance;
  // keyword<5> ::instance;
   }
