
float A[256][256];
void foo ()
{
  int i, j; 
#pragma aitool fp_plus(1)
  for (i=0; i<256; i++)
       A[i][j] = A[i][j] + 0.5;
}
