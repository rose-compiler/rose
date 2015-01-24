#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <inttypes.h>

uintptr_t prog_stack_bottom;
uintptr_t prog_stack_local;
uintptr_t prog_stack_max;


void init_stack_info() {
  pthread_t self = pthread_self();
  pthread_attr_t attr;
  void *stack;
  size_t stacksize;
  pthread_getattr_np(self, &attr);
  pthread_attr_getstack(&attr, &stack, &stacksize);
  uintptr_t stackmax=(uintptr_t)((long int)stack+(long int)stacksize);
  prog_stack_bottom=(uintptr_t)stack;
  prog_stack_max=stackmax;
  prog_stack_local=prog_stack_max;
}

void print_stack_info() {
  printf("STACKTOP     : %x\n",prog_stack_max);
  printf("STACKLOCALTOP: %x\n",prog_stack_local);
  printf("STACKEND     : %x\n",prog_stack_bottom);
}


int is_stack2(void *ptr) {
  return ((uintptr_t) ptr >= (uintptr_t) prog_stack_bottom
          && (uintptr_t) ptr <= (uintptr_t) prog_stack_local);
}

int is_stack(void *ptr)
{
  pthread_t self = pthread_self();
  pthread_attr_t attr;
  pthread_attr_init(&attr);
 void *stack; // lowest addressable byte
  size_t stacksize; // storage size (increasing addresses)
  pthread_getattr_np(self, &attr);
  pthread_attr_getstack(&attr, &stack, &stacksize);
  //printf("Stackptr: %x stacksize:%d\n",stack, stacksize);
  //printf("CHECK: stackstart: %x stackend: %x check: %x\n",stack,(uintptr_t*)stack+stacksize,ptr);
  return ((uintptr_t) ptr >= (uintptr_t) stack
          && (uintptr_t) ptr < (uintptr_t) stack + stacksize);
}

int f(int* a, int* b, int* c) {
  int y;
  int z;
  prog_stack_local=(uintptr_t)&y;
  printf("&f::y : %x\n",&y);
  printf("&f::z : %x\n",&z);

  printf("&f::y:%d  &f::z:%d\n", is_stack(&y),is_stack(&z));
  printf("&a:%d b:%d c:%d\n", is_stack(a),is_stack(b), is_stack(c));
  printf("-----------------------------\n");
  printf("&a:%d b:%d c:%d\n", is_stack2(a),is_stack2(b), is_stack2(c));
  printf("&f::y:%d  &f::z:%d\n", is_stack2(&y),is_stack2(&z));
  printf("-----------------------------\n");
  print_stack_info();
}
 
int main()
{
  int x;
  init_stack_info();
  prog_stack_local=(uintptr_t)&x;
  print_stack_info();

  int *p1 = (int*) malloc(sizeof(int));
  int *p2 = &x;

  printf("&x : %x\n",&x);
  printf("p1 : %x\n",p1);
  printf("p2 : %x\n",p2);

  printf("&x:%d p1:%d p2:%d\n", is_stack(&x),is_stack(p1), is_stack(p2));

  f(&x,p1,p2);

  return 0;
}
