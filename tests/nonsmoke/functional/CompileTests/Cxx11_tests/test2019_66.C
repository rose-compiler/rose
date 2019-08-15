
struct B {};

typedef const int B::*ponter_to_member_integer_type;

template<ponter_to_member_integer_type Pointer>
int foo();

void foobar()
   {
     const ponter_to_member_integer_type ponter_to_member = nullptr;

  // Should be unparsed to: foo< nullptr > ();
  // Or foo<ponter_to_member>();
  // But is unparsed to:    ::foo< 0L > ();
     foo<ponter_to_member>();
   }
