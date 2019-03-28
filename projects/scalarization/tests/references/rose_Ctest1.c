
int main()
{
  const int vecsize = 10;
  
#pragma rose scalarization
  double array1[10];
  for (int i = 0; i < vecsize; i++) 
    array1[i] = 1.;
  return 0;
}
