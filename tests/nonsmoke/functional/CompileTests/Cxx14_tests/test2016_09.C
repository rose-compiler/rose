
#include <memory>

using namespace std;

typedef int* some_type;

// template <typename T> int make_unique( int, int );

int do_something_with( some_type );

struct GO
   {
  // int run( some_type );
     int run( int );
   };

GO go;

void foobar()
   {
     some_type parameters;

#ifdef __INTEL_COMPILER
  // Intel v16 support for this C++14 feature appears to be incomplete in it's header files.
  // auto u = std::make_unique<some_type>( parameters );  // a unique_ptr is move-only
#else
     auto u = make_unique<some_type>( parameters );  // a unique_ptr is move-only
#endif
  // go.run( [ u=move(u) ] { do_something_with( u ); } ); // move the unique_ptr into the lambda
   }
