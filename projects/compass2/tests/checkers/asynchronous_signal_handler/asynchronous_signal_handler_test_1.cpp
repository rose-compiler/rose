// 2 Matches
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <iostream>

// Included to get sleep() function
#include <unistd.h>

char *err_msg;
volatile sig_atomic_t e_flag = 0;

void nonAsyncSafeFunction()
{
  return;
}

void wait1()
{
  return;
}

void handler(int signum) {
  signal(signum, handler);
  nonAsyncSafeFunction(); // not async safe
  wait(); // async safe
  sleep(1); // async safe
  wait1(); // not async safe

  e_flag = 1;
}

int main(void) {
  return 0;
}

