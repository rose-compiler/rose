
float A[256][256], B[256][256];
#pragma aitool fp_plus(2)
void foo (float a, float b)
{
  int i, j; 
  float f = a+ b; 
#pragma aitool fp_plus(1)
  for (i=0; i<256; i++)
     for (j=0; j<256; j++)
       A[i][j] = B[i][j] + B[i][j+1];
}
