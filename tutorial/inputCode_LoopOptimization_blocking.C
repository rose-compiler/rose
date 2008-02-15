// Example program showing matrix multiply 
// (for use with loop optimization tutorial example)

#define N 50

int main()
   {
     int i,j, k;
     double a[N][N], b[N][N], c[N][N];

     for (i = 0; i <= N-1; i+=1)
        {
          for (j = 0; j <= N-1; j+=1)
             {
               for (k = 0; k <= N-1; k+=1)
                  {
                    c[i][j] = c[i][j] + a[i][k] * b[k][j];
                  }
             }
        }

     return 0;
   }
