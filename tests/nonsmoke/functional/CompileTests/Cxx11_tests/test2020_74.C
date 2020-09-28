// RC-75

template <typename T>
struct array {};

template <typename K, typename V>
struct map {
  using mapped_type = V;
};

template< typename T >
void foo( T && );

void foo() 
   {
     map<void*, array<int> > bar;

  // Original code: foo(decltype(bar)::mapped_type{});
  // Unparsed code: ::foo(array< int  > {});
     foo(decltype(bar)::mapped_type{});
   }

