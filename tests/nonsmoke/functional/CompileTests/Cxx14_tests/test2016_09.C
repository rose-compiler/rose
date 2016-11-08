
auto u = make_unique<some_type>( some, parameters );  // a unique_ptr is move-only
go.run( [ u=move(u) ] { do_something_with( u ); } ); // move the unique_ptr into the lambda
