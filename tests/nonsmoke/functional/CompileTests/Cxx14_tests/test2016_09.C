
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

     auto u = make_unique<some_type>( parameters );  // a unique_ptr is move-only

  // go.run( [ u=move(u) ] { do_something_with( u ); } ); // move the unique_ptr into the lambda
   }
