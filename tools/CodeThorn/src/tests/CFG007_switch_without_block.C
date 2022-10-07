int main() {
  int x=1;
  switch(x) { case 2: return 1; }
  switch(x) { case 1: x=2; break; default:;}
  switch(x) case 1: return 2; // 'return' is dead, x==2 here
  return 0;
}
