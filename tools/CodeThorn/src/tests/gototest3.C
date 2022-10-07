int main() {
  int x;
  goto lab1;
 lab3:
  while(1) {
    if(x>0) {
      goto lab2;
    } else {
      goto lab3;
    }
  }
  lab1:
    goto lab3;
 lab2:
  return 0;
}
