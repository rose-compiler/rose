int* p;

int f() {
  return 1;
}

int* g() {
  return p;
}


int main() {
  int x;
  x=1;
  int y=1;
  x=x+y+2;
  int a[3];
  a[0]=x+1;
  a[y]=x;
  a[y+1]=x;
  x+=(y+f());
  p=a;
  (*g())++;
  return 0;
}
