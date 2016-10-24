//Test to make sure we don't get into instantiated template functions
template<typename T> // better choice
//template <class T>
T GetMax (T a, T b) {
  T result;
  result = (a>b)? a : b;
  return (result);
}

int main()
{
  int i=10, j=20,k;
  k = GetMax(i,j);

  return 0;
}
