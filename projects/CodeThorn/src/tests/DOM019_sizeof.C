
int f(int a) {
  return a+1;
}

int main() {
  int x;
  x=sizeof(int);
  x=x+sizeof(x);
  x=f(x);
  return 0;
}
  
