/* A kernel for two level parallelizable loop with reduction */
float u[100][100];
float foo ()
{
  int i,j;
  float temp, error;
  for (i = 0; i < 100; i++)
    for (j = 0; j < 100; j++)
      {
        temp = u[i][j];
        error = error + temp * temp;
      }
  return error;
}
