/*
This file test slicing on global variables.
*/
int printf(const char *, ...);

int x;
int y;

int main(){
  int z;
  y=5;
  x=5;
  z=3;
  x--;
  y++;
  z--;
  z=x+y;
  return 0;
}
