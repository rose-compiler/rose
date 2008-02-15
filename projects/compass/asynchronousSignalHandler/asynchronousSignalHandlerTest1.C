
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

// Included to get sleep() function
#include <unistd.h>

char *err_msg; 
volatile sig_atomic_t e_flag = 0;

void nonAsyncSafeFunction();

void handler(int signum) { 
  signal(signum, handler);

// This function is NOT on the async-safe list and IS reported (line 19)
  nonAsyncSafeFunction();

// This function is on the async-safe list and is NOT reported (line 22)
  sleep(1);

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
