/*
  Test vectorization for FMA instructions.
  a = b * c + d ==> a = _SIMD_madd_ps(b,c,d);
*/
int main(){
  float a[16];
  float b[16];
  float c[16];
  int n = 16;
  float as,bs,cs;
  for (int i=0;i<n;i++)
  {
    a[i] = a[i] * b[i] + c[i];
    a[i] = a[i] * b[i] - c[i];
    a[i] = c[i] + a[i] * b[i];
    a[i] = c[i] - a[i] * b[i];
    cs = as * bs + 1;  
  }
}
