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
#include <stdint.h>

void EXP00();
void EXP01();
void EXP02();
void EXP03();
void EXP04();
void EXP05();
void EXP06();
void EXP07();
void EXP09();
void EXP10();
void EXP11();
void EXP12();
void EXP31();
void EXP32();
void EXP33();
void EXP34();
void EXP35();
void EXP37();

void EXP() {
  EXP00();
  EXP01();
  EXP02();
  EXP03();
  EXP04();
  EXP05();
  EXP06();
  EXP07();
  EXP09();
  EXP10();
  EXP11();
  EXP12();
  EXP31();
  EXP32();
  EXP33();
  EXP34();
  EXP35();
  EXP37();
}


/* EXP00_A v.52 */

void EXP00() {
  const int x = random();

  if ((x & 1) == 0) {
	printf("EXP00");
  }
}


/* EXP01_A v.43 */

double *EXP01_allocate_array(size_t num_elems) {
  double *d_array;

  if (num_elems > SIZE_MAX/sizeof(*d_array)) {
    /* handle error condition */
  }
  d_array = (double *)malloc(sizeof(*d_array) * num_elems);
  if (d_array == NULL) {
    /* handle error condition */
  }
  return d_array;
}

void EXP01() {
  free(EXP01_allocate_array(3));
}


/* EXP02_A v.38 */

void EXP02() {
  char *p = NULL;

  const unsigned int BUF_SIZE = 12;

  if (p == NULL) p = (char *)malloc(BUF_SIZE);
  if (p == NULL) {
    /* handle error */
    return;
  }
  
  /* do stuff with p */
  
  free(p);
}


/* EXP03_A v.59 */

enum {EXP03_buffer_size = 50};

struct EXP03_buffer {
  size_t size;
  char bufferC[EXP03_buffer_size];
};

void EXP03_func(const struct EXP03_buffer *buf) {

  struct EXP03_buffer *buf_cpy = 
    (struct EXP03_buffer *)malloc(sizeof(struct EXP03_buffer));

  if (buf_cpy == NULL) {
    /* Handle malloc() error */
  }

  /* ... */

  memcpy(buf_cpy, buf, sizeof(struct EXP03_buffer));

  /* ... */

  free(buf_cpy);
}


/* EXP04_A v.72 */

struct EXP04_my_buf {
  char buff_type;
  size_t size;
  char buffer[50];
};

unsigned int EXP04_buf_compare(
  const struct EXP04_my_buf *s1, 
  const struct EXP04_my_buf *s2) 
{
  if (s1->buff_type != s2->buff_type) return 0;
  if (s1->size != s2->size) return 0;
  if (strcmp(s1->buffer, s2->buffer) != 0) return 0;
  return 1;
}

void EXP04() {
	if (EXP04_buf_compare(NULL, NULL) != 0) {
		/* Handle Error */
	}
}


/* EXP05_A v.59 */

void EXP05_remove_spaces(char *str, size_t slen) {
  char *p = (char *)str;
  size_t i;
  for (i = 0; i < slen && str[i]; i++) {
    if (str[i] != ' ') *p++ = str[i];
  }
  *p = '\0';
}

void EXP05() {
  EXP05_remove_spaces(NULL, 0);
}


/* EXP06_A v.29 */

void EXP06() {
  int a = 14;
  const int b = sizeof(a);
  a++;
  printf("EXP06 %d\n", b);
}


/* EXP07_A v.14 */

void EXP07() {
  const unsigned int nbytes = 4;
  const unsigned int nblocks = 1 + (nbytes - 1) / BUFSIZ;
  printf("EXP07 %d\n", nblocks);
}


/* EXP09_A v.44 */

void EXP09() {
  size_t i;
  int **matrix = (int **)calloc(100, sizeof(*matrix));
  if (matrix == NULL) {
    /* handle error */
  }

  for (i = 0; i < 100; i++) {
    matrix[i] = (int *)calloc(i, sizeof(**matrix));
    if (matrix[i] == NULL) {
      /* handle error */
    }
  }
}


/* EXP10_A v.15 */

int EXP10_g;

int EXP10_f(int i) {
  EXP10_g = i;
  return i;
}

void EXP10() {
  int x = EXP10_f(1);
  x += EXP10_f(2); 
}


/* EXP30_C v.58 */

void EXP30() {
  int i = 0;
  i = i + 2;
}


/* EXP11_A v.42 */

void EXP11() {
  float f = 0.0;
  int i = 0;
  float *fp;
  int *ip;

  ip = &i;
  fp = &f;
  printf("int is %d, float is %f\n", i, f);
  (*ip)++;
  (*fp)++;

  printf("int is %d, float is %f\n", i, f);
}

/* EXP12_A v.12 */

void EXP12() {
	char dst[] = "foo";
	char src[] = "bar";

	if (puts("foo") == EOF) {
		/* Handle Error */
	}

	/* Due to a problem in ROSE this will still flag the rule */
	(void) rename(src, dst);

	strcpy(dst, src);
}

/* EXP31_C v.14 */

void EXP31() {
  unsigned int EXP31_index = 0;
  assert(EXP31_index > 0);
  EXP31_index++;
}


/* EXP32_C v.32 */

void EXP32() {
  static volatile int **ipp;
  static volatile int *ip;
  static volatile int i = 0;

  printf("i = %d.\n", i);

  ipp = &ip;
  *ipp = &i;
  if (*ip != 0) {
    /* ... */
  }
}


/* EXP33_C v.88 */

void EXP33_set_flag(int number, int *sign_flag) {
  if (sign_flag == NULL) {
    return;
  }
  if (number >= 0) {
    *sign_flag = 1;
  }
  else if (number < 0) {
    *sign_flag = -1;
  }
}

void EXP33() {
  int sign;
  int number = 2;

  EXP33_set_flag(number, &sign);

  number = sign + 1;
}


/* EXP34_C v.72 */

void EXP34() {
  char input_str[] = "foo";
  const size_t size = strlen(input_str)+1;
  char *str = (char*) malloc(size);
  if(str == NULL) {
    /* Handle allocation error */
  }

  memcpy(str, input_str, size);
  /* ... */
  free(str);
  str = NULL; 
}


/* EXP35_C v.26 */

struct EXP35_X { char a[6]; };

struct EXP35_X EXP35_addressee(void) {
  const struct EXP35_X result = { "world" };
  return result;
}

void EXP35() {
  const struct EXP35_X my_x = EXP35_addressee();
  printf("EXP35 %s\n", my_x.a);
}


/* EXP37_C v.50 */

void EXP37() {
	char *(*fp) (const char *, int);
  char *c;
  fp = strchr;
  c = fp("Hello",'H');
  printf("%s\n", c);
}
