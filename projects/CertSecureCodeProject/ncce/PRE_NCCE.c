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

void PRE00(void);
void PRE01(void);
void PRE02(void);
void PRE03(void);
void PRE04(void);
void PRE05(void);
void PRE06(void);
void PRE07(void);
void PRE08(void);
void PRE09(void);
void PRE10(void);
void PRE31(void);

void PRE(void) {
  PRE00();
  PRE01();
  PRE02();
  PRE03();
  PRE04();
  PRE05();
  PRE06();
  PRE07();
  PRE08();
  PRE09();
  PRE10();
  PRE31();
}

void PRE00_A();
void PRE00_B();
void PRE00_C(); 

void PRE00(void) {
  PRE00_A();
  PRE00_B();
  PRE00_C();
}

/* PRE00_A v.8 */

#define PRE00_A_CUBE(X) ((X) * (X) * (X))

void PRE00_A() {
  int i = 2;
  const int a = 81 / PRE00_A_CUBE(++i);
}

size_t PRE00_B_count = 0;

#define PRE00_B_EXEC_BUMP(func) (func(), ++PRE00_B_count)

void PRE00_B_B(void) {
  printf("Called PRE00_B_A, PRE00_B_count = %zu.\n", PRE00_B_count);
}

void PRE00_B_A(void) {
  PRE00_B_count = 0;
  while (PRE00_B_count++ < 10) {
    PRE00_B_EXEC_BUMP(PRE00_B_B);
  }
}

void PRE00_B() {
  PRE00_B_A();
}

#define PRE00_C_A(x) (++operations, ++calls_to_PRE00_C_A, 2*x)
#define PRE00_C_B(x) (++operations, ++calls_to_PRE00_C_B, x + 1)

void PRE00_C() {
  unsigned int operations = 0;
  unsigned int calls_to_PRE00_C_A = 0;
  unsigned int calls_to_PRE00_C_B = 0;
  const unsigned int x = 3;
  const unsigned int y = PRE00_C_A(x) + PRE00_C_B(x);  
}


/* PRE01_A v.66 */

#define PRE01_CUBE(I) (I * I * I)

void PRE01() {
  const int a = 81 / PRE01_CUBE(2 + 1);
	printf("PRE01 %d\n", a);
}


/* PRE02_A v.59 */

#define PRE02_CUBE(X) (X) * (X) * (X)

void PRE02_A() {
  const int i = 3;
  const int a = 81 / PRE02_CUBE(i);
	printf("PRE02 %d\n", a);
}

#define PRE02_EOF -1

void PRE02_B() {
  if (getchar() PRE02_EOF) {
   /* ... */
  }
}

void PRE02() {
  PRE02_A();
  PRE02_B();
}


/* PRE03_A v.49 */

#define PRE03_cstring char *

void PRE03() {
  PRE03_cstring s1, s2;
	printf("PRE03 %p %p\n", &s1, &s2);
}

/* PRE04_A v.36 */

#include "stdio.h"

void PRE04() {}


/* PRE05_A v.31 */

#define JOIN(x, y) x ## y

void PRE05() {}


/* PRE06_A v.23 */

#include "PRE06_NCCE.h"

void PRE06() {}


/* PRE07_A v.33 */

void PRE07() {
  const size_t i = 0;
  /* assignment of i */
  if (i > 9000) {
    if (puts("Over 9000!??!") == EOF) {}
  }
}

/* PRE08_A v.25 */

#include "PRE08_library1.h"
#include "PRE08_library2.h"

void PRE08() {}


/* PRE09_A v.18 */

#define vsnprintf(buf, size, fmt, list) \
vsprintf(buf, fmt, list)

void PRE09() {}


/* PRE10_A v.16 */

#define PRE10_SWAP(x, y) \
  tmp = x; \
  x = y; \
  y = tmp

void PRE10() {
  const int z = 0;
  int x = 0, y = 0, tmp;
  if (z == 0)
    PRE10_SWAP(x, y);
}


/* PRE31_C v.27 */

#define PRE31_ABS(x) (((x) < 0) ? -(x) : (x))

void PRE31() {
  int n=0, m=0;
  m = PRE31_ABS(++n); 
	printf("PRE31 %d\n", m);
}
