//A test case for possible duplicated declarations for private variables as loop index
// Coined based on the bug report from Allan Porterfield
void foo(int a[10][10])
{
  int i;
  int j;

#pragma omp parallel private (i,j)
#pragma omp for
for (j =0; j< 10; j++)
  for (i =0; i< 10; i++)
    a[i][j] = 0;
}
