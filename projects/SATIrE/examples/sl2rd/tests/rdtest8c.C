int inc(int v) {
  int r=v+1;
  return r;
}

int main() {
  int x,r1,r2;
  x=5;
  r1=inc(x); 
  {
    int x;
    x=1;
    {
      int y;
      y=1;
      return x+y;
    }
  }
}
