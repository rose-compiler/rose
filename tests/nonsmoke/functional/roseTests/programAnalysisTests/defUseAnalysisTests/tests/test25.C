
int foo() {
  int x = 0;
  if(x) { 
    x=x+1;
    goto a;
  }
  goto b;
 d:
 a:
 b:  x += 1;
  int y=x;
  if(false)
    goto d;
  return 0;
}
