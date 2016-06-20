
float A[256][256], B[256][256];
void foo ()
{
  int i, j; 
  for (i=0; i<256; i++)
     for (j=0; j<256; j++)
       A[i][j] = B[i][j] + B[i][j+1];
}
