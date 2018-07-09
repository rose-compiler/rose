#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PTR_SIZE 100
#define PTR2_SIZE 10
#define PTR3_SIZE 10
#define OUT_OF_BOUNDS_EXCESS 1
#define LINKED_LIST_SIZE 10

struct User{
    float* user_ptr1;
};

class Base{
    unsigned int *ptr1;
    unsigned int *ptr2;
    unsigned int var1;
    char var2;
    float var3;
    float* ptr3;
    unsigned int *ptr4;
    struct User *str_ptr1;
    public:
    virtual void print() {
        int* print_ptr1;
        printf("This is base class\n");
    }
};

class Derived1 : public Base {
    unsigned int* der1_ptr1;
    float* der1_ptr2;
    public:
    void print() {
        int* print_ptr1;
        printf("This is Derived1 class\n");
    }
};

class Derived2 : public Derived1 {
    unsigned int* der2_ptr1;
    float* der2_ptr2;
    class Base* der2_base_ptr1;
    class Derived1 der2_der1_obj;
    public:
    void print() {
        int* print_ptr1;
        printf("This is Derived2 class\n");
    }

    void set_base_ptr(unsigned int size) {

        class Base* temp = new class Base[size];
        
        der2_base_ptr1 = &temp[size-1]; 
    }

    void set_two_pointers(unsigned int size) {

        der2_ptr1 = (unsigned int*)malloc(size*sizeof(unsigned int));

        der2_ptr2 = (float*)der2_ptr1++;

        set_base_ptr(size*2);

    }


};

struct node {
    class Base* base_ptr;
    unsigned int node_ctr;
    struct node* next;
    struct node* prev;
};

class Base fn2(int* input1, float* input2, class Derived1* input3, class Derived1* input4,
        class Base input5, class Base* input6) {

    return input5;
}

class Base* fn1(int* input1,  char input4, float* input2, class Derived2* input3, class Base input5) {

    input3->set_two_pointers(*input1);

    return dynamic_cast<class Base*>(input3);
}

struct node* insert_node(class Base* ptr, struct node* pos) {
    

    struct node* new_node = new struct node;
    new_node->base_ptr = ptr;
    new_node->node_ctr = ++pos->node_ctr;
    new_node->next = NULL;
    new_node->prev = pos;

    pos->next = new_node;

    return new_node;

}

struct node* delete_node(struct node* pos) {

    struct node* prev = pos->prev;
    struct node* next = pos->next;

    prev->next = next;
    next->prev = prev;

    free(pos->base_ptr);

    free(pos);
    
    return prev;

}

int main() {
    int *ptr = (int*)malloc(PTR_SIZE*sizeof(int));
    int *ptr2 = (int*)malloc(PTR2_SIZE*sizeof(int));


    class Base base_obj;
    class Base* base_ptr = new class Base;
    base_ptr->print();
    class Base* base_ptr2 = base_ptr;
    base_ptr->print();

    class Derived1 der1_obj;
    base_ptr = &der1_obj;
    (dynamic_cast<class Derived1*>(base_ptr))->print();

    class Derived2* der2_ptr = new class Derived2;
    base_ptr = der2_ptr;    
    (dynamic_cast<class Derived2*>(base_ptr))->print();

    der2_ptr = dynamic_cast<class Derived2*>(fn1(ptr, 'a', (float*)ptr2, der2_ptr, base_obj));


    int* start_ptr = ptr;
    int* start_ptr2 = ptr2;

    // Crossing the boundary of ptr. The condition should
    // be less than, not less than or equal to
    // ptr[PTR_SIZE] is an out-of-bounds access
    for(int index = 0; index <= (PTR_SIZE + OUT_OF_BOUNDS_EXCESS); index++) {
        *ptr = index;
        ptr++;
    }


    // Resetting ptr to start_ptr, so that it points to the beginning
    // of the allocation
    ptr = start_ptr;
    // Printing what we wrote above
    for(int index = 0; index <= (PTR_SIZE + OUT_OF_BOUNDS_EXCESS); index++) {
        printf("ptr[%d]=%d\n", index, *ptr);
        ptr++;
    }
    
    printf("Linked list\n");

    printf("Head\n");
    // Linked list traversal
    struct node* head = new struct node;
    head->base_ptr = (class Base*)malloc(sizeof(class Derived1));
    head->node_ctr = 0;
    head->next = NULL;
    head->prev = NULL;

    printf("Insert Node\n");
    struct node* new_node = insert_node((class Base*)malloc(sizeof(class Derived2)), head);

    struct node* prev_node = new_node;


    printf("Insert Node Loop\n");
    // Create a linked list of size LINKED_LIST_SIZE -- already has two nodes -- so total size
    // would be LINKED_LIST_SIZE + 2
    for(unsigned int index = 0; index < LINKED_LIST_SIZE; index++) {
        
        class Base* temp = (class Base*)malloc(sizeof(class Derived1));
        printf("sizeof(temp): %u\n", sizeof(*temp));

        new_node = insert_node(temp, prev_node);
        #if 0
        if(new_node->base_ptr != NULL) {
            printf("Not NULL: %u\n", new_node->base_ptr);
            new_node->base_ptr->print();
        }
        new_node->base_ptr->print();
        #endif
        prev_node = new_node;

    }
    
    
    // prev_node now points to the last node
    // lets delete from one before the last node
    //struct node* to_delete = prev_node->prev;
    prev_node = prev_node->prev;

    printf("Delete Node Loop\n");
    for(unsigned int index = 0; index < LINKED_LIST_SIZE; index++) {
        #if 0
        dynamic_cast<class Base*>(prev_node->base_ptr)->print();
        #endif
        prev_node = delete_node(prev_node);    

    }

    printf("Asserting\n");
    // Should be left with two nodes
    // Verifying that
    assert(head != NULL);
    assert(head->next != NULL);
    assert(head->next->next == NULL);
    printf("Prints\n");
    printf("head: ctr: %d\n", head->node_ctr);
    printf("next: ctr: %d\n", head->next->node_ctr);


    return 1;
}
