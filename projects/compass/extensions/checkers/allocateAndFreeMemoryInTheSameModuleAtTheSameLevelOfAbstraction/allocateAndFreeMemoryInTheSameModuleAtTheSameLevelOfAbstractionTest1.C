
//Your test file code goes here.

#include <stdlib.h>
#include <stdio.h>

#define MIN_SIZE_ALLOWED 10

int verify_size(char *list, size_t list_size) {
  if (list_size < MIN_SIZE_ALLOWED) {
    /* Handle Error Condition */
    free(list);
    return -1;
  }
  return 0;
}

void process_list(size_t number) {
  char *list1 = (char*) malloc(number);
  char *list2;
  list2 = (char*) malloc(number);

  if (list2 == NULL) {
    /* Handle Allocation Error */
  }

  if (verify_size(list2, number) == -1) {
    /* Handle Error */

  }

  /* Continue Processing list */

  free(list1);
  free(list2);
}

int main(void) {
	process_list(5);
	return 0;
}
