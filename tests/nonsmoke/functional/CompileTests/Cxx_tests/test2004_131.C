// This test code demonstrates a bug where the const modifier is lost in the unparsing when 
// applied to a typedef type.  Only works with a typedef type!  How bizzare ... 

#if 0
#include <string>
class A {};
namespace X
   {
     class B {};
     typedef B B_typedef;
   }

void foo ( int & x ) {}
void foo ( const int & x ) {}

// const is lost in the 2nd function because std::string is a typedef
void foo(std::string & x) {}
void foo(const std::string & x) {}

void foo( A & x) {}
void foo(const A & x) {}

void foo( X::B & x) {}
void foo(const X::B & x) {}

// const is lost in the 2nd function because X::B_typedef is a typedef
void foobar( X::B_typedef & x) {}
void foobar(const X::B_typedef & x) {}

// prototypes
void foobar2( std::string & s );
void foobar3( const std::string & s );
#endif

typedef float Float;

#if 0
void foo( float & x) {}
void foo(const Float & x) {}
#endif

float x0;
const float x1 = 0.0;

// const is lost when used with a typedef type (so it does not have anything to do with function parameters)
const Float x2 = 0.0;

