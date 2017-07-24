//--------------- input code
//cat /home/liao6/workspace/raja/templateInstantiation/template1.cpp

template<typename T> 
T GetMax (T a, T b) {
  T result;
  result = (a>b)? a : b;
  return (result);
}

void foo(int i, int j)
{
 int  k=GetMax<int>(i,j);
}

#if 0
//--------------- output code
//cat rose_template1.cpp 
template < typename T >
T GetMax ( T a, T b ) {
  T result;
  result = ( a > b ) ? a : b;
  return ( result );
}

void foo(int i,int j)
{
  int k = ::GetMax< int  > (i,j);
}

template<> int GetMax< int  > (int a,int b)
{
  int result;
  result = (a > b?a : b);
  return result;
}
#endif

