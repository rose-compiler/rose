// constant index dimension
int a[10][20][30];
float b[2];

int foo (int i, int j, int k)
{
  // constant and variable indices
  return a[i][j][k] + a[1][2][3];
}
