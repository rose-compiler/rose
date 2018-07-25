int main()
{
#pragma array1
  const int vecsize = 10;
  double array1[vecsize],array2[vecsize];
  for(int i=0; i < vecsize; i++)
    array2[i] = array1[i];
  return 0;
}
