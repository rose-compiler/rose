// A test case of a non-canonical loop.
// Naively putting openmp parallel for for it is wrong.
int a[100], b[100];

int main()
{
  int i,j;
  int i_ub =100;
  for (i = 0, j=0 ; i < i_ub ; i++, j++ )
  {
    b[j]=a[i];
  }
  return 0;
}
