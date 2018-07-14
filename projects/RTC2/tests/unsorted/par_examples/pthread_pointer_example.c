#include <pthread.h>
//#include <cstdio>
//#include <cstdlib>
//#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define NUM_THREADS 3
#define COUNT 100

//typedef std::queue<int> IntQ;
// elements are "push"ed into the back, 
// and "pop"ped from the front

//IntQ SendQ;

typedef enum {FALSE = 0, TRUE = 1} BOOLEAN;

BOOLEAN finish = FALSE;
pthread_mutex_t lock;
unsigned int* array1;
unsigned int* array2;

// Node in the queue
typedef struct node {
    struct node* next;
    int val;
} node;

// Queue implementation
//typedef struct QueueImpl {

struct node* front_node;
struct node* back_node;

// pop from the front
void pop() {
    if(front_node == NULL) {
        printf("empty list. Nothing to be popped. Quitting\n");
        exit(1);
    }
    else if(front_node == back_node) {
        // single node in the list
        assert(front_node->next == NULL);
        free(front_node);
        front_node = NULL;
        back_node = NULL;
    }
    else {
        struct node* popped = front_node;
        front_node = front_node->next;
        free(popped);
    }
}

// push at the back
void push(int val) {
    if(back_node == NULL) {
        // empty list
        back_node = (struct node*)malloc(sizeof(struct node));
        back_node->next = NULL;
        back_node->val = val;
        // make the front node point to the same node
        assert(front_node == NULL);
        front_node = back_node;
    }
    else {
        struct node* new_node = (struct node*)malloc(sizeof(struct node));
        new_node->next = NULL;
        new_node->val = val;
        // make back_node->next point to the new node
        assert(back_node->next == NULL);
        back_node->next = new_node;
        // then back_node itself points to the new node
        back_node = new_node;
    }    
}

// return the front
struct node* front() {
    return front_node;    
}

// return the back
struct node* back() {
    return back_node;
}

// call me first
void init() {
    front_node = NULL;
    back_node = NULL;
}

// check if empty
int empty() {
    if(front_node == NULL) {
        assert(back_node == NULL);
        return 1;
    }
    else {
        return 0;
    }
}

// print queue
void printQ() {
    if(empty()) {
        printf("Queue is empty, nothing to print\n");
    }
    else {
        printf("Printing Q\n");
        // go from front to back
        struct node* temp = front_node;
        do {
                printf("%d,", temp->val);
                temp = temp->next;
        }while(temp != NULL);
    }
}


//} Queue;

//Queue SendQ;

#if 0
void *PrintNum(void *msg) {

    char* m;
    m = (char*)msg;
    printf("\t\t\t\tHello World from %s\n", m);

//    while(!finish || !SendQ.empty()) {
    while(!finish || !empty()) { 
//        if(!SendQ.empty()) {
        if(!empty()) {
            pthread_mutex_lock(&lock);
//            int val = SendQ.front();
            int val = front();
//            SendQ.pop();
            pop();
            pthread_mutex_unlock(&lock);
            printf("\t\t\t\tval: %d\n", val);
        }
    }

    pthread_exit(NULL);

    return NULL;
}


void *Enqueue(void* threadid) {
    long tid;
    tid = (long)threadid;
    for(int index = 0; index < COUNT; index++) {
        printf("(%d,%ld)\n", index, tid);
        pthread_mutex_lock(&lock);
//        SendQ.push(tid);
        push(tid);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}
#endif

void* Fibonacci(void* threadid) {
    
    printf("%s\n", (char*)threadid);

    for(int index = 0; index < COUNT; index++) {
        if(index == 0) {
            array1[index] = 1;
        }
        else if(index == 1) {
            array1[index] = array1[0];
        }
        else {
            array1[index] = array1[index - 1] + array1[index - 2];
        }
        printf("array1[%d] = %d\n", index, array1[index]);
    }

    printf("freeing array1\n");

    free(array1);

    return (void*)array1;
}

void* LinearScan(void* threadid) {
    
    printf("%s\n", (char*)threadid);

    for(int index = 0; index < COUNT; index++) {    
        if(index == 0) {
            array2[index] = array1[index];
        }
        else {
            array2[index] = array1[index] + array1[index - 1];
        }
        printf("\t\t\t\tarray2[%d] = %d\n", index, array2[index]);
    }

    printf("\t\t\t\tfreeing array2\n");

    free(array2);

    return (void*)array2;
}


int main(int argc, char* argv[]) {
    

    //SendQ.init();
    init();

    array1 = (unsigned int*)malloc(COUNT*sizeof(unsigned int));
    array2 = (unsigned int*)malloc(COUNT*sizeof(unsigned int));

    pthread_t fibo_th;
    pthread_t lins_th;
    pthread_create(&fibo_th, NULL, Fibonacci, (void*)"fibonacci thread");
    pthread_create(&lins_th, NULL, LinearScan, (void*)"linear scan thread");

    void* status;
    pthread_join(fibo_th, &status);
    pthread_join(lins_th, &status);
    finish = TRUE;

    #if 0
    pthread_t threads[NUM_THREADS];
    pthread_t thread_x;

    pthread_mutex_init(&lock, NULL);
    void* status;

    int rc;
    char* m = "thread_x";
    printf("In main: creating %s\n", m);
    rc = pthread_create(&thread_x, NULL, PrintNum, (void*)m);
    if(rc) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

    for(int index = 0; index < NUM_THREADS; index++) {
        rc = pthread_create(&threads[index], NULL, Enqueue, (void*)index);
        if(rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    
    for(int index = 0; index < NUM_THREADS; index++) {
        rc = pthread_join(threads[index], &status);
        if(rc) {
            printf("ERROR: unable to join %d\n", rc);
            exit(-1);
        }
        printf("thread %d joined\n", index);
    }


    #if 0    
    for(int index = 0; index < COUNT; index++) {
        SendQ.push(index);
    }
    #endif

    finish = true;
    #endif

    // Last thing for main to do - wait for other threads to finish
    pthread_exit(NULL);
    return 1;
}
