
int global;

void foo() {
  global++;  // global access
  int global;
  global++;  // local access
}

void main()
{
  foo();
}
