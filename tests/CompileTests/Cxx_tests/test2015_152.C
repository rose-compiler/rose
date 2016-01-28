template <int Value>
struct keyword
   {
  // This is a static template variable.
     static keyword<Value> instance;
   };

template <int Value> keyword<Value> keyword<Value>::instance = {};

// Note that when the testTemplats translator is used the instantiated 
// classes will be output as class definitions and they should be prototype 
// class declarations (if they should even be output at all).

void foobar()
   {
     keyword<3> ::instance;
  // keyword<4> ::instance;
  // keyword<5> ::instance;
   }
