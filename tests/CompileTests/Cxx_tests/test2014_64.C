template< void FUNC() >
void bar() {
  FUNC();
}

template< void (*FUNC)() >
void bar2() {
  FUNC();
}

void my_func()
{
}

int main(int argc, char* argv[])
{
  bar<my_func>();
  bar2<my_func>();
  return 0;
}

