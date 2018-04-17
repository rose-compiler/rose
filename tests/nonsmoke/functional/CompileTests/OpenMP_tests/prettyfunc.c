// Test the handling of pretty function
// There should be a hidden variable declaration inserted under the closest enclosing scope
// Liao 2013-1-10
int main()
{
//  int i=100,sum=0;
#pragma omp parallel 
  {
     __PRETTY_FUNCTION__;
  }
  return 0;
}

