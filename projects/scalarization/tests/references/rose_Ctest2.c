
int main()
{
  const int vecsize = 10;
  
#pragma rose scalarization
  double array1[10];
  double array2[10];
  for (int i = 0; i < vecsize; i++) 
    array2[i] = array1[i];
  return 0;
}
