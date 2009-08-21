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

void POS30();
void POS33();
void POS34();
void POS35();
void POS36();
void POS37();

void POS() {
  POS30();
  POS33();
  POS34();
  POS35();
  POS36();
  POS37();
}


/* POS30_C v.33 */

void POS30() {
  char buf[1024];
  const ssize_t len = readlink("/usr/bin/perl", buf, sizeof(buf));
  buf[len] = '\0';
}


/* POS33_C v.41 */

void POS33() {
  const char *filename = "foo";

  const pid_t pid = vfork();
  if ( pid == 0 )  /* child */ {
    if (execve(filename, NULL, NULL) == -1) {
      /* Handle Error */
    }
    _exit(1);  /* in case execve() fails */
  }
}


/* POS34_C v.76 */

int POS34_func(char const *var) {
  char env[1024];

  if (snprintf(env, sizeof(env),"TEST=%s", var) < 0) {
    /* Handle Error */
  }
  
  return putenv(env);
}

void POS34() {
  int a = POS34_func("hello");
  ++a;
}


/* POS35_C v.33 */

void POS35() {
  const char *filename = "foo";
  char userbuf[]  = "foo";
  const unsigned int userlen = sizeof(userbuf);

  struct stat lstat_info;
  int fd;

  if (lstat(filename, &lstat_info) == -1) {
    /* handle error */
  }
  if (!S_ISLNK(lstat_info.st_mode)) {
    if ((fd = open(filename, O_RDWR)) == -1) {
      /* handle error */
    }
  }
  if (write(fd, userbuf, userlen) < userlen) {
    /* Handle Error */
  }

  close(fd);
}


/* POS36_C v.19 */

void POS36() {
  /* Drop superuser privileges in incorrect order */

  if (setuid(getuid()) == -1) {
    /* handle error condition */
  }
  if (setgid(getgid()) == -1) {
    /* handle error condition */
  }

  /* It is still possible to regain group privileges due to 
   * incorrect relinquishment order */
}


/* POS37_C v.29 */

void POS37() {
  /*  Code intended to run with elevated privileges   */

  /* Temporarily drop privileges */
  if (seteuid(getuid()) != 0) {
    /* Handle error */
  }

  /*  Code intended to run with lower privileges  */

  if (1) { /* need_more_privilege */
    /* Restore privileges */
    if (seteuid(0) != 0) {
      /* Handle Error */
    }

    /*  Code intended to run with elevated privileges   */
  }

  /* ... */

  /* Permanently drop privileges */
  if (setuid(getuid()) != 0) {
    /* Handle Error */
  }

  /*  Code intended to run with lower privileges,
      but if privilege relinquishment failed, 
      attacker could regain elevated priveleges! */
}
