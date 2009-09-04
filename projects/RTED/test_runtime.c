#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "RuntimeSystem.h"

/* -----------------------------------------------------------
 * Main Function for RTED Runtime test
 * -----------------------------------------------------------*/
int main(int argc, char** argv) {
  printf("Testing runtime system\n");
  RuntimeSystem_roseRtedClose("main");
  printf("Done\n");
  return 0;
}
