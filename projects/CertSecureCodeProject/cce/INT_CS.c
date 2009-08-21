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

void INT00();
void INT02();
void INT04();
void INT05();
void INT06();
void INT07();
void INT08();
void INT09();
void INT10();
void INT11();
void INT12();
void INT13();
void INT14();
void INT30();
void INT31();
void INT32();
void INT33();
void INT34();
void INT35();

void INT() {
  INT00();
  INT02();
  INT04();
  INT05();
  INT06();
  INT07();
  INT08();
  INT09();
  INT10();
  INT11();
  INT12();
  INT13();
  INT14();
  INT30();
  INT31();
  INT32();
  INT33();
  INT34();
  INT35();
}


/* INT00_A v.41 */

void INT00() {
#if UINT_MAX > (UINTMAX_MAX/UINT_MAX)
 #error "No safe type is available."
#endif
  /* ... */
  const unsigned int a = 2, b = 3;
  uintmax_t c;
  /* ... */
  c = (uintmax_t)a * b; /* guaranteed to fit, verified above */
  printf("INT00 %ju\n", c);
}


/* INT02_A v.54 */

void INT02() {
  const int si = -1;
  const unsigned ui = 1;
  if(si < (int) ui)
    return;
  return;
}


/* INT04_A v.44 */

enum { INT04_MAX_TABLE_LENGTH = 256 };

int INT04_create_table(size_t length) {
  size_t table_length;
  char **table;

  if (length == 0 || length > INT04_MAX_TABLE_LENGTH) {
    /* Handle invalid length */
  }

  /*
   * The wrap check has been omitted based on the assumption 
   * that MAX_TABLE_LENGTH * sizeof(char *) cannot exceed 
   * SIZE_MAX. If this assumption is not valid, a check must 
   * be added.
   */
  assert(length <= SIZE_MAX/sizeof(char *));

  table_length = length * sizeof(char *);

  table = (char **)malloc(table_length);
  if (table == NULL) {
    /* Handle error condition */
  }
  else
    free(table);

  /* ... */
  return 0;
}

void INT04() {
	if (INT04_create_table(5) != 0) {
		/* Handle Error */
	}
}


/* INT05_A v. */

void INT05() {
  char buff[25];
  char *end_ptr;
  long sl;

  if (fgets(buff, sizeof(buff), stdin) == NULL) {
    if (puts("EOF or read error\n") == EOF) {
		/* Handle Error */
	}
  } else {
    errno = 0;

    sl = strtol(buff, &end_ptr, 10);

    if (ERANGE == errno) {
      if (puts("number out of range\n") == EOF) {
        /* Handle Error */
      }
    }
    else if (end_ptr == buff) {
      if (puts("not valid numeric input\n") == EOF) {
        /* Handle Error */
      }
    }
    else if ('\n' != *end_ptr && '\0' != *end_ptr) {
      if (puts("extra characters on input line\n") == EOF) {
        /* Handle Error */
      }
    }
    printf("INT05 %ld\n", sl);
  }
}


/* INT06_A v.46 */

void INT06() {
  long sl;
  int si;
  char *end_ptr;

  sl = strtol("4", &end_ptr, 10);

  if ((sl == LONG_MIN)
      || (sl == LONG_MAX) 
      || (end_ptr == "4"))
    {
      if (errno != 0) {
	/* perror(errno) */
      }
      else {
	if (puts("error encountered during conversion") == EOF) {
      /* Handle Error */
    }
      }
    }
  else if (sl > INT_MAX) {
    printf("%ld too large!\n", sl);
  }
  else if (sl < INT_MIN) {
    printf("%ld too small!\n", sl);
  }
  else if ('\0' != *end_ptr) {
    if (puts("extra characters on input line\n") == EOF) {
      /* Handle Error */
    }
  }
  else {
    si = (int)sl;
    printf("INT06 %d\n", si);
  }
  printf("INT06 %ld\n", sl);
}


/* INT07_A v.34 */

void INT07() {
  const unsigned char c = 200;
  const int i = 1000;
  printf("INT05 %d\n", i/c);
}


/* INT08_A v.32 */

void INT08() {
  long i = 32766 + 1;
  i++;
}


/* INT09_A v.28 */

void INT09() {
  enum { red=4, orange, yellow, green, blue, indigo, violet};
}


/* INT10_A v.30 */

int INT10_insert(size_t INT10_index, int *list, size_t size, int value) {
  if (size != 0) {
    INT10_index = (INT10_index + 1) % size;
    list[INT10_index] = value;
    return INT10_index;
  }
  else {
    return -1;
  }
}

void INT10() {
  int list[10];

  if (INT10_insert(0, list, SIZE_MAX, 4) != 0) {
    /* Handle Error */
  }
}


/* INT11_A v.40 */

struct INT11_ptrflag {
  char *pointer;
  unsigned int flag :9;
} ptrflag;

void INT11() {
  char *ptr = NULL;
  const unsigned int flag = 0;

  ptrflag.pointer = ptr;
  ptrflag.flag = flag;
}

/* INT12_A v.44 */

struct {
  unsigned int a: 8;
} INT12_bits = {225};

void INT12() {
  printf("INT12 %d\n", INT12_bits.a);
}


/* INT13_A v.34 */

void INT13() {
  int rc = 0;
  const unsigned int stringify = 0x80000000;
  char buf[sizeof("256")];
  rc = snprintf(buf, sizeof(buf), "%u", stringify >> 24);
  if (rc == -1 || rc >= sizeof(buf)) {
    /* handle error */
  }
}


/* INT14_A v.49 */

void INT14() {
  unsigned int x = 50;
  x = 5*x+1;
}


/* INT30_C v.24 */

void INT30() {
  unsigned int ui1, ui2, sum;

 ui1 = 0;
 ui2 = 0;

  if(UINT_MAX - ui1 < ui2)
    return;
  sum = ui1 + ui2;
  printf("INT30 %d\n", sum);
}


/* INT31_C v.66 */

void INT31() {
  const unsigned long int ul = ULONG_MAX;
  signed char sc;
  if (ul <= SCHAR_MAX) {
    sc = (signed char)ul;  /* use cast to eliminate warning */
    printf("INT31 %c\n", sc);
  }
  else {
    /* handle error condition */
  }
}


/* INT32_C v.110 */

void INT32() {
  signed int si1, si2;
  si1 = si2 = 0;

  if (((si1>0) && (si2>0) && (si1 > (INT_MAX-si2))) 
      || ((si1<0) && (si2<0) && (si1 < (INT_MIN-si2)))) 
    {
      /* handle error condition */
    }

	signed int result;

	if (si1 == INT_MIN) {
		/* handle error condition */
	}
	result = -si1;
	printf("%d\n", result);
}


/* INT33_C v.41 */

void INT33() {
  signed long sl1, sl2, result;
  sl1 = sl2 = 0;

  if ( (sl2 == 0) || ( (sl1 == LONG_MIN) && (sl2 == -1) ) ) {
    /* handle error condition */
  }
  result = sl1 / sl2;
  printf("INT33 %d\n", result);
}


/* INT34_C v.43 */

void INT34() {
	const unsigned int ui1 = 0, ui2 = 0;
	unsigned int sresult;

	if ((ui2 >= sizeof(int)*CHAR_BIT) 
	  || (ui1 > (INT_MAX >> ui2)) ) 
	{
	  /* handle error condition */
	}
	sresult = ui1 << ui2;
	printf("INT34 %d\n", sresult);
}




/* INT35_C v.34 */

enum { INT35_BLOCK_HEADER_SIZE = 16 };
unsigned long long INT35_max = UINT_MAX;

void *INT35_AllocateBlock(size_t length) {
  struct memBlock *mBlock;

  if ((unsigned long long)length + INT35_BLOCK_HEADER_SIZE > INT35_max) 
    return NULL;
  mBlock 
    = (struct memBlock *)malloc(length + INT35_BLOCK_HEADER_SIZE);
  if (!mBlock) return NULL;

  /* fill in block header and return data portion */

  return mBlock;
}

void INT35() {
  free(INT35_AllocateBlock(10));
}
