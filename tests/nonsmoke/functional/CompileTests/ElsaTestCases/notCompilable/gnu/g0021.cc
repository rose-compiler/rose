// g0021.cc
// coverage: arrays

extern int sizeLess[];

void f(int s)
{
  int dynSize[s];       // legal in GNU mode
  int fixedSize[5];

  sizeof(fixedSize);
  sizeof(dynSize);      // also legal in GNU mode
  //ERROR(1): sizeof(sizeLess);
}


