void main()
{
  int i,j,k,beta;
  int C[10][10];
  int D[10][10];
  int tmp[10][10];
  beta = 1;
  for (i = 0; i < 10; i++)
    for (j = 0; j < 10; j++)
      {
	D[i][j] *= beta;
	for (k = 0; k < 10; ++k)
	  D[i][j] += tmp[i][k] * C[k][j];
      }
}
