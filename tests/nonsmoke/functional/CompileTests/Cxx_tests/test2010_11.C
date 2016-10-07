#include <stdio.h>
#include <assert.h>

#pragma pack(1)
struct foo
{
  char z;
  int x;
};
#pragma pack()

int main(int argc, char**argv)
{
  assert(sizeof(struct foo) == sizeof(int) + sizeof(char));
  printf("sizeof(struct foo) = %d\n", sizeof(struct foo));
  return 0;
}
