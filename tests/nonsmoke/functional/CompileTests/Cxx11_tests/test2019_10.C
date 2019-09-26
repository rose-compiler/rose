// Allow sizeof to work on members of classes without an explicit object

// In C++03, the sizeof operator can be used on types and objects. But it cannot be used to 
// do the following:

// typedef int OtherType;

struct SomeType { int member; };

void foobar()
   {
     sizeof(SomeType::member); // Does not work with C++03. Okay with C++11
   }

// This should return the size of OtherType. C++03 does not allow this, so it is a compile error. 
// C++11 does allow it.
