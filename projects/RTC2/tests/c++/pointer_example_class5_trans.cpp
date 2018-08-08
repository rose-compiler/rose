#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <map>

#define PTR_SIZE 100
#define PTR2_SIZE 10
#define PTR3_SIZE 10
#define OUT_OF_BOUNDS_EXCESS 1
#define LINKED_LIST_SIZE 10

struct VoidStr {
    void* ptr;
    uint64_t addr;
};

struct MetaData {
    void* L;
    void* H;
    void* lock_loc;
    uint64_t key;
};

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



typedef std::map<uint64_t, struct MetaData> MetaMap; 

MetaMap TrackingDB;

#if 0
void create_entry(void** obj, uint64_t addr) {
    uint64_t stack_addr = reinterpret_cast<uint64_t>(*obj);
    struct MetaData caller_meta = TrackingDB[addr];
    // Entering same data at new place
    TrackingDB[stack_addr] = caller_meta;
}
#endif

void create_entry(uint64_t destination, uint64_t source) {
}

void create_entry(uint64_t dest, uint64_t base, uint64_t offset) {
}

void check_deref(uint64_t index, uint64_t ptr_val) {
}
    

class Base fn2(struct VoidStr input1_str, 
               struct VoidStr input2_str,
               struct VoidStr input3_str,
               struct VoidStr input4_str,
               class Base input5,
               struct VoidStr input6_str) {
    
    int* input1 = (int*)input1_str.ptr;
    // creates an entry in the table for this object
    uint64_t input1_addr = reinterpret_cast<uint64_t>(&input1);
    create_entry(input1_addr, input1_str.addr);

    
    float* input2 = (float*)input2_str.ptr;
    // creates an entry in the table for this object
    uint64_t input2_addr = reinterpret_cast<uint64_t>(&input2);
    create_entry(input2_addr, input2_str.addr);


    class Derived1* input3 = (class Derived1*)input3_str.ptr;
    // creates an entry in the table for this object
    uint64_t input3_addr = reinterpret_cast<uint64_t>(&input3);
    create_entry(input3_addr, input3_str.addr);

    class Derived1* input4 = (class Derived1*)input4_str.ptr;
    // creates an entry in the table for this object
    uint64_t input4_addr = reinterpret_cast<uint64_t>(&input4);
    create_entry(input4_addr, input4_str.addr);

    

    // NOTHING TO DO FOR class Base input5 -- the copy constructor
    // should be modified to copy over the pointer data
    // For all the members within classes, the copy constructor
    // would be doing this work. This is necessary when an object 
    // to a class is passed by value


    class Base* input6 = (class Base*)input6_str.ptr;
    // creates an entry in the table for this object
    uint64_t input6_addr = reinterpret_cast<uint64_t>(&input6);
    create_entry(input6_addr, input6_str.addr);

    // When returning objects, we make no change -- the copy
    // constructor will copy over the necessary data
    return input5;
}

class Base fn2(int* input1, 
               float* input2, 
               class Derived1* input3, 
               class Derived1* input4,
               class Base input5, 
               class Base* input6) {

    return input5;
}

struct VoidStr fn1(struct VoidStr input1_str,
                   char input4,
                   struct VoidStr input2_str,
                   struct VoidStr input3_str,
                   class Base input5) {

    int* input1 = (int*)input1_str.ptr;
    // creates an entry in the table for this object
    uint64_t input1_addr = reinterpret_cast<uint64_t>(&input1);
    create_entry(input1_addr, input1_str.addr);

    
    float* input2 = (float*)input2_str.ptr;
    // creates an entry in the table for this object
    uint64_t input2_addr = reinterpret_cast<uint64_t>(&input2);
    create_entry(input2_addr, input2_str.addr);

    class Derived2* input3 = (class Derived2*)input3_str.ptr;
    // Remember, we always use the address of the variable
    // to look up the global data structure 
    uint64_t input3_addr = reinterpret_cast<uint64_t>(&input3);
    // creates an entry in the table for this object
    create_entry(input3_addr, input3_str.addr);

    // check_deref takes two arguments
    // - the index into the entry in the global table
    // - the current address pointed to by the pointer
    // NOTE::: NO & OPERATOR FOR THE _pointed_to
    uint64_t input3_addr_pointed_to = reinterpret_cast<uint64_t>(input3);
    check_deref(input3_addr, input3_addr_pointed_to);
    // check_deref takes two arguments
    // - the index into the entry in the global table
    // - the current address pointed to by the pointer
    uint64_t input1_addr_pointed_to = reinterpret_cast<uint64_t>(input1);
    check_deref(input1_addr, input1_addr_pointed_to);
    // Actual code
    input3->set_two_pointers(*input1);

    // When returning a pointer, create a void struct to hold the info
    struct VoidStr ret_str = {(void*)(dynamic_cast<class Base*>(input3)), input3_addr};
    return ret_str;
}



class Base* fn1(int* input1,  char input4, float* input2, class Derived2* input3, class Base input5) {

    input3->set_two_pointers(*input1);

    return dynamic_cast<class Base*>(input3);
}


struct VoidStr insert_node(struct VoidStr ptr_str, struct VoidStr pos_str) {

// Create a copy of the arguments and use this copy instead for all the work below

    class Base* ptr = (class Base*)ptr_str.ptr;
    struct node* pos = (struct node*)pos_str.ptr;

    // This specifies that these two pointers share their control metadata with
    // the pointers residing at addr
    // This mapping does the work of initializing the correct set of control metadata
    // for these pointers
    uint64_t ptr_addr = reinterpret_cast<uint64_t>(&ptr);
    create_entry(ptr_addr, ptr_str.addr);

    uint64_t pos_addr = reinterpret_cast<uint64_t>(&pos);
    create_entry(pos_addr, pos_str.addr);

    struct node* new_node = new struct node;
    uint64_t new_node_addr = reinterpret_cast<uint64_t>(&new_node);
    uint64_t alloc_base = reinterpret_cast<uint64_t>(&new_node);
//    uint64_t alloc_offset = reinterpret_cast<uint64_t>(sizeof(struct node));
    uint64_t alloc_offset = sizeof(struct node);
    create_entry(new_node_addr, alloc_base, alloc_offset); 

    // check_deref takes two arguments
    // - the index into the entry in the global table
    // - the current address pointed to by the pointer
    uint64_t new_node_addr_pointed_to = reinterpret_cast<uint64_t>(new_node);
    check_deref(new_node_addr, new_node_addr_pointed_to);
    new_node->base_ptr = ptr;
    uint64_t base_ptr_addr = reinterpret_cast<uint64_t>(&new_node->base_ptr);

    create_entry(base_ptr_addr, ptr_addr);

    // Since we checked for deref already -- and no change has happened --
    // we could skip the deref for new_node here. But we should check
    // for pos
    // check_deref takes two arguments
    // - the index into the entry in the global table
    // - the current address pointed to by the pointer
    // FIXME: This pointed_to could be done in the library function as well
    // that might be much simpler 
    uint64_t pos_addr_pointed_to = reinterpret_cast<uint64_t>(pos);
    check_deref(pos_addr, pos_addr_pointed_to);
    new_node->node_ctr = ++pos->node_ctr;

    // We already checked new_node -- could do it again here. No issues.
    new_node->next = NULL;
    uint64_t next_addr = reinterpret_cast<uint64_t>(&new_node->next);
    // create_entry comes in two flavours
    // 1. two uint64_t arguments -- the first one is the destination entry
    // the second is the source entry. Copy from the source to the destination
    // in the global structure
    // Source could be NULL
    // lock data is carried over
    // 2. three uint64_t arguments -- this happens in the case of a malloc call
    // to initialize a pointer -- the second and (second + third) will be used
    // to initialize the entry given by the first argument in the global structure
    // Corresponding lock is determined by using the second argument
    create_entry(new_node_addr, NULL);


    new_node->prev = pos;
    uint64_t prev_addr = reinterpret_cast<uint64_t>(&new_node->prev);
    create_entry(prev_addr, pos_addr);
    
    // Update pos_addr_pointed_to
    pos_addr_pointed_to = reinterpret_cast<uint64_t>(pos);
    check_deref(pos_addr, pos_addr_pointed_to); 

    // Create void str to pass back the value
    struct VoidStr ret_str = {(void*)(new_node), new_node_addr};
    return ret_str;
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

struct VoidStr delete_node(struct VoidStr pos_str) {

    struct node* pos = (struct node*)pos_str.ptr;

    uint64_t pos_addr = reinterpret_cast<uint64_t>(&pos);
    uint64_t pos_addr_pointed_to = reinterpret_cast<uint64_t>(pos);
    
    check_deref(pos_addr, pos_addr_pointed_to);    
    struct node* prev = pos->prev;
    
    uint64_t prev_addr = reinterpret_cast<uint64_t>(&prev);
    uint64_t pos_prev_addr = reinterpret_cast<uint64_t>(&pos->prev);
    create_entry(prev_addr, pos_prev_addr);

    struct node* next = pos->next;
    uint64_t next_addr = reinterpret_cast<uint64_t>(&next);
    uint64_t pos_next_addr = reinterpret_cast<uint64_t>(&pos->next);
    create_entry(next_addr, pos_next_addr);


    uint64_t prev_addr_pointed_to = reinterpret_cast<uint64_t>(prev);
    check_deref(prev_addr, prev_addr_pointed_to);
    prev->next = next;
    uint64_t prev_next_addr = reinterpret_cast<uint64_t>(&prev->next);
    create_entry(prev_next_addr, next_addr);

    uint64_t next_addr_pointed_to = reinterpret_cast<uint64_t>(&next);
    check_deref(next_addr, next_addr_pointed_to);
    next->prev = prev;

    uint64_t pos_base_ptr_addr = reinterpret_cast<uint64_t>(&pos->base_ptr);
    uint64_t pos_base_ptr_addr_pointed_to = reinterpret_cast<uint64_t>(pos->base_ptr);
    check_deref(pos_base_ptr_addr, pos_base_ptr_addr_pointed_to);
    
    // Update pos_addr_pointed_to since pos may have been reassigned/changed
    pos_addr_pointed_to = reinterpret_cast<uint64_t>(pos);
    check_deref(pos_addr, pos_addr_pointed_to);

    

    // Create struct for return
    struct VoidStr ret_Str = {(void*)prev, prev_addr};
    return ret_Str; 

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

struct node* fn3(unsigned int size) {
    return (struct node*)malloc(size*sizeof(struct node));
};

int main() {

    
    int *ptr = (int*)malloc(PTR_SIZE*sizeof(int));
    uint64_t ptr_addr = reinterpret_cast<uint64_t>(&ptr_addr);
    uint64_t alloc_base = reinterpret_cast<uint64_t>(ptr);
//    uint64_t alloc_offset = reinterpret_cast<uint64_t>(PTR_SIZE*sizeof(int));
    uint64_t alloc_offset = PTR_SIZE*sizeof(int);
    create_entry(ptr_addr, alloc_base, alloc_offset);

    int *ptr2 = (int*)malloc(PTR2_SIZE*sizeof(int));
    uint64_t ptr2_addr = reinterpret_cast<uint64_t>(&ptr2_addr);
    uint64_t alloc2_base = reinterpret_cast<uint64_t>(ptr2);
//    uint64_t alloc2_offset = reinterpret_cast<uint64_t>(PTR2_SIZE*sizeof(int));
    uint64_t alloc2_offset = PTR2_SIZE*sizeof(int);
    create_entry(ptr2_addr, alloc2_base, alloc2_offset);

    class Base base_obj;

    class Base* base_ptr = new class Base;
    uint64_t base_ptr_addr = reinterpret_cast<uint64_t>(&base_ptr_addr);
    uint64_t base_ptr_alloc_base = reinterpret_cast<uint64_t>(base_ptr);
//    uint64_t base_ptr_alloc_offset = reinterpret_cast<uint64_t>(sizeof(class Base));
    uint64_t base_ptr_alloc_offset = sizeof(class Base);
    create_entry(base_ptr_addr, base_ptr_alloc_base, base_ptr_alloc_offset);

    uint64_t base_ptr_addr_pointed_to = reinterpret_cast<uint64_t>(base_ptr);
    check_deref(base_ptr_addr, base_ptr_addr_pointed_to);
    base_ptr->print();


    class Base* base_ptr2 = base_ptr;
    uint64_t base_ptr2_addr = reinterpret_cast<uint64_t>(&base_ptr2);
    uint64_t base_ptr2_alloc_base = reinterpret_cast<uint64_t>(base_ptr2);
//    uint64_t base_ptr2_alloc_offset = reinterpret_cast<uint64_t>(sizeof(class Base));
    uint64_t base_ptr2_alloc_offset = sizeof(class Base);
    create_entry(base_ptr2_addr, base_ptr2_alloc_base, base_ptr2_alloc_offset);


    base_ptr_addr_pointed_to = reinterpret_cast<uint64_t>(base_ptr);
    check_deref(base_ptr_addr, base_ptr_addr_pointed_to);
    base_ptr->print();


    class Derived1 der1_obj;
    base_ptr = &der1_obj;
    base_ptr_alloc_base = reinterpret_cast<uint64_t>(&der1_obj);
//    base_ptr_alloc_offset = reinterpret_cast<uint64_t>(sizeof(class Derived1));
    base_ptr_alloc_offset = sizeof(class Derived1);
    create_entry(base_ptr_addr, base_ptr_alloc_base, base_ptr_alloc_offset);

    // FIXME: What do we do here? Simply do a deref check?
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

    struct node* another_node = fn3(PTR_SIZE);
    another_node->node_ctr = ++head->next->node_ctr;
    printf("another: ctr: %d\n", another_node->node_ctr);

    return 1;
}
