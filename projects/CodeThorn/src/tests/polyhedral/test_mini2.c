int main()
{
  int i,j,beta;
  int C[3][3];
  int D[3][3];
  beta = 1;
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      D[i][j] *= (beta + 1+2+C[1+(i-1)+2][3+j-1+2]+3-4);
}
