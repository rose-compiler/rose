// cc.in11
// experimenting with one definition rule


extern int x;
int x;

int y;
extern int y;

extern int x;
//ERROR(1): int x;

int foo();
int foo();

int foo()
{
  return 4;
}


int foo();


//ERROR(2): int foo() { return 4; }


int foo2(int q);

// definition uses different param name than prototype
int foo2(int z)
{
  return z;
}
