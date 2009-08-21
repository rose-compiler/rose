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

void MSC01();
void MSC02();
void MSC03();
void MSC04();
void MSC05();
void MSC06();
void MSC07();
void MSC09();
void MSC12();
void MSC13();
void MSC14();
void MSC15();
void MSC30();
void MSC31();

void MSC() {
  MSC01();
  MSC02();
  MSC03();
  MSC04();
  MSC05();
  MSC06();
  MSC07();
  MSC09();
  MSC12();
  MSC13();
  MSC14();
  MSC15();
  MSC30();
  MSC31();
}


/* MSC01_A v.35 */

void MSC01() {
  const unsigned int a = 0;
  const unsigned int b = 0;
  const unsigned int c = 0;

  if (a == b) {
    /* ... */
  }
  else if (a == c) {
    /* ... */
  }

	enum WidgetEnum { WE_W, WE_X, WE_Y, WE_Z } widget_type;

	widget_type = WE_W;

	switch (widget_type) {
	  case WE_X:
		/* ... */
		break;
	  case WE_Y:
		/* ... */
		break;
	  case WE_Z:
		/* ... */
		break;
	}
}




/* MSC02_A v.46 */

void MSC02() {
  unsigned int a;
  const unsigned int b = 0;
 
  if (a = b) {
    /* ... */
  }
  printf("MSC02 %d\n", a);
}


/* MSC03_A v.26 */

void MSC03() {
  const unsigned int a = 0;
  const unsigned int b = 0;

/* ROSE catches this */
  a == b;
}


/* MSC04_A v.45 */

void MSC04_critical() {
  /* something critical */
}

void MSC04() {
  
/* ROSE catches this */
  /* comment with end comment marker unintentionally omitted
     MSC04_critical();
     /* some other comment */

}


/* MSC05_A v.40 */

int MSC05_do_work(int seconds_to_work) {
  const time_t start = time(NULL);

  if (start == (time_t)(-1)) {
    /* Handle error */
  }
  while (time(NULL) < start + seconds_to_work) {
    /* ... */
  }
	return 0;
}

void MSC05() {
  const int a = MSC05_do_work(1);
  printf("%d\n", a);
}


/* MSC06_A v.41 */

void MSC06() {
  char pwd[64];

  /* assign pwd */ 

  memset(pwd, 0, sizeof(pwd));
}


/* MSC07_A v.60 */

void MSC07() {
    char *s;
    if (1) {
        s = (char *)malloc(10);
        if (s == NULL) {
           /* Handle Error */
        }
        /* Process s */
        return;
    }
    /* ... */
/* ROSE catches this */
    if (s) {
        /* This code is never reached */
    }
    return;
}


/* MSC09_A v.50 */

void MSC09() {
  const char *file_name = "&#xBB;&#xA3;???&#xAB;";
  const mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

  const int fd = open(file_name, O_CREAT | O_EXCL | O_WRONLY, mode);
  if (fd == -1) {
    /* Handle Error */
  }
  close(fd);
}


/* MSC12_A v.34 */

void MSC12() {
  int a = 0;
  const int b = 1;

/* ROSE catches this */
  a == b;

  int *p = &a;
  *p++;
}

/* MSC13-C */
int * MSC13_foo() {
	return NULL;
}

int * MSC13_bar() {
	return NULL;
}

int * MSC13_baz() {
	return NULL;
}

int *MSC13_() {
	int *p1, *p2;
	p1 = MSC13_foo();
	p2 = MSC13_bar();

	if (MSC13_baz()) {
	  return p1;
	}
	else {
	  p2 = p1;
	}
	return p2;
}

void MSC13() {
	int *x = MSC13_();
	printf("MSC13 %p\n", x);
}

/* MSC14_A v.14 */

void MSC14() {
  const unsigned int ui1 = 0, ui2 = 0;

  if (~ui1 < ui2) {
    /* handle error condition */
  }
  const unsigned int sum = ui1 + ui2;
	printf("MSC14 %d\n",sum);
}


/* MSC15_A v.25 */

int MSC15_foo(int a) {
  assert(a + 100 > a);
  printf("%d %d\n", a + 100, a);
  return a;
}

void MSC15() {
  const int a = MSC15_foo(100);
  const int b = MSC15_foo(INT_MAX);
  printf("%d\n", a + b);
}


/* MSC30_C v.45 */

void MSC30() {
  enum {len = 12};
  char id[len];  /* id will hold the ID, starting with 
		  * the characters "ID" followed by a 
		  * random integer */
  int r;
  int num;
  /* ... */
  r = rand();  /* generate a random integer */
  num = snprintf(id, len, "ID%-d", r);  /* generate the ID */
	printf("MSC30 %d\n", num);
}


/* MSC31_C v.104 */

void MSC31() {
  const time_t now = time(NULL);
  if (now != -1) {
    /* Continue processing */
  }
}


