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

void ERR01();
void ERR04();
void ERR06();
void ERR30();
void ERR31();
void ERR32();


void ERR() {
  ERR01();
  ERR04();
  ERR06();
  ERR30();
  ERR31();
  ERR32();
}


/* ERR01_A v.27 */

void ERR01() {
  errno = 0;
  printf("This\n");
  printf("is\n");
  printf("a\n");
  printf("test.\n");
  if (errno != 0) {
    fprintf(stderr, "printf failed: %s\n", strerror(errno));
  }
}


/* ERR04_A v.40 */

void ERR04() {
  char const *filename = "hello.txt";
  FILE *f = fopen(filename, "w");
  if (f == NULL) {
    /* handle error */
  }
  fprintf(f, "Hello, World\n");

  abort(); /* oops! data might not get written! */
}


/* ERR06_A v.14 */

void ERR06_cleanup(void) {
  /* delete temporary files, restore consistent state, etc. */
}

void ERR06() {
  if (atexit(ERR06_cleanup) != 0) {
    /* Handle Error */
  }

  assert(1);
}


/* ERR30_C v.38 */

void ERR30() {
  unsigned long number;
  char *string = NULL;
  char *endptr;

  /* ... */

  number = strtoul(string, &endptr, 0);
  if (endptr == string || (number == ULONG_MAX 
                           && errno == ERANGE)) 
    {
      /* handle the error */
    }
  else {
    /* computation succeeded */
  }
}


/* ERR31_C v.22 */

extern int errno;

void ERR31() {}


/* ERR32_C v.29 */

typedef void (*ERR32_pfv)(int);

void ERR32_handler(int signum) {
  const ERR32_pfv old_handler = signal(signum, SIG_DFL);
  if (old_handler == SIG_ERR) {
    perror("SIGINT handler"); /* undefined behavior */
    /* handle error condition */
  }
}

void ERR32() {
  const ERR32_pfv old_handler = signal(SIGINT, ERR32_handler);
  if (old_handler == SIG_ERR) {
    perror("SIGINT handler");
    /* handle error condition */
  }

  /* main code loop */

  return;
}

