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

void DCL00();
void DCL01();
void DCL03();
void DCL04();
void DCL05();
void DCL06();
void DCL08();
void DCL10();
void DCL11();
void DCL13();
void DCL30();
void DCL31();
void DCL32();
void DCL33();

void DCL() {
  DCL00();
  DCL01();
  DCL03();
  DCL04();
  DCL06();
  DCL08();
  DCL10();
  DCL11();
  DCL13();
  DCL30();
  DCL31();
  DCL32();
  DCL33();
}


/* DCL00_A v.69 */

void DCL00() {
  float pi = 3.14159f;
  const float degrees = 180.0;
  float radians;
  /* ... */
  radians = degrees * pi / 180;
  printf("DCL00 %f\n", radians);
}


/* DCL01_A v.51 */

char DCL01_msg[100];

void DCL01_report_error(char const *error_msg) {
  char DCL01_msg[80];

  /* Assume error_msg isn't too long */
  strncpy(DCL01_msg, error_msg, sizeof(DCL01_msg));
  return;
}

void DCL01(void) {
  char error_msg[] = "Error!";

  DCL01_report_error(error_msg); /* oops! */
}


/* DCL03_A v.35 */

struct DCL03_timer {
  uint8_t MODE;
  uint32_t DATA;
  uint32_t COUNT;
};

void DCL03(void) {
  assert(offsetof(struct DCL03_timer, DATA) == 4);
}

/* DCL04_A v.37 */

void DCL04() {
	char *src, c;
	printf("DCL04 %p %p\n", &src, &c);
}

/* DCL05_C */
void (*DCL05_signal(int, void (*)(int)))(int);

/* DCL06_A v.88 */
void DCL06() {
  const int age = random() % 100;

  if(age > 18) {
    /* something */
  }
  else {
    /* something */
  }
}


/* DCL08_A v.20 */

void DCL08() {
  enum { ADULT_AGE=18 };

  /* misleading, relationship established when none exists */
  enum { ALCOHOL_AGE=ADULT_AGE+3 };
}


/* DCL10_A v.43 */

void DCL10() {
  char const *error_msg = "Resource not available to user.";

  printf("Error (%s): %s", error_msg);
}


/* DCL11_A v.49 */

void DCL11() {
  const long long a = 1;
  char const msg[] = "Default message";

  printf("%d %s", a, msg);
}


/* DCL13_A v.22 */

void DCL13_foo(int *x) {
  if (x != NULL) {
    *x = 3; /* visible outside function */
  }
  /* ... */
}

void DCL13() {
  int *x = NULL;
  DCL13_foo(x);
}


/* DCL30_A v.63 */

char const *DCL30_p;

void DCL30_dont_do_this(void) {
  char const str[] = "This will change";
  DCL30_p = str; /* dangerous */
  /* ... */
}

void DCL30_innocuous(void) {
  char const str[] = "Surprise, surprise";
  printf("DCL30 %p\n", str);
}


void DCL30() {
  DCL30_dont_do_this();
  DCL30_innocuous();
  /* p might be pointing to "Surprise, surprise" */
}


/* DCL31_C v.20 */

void DCL31() {
  const unsigned foo = 0;
  printf("DCL31 %d\n", foo);
}


/* DCL32_C v.47 */

void DCL32() {
  extern int *global_symbol_definition_lookup_table_a;
  extern int *global_symbol_definition_lookup_table_b;
  printf("DCL32 %p %p\n", global_symbol_definition_lookup_table_a, global_symbol_definition_lookup_table_b);
}


/* DCL33_C v.42 */

void DCL33() {
  char str[]="test string";
  char *ptr1=str;
  char *ptr2;

  ptr2 = ptr1 + 3;
  memcpy(ptr2, ptr1, 6);
}
