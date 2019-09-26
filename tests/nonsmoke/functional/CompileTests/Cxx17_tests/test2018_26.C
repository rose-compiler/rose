// Lambda Capture of *this by Value as [=,*this]

struct S2 { void f(int i); };

void S2::f(int i) 
   {
     [&, i]{ }; // OK
  // [&, &i]{ };// error: i preceded by & when & is the default
  // [=, *this]{ };// OK
  // [=, this]{ };// error: this when = is the default
  // [i, i]{ };// error: i repeated
  // [this, *this]{ };// error: this appears twice
   }


