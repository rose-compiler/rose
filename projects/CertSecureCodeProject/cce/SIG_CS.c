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

void handler(int signum) {
  if (signum == SIGUSR1) {
    SIG00_sig1 = 1;
  }
  else if (SIG00_sig1) {
    SIG00_sig2 = 1;
  }
}

void SIG00(void) {
  struct sigaction act;
  act.sa_handler = &handler;
  act.sa_flags = 0;
  if (sigemptyset(&act.sa_mask) != 0) {
    /* handle error */
  }
  if (sigaddset(&act.sa_mask, SIGUSR1)) {
    /* handle error */
  }
  if (sigaddset(&act.sa_mask, SIGUSR2)) {
    /* handle error */
  }

  if (sigaction(SIGUSR1, &act, NULL) != 0) {
    /* handle error */
  }
  if (sigaction(SIGUSR2, &act, NULL) != 0) {
    /* handle error */
  }

  while (SIG00_sig2 == 0) {
    /* do nothing or give up CPU for a while */
  }

  /* ... */

  return;
}


/* SIG01_A v.51 */

void SIG01_handler(int signum) {
  /* handle signal */
}

void SIG01() {
  struct sigaction act;
  act.sa_handler = SIG01_handler;
  act.sa_flags = 0;
  if (sigemptyset(&act.sa_mask) != 0) {
    /* handle error */
  }
  if (sigaction(SIGUSR1, &act, NULL) != 0) {
    /* handle error */
  }
}


/* SIG30_C v.74 */

enum { SIG30_MAXLINE = 1024 };
volatile sig_atomic_t SIG30_eflag = 0;
char *SIG30_info = NULL;

void SIG30_log_message(void) {
  /* something */
}

void SIG30_handler(int signum) {
  SIG30_eflag = 1;
}

void SIG30() {
  if (signal(SIGINT, SIG30_handler) == SIG_ERR) {
    /* Handle Error */
  }
  SIG30_info = (char*)malloc(SIG30_MAXLINE);
  if (SIG30_info == NULL) {
    /* Handle Error */
  }


  while (!SIG30_eflag) {
    /* main loop program code */

    SIG30_log_message();

    /* more program code */
  }

  SIG30_log_message();
  free(SIG30_info);
  SIG30_info = NULL;

  return;
}


/* SIG31_C v.46 */

volatile sig_atomic_t SIG31_e_flag = 0;
volatile sig_atomic_t SIG31_e_value = 1;

void SIG31_handler(int signum) {
  SIG31_e_flag = SIG31_e_value;
}

void SIG31() {
  char *SIG31_err_msg;
  enum { SIG31_MAX_MSG_SIZE = 24 };
  if (signal(SIGINT, SIG31_handler) == SIG_ERR) {
    /* Handle Error */
  }

  SIG31_err_msg = (char *)malloc(SIG31_MAX_MSG_SIZE);
  if (SIG31_err_msg == NULL) {
    /* handle error condition */
  }

  strcpy(SIG31_err_msg, "No errors yet.");

  /* main code loop */

  if (SIG31_e_flag) {
    strcpy(SIG31_err_msg, "SIGINT received.");
  }

  return;
}


/* SIG32_C v.87 */

enum { SIG32_MAXLINE = 1024 };
volatile sig_atomic_t SIG32_eflag = 0;

void SIG32_handler(int signum) {
  SIG32_eflag = 1;
}

void SIG32_log_message(const char *info1, const char *info2) {
  static char *buf = NULL;
  static size_t bufsize = 0;
  char buf0[SIG32_MAXLINE];

  if (buf == NULL) {
    buf = buf0;
    bufsize = sizeof(buf0);
  }

  /*
   *  Try to fit a message into buf, else re-allocate
   *  it on the heap and then log the message.
   */
  if (buf == buf0) {
    buf = NULL;
  }
  printf("SIG32 %d\n", bufsize);
}

void SIG32() {
  if (signal(SIGINT, SIG32_handler) == SIG_ERR) {
    /* Handle Error */
  }
  char *info1;
  char *info2;

  /* info1 and info2 are set by user input here */

  while (!SIG32_eflag) {
    /* main loop program code */
    SIG32_log_message(info1, info2);
    /* more program code */
  }

  SIG32_log_message(info1, info2);

  return;
}


/* SIG33_C v.36 */

void SIG33_log_msg(int signum) {
  /* log error message in some asynchronous-safe manner */
}

void SIG33_handler(int signum) {
  /* do some handling specific to SIGINT */
  SIG33_log_msg(SIGUSR1);
}

void SIG33() {
  if (signal(SIGUSR1, SIG33_log_msg) == SIG_ERR) {
    /* Handle Error */
  }
  if (signal(SIGINT, SIG33_handler) == SIG_ERR) {
    /* Handle Error */
  }

  /* program code */
  if (raise(SIGINT) != 0) {
    /* Handle Error */
  }
  /* more code */

  return;
}


/* SIG34_C v.19 */

void SIG34_handler(int signum) {
  /* handle signal */
}

void SIG34() {
  struct sigaction act;
  act.sa_handler = SIG34_handler;
  act.sa_flags = 0;
  if (sigemptyset( &act.sa_mask) != 0) {
    /* handle error */
  }
  if (sigaction(SIGUSR1, &act, NULL) != 0) {
    /* handle error */
  }
}
