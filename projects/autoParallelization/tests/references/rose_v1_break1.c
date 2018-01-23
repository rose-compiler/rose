//! loop with a break

void foo()
{
  int i;
  int a[100];
  for (i = 0; i <= 99; i += 1) {
    a[i] = a[i] + 1;
    if (a[i] == 100) 
      break; 
  }
}
