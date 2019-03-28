int main()
{
  const int vecsize = 10;
#pragma rose scalarization
  double array1[vecsize];
  double array2[vecsize];
  for(int i=0; i < vecsize; i++)
    array2[i] = array1[i];
  return 0;
}
