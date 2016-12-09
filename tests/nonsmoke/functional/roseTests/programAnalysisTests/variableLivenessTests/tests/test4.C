int globalvar = 5;

int foo(int argc, char **argv) {
  //  globalvar++;
  argc++;
  globalvar = 5;
  return 0;
}

