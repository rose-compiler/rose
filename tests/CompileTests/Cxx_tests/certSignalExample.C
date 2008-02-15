#if 0
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#else
// Alternative program to reduce the size and complexity of the whole program graph
typedef void (*functionType)(int x);
void signal(int s, functionType f);
typedef long sig_atomic_t;
void strcpy(char* s1, char* s2);
const int SIGINT = 42;
const void* NULL = 0L;
void* malloc(int size);
#endif

char *err_msg; 
volatile sig_atomic_t e_flag = 0;
 
void handler(int signum) { 
  signal(signum, handler);
  e_flag = 1;
} 
 
int main(void) { 
  signal(SIGINT, handler); 

  err_msg = (char*) malloc(24);
  if (err_msg == NULL) {
    /* handle error condition */
  }

  strcpy(err_msg, "No errors yet.");
 
  /* main code loop */
  if (e_flag) {
    strcpy(err_msg, "SIGINT received.");
  }

  return 0;
}
