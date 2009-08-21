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
void ARR32();
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
	ARR32();
	ARR33();
	ARR34();
//	ARR35();
	ARR36();
	ARR37();
	ARR38();
}

/*ARR01_A v.46 */

void clear2(int array[]) {
  size_t i;
  for (i = 0; i < sizeof(array) / sizeof(array[0]); ++i) {
     array[i] = 0;
   }
}

void ARR01(void) {
  int dis[12];

  clear2(dis);
  /* ... */
}

/*ARR02_A v.08 */

void ARR02(void) {
/* gcc won't allow this
  int ARR02_a[3] = {1, 2, 3, 4};
*/
}

/*ARR30_C v.47 */

int *table2 = NULL;

int ARR30_NCCE_insert_in_table(int pos, int value){
enum { TABLESIZE = 100 };
  if (!table2) {
    table2 = (int *)malloc(sizeof(int) * TABLESIZE);
  }
  if (pos >= TABLESIZE) {
    return -1;
  }
  table2[pos] = value;
  return 0;
}

void ARR30(void) {
	if (ARR30_NCCE_insert_in_table(0, 1) == -1) {}
}


/*ARR31_C v.53 */

/* not implemented */


///*ARR32_C v.71 */
//void func(size_t s) {
//  int vla[s];
//  /* ... */
//  vla[s-1] = 0;
//}
//
//void ARR32(void) {
//func(50);
//}

/*ARR33_C v.50 */

void ARR33_C(int const src[], size_t len) {
enum { WORKSPACE_SIZE = 256 };
  int dest[WORKSPACE_SIZE];
  memcpy(dest, src, len * sizeof(int));
  /* ... */
}

void ARR33(void) {
	int lol[32];
	ARR33_C(lol, 15);
}

/*ARR34_C v.30 */

void ARR34(void) {
	enum { a = 10, b = 15, c = 20 };

	int arr1[c][b];
	int (*arr2)[a] = NULL;
	/* GCC won't let this by, (the whole purpose of the rule
	arr2 = arr1;
	*/
	printf("ARR34 %p %p\n", arr1, arr2);
}

/*ARR35_C v.21 */

/* No linux solution */

/*ARR36_C v.17 */

void ARR36(void) {
	const int nums[32];
	char *strings[32];
	const int *next_num_ptr = nums;
	int free_bytes;

	free_bytes = strings - (char **)next_num_ptr;
	printf("ARR36 %d\n", free_bytes);
}

/*ARR37_C v.22 */
struct numbers {
  int num1;
  int num2;
  /* ... */
  int num64;
 };

int sum_numbers2(const struct numbers *numb){
  int total = 0;
  int const *numb_ptr;

  for (numb_ptr = &numb->num1; 
       numb_ptr <= &numb->num64; 
       numb_ptr++) 
  {
    total += *(numb_ptr);
  }

  return total;
}

void ARR37(void) {
  struct numbers my_numbers = { 1, 2, /* ... */ 64 };
  if (sum_numbers2(&my_numbers) < 100) {}
}

/*ARR38_C v.28 */
void ARR38(void) {
	int ar[20];
	int *ip;

/* ROSE catches this */
	for (ip = &ar[0]; ip < &ar[21]; ip++) {
	  *ip = 0;
	}

	char *buf = NULL;
	size_t len = 1 << 30;

	/* Check for overflow */
	if (buf + len < buf) {
	  len = -(uintptr_t)buf-1;
	}
}
