
#include "assert.h"

void set_flag(int number, int *sign_flag) {
  //   assert(sign_flag);

  if (number > 0) {
    *sign_flag = 1;
  }
  else if (number <0) {
    *sign_flag = -1;
  }
  int x = *sign_flag;
}

int main(int argc, char** argv) {
  int sign;
  set_flag(0,&sign);
  return 0;
}

