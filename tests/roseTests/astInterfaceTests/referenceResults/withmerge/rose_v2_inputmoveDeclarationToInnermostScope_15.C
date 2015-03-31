//Test to make sure we don't get into instantiated template functions
// better choice
//template <class T>
template < typename T >
T GetMax ( T a, T b ) {
  T result;
  result = ( a > b ) ? a : b;
  return ( result );
}

int main()
{
  int i = 10;
  int j = 20;
  int k;
  k = ::GetMax(i,j);
  return 0;
}
