// This demonstrated renaming of capture list variables.

#include <memory>

using namespace std;

typedef int* some_type;

// template <typename T> int make_unique( int, int );

// int do_something_with( some_type );
// int do_something_with( const unique_ptr<some_type> xxx );
int do_something_with( );

struct GO
   {
  // int run( some_type );
  // int run( int );
  // some_type run( some_type );
     GO();
  // void run ( const unique_ptr<some_type> xxx );
     template <class T> void run ( T xxx ) {};
   };

GO go;

void foobar()
   {
     some_type parameters;

     auto u = make_unique<some_type>( parameters );  // a unique_ptr is move-only

  // go.run( [ u=move(u) ] { do_something_with( u ); } ); // move the unique_ptr into the lambda
  // go.run( [ u=move(u) ] { do_something_with( ); } ); // move the unique_ptr into the lambda
     go.run( [ u=move(u) ] { } ); // move the unique_ptr into the lambda

  // go.run( [ u2=move(u) ] { do_something_with( u2 ); } ); // capture as "u2"
     go.run( [ u2=move(u) ] { } ); // capture as "u2"
   }
