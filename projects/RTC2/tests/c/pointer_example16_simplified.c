#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PTR_SIZE 100
#define PTR2_SIZE 10
#define PTR3_SIZE 10
#define OUT_OF_BOUNDS_EXCESS 1
#define ARRAY_SIZE 29
#define ARRAY2_SIZE 49


struct node {
  struct node* prev;
  struct node* next;
  unsigned int val;
  //unsigned int array1[PTR2_SIZE];
};

void insert_node(struct node* insert, struct node* after) {
  assert(insert != NULL);
  assert(after != NULL);

  // start: after -> second
  // start: after <- second

  // after -> insert -> second
  insert->next = after->next;
  after->next = insert;

  // after <- insert
  insert->prev = after;

  // insert <- second
  if(insert->next != NULL) {
    insert->next->prev = insert;
  }
}

void delete_node(struct node* to_del) {

  assert(to_del != NULL);

  // start: first -> to_del -> second
  // start: first <- to_del <- second

  // first -> second
  to_del->prev->next = to_del->next;

  // first <- second
  to_del->next->prev = to_del->prev;

  free(to_del);
}


unsigned int* fn2(void* voidin, unsigned int*, float*, double*, char*);

void print_array(unsigned int* array, unsigned int size);


unsigned int* fn1(unsigned int* input) {

  unsigned int* nixptr = fn2((void*)input, input, (float*)input, (double*)input, (char*)input);

  printf("input: %u\n", *input);
  return (unsigned int*)malloc(PTR_SIZE*sizeof(unsigned int));
}

unsigned int* fn2(void* voidin, unsigned int* input, float* input2, double* input3, char* input4) {
  return NULL;
}


int main() {

  unsigned int *no_init_ptr;

  unsigned int *ptr = (unsigned int*)malloc(PTR_SIZE*sizeof(int));
  unsigned int *ptr2 = (unsigned int*)malloc(PTR2_SIZE*sizeof(int));

  unsigned int* ptr_index;
  unsigned int counter = 0;
  for(ptr_index = ptr; ptr_index < ptr + PTR_SIZE; ptr_index++) {
    *ptr_index = counter++;
  }

  // dot exp check
  struct node dot_head;
  no_init_ptr = fn2((void*)dot_head.next, ptr, (float*)dot_head.prev, (double*)ptr2, (char*)NULL);

  // arrow exp check
  struct node* head = (struct node*)malloc(sizeof(struct node));
  no_init_ptr = fn2((void*)head->next, ptr, (float*)head->prev, (double*)ptr2, (char*)NULL);

  for(ptr_index = ptr + PTR_SIZE - 1; ptr_index >= ptr; ptr_index--) {
    printf("%u\n", *ptr_index);
  }

  free(ptr);
  free(ptr2);
  free(head);
  return 0;
}
