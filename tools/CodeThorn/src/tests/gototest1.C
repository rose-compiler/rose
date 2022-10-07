int main() {
  int x;
  goto lab1;
  x=1;
  x=2;
 lab1:
  x=3;
  x=4;
 lab2:
  if(x==x) {
    x=5;
    goto lab2;
  } 
  x=6;
  return 0;
}
