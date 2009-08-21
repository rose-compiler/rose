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

void ARR01();
void ARR02();
void ARR30();
//void ARR31();
//void ARR32();
void ARR33();
void ARR34();
//void ARR35();
void ARR36();
void ARR37();
void ARR38();

void ARR() {
	ARR01();
	ARR02();
	ARR30();
//	ARR31();
//	ARR32();
	ARR33();
	ARR34();
//	ARR35();
	ARR36();
	ARR37();
	ARR38();
}

/*ARR01_A v.46 */

void clear(int array[], size_t size) {
  size_t i;
  for (i = 0; i < size; i++) {
     array[i] = 0;
  }
}

void ARR01(void) {
  int dis[12];

  clear(dis, sizeof(dis) / sizeof(dis[0]));
}

/*ARR02_A v.08 */

void ARR02(void) {
  const int ARR02_a[4] = {1, 2, 3, 4};
  printf("ARR02: %d\n", ARR02_a);
}

/*ARR30_C v.47 */


int *table = NULL;

int ARR30_CS_insert_in_table(size_t pos, int value){
enum { TABLESIZE = 100 };
  if (!table) {
    table = (int *)malloc(sizeof(int) * TABLESIZE);
  }
  if (pos >= TABLESIZE) {
    return -1;
  }
  table[pos] = value;
  return 0;
}

void ARR30(void) {
	if (ARR30_CS_insert_in_table(0, 1) != 0) {
		/* Handle Error */
	}
}

/*ARR31_C v.53 */

/* Not implemented. */

/*ARR32_C v.71 */

/* VLAs are NOT Supported */

//enum { MAX_ARRAY = 1024 };
//
//void func(size_t s) {
//  if (s < MAX_ARRAY && s != 0) {
//    int vla[s];
//    /* ... */
//	vla[s-1] = 0;
//  } else {
//    /* Handle Error */
//  }
//}
//
//void ARR32(void) {
//  func(50);
//}

/*ARR33_C v.50 */

void ARR33_A(int const src[], size_t len) {
enum { WORKSPACE_SIZE = 256 };
  int dest[WORKSPACE_SIZE];
  if (len > WORKSPACE_SIZE) {
      /* Handle Error */
  }
  memcpy(dest, src, sizeof(int)*len);
  /* ... */
}

void ARR33_B(int const src[], size_t len) {
  int *dest;
  if (len > SIZE_MAX/sizeof(int)) {
   /* handle overflow */
  }
  dest = (int *)malloc(sizeof(int) * len);
  if (dest == NULL) {
     /* Couldn't get the memory - recover */
  }
  memcpy(dest, src, sizeof(int) * len);
  /* ... */
  free(dest);
}

void ARR33(void) {
	int lol[32];
	ARR33_A(lol, 15);
	ARR33_B(lol, 15);
}

/*ARR34_C v.30 */

void ARR34(void) {
	enum { ARR34_a = 10, ARR34_b = 10, ARR34_c = 20 };
	int arr1[ARR34_c][ARR34_b];
	int (*arr2)[ARR34_a];
	arr2 = arr1; /* OK, because a == b */
	printf("ARR34 %d\n", arr2[0]);
}

/*ARR35_C v.21 */

/* No linux solution */

/*ARR36_C v.17 */

void ARR36(void) {
	const int nums[16];
	char *strings[16];
	const int *next_num_ptr = nums;
	int free_bytes;

	free_bytes = (next_num_ptr - nums) * sizeof(int);
	printf("ARR36 %d %p\n", free_bytes, strings);
}

/*ARR37_C v.22 */

int sum_numbers(int const *numb, size_t dim) {
  int total = 0;
  int const *numb_ptr;

  for (numb_ptr = numb; numb_ptr < numb + dim; numb_ptr++) {
    total += *(numb_ptr);
  }

  return total;
}

void ARR37(void) {
  int my_numbers[3] = { 1, 2, 3 };
	const int sum = sum_numbers(my_numbers, sizeof(my_numbers)/sizeof(my_numbers[0]));
	printf("%d\n", sum);
}

/*ARR38_C v.28 */

void ARR38(void) {
	int ar[20];
	int *ip;

	for (ip = &ar[0]; ip < &ar[sizeof(ar)/sizeof(ar[0])]; ip++) {
	  *ip = 0;
	}

	char buf[] = "foo";
	size_t len = 1u << 30u;

	/* Check for overflow */
	if ((uintptr_t) buf+len < (uintptr_t) buf) {
	  len = -(uintptr_t)buf - 1;
	}

	/* Check for overflow */
	if (UINTPTR_MAX - len < (uintptr_t)buf) {
	  len = -(uintptr_t)buf-1;
	}
}
