int main()
{
  int i,j,beta;
  int C[3][3];
  int D[3][3];
  beta = 1;
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
	  D[i][j] *= (beta + C[i][j]);
}
