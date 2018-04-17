// Liao 10/20/2015

template <typename T>
T getMax (T t1, T t2)
{
  T tmax = t1>t2?t1:t2;
  return tmax;
}

#if 1
int main()
{
  int a=10, b=30;
//  float f1=9.99, f2=8.9889; 

  getMax <float> (a,b);
  // function<pattern> (): pattern is optional 
  // // compilers can usually deduce the right type
//  cout<<getMax(f1,f2)<<endl;

  return 0;
}
#endif
