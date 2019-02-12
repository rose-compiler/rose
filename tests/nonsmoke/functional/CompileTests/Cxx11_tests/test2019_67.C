struct B {};

typedef const int B::*ponter_to_member_integer_type;

template<ponter_to_member_integer_type Pointer> int foo();

void foobar()
   {
  // Should be unparsed to: foo< nullptr > ();
  // But is unparsed to:    ::foo< 0L > ();
     foo<nullptr>();
   }
