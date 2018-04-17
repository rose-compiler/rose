// A typedef-name can also be introduced by an alias-declaration. The identifier 
// following the using keyword becomes a typedef-name and the optional 
// attribute-specifier-seq following the identifier appertains to that typedef-name. 
// It has the same semantics as if it were introduced by the typedef specifier. 
// In particular, it does not define a new type and it shall not appear in the type-id.

// I know that in C++11 we can now use using to write type alias, like typedefs:
typedef int MyInt_A;

// Is, from what I understand, equivalent to:
using MyInt_B = int;

// And that new syntax emerged from the effort to have a way to express "template typedef":
class MyAllocatorType;
template< class T, class U > class AnotherType;

template< class T > using MyType_C = AnotherType< T, MyAllocatorType >;
