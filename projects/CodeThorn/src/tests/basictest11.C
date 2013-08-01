int main() {
  int x;
  int y;
  x=1;
  y=0;
  if(true&&false)
    y=1;
  if(true||false)
    y=2;
  if(!false)
    y=3;
  return 0;
}
