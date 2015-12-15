void foo1();
int foo2();

int foobar()
{
#if 1
  if (1) {
    int pid;
#if 0
#else
    pid = foo2();
#endif
    if (pid == - 1) {
    }
  }
#else
#endif
  return 0;
}
