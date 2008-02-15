// t0247.cc
// from oink/test/template_func2.cc
// and template_func3_partial.cc

// function template
template<int I, class T>
T f(T x)
{
  return sizeof(T) + I;
}

// specialization
template<>
int f<2, int>(int x)
{
  return x;
}

// partial specialization: not allowed!
//ERROR(1): template<class S>
//ERROR(1): S f<2, S>(S x)
//ERROR(1): {
//ERROR(1):   return x;
//ERROR(1): }

int main()
{
  int y;
  int z = f<1, int>(y);     // use primary
  
  f<2, int>(y);             // use specialziation
}
