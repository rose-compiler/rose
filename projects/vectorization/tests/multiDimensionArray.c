/*
  Test vectorization for multi-dimensional array
*/
int main(){
  float a[64][32][16];
  float b[32][16];
  float c[16];
  for (int k=0;k<64;k++)
  {
  for (int j=0;j<32;j++)
  {
  for (int i=0;i<16;i++)
  {
    a[k][j][i] = 5.f;
    b[j][i] = 2.f;
    c[i] = 4.f;
  }
  }
  }
}
