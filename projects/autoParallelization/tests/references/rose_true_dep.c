
void foo()
{
  int i;
  int a[100];
  for (i = 0; i <= 98; i += 1) {
    a[i + 1] = a[i] + 1;
  }
}
