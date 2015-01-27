void foo(int a[4]);

namespace
{
  const unsigned array_size = 3;
}

void foo(int a[array_size] ) // decays to a pointer and becomes int*
{
  a[0]=a[1]+1;
} 

int main() {
  int a[2];
  a[1]=1;
  foo(a);
  return 0;
}
