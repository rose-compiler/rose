/*
  Test vectorization for simple arithemetic statements.
*/
int main(){
  float a[16];
  float b[16];
  float c[16];
  int n = 16;
  for (int i=0;i<n;i++)
  {
    c[i] = a[i] + b[i];
    c[i] = a[i] - b[i];
    c[i] = a[i] * b[i];
    c[i] = a[i] / b[i];
  }
}
