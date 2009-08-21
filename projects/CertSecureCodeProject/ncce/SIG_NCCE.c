/*
 * Copyright (c) 2007 Carnegie Mellon University.
 * All rights reserved.

 * Permission to use this software and its documentation for any purpose is hereby granted,
 * provided that the above copyright notice appear and that both that copyright notice and
 * this permission notice appear in supporting documentation, and that the name of CMU not
 * be used in advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.
 *
 * CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, RISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "util.h"

void SIG00();
void SIG01();
void SIG30();
void SIG31();
void SIG32();
void SIG33();
void SIG34();

void SIG() {
  SIG00();
  SIG01();
  SIG30();
  SIG31();
  SIG32();
  SIG33();
  SIG34();
}


/* SIG00_A v.51 */

volatile sig_atomic_t SIG00_sig1 = 0;
volatile sig_atomic_t SIG00_sig2 = 0;

void SIG00_handler(int signum) {
  if (signum == SIGUSR1) {
    SIG00_sig1 = 1;
  }
  else if (SIG00_sig1) {
    SIG00_sig2 = 1;
  }
}

void SIG00() {
  if (signal(SIGUSR1, SIG00_handler) == SIG_ERR) {
    /* handle error */
  }
  if (signal(SIGUSR2, SIG00_handler) == SIG_ERR) {
    /* handler error */
  }

  while (SIG00_sig2 == 0) {
    /* do nothing or give up CPU for a while */
  }

  /* ... */

  return;
}


/* SIG01_A v.51 */

void SIG01_handler(int signum) {
 if (signal(signum, SIG01_handler) == SIG_ERR) {

}
  /* handle signal */
}

void SIG01() {
  if (signal(SIGUSR1, SIG01_handler) == SIG_ERR) {}
}


/* SIG30_C v.74 */

enum { SIG30_MAXLINE = 1024 };
char *SIG30_info = NULL;

void SIG30_log_message(void) {
  /* something */
}

void SIG30_handler(int signum) {
  SIG30_log_message();
  free(SIG30_info);
  SIG30_info = NULL;
}

void SIG30(void) {
  if (signal(SIGINT, SIG30_handler) == SIG_ERR) {}
  SIG30_info = (char*)malloc(SIG30_MAXLINE);
  if (SIG30_info == NULL) {
    /* Handle Error */
  }

  while (1) {
    /* main loop program code */

    SIG30_log_message();

    /* more program code */
  }
}


/* SIG31_C v.46 */

char *SIG31_err_msg;
enum { SIG31_MAX_MSG_SIZE = 24 };

void SIG31_handler(int signum) {
  strcpy(SIG31_err_msg, "SIGINT encountered.");
}

void SIG31() {
  if (signal(SIGINT, SIG31_handler) == SIG_ERR) {}

  SIG31_err_msg = (char *)malloc(SIG31_MAX_MSG_SIZE);
  if (SIG31_err_msg == NULL) {
    /* handle error condition */
  }
  strcpy(SIG31_err_msg, "No errors yet.");

  /* main code loop */

  return;
}


/* SIG32_C v.87 */

enum { SIG32_MAXLINE = 1024 };
static jmp_buf SIG32_env;

void SIG32_handler(int signum) {
  longjmp(SIG32_env, 1);
}

void SIG32_log_message(const char *info1, const char *info2) {
  static char *buf = NULL;
  static size_t bufsize;
  char buf0[SIG32_MAXLINE];

  if (buf == NULL) {
    buf = buf0;
    bufsize = sizeof(buf0);
  }

  /*
   *  Try to fit a message into buf, else re-allocate
   *  it on the heap and then log the message.
   */

/*** VULNERABILITY IF SIGINT RAISED HERE ***/

  if (buf == buf0) {
    buf = NULL;
  }
	printf("SIG32 %d\n", bufsize);
}

void SIG32() {
  if (signal(SIGINT, SIG32_handler) == SIG_ERR) {}
  char *info1;
  char *info2;

  /* info1 and info2 are set by user input here */

  if (setjmp(SIG32_env) == 0) {
    while (1) {
      /* main loop program code */
      SIG32_log_message(info1, info2);
      /* more program code */
    }
  }
  else {
    SIG32_log_message(info1, info2);
  }

  return;
}


/* SIG33_C v.36 */

void SIG33_log_msg(int signum) {
  /* log error message in some asynchronous-safe manner */
}

void SIG33_handler(int signum) {
  /* do some handling specific to SIGINT */
  if (raise(SIGUSR1)!=0) {}
}

void SIG33() {
  if (signal(SIGUSR1, SIG33_log_msg) != SIG_ERR) {}
  if (signal(SIGINT, SIG33_handler) != SIG_ERR) {}

  /* program code */
  if (raise(SIGINT) != 0) {}
  /* more code */

  return;
}


/* SIG34_C v.19 */

void SIG34_handler(int signum) {
  if (signal(signum, SIG34_handler) == SIG_ERR) {}
  /* handle signal */
}

void SIG34() {
  if (signal(SIGUSR1, SIG34_handler) == SIG_ERR) {}
}


