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
  enum { BUFFERSIZE = 1024 };
  char buf[BUFFERSIZE];
  ssize_t len;

  if ((len = readlink("/usr/bin/perl", buf, sizeof(buf)-1)) 
      != -1)
    {
      buf[len] = '\0';
    }
  else {
    /* handle error condition */
  }
}


/* POS33_C v.41 */

void POS33() {
  const char *filename = "foo";

  const pid_t pid = fork();
  if ( pid == 0 )  /* child */ {
    if (execve(filename, NULL, NULL) == -1) {
      /* Handle Error */
    }
    _exit(1);  /* in case execve() fails */
  }
}


/* POS34_C v.76 */

int POS34_func(char const *var) {
  static char *oldenv;
  char const *env_format = "TEST=%s";
  size_t const len = strlen(var) + strlen(env_format);
  char *env = (char *) malloc(len);
  if (env == NULL) {
    return -1;
  }
  const int rc = snprintf(env, len, env_format, var);
  if (rc < 0 || (size_t)rc >= len) {
    /* Handle Error */
  }
  if (putenv(env) != 0) {
    free(env);
    return -1;
  }
  if (oldenv != NULL)
    free(oldenv); // avoid memory leak
  oldenv = env;
  return 0;
}

void POS34() {
  if (POS34_func("hello") != 0) {
    /* Handle Error */
  }
}


/* POS35_C v.33 */

void POS35() {
  const char *filename = "foo";
  char userbuf[] = "foobar";
  const unsigned int userlen = sizeof(userbuf);

  struct stat lstat_info;
  struct stat fstat_info;
  int fd;
  /* ... */
  if (lstat(filename, &lstat_info) == -1) {
    /* handle error */
  }
  if ((fd = open(filename, O_RDWR)) == -1) {
    /* handle error */
  }
  if (fstat(fd, &fstat_info) == -1) {
    /* handle error */
  }
  if (lstat_info.st_mode == fstat_info.st_mode &&
      lstat_info.st_ino == fstat_info.st_ino  &&
      lstat_info.st_dev == fstat_info.st_dev) {
    if (write(fd, userbuf, userlen) < userlen) {
      /* Handle Error */
    }
  }
  /* ... */
  close(fd);
}


/* POS36_C v.19 */

void POS36() {
  /*  Drop superuser privileges in correct order */

  if (setgid(getgid()) == -1) {
    /* handle error condition */
  }
  if (setuid(getuid()) == -1) {
    /* handle error condition */
  }

  /*  Not possible to regain group privileges due to correct 
   * relinquishment order  */
}


/* POS37_C v.29 */

void POS37() {
  /*  Code intended to run with elevated privileges   */

  /* Temporarily drop privileges */
  if (seteuid(getuid()) != 0) {
    /* Handle error */
  }

  /*  Code intended to run with lower privileges  */

  if (1) {  /*need_more_privilege */
    /* Restore Privileges */
    if (seteuid(0) != 0) {
      /* Handle error */
    }

    /*  Code intended to run with elevated privileges   */
  }

  /* ... */

  /* Permanently drop privileges */
  if (setuid(getuid()) != 0) {
    /* Handle error */
  }

  if  (setuid(0) != -1) {
    /* Privileges can be restored, handle error */
  }

  /*  Code intended to run with lower privileges;
      attacker can not regain elevated privileges */

}
