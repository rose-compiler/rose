int main(){
  int a[16];
  int b[16];
  int c[16];
  for (int i=0;i<16;i++)
  {
    a[i] = a[i] * b[i] + c[i];
    a[i] = a[i] * b[i] - c[i];
    a[i] = c[i] + a[i] * b[i];
    a[i] = c[i] - a[i] * b[i];
  }
}
