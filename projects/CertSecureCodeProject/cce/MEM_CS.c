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
//      MEM02();
        MEM03();
        MEM04();
        MEM05();
        MEM06();
        MEM07();
        MEM08();
        MEM09();
//      MEM10();
        MEM30();
        MEM31();
        MEM32();
        MEM33();
        MEM34();
        MEM35();
}

/* MEM00 v.48 */
int verify_size(char const *list, size_t size) {
  if (size < 100) {
    /* Handle Error Condition */
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
  char *message = malloc(1); if (message == NULL) {}
  int message_type = 1;

  if (message_type == 1) {
    /* Process message type 1 */
    free(message);
    message = NULL;
    return;
  }
  message_type = 2;
  /* ...*/
  if (message_type == 2) {
    /* Process message type 2 */
    free(message);
    message = NULL;
  }
}
/* MEM02 v.51 */
/* TODO */
/* MEM03 v.95 */
void free_example(char *secret) {
  char *new_secret;
  const size_t size = strlen(secret);
  if (size == SIZE_MAX) {
    /* Handle Error */
  }
  /* use calloc() to zero-out allocated space */
  new_secret = (char *)calloc(size+1, sizeof(char));
  if (!new_secret) {
    /* Handle Error */
  }
  strcpy(new_secret, secret);

  /* Process new_secret... */

  /* sanitize memory  */
  memset(new_secret, '\0', size);
  free(new_secret);
  new_secret = NULL;
}

void realloc_example(char *secret) {
  char *temp_buff;
  const size_t secret_size = strlen(secret);
  /* ... */
  if (secret_size > SIZE_MAX/2) {
     /* handle error condition */
  }
  /* calloc() initializes memory to zero */
  temp_buff = (char *)calloc(secret_size * 2, sizeof(char));
  if (temp_buff == NULL) {
   /* Handle Error */
  }

  memcpy(temp_buff, secret, secret_size);

  /* sanitize the buffer */
  memset(secret, '\0', secret_size);

  free(secret);
  secret = temp_buff; /* install the resized buffer */
  temp_buff = NULL;
}

void MEM03(void) {
        char *ptr = (char *)malloc(10);
        if(ptr == NULL) {
                return;
        }
        free_example(ptr);
        realloc_example(ptr);
}
/* MEM04 v.70 */
void MEM04_list(size_t size) {
  if (size == 0) {
    /* Handle Error */
  }
  const int *list = (int *)malloc(size);
  if (list == NULL) {
    /* Handle Allocation Error */
  }
}

void MEM04(void) {
  const size_t nsize = 0;
  char *p2;
  char *p = (char *)malloc(100);

  if (p == NULL) {
    /* Handle Error */
  }

  /* ... */
  if ( (nsize == 0) 
    || (p2 = (char *)realloc(p, nsize)) == NULL) 
  {
    free(p);
    p = NULL;
    return;
  }
  p = p2;

  MEM04_list(0);
}

/* MEM05 v.47 */
static int copy_file(FILE *src, FILE *dst, size_t bufsize) {
  if (bufsize == 0) {
    /* Handle Error */
  }
  char *buf = (char *)malloc(bufsize);
  if (!buf) {
    return -1;
  }

  while (fgets(buf, bufsize, src)) {
    if (fputs(buf, dst) == EOF) {
      /* Handle Error */
    }
  }
  /* ... */
  free(buf);
  return 0;
}

unsigned long fib2(unsigned int n) {
  if (n == 0) {
    return 0;
  }
  else if (n == 1 || n == 2) {
    return 1;
  }

  unsigned long prev = 1;
  unsigned long cur = 1;

  unsigned int i;

  for (i = 3; i <= n; i++) {
    const unsigned long tmp = cur;
    cur = cur + prev;
    prev = tmp;
  }

  return cur;
}

void MEM05(void) {
  if (copy_file(NULL, NULL, 256) != 0) {
    /* Handle Error */
  }
  printf("fib : %d\n",fib2(5));
}

/* MEM06 v.22 */

void MEM06(void) {
        struct rlimit limit;
        char *secret = NULL;

        limit.rlim_cur = 0;
        limit.rlim_max = 0;
        if (setrlimit(RLIMIT_CORE, &limit) != 0) {
                /* Handle Error */
        }

        /* Create or otherwise obtain some sensitive data */
        if (fgets(secret, sizeof(secret), stdin) == NULL) { 
                /* Handle Error */
        }
}

/* MEM07 v.55 */

void MEM07(void) {
  long *buffer;
  const size_t num_elements = 10;
  if (num_elements > SIZE_MAX/sizeof(long)) {
    /* handle error condition */
  }
  buffer = (long *)calloc(num_elements, sizeof(long));
  if (buffer == NULL) {
    /* handle error condition */
  }
}
/* MEM08 v.12 */
typedef struct widget widget;
struct widget {
  char *q;
  int j;
  double e;
};

void MEM08(void) {
        widget *wp, *wq;

        wp = (widget *)malloc(10 * sizeof(widget));
        wq = (widget *)realloc(wp, 20 * sizeof(widget));
        wp = (widget *)realloc(wq, 15 * sizeof(widget));
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
          /* handle allocation error */
        }

        /* initialize memory to default value */
        memset(buf, '\0', MAX_BUF_SIZE); 
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
  strncpy(buff, arg, BUFSIZ-1);
  /* ... */
  free(buff);
}

void MEM30(void) {
  MEM30_A("lul");
}
/* MEM31 v.61 */

void MEM31(void) {
  const size_t number = 10;
  const int error_condition = 0;

  if (number > SIZE_MAX/sizeof(int)) {
    /* handle overflow */
  }
  int *x = (int *)malloc(number * sizeof(int));
  if (x == NULL) {
    /* Handle Allocation Error */
  }
  /* ... */
  if (error_condition == 1) {
    /* Handle Error Condition*/
  }
  /* ... */
  free(x);
  x = NULL;
}
/* MEM32 v.63 */
void MEM32(void) {
  const char *input_string = "awefawe";
  void *p = malloc(sizeof(void)); if (p == NULL) {}
  void *q = NULL;
  const size_t new_size = 10;

  const size_t size = strlen(input_string) + 1;
  char *str = (char *)malloc(size);
  if (str == NULL) {
    /* handle allocation error */
  }
  strcpy(str, input_string);
  /* ... */
  free(str);
  str = NULL;

  /* initialize new_size */

  q = realloc(p, new_size);
  if (q == NULL)   {
   /* handle error */
  }
  else {
   p = q;
  }
}

/* MEM33 v.28 */
struct flexArrayStruct{
  int num;
  int data[];
};

void MEM33(void) {
        const size_t array_size = 100;
        size_t i;

        /* initialize array_size */

        /* Space is allocated for the struct */
        struct flexArrayStruct *structP 
          = (struct flexArrayStruct *)
                 malloc(sizeof(struct flexArrayStruct) 
                          + sizeof(int) * array_size);
        if (structP == NULL) {
          /* handle malloc failure */
        }

        structP->num = 0;

        /* Access data[] as if it had been allocated 
         *  * as data[array_size] */
        for (i = 0; i < array_size; i++) {
          structP->data[i] = 1;
        }
}

/* MEM34 v.49 */

enum { MAX_ALLOCATION = 1000 };

static int MEM34_A(int argc, char const *argv[]) {
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
    printf("%s\n", "usage: $>a.exe [string]");
    return -1;
  }
  /* ... */
  free(str);
  return 0;
}

void MEM34(void) {
  char const * argv[2] = {"lol", "huh"};
  if (MEM34_A(2, argv) != 0) {
    /* Handle Error */
  }
}

/* MEM35 v.73 */

enum { BLOCKSIZE = 16 };
/* ... */
static void *alloc_blocks(size_t num_blocks) {
  if (num_blocks == 0 || num_blocks > SIZE_MAX / BLOCKSIZE) 
    return NULL;
  return malloc(num_blocks * BLOCKSIZE);
}

void MEM35(void) {
  if (alloc_blocks(10) == NULL) {
    /* Handle Error */
  }
}
