/*
  Test vectorization for simple arithmetic statements using integer operands.
*/
int main(){
  int a[16];
  int b[16];
  int c[16];
  int n = 16;
  for (int i=0;i<n;i++)
  {
    c[i] = a[i] + b[i];
    c[i] = a[i] - b[i];
    c[i] = a[i] * b[i];
  }
}
