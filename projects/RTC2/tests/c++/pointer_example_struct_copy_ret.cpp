#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <map>
#include <stack>
#include <list>

#if 1
#ifdef __cplusplus
extern "C" {
#endif
#endif


typedef std::pair<uint64_t, uint64_t> IntPair;

IntPair insertIntoSLK();
void removeFromSLK();
IntPair insert_lock();
void remove_lock(uint64_t lock_index);

void execAtFirst();
void execAtLast();

struct node {
    struct node* next;
    struct node* prev;
    int val;
};

struct node fn3(struct node copy_this);
struct node fn2(struct node copy_this);
int* fn1(int* temp, struct node copy_this);

struct node fn3(struct node copy_this) {
    printf("fn3\n");
    printf("copy_this: next_val: %u, prev_val: %u, val: %u\n", copy_this.next->val,
            copy_this.prev->val, copy_this.val);

    return copy_this;
}

struct node fn2(struct node copy_this) {
    printf("fn2\n");
    printf("copy_this: next_val: %u, prev_val: %u, val: %u\n", copy_this.next->val,
            copy_this.prev->val, copy_this.val);

    fn3(copy_this);
    return copy_this;
}

//int* fn1(int *temp, int lock, struct node copy_this) {
//    insertIntoSLK();
int* fn1(int *temp, struct node copy_this) {
    
    insertIntoSLK();

    printf("copy_this: next_val: %u, prev_val: %u, val: %u\n", copy_this.next->val,
            copy_this.prev->val, copy_this.val);

    free(temp);
//    remove_lock(lock);
    
    fn2(copy_this);

    removeFromSLK();

    return temp;
}

int main() {

    execAtFirst();

    int* temp = (int*)malloc(sizeof(int));
//    IntPair lock_key = insert_lock();


    struct node head;
    head.val = 1;

    struct node next;
    next.val = 2;
    next.prev = &head;

    struct node third;
    third.val = 3;
    third.prev = &next;
    
    next.next = &third;

    *temp = 3;

//    temp = fn1(temp, lock_key.first, next);
    temp = fn1(temp, next);

    #if 0
    free(temp);

    *temp = 2;
    #endif

    execAtLast();

    return 1;
}

#if 1
#ifdef __cplusplus
}
#endif
#endif

