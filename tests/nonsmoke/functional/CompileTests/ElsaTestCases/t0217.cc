// t0217.cc
// two gotos to the same target (!)

void f()
{
  int x;
  
lab:       
  if (x == 3) {
    goto lab;
  }
  
  if (x == 4) {
    goto lab;
  }
}
