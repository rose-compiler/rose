#include <stddef.h>

struct A {
  int x;
  int y;
};

int main(int argc, char** argv) {
  offsetof(struct A,x);

}
