// handle variable reference to structure field
struct TTT{
  int array[100];
};

void foo()
{
struct TTT ttt;
#pragma rose_outline
  ttt.array[10]=100;
}

