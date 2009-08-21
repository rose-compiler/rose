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

void ENV00();
void ENV01();
void ENV02();
void ENV03();
void ENV04();
void ENV30();
void ENV31(char const *envp[]);
void ENV32();
void ENV33();

void ENV(char const *envp[]) {
  ENV00();
  ENV01();
  ENV02();
  ENV03();
  ENV04();
  ENV30();
  ENV31(envp);
  ENV32();
  ENV33();
}


/* ENV00 v.36 */

void ENV00() {
  const char *tmpvar;
  const char *tempvar;

  tmpvar = getenv("TMP");
  if (!tmpvar) return;
  tempvar = getenv("TEMP");
  if (!tempvar) return;

  if (strcmp(tmpvar, tempvar) == 0) {
    if (puts("TMP and TEMP are the same.\n") == EOF) {}
  }
  else {
    if (puts("TMP and TEMP are NOT the same.\n") == EOF) {}
  }
}


/* ENV01 v.29 */

void ENV01() {
  char copy[16];
  const char *temp = getenv("TEST_ENV");
  if (temp != NULL) {
    strcpy(copy, temp);
  }
}


/* ENV02_A v.43 */

void ENV02() {
  if (putenv("TEST_ENV=foo") != 0) {
    /* Handle Error */
  }
  if (putenv("Test_ENV=bar") != 0) {
    /* Handle Error */
  }

  const char *temp = getenv("TEST_ENV");

  if (temp == NULL) {
    /* Handle Error */
  }
}


/* ENV03_A v.62 */

void ENV03() {
  if (system("/bin/ls dir.`date +%Y%m%d`") != 0) {}
}


/* ENV04_A v.65 */

void ENV04() {
  char *input = NULL;

  /* input gets initialized by user */

  char cmdbuf[512];
  const int len_wanted = snprintf(
			    cmdbuf, 
			    sizeof(cmdbuf), 
			    "any_cmd '%s'", 
			    input
			    );
  if (len_wanted >= sizeof(cmdbuf)) {
    perror("Input too long");
  }
  else if (len_wanted < 0) {
    perror("Encoding error");
  }
  else if (system(cmdbuf) == -1) {
    perror("Error executing input");
  }
}


/* ENV30_C v.41 */

void ENV30() {
  char *env = getenv("TEST_ENV");
  env[0] = 'a';
}


/* ENV31_C v.43 */

void ENV31(char const *envp[]) {
   size_t i;
   if (setenv("MY_NEW_VAR", "new_value", 1) == -1) {}
   if (envp != NULL) {
      for (i = 0; envp[i] != NULL; i++) {
         if (puts(envp[i]) == EOF) {}
      }
   }
}


/* ENV32_C v.35 */

void ENV32_exit1(void) {
  /* ...cleanup code... */
  return;
}

void ENV32_exit2(void) {
  if (1) {
    /* ...more cleanup code... */
    exit(0);
  }
  return;
}

void ENV32() {
  if (atexit(ENV32_exit1) != 0) {
    /* Handle Error */
  }
  if (atexit(ENV32_exit2) != 0) {
    /* Handle Error */
  }
  /* ...program code... */
  exit(0);
}


/* ENV33_C v.25 */

jmp_buf ENV33_env;
int ENV33_val;

void ENV33_exit1(void) {
  /* ... */
  longjmp(ENV33_env, 1);
}

void ENV33(void) {
  if (atexit(ENV33_exit1) != 0) {
    /* handle error */
  }
  /* ... */
  if (setjmp(ENV33_env) == 0) {
    exit(0);
  }
  else {
    return;
  }
}
