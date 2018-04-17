template < typename T >
struct A {
 T foo ( T _t );
};
template < typename T >
T A < T > :: foo ( T _t ) {
 return _t;
}
template<> struct A< int  > 
{
  int foo(int _t);
};

int A< int > ::foo(int _t)
{
  return _t;
}
