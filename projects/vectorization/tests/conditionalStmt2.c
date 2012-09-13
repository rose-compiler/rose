/*
  Test vectorization for simple arithemetic statements.
*/
int main(){
  float a[16];
  float tmp;
  int n = 16;
  for (int i=0;i<n;i++)
  {
    if(tmp == 0.f)
      a[i] = 0.f;
    else
      a[i] = 1.f;
  }
}
