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

// New structures
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

typedef std::map<uint64_t, struct MetaData> MetaMap;
MetaMap TrackingDB;

// end

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

struct VoidStr new_overload(void* base, uint64_t size) {
    
    struct VoidStr temp;
    temp.ptr = base;
    temp.addr = reinterpret_cast<uint64_t>(&temp.ptr);
    // fill up the entry 
    create_entry(temp.addr, reinterpret_cast<uint64_t>(base), size);    

    return temp;
}

void Assign_Overload(struct VoidStr &lhs, struct VoidStr rhs) {
    
    // Update pointer data    
    lhs.ptr = rhs.ptr;
    // Update/Initialize metadata at lhs.addr using the addr
    // from rhs.addr. 
    // IMPORTANT: We are *NOT* using the same address as rhs.addr
    // but creating a new entry for the lhs.addr in the TrackingDB
    // global structure
    create_entry(lhs.addr, rhs.addr);
}

// Passing back the same entry with an offset pointer. Since we don't
// manipulate metadata here, we don't need to create a local copy 
// of the metadata
struct VoidStr ArrayRefOp(struct VoidStr input, unsigned int index) {
    input.ptr += index;
    return input;
}

// FIXME: Think this should be okay. Since the input is of struct
// type, returning it as is might be okay -- not sure
struct VoidStr AddressOp(struct VoidStr input) {
    return input;
}

struct VoidStr malloc_overload(unsigned int size) {
    struct VoidStr temp;
    temp.ptr = (void*)malloc(size);
    temp.addr = reinterpret_cast<uint64_t>(&temp.ptr);
    // Arguments:
    // 1. index into TrackingDB
    // 2. pointer to the buffer
    // 3. size of the buffer
    create_entry(temp.addr, reinterpret_cast<uint64_t>(temp.ptr), size);
    
    return temp;
}

// Again: Since no changes are made to the metadata, we don't have to necessarily
// create a local copy of the metadata entry
struct VoidStr IncrementOp(struct VoidStr &input, uint64_t size) {
    input.ptr += size;
    return input;
}

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
    
        struct VoidStr temp_str;
        // Original code
        //class Base* temp = new class Base[size];
        Assign_Overload(temp_str, new_overload(new class Base[size], size*sizeof(class Base))); 
        
        // No deref, so no need to check
        // In this case, a void struct needs to be created on the fly since
        // this variable is a class member variable, and not introduced through
        // the arguments
        // The entry for this variable could've been created in the constructor or
        // not, so we go ahead and update the entry, or create one if it odesn't exist,
        // and copy over temp data
        struct VoidStr der2_base_ptr1_str = {(void*)der2_base_ptr1, &der2_base_ptr1};
        
        // FIXME: This is a tricky scenario since we need to create a VoidStr on the
        // rhs as well, and then assign it to the lhs.
        // Original code
        //der2_base_ptr1 = &temp[size-1]; 
        Assign_Overload(der2_base_ptr1_str, AddressOp(ArrayRefOp(temp_str, size-1)));

    }

    void set_two_pointers(unsigned int size) {

        struct VoidStr der2_ptr1_str = {(void*)der2_ptr1, &der2_ptr1};
        Assign_Overload(der2_ptr1_str, malloc_overload(size*sizeof(unsigned int)));
        //der2_ptr1 = (unsigned int*)malloc(size*sizeof(unsigned int));


        struct VoidStr der2_ptr2_str = {(void*)der2_ptr2, &der2_ptr2};
        Assign_Overload(der2_ptr2_str, IncrementOp(der2_ptr1_str, sizeof(unsigned int)));
        //der2_ptr2 = (float*)der2_ptr1++;

        set_base_ptr(size*2);

    }


};

struct node {
    class Base* base_ptr;
    unsigned int node_ctr;
    struct node* next;
    struct node* prev;
};

class Base fn2(struct VoidStr input1_str,
           struct VoidStr input2_str,
           struct VoidStr input3_str,
           struct VoidStr input4_str,
           class Base input5,
           struct VoidStr input6_str) {
    // Since there are no operations on the pointer inputs,
    // nothing to do

    return input5;

}

class Base fn2(int* input1, float* input2, class Derived1* input3, class Derived1* input4,
        class Base input5, class Base* input6) {

    return input5;
}

void* Deref_Overload(struct VoidStr input) {
    return input.ptr;
}

struct VoidStr fn1(struct VoidStr input1_str,
           char input4,
           struct VoidStr input2_str,
           struct VoidStr input3_str,
           class Base input5) {

    // input3_str contains the location pointed to by the pointer
    // and the index into the TrackingDB structure
    // If we were modifying the pointer's metadata in any way,
    // we would have to create a local copy for it
    // This check could be done in the Deref_Overload itself
    //check_deref(input3_str); 
    ((class Derived2*)Deref_Overload(input3_str))->set_two_pointers((int*)Deref_Overload(input1_str));
    
    // We could do the dynamic_casting of the return value -- but not 
    // necessary since we pass back struct variable anyways
    return input3_str;
}

class Base* fn1(int* input1,  char input4, float* input2, class Derived2* input3, class Base input5) {

    input3->set_two_pointers(*input1);

    return dynamic_cast<class Base*>(input3);
}

struct VoidStr insert_node(struct VoidStr ptr_str,
               struct VoidStr pos_str) {
    struct VoidStr new_node_str;
    Assign_Overload(new_node_str, new_overload(new struct node, sizeof(struct node)));
    
    // Need to initialize the struct for base_ptr
    // Second argument has an & because thats the address of this poitner variable
    struct VoidStr new_node_base_ptr_str = {(struct node*)Deref_Overload(new_node_str)->base_ptr, 
                        &(struct node*)Deref_Overload(new_node_str)->base_ptr};
    Assign_Overload(new_node_base_ptr_str, ptr_str);

    ((struct node*)Deref_Overload(new_node_str))->node_ctr = ++((struct node*)Deref_Overload(pos_str))->node_ctr;



    struct VoidStr new_node_next_str = {(struct node*)Deref_Overload(new_node_str)->next, 
                        &(struct node*)Deref_Overload(new_node_str)->next};

    // FIXME: Handle this case as a special case in Assign_Overload
    Assign_Overload(new_node_next_str, NULL);

    struct VoidStr new_node_prev_str = {(struct node*)Deref_Overload(new_node_str)->prev, 
                        &(struct node*)Deref_Overload(new_node_str)->prev};

    Assign_Overload(new_node_prev_pos, pos_str);

    struct VoidStr pos_next_str = {(struct node*)Deref_Overload(pos_str)->next, 
                        &(struct node*)Deref_Overload(pos_str)->next};

    Assign_Overload(pos_next_str, new_node_str);

    return new_node_str;
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

    struct VoidStr pos_prev_str = {Deref_Overload(pos)->prev,
                       &Deref_Overload(pos)->prev};
    
    struct VoidStr pos_next_str = {Deref_Overload(pos)->next,
                       &Deref_Overload(pos)->next};

    struct VoidStr prev_str;
    Assign_Overload(prev_str, pos_prev_str);
    
    struct VoidStr next_str;
    Assign_Overload(next_str, pos_next_str);

    struct VoidStr prev_next_str;
    Assign_Overload(prev_next_str, next_str);

    struct VoidStr next_prev_str;
    Assign_Overload(next_prev_str, prev_str);

    // Could convert base_ptr below into a struct by itself
    // and use the free_Overload below
    free((struct_node*)Deref_Overload(pos_str)->base_ptr);
    // frees the pointer in the struct. checks the 
    // TrackingDB entry for any errors -- pointer checking
    free_Overload(pos_str);

    return prev_str;
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

struct VoidStr set_bounds(uint64_t var_addr, uint64_t offset) {
    
    // NOTE: var_addr is also the lower bound
    uint64_t lower_bound = var_addr;

    struct VoidStr temp;
    temp.ptr = reinterpret_cast<void*>(var_addr);
    
    uint64_t temp_ptr_addr = reinterpret_cast<uint64_t>(temp.ptr);

    create_entry(temp_ptr_addr, lower_bound, lower_bound + offset);

    return temp;
}

int main() {
    
    struct VoidStr ptr_str;
    Assign_Overload(ptr_str, malloc_Overload(PTR_SIZE*sizeof(int)));
    //int *ptr = (int*)malloc(PTR_SIZE*sizeof(int));
    
    struct VoidStr ptr2_str;
    Assign_Overload(ptr2_str, malloc_Overload(PTR2_SIZE*sizeof(int)));
    //int *ptr2 = (int*)malloc(PTR2_SIZE*sizeof(int));

    
    // Nothing to do for objects here. Costructors and copy constructors
    // should handle them properly
    class Base base_obj;

    struct VoidStr base_ptr_str;

    // NOTE: Using malloc_Overload instead of new_Overload -- could work
    // for most cases
    Assign_Overload(base_ptr_str, malloc_Overload(sizeof(class Base)));
    //class Base* base_ptr = new class Base;

    ((class Base*)Deref_Overload(base_ptr_str))->print();
    //base_ptr->print();

    struct VoidStr base_ptr2_str;
    Assign_Overload(base_ptr2_str, base_ptr_str);
//    class Base* base_ptr2 = base_ptr;

    ((class Base*)Deref_Overload(base_ptr_str))->print();
//    base_ptr->print();


    class Derived1 der1_obj;
    // This is an interesting case
    // The bounds of this class object are available on the 
    // stack, and all we need to do is assign those bounds and 
    // whatever other metadata to the base_ptr. 
    // We simply use the set_bounds function
    // Since its a class object, its size is equal to the size of the class
    // Its lower bound is given by its stack address
    Assign_Overload(base_ptr_str, set_bounds(&der1_obj, sizeof(class Derived1)));
//    base_ptr = &der1_obj;

    (dynamic_cast<class Derived1*>((class Base*)Deref_Overload(base_ptr)))->print();
//    (dynamic_cast<class Derived1*>(base_ptr))->print();

    struct VoidStr der2_ptr_str;
    Assign_Overload(der2_ptr_str, malloc_Overload(sizeof(class Derived2)));
//    class Derived2* der2_ptr = new class Derived2;

    Assign_Overload(base_ptr_str, der2_ptr_str);
    base_ptr = der2_ptr;    

    (dynamic_cast<class Derived2*>((class Base*)Deref_Overload(base_ptr_str)))->print();
//    (dynamic_cast<class Derived2*>(base_ptr))->print();

    Assign_Overload(der2_ptr_str, fn1(ptr_str, 'a', ptr2_str, der2_ptr_str, base_obj));
//    der2_ptr = dynamic_cast<class Derived2*>(fn1(ptr, 'a', (float*)ptr2, der2_ptr, base_obj));

    struct VoidStr start_ptr_str;
    Assign_Overload(start_ptr_str, ptr_str);
//    int* start_ptr = ptr;

    struct VoidStr start_ptr2_str;
    Assign_Overload(start_ptr2_str, ptr2_str);
//    int* start_ptr2 = ptr2;

    // Crossing the boundary of ptr. The condition should
    // be less than, not less than or equal to
    // ptr[PTR_SIZE] is an out-of-bounds access
    for(int index = 0; index <= (PTR_SIZE + OUT_OF_BOUNDS_EXCESS); index++) {
        (*(int*)Deref_Overload(ptr_str)) = index;
        //*ptr = index;

        IncrementOp(ptr_str, sizeof(int));
        //ptr++;
    }


    // Resetting ptr to start_ptr, so that it points to the beginning
    // of the allocation
    Assign_Overload(ptr_str, start_ptr);
    //ptr = start_ptr;


    // Printing what we wrote above
    for(int index = 0; index <= (PTR_SIZE + OUT_OF_BOUNDS_EXCESS); index++) {
        printf("ptr[%d]=%d\n", index, *((int*)Deref_Overload(ptr));
        //printf("ptr[%d]=%d\n", index, *ptr);

        IncrementOp(ptr_str, sizeof(int));
        //ptr++;
    }
    
    printf("Linked list\n");

    printf("Head\n");
    // Linked list traversal

    struct VoidStr head_str;
    // NOTE: Replacing new with malloc
    Assign_Overload(head_str, malloc_Overload(sizeof(struct node)));
//    struct node* head = new struct node;

    struct VoidStr head_base_ptr_str;
    Assign_Overload(head_base_ptr_str, malloc_Overload(sizeof(class Derived1)));
//    head->base_ptr = (class Base*)malloc(sizeof(class Derived1));

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
