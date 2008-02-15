
// Function prototype
void dgemm(double *a, double *b, double *c, int n);

// Function definition
void dgemm(double *a, double *b, double *c, int n)
   {
     int i, j, k;

  // int n;

     for(k=0;k<n;k+=1){
       for(j=0;j<n;j+=1){
         for(i=0;i<n;i+=1){
           c[j*n+i]=c[j*n+i]+a[k*n+i]*b[j*n+k];
         }
       }
     }
   }

