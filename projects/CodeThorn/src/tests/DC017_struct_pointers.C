#include <stdlib.h>
struct JIM_TYPE {
  int JIM_STATE;
};

extern struct JIM_TYPE JIM;
char Read_8bit(int offset) {
  return rand();
}

void FOO() {};

void main(void) {
  int J;
  char* P_JIM;
  P_JIM = (char*) &JIM;
  for(J=0; J < sizeof(JIM);J++){
    *P_JIM = Read_8bit(J);
    P_JIM=P_JIM+1;
  }

  if(JIM.JIM_STATE == 0) {
    FOO();
  }
}
