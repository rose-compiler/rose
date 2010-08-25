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
#include <sys/resource.h>

void MEM00();
void MEM01();
//void MEM02();
void MEM03();
void MEM04();
void MEM05();
void MEM06();
void MEM07();
void MEM08();
void MEM09();
//void MEM10();
void MEM30();
void MEM31();
void MEM32();
void MEM33();
void MEM34();
void MEM35();

void MEM(void){
	MEM00();
	MEM01();
//	MEM02();
	MEM03();
	MEM04();
	MEM05();
	MEM06();
	MEM07();
	MEM08();
	MEM09();
//	MEM10();
	MEM30();
	MEM31();
	MEM32();
	MEM33();
	MEM34();
	MEM35();
}

/* MEM00 v.48 */
int verify_size(char *list, size_t size) {
  if (size < 100) {
    /* Handle Error Condition */
    free(list);
    return -1;
  }
  return 0;
}

void MEM00() {
  const size_t number = 15;
  char *list = (char *)malloc(number);

  if (list == NULL) {
    /* Handle Allocation Error */
  }

  if (verify_size(list, number) == -1) {
      free(list);
      return;
  }

  /* Continue Processing list */

  free(list);
}
/* MEM01 v.47 */
void MEM01(void) {
char *message = (char *)malloc(10);
	if(message == NULL) {
		return;
	}
	int message_type = 1;
	if (message_type == 1) {
	  /* Process message type 1 */
	  free(message);
	}
	message_type = 2;
	/* ...*/
	if (message_type == 2) {
          /* Process message type 2 */
	  free(message);
	}
}

/* MEM02 v.51 */
/* TODO */
/* MEM03 v.95 */
void ncce_free_example(char *secret) {
const size_t secret_size = strlen(secret);
char *new_secret;
const size_t size = strlen(secret);
if (size == SIZE_MAX) {
  /* Handle Error */
}

new_secret = (char *)malloc(size+1);
if (!new_secret) {
  /* Handle Error */
}
strcpy(new_secret, secret);

/* Process new_secret... */

free(new_secret);
new_secret = NULL;
}

void ncce_realloc_example(char *secret) {
const size_t secret_size = strlen(secret);
/* ... */
if (secret_size > SIZE_MAX/2) {
   /* handle error condition */
}

secret = (char *)realloc(secret, secret_size * 2);
}

void MEM03(void) {
	char *ptr = (char *)malloc(10);
	if(ptr == NULL) {
		return;
	}
	ncce_free_example(ptr);
	ncce_realloc_example(ptr);
}
/* MEM04 v.70 */
void MEM04_list2(size_t size) {
	int *list = (int *)malloc(size);
	if (list == NULL) {
	  /* Handle Allocation Error */
	}
	free(list);
}

void MEM04(void) {
	char *p2;
	char *p = (char *)malloc(100);

	if (p == NULL) {
	  /* Handle Error */
	}

	/* ... */
	const int nsize = 0;

	if ((p2 = (char *)realloc(p, nsize)) == NULL) {
	  free(p);
	  p = NULL;
	  return;
	}
	p = p2;
	MEM04_list2(0);
}

/* MEM05 v.47 */
static int copy_file(FILE *src, FILE *dst, size_t bufsize) {

  char buf[bufsize];

  while (fgets(buf, bufsize, src)) {
    if(fputs(buf, dst) == EOF) {
      /* handle error */
	}
  }

  return 0;
}

unsigned long fib1(unsigned int n) {
  if (n == 0) {
    return 0;
  }
  else if (n == 1 || n == 2) {
    return 1;
  }
  else {
    return fib1(n-1) + fib1(n-2);
  }
}

void MEM05(void) {
  const int i = copy_file(NULL, NULL, 256);
  const long b = fib1(5);
  printf("%d\n", i + b);
}
/* MEM06 v.22 */
void MEM06(void) {
char *secret;

secret = (char *)malloc(111);
if (!secret) {
  /* Handle Error */
}

/* Perform operations using secret... */

free(secret);
secret = NULL;
}

/* MEM07 v.55 */
void MEM07(void) {
const size_t num_elements = 10;
long *buffer = (long *)calloc(num_elements, sizeof(long));
if (buffer == NULL) {
  /* handle error condition */
}
/*...*/
free(buffer);
buffer = NULL;
}

/* MEM08 v.12 */
typedef struct gadget gadget;
struct gadget {
  int i;
  double d;
  char *p;
};

typedef struct widget widget;
struct widget {
  char *q;
  int j;
  double e;
};

void MEM08(void) {
	gadget *gp = (gadget *) malloc(sizeof(gadget)); if (gp ==NULL) {/* ... */};
	widget *wp;

	/* ... */

	wp = (widget *)realloc(gp, sizeof(widget));
	if (wp == NULL) {
		/* Handle Error */
	}
	printf("MEM08 %p\n", wp);
}

/* MEM09 v.62 */
void MEM09(void) {
	enum { MAX_BUF_SIZE = 256 };

	char *str = NULL;

	/* initialize string to be copied */

	const size_t len = strlen(str);
	if (len >= MAX_BUF_SIZE)  {
	  /* handle string too long error */
	}
	char *buf = (char *)malloc(MAX_BUF_SIZE);
	if (buf == NULL) {
	  /* handle Allocation Error */
	}
	strncpy(buf, str, len);

	/* process buf */

	free(buf);
	buf = NULL;
}
/* MEM10 v.08 */

/* TODO */

/* MEM30 v.68 */
void MEM30_A(char const *arg) {
  char *buff;

  buff = (char *)malloc(BUFSIZ);
  if (!buff) {
     /* handle error condition */
  }
  /* ... */
  free(buff);
  /* ... */
  strncpy(buff, arg, BUFSIZ-1);
}

void MEM30(void) {
  MEM30_A("lul");
}
/* MEM31 v.61 */

void MEM31(void) {
const size_t number = 10;
const int error_condition = 0;
int *x = (int *)malloc (number * sizeof(int));
if (x == NULL) {
  /* Handle Allocation Error */
}
/* ... */
if (error_condition == 1) {
  /* Handle Error Condition*/
  free(x);
}
/* ... */
free(x);
}

/* MEM32 v.63 */
void MEM32(void) {
	const char *input_string = "Awefajgae";
	void *p = NULL;
	const size_t new_size = 10;

	/* initialize input_string */

	const size_t size = strlen(input_string) + 1;
	char *str = (char *)malloc(size);
	strcpy(str, input_string);
	/* ... */
	free(str);
	str = NULL;

	/* initialize new_size */

	p = realloc(p, new_size);
	if (p == NULL)   {
	  /* handle error */
	}
	free(p);
}

/* MEM33 v.28 */

struct flexArrayStruct {
  int num;
  int data[1];
};

/* ... */

void MEM33(void) {
	const size_t array_size = 0;
	size_t i;

	/* space is allocated for the struct */
	struct flexArrayStruct *structP 
	  = (struct flexArrayStruct *) 
		malloc(sizeof(struct flexArrayStruct) 
			+ sizeof(int) * (array_size - 1));
	if (structP == NULL) {
	  /* handle malloc failure */
	}
	structP->num = 0;

	/* access data[] as if it had been allocated
         * as data[array_size] */
        for (i = 0; i < array_size; i++) {
	  structP->data[i] = 1;
	}
}

/* MEM34 v.49 */

enum { MAX_ALLOCATION = 1000 };

static void MEM34_A(int argc, char const *argv[]) {
  char *str = NULL;
  size_t len;

  if (argc == 2) {
    len = strlen(argv[1])+1;
    if (len > MAX_ALLOCATION) {
      /* Handle Error */
    }
    str = (char *)malloc(len);
    if (str == NULL) {
      /* Handle Allocation Error */
    }
    strcpy(str, argv[1]);
  }
  else {
    str = "usage: $>a.exe [string]";
    printf("%s\n", str);
  }
  /* ... */
  free(str);
}

void MEM34(void) {
  char const * argv[2] = {"lol", "huh"};
  MEM34_A(2, argv);
}

/* MEM35 v.73 */

enum { BLOCKSIZE = 16 };
/* ... */
static void *alloc_blocks(size_t num_blocks) {
  if (num_blocks == 0) {
    return NULL;
  }
  const unsigned long long alloc = num_blocks * BLOCKSIZE ;
  return (alloc < UINT_MAX)
     ? malloc(num_blocks * BLOCKSIZE )
     : NULL;
}

void MEM35(void) {
  if(alloc_blocks(10) == NULL) {}
}
