int main() {
  int x;
  goto lab1;
 lab3:
  if(x>0) {
    x=x-1;
    goto lab2;
  } else {
    x=x+1;
    goto lab3;
  }
 lab1:
  goto lab3;
 lab2:
  return 0;
}
