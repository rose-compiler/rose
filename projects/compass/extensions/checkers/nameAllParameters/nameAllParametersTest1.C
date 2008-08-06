void f(int) // bad
{
}

void g(int i) // good
{
  static_cast<void>(i);
}
