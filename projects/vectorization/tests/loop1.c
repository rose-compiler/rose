int main(){
  int a[16];
  int b[16];
  b[15] = 0;
  for (int i=0;i<15;i++)
  {
    b[i] = i;
    a[i] = b[i+1];
  }
}
