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


int* fn1(int *temp, int lock) {
    insertIntoSLK();
    
    free(temp);
    remove_lock(lock);

    removeFromSLK();

    return temp;
}

int main() {

    execAtFirst();

    int* temp = (int*)malloc(sizeof(int));
    IntPair lock_key = insert_lock();

    *temp = 3;

    temp = fn1(temp, lock_key.first);

    free(temp);

    *temp = 2;

    execAtLast();

    return 1;
}

#if 1
#ifdef __cplusplus
}
#endif
#endif

