int main() {
  int x=1;
  switch(x) { case 2: return 0; default:; }
  switch(x) { case 1: x=2; break; default:;}
  return 1;
}
