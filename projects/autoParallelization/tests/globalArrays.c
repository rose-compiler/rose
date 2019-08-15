#define MSIZE 256

double u[MSIZE][MSIZE], f[MSIZE][MSIZE]; 
int n, m; 

void initialize ()
{
  int i, j, xx; 

  n = MSIZE;
  m = MSIZE;
  double dx = 2.0 / (n - 1);

  for (i = 0; i < n; i++)
    for (j = 0; j < m; j++)
      {   
        xx = (int) (-1.0 + dx * (i - 1));
        u[i][j] = 0.0;
        f[i][j] = -1.0 * (1.0 - xx * xx);  
      }   
}
