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
#define ARRAY_SIZE 10


// New structures
struct VoidStr {
    void* ptr;
    uint64_t addr;
};

struct MetaData {
    uint64_t L;
    uint64_t H;
    uint64_t lock_loc;
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

    virtual void set_var1(unsigned int val) {
        var1 = val;
    }

    unsigned int get_var1() {
        return var1;
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

void create_entry(uint64_t addr, uint64_t lower, uint64_t offset) {
    printf("create_entry: addr: %lu, lower: %lu, offset: %lu\n", addr, lower, offset);
    struct MetaData* mt = &TrackingDB[addr];
    mt->L = lower;
    mt->H = lower+offset;
    // Have to fill in lock_loc and key stuff here
}

void create_entry(uint64_t dest, uint64_t src) {
    printf("create_entry: dest: %lu, src: %lu\n", dest, src);
    MetaMap::iterator iter = TrackingDB.find(src);
    assert(iter != TrackingDB.end());

    struct MetaData str = TrackingDB[src];
    TrackingDB[dest] = str;
}

struct VoidStr new_overload(void* base, uint64_t size) {
    printf("new_overload: base: %lu, size: %lu\n", reinterpret_cast<uint64_t>(base), size);    
    struct VoidStr temp;
    temp.ptr = base;
    temp.addr = reinterpret_cast<uint64_t>(&temp.ptr);
    // fill up the entry 
    create_entry(temp.addr, reinterpret_cast<uint64_t>(base), size);    

    return temp;
}

void Assign_Overload(struct VoidStr &lhs, struct VoidStr rhs) {
    printf("Assign_Overload(lhs,rhs)\n");    
    // Update pointer data    
    lhs.ptr = rhs.ptr;
    // Update/Initialize metadata at lhs.addr using the addr
    // from rhs.addr. 
    // IMPORTANT: We are *NOT* using the same address as rhs.addr
    // but creating a new entry for the lhs.addr in the TrackingDB
    // global structure
    create_entry(lhs.addr, rhs.addr);
}

void Assign_Overload(struct VoidStr &lhs, long int null_val) {
    printf("Assign_Overload(lhs, NULL)\n");    
    // Should be the NULL value case
    assert(null_val == NULL);
    
    // FIXME: 
    // Lets allow the case where the lhs hasn't been allocated
    // to a location yet. Technically, the constructor
    // should allocate some space, but lets skip that for now
    
    MetaMap::iterator iter = TrackingDB.find(lhs.addr);
    #if 0
    // Entry should be valid
    assert(iter != TrackingDB.end());
    #endif

    // No metadata to put in. So, we zero out everything.
    // This should allocate an entry, if it doesn't already exist    
    struct MetaData* mt = &TrackingDB[lhs.addr];
    mt->L = 0;
    mt->H = 0;
    // lock_loc and key would also be set to zero
    mt->lock_loc = 0;
    mt->key = 0;
}

// Passing back the same entry with an offset pointer. Since we don't
// manipulate metadata here, we don't need to create a local copy 
// of the metadata
struct VoidStr ArrayRefOp(struct VoidStr input, unsigned int index, unsigned int factor) {
    printf("ArrayRefOp: input, index: %lu, factor: %lu\n", index, factor);    
    // factor translates the size of unit offset in the original variable, into
    // the number of offsets in char. So, for example, if an int uses 4 bytes, and
    // here, lets assume char is 1byte.
    // Therefore, the factor is 4.
    char* curr = (char*)input.ptr;
    input.ptr = (void*)(curr + index*factor);
    return input;
}

// FIXME: Think this should be okay. Since the input is of struct
// type, returning it as is might be okay -- not sure
struct VoidStr AddressOp(struct VoidStr input) {
    printf("AddressOp: input\n");    
    return input;
}

struct VoidStr malloc_overload(unsigned int size) {
    printf("malloc_overload: size: %lu\n", size);
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
struct VoidStr IncrementOp(struct VoidStr &input, uint64_t factor) {
    printf("IncrementOp: input, factor: %lu\n", factor);
    char* curr = (char*)input.ptr;
    input.ptr = (void*)(curr + factor);
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
        printf("set_base_ptr: size: %lu\n", size);
        struct VoidStr temp_str;
        
        Assign_Overload(temp_str, new_overload(new class Base[size], size*sizeof(class Base))); 
        // Original code
        //class Base* temp = new class Base[size];
        // Copy over the updated data
        class Base* temp = (class Base*)temp_str.ptr;

        // No deref, so no need to check
        // In this case, a void struct needs to be created on the fly since
        // this variable is a class member variable, and not introduced through
        // the arguments
        // The entry for this variable could've been created in the constructor or
        // not, so we go ahead and update the entry, or create one if it odesn't exist,
        // and copy over temp data
        struct VoidStr der2_base_ptr1_str = {(void*)der2_base_ptr1, reinterpret_cast<uint64_t>(&der2_base_ptr1)};
        
        // FIXME: This is a tricky scenario since we need to create a VoidStr on the
        // rhs as well, and then assign it to the lhs.
        // Assuming that the size of an element in temp array is equal to sizeof(class Base)
        Assign_Overload(der2_base_ptr1_str, AddressOp(ArrayRefOp(temp_str, size-1, sizeof(class Base)/sizeof(char))));
        // Original code    
        //der2_base_ptr1 = &temp[size-1]; 
        // Copy over the updated data
        der2_base_ptr1 = (class Derived2*)der2_base_ptr1_str.ptr;
    }

    void set_two_pointers(unsigned int size) {
        printf("set_two_pointers: size: %lu\n", size);

        struct VoidStr der2_ptr1_str = {(void*)der2_ptr1, reinterpret_cast<uint64_t>(&der2_ptr1)};
        Assign_Overload(der2_ptr1_str, malloc_overload(size*sizeof(unsigned int)));
        // Original code
        //der2_ptr1 = (unsigned int*)malloc(size*sizeof(unsigned int));
        // Copy over the updated data
        der2_ptr1 = (unsigned int*)der2_ptr1_str.ptr;

        struct VoidStr der2_ptr2_str = {(void*)der2_ptr2, reinterpret_cast<uint64_t>(&der2_ptr2)};
        Assign_Overload(der2_ptr2_str, IncrementOp(der2_ptr1_str, sizeof(unsigned int)/sizeof(char)));
        // Original code
        //der2_ptr2 = (float*)der2_ptr1++;
        // Copy over the updated data
        der2_ptr2 = (float*)der2_ptr2_str.ptr;

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

void* Deref_Overload(struct VoidStr input) {
    printf("Deref_Overload: input\n");
    return input.ptr;
}


struct VoidStr fn1(struct VoidStr input1_str,
           char input4,
           struct VoidStr input2_str,
           struct VoidStr input3_str,
           class Base input5) {
    printf("fn1: input1_str, input4, input2_str, input3_str, input5\n");
    class Derived2* input3 = (class Derived2*)input3_str.ptr;
    // Currently, input3_str contains the caller's pointer address. Need
    // to convert it to the current one
    uint64_t input3_addr = reinterpret_cast<uint64_t>(&input3);
    // This will copy over the info from input3_str.addr (the source pointer's address)
    // to an entry at input3_addr (the local pointer's addr)
    create_entry(input3_addr, input3_str.addr);
    // Now, use input3_str to use the new entry instead of the old one.
    // We could've created another struct variable, but we are simply reusing input3_str
    input3_str.addr = input3_addr;

    // No changes to input3_str, so we don't need to update
    ((class Derived2*)Deref_Overload(input3_str))->set_two_pointers(*(int*)Deref_Overload(input1_str));
    
    // We could do the dynamic_casting of the return value -- but not 
    // necessary since we pass back struct variable anyways
    return input3_str;
}


struct VoidStr insert_node(struct VoidStr ptr_str,
               struct VoidStr pos_str) {
    printf("fn2: ptr_str, pos_str\n");
    class Base* ptr = (class Base*)ptr_str.ptr;    
    uint64_t ptr_addr = reinterpret_cast<uint64_t>(&ptr);
    create_entry(ptr_addr, ptr_str.addr);
    ptr_str.addr = ptr_addr;

    struct node* pos = (struct node*)pos_str.ptr;    
    uint64_t pos_addr = reinterpret_cast<uint64_t>(&pos);
    create_entry(pos_addr, pos_str.addr);
    pos_str.addr = pos_addr;

    struct node* new_node;
    struct VoidStr new_node_str = {(void*)new_node, reinterpret_cast<uint64_t>(&new_node)};
    Assign_Overload(new_node_str, new_overload(new struct node, sizeof(struct node)));
    // Original code
    //struct node* new_node = new struct node;
    new_node = (struct node*)new_node_str.ptr;  

    // Need to initialize the struct for base_ptr
    // Second argument has an & because thats the address of this poitner variable
    struct VoidStr new_node_base_ptr_str = {(void*)(((struct node*)Deref_Overload(new_node_str))->base_ptr), 
                        reinterpret_cast<uint64_t>(&((struct node*)Deref_Overload(new_node_str))->base_ptr)};
    // No changes to new_node_str.ptr itself, so, no need to copy to the original new_node
    Assign_Overload(new_node_base_ptr_str, ptr_str);
    // Original code
    //new_node->base_ptr = ptr;
    // new_node->base_ptr was assigned values above. So, we update it with the 
    // correct values from its struct var.ptr;
    new_node->base_ptr = (class Base*)new_node_base_ptr_str.ptr;

    ((struct node*)Deref_Overload(new_node_str))->node_ctr = ++((struct node*)Deref_Overload(pos_str))->node_ctr;
    // Original code
    //new_node->node_ctr = ++pos->node_ctr;
    // no need to update new_node or pos, since it hasn't been change above

    // Technically, we could replace new_node_str here with new_node, but for uniformity
    // all uses of new_node are replaced with new_node_str, and if new_node_str.ptr is changed
    // then, we update new_node as well
    struct VoidStr new_node_next_str = {(void*)((struct node*)Deref_Overload(new_node_str))->next, 
                        reinterpret_cast<uint64_t>(&((struct node*)Deref_Overload(new_node_str))->next)};
    
    // FIXME: Handle this case as a special case in Assign_Overload
    Assign_Overload(new_node_next_str, NULL);
    // Original code
    //new_node->next = NULL;
    // Updating new_node->next with the updated value from the Assign_Overload
    // Assign_Overload lead to pointer updates... so, should update the original
    // pointer after an Assign_Overload
    new_node->next = (struct node*)new_node_next_str.ptr;

    struct VoidStr new_node_prev_str = {(void*)((struct node*)Deref_Overload(new_node_str))->prev, 
                        reinterpret_cast<uint64_t>(&((struct node*)Deref_Overload(new_node_str))->prev)};

    Assign_Overload(new_node_prev_str, pos_str);
    // Original Code
    //new_node->prev = pos;
    new_node->prev = (struct node*)new_node_prev_str.ptr;

    struct VoidStr pos_next_str = {(void*)((struct node*)Deref_Overload(pos_str))->next, 
                        reinterpret_cast<uint64_t>(&((struct node*)Deref_Overload(pos_str))->next)};

    Assign_Overload(pos_next_str, new_node_str);
    // Original Code
    //pos->next = new_node;
    // Always use the struct variable associated with the pointer to copy
    // the updated data over. This is a simple, fail-safe technique
    pos->next = (struct node*)pos_next_str.ptr;

    // The struct variable has the right values. Just pass it back    
    return new_node_str;

}

void free_Overload(struct VoidStr str) {
    printf("free_Overload: str\n");
    // Do some checks before freeing here

    // Free the buffer
    free(str.ptr);

    // Remove entry in TrackingDB
    TrackingDB.erase(str.addr);
};

struct VoidStr delete_node(struct VoidStr pos_str) {
    printf("delete_node: pos_str\n");
    struct node* pos = (struct node*)pos_str.ptr;
    uint64_t pos_addr = reinterpret_cast<uint64_t>(&pos);
    create_entry(pos_addr, pos_str.addr);
    pos_str.addr = pos_addr;

    // Since pos->prev and pos->next don't have struct
    // variables associated with them in this scope, create
    // them. Again, the reason for creating these variables is
    // to track the metadata through all the operations, and
    // ensure that the pointer is being checked against a valid
    // and correct metadata
        
    struct VoidStr pos_prev_str = {(void*)(((struct node*)Deref_Overload(pos_str))->prev),
                       reinterpret_cast<uint64_t>(&(((struct node*)Deref_Overload(pos_str))->prev))};

    struct node* prev;
    // Create struct variable for prev
    struct VoidStr prev_str = {(void*)prev, reinterpret_cast<uint64_t>(&prev)};
    Assign_Overload(prev_str, pos_prev_str);
    // Original code
    //struct node* prev = pos->prev;
    // Update prev with the new value
    prev = (struct node*)prev_str.ptr;
    
    struct VoidStr pos_next_str = {(void*)(((struct node*)Deref_Overload(pos_str))->next),
                       reinterpret_cast<uint64_t>(&((struct node*)Deref_Overload(pos_str))->next)};


    struct node* next;
    // Create struct variable for next
    struct VoidStr next_str = {(void*)next, reinterpret_cast<uint64_t>(&next)};
    Assign_Overload(next_str, pos_next_str);
    // Original code
    //struct node* next = pos->next;
    // Update next with the new value
    next = (struct node*)next_str.ptr;

    
    struct VoidStr prev_next_str = {(void*)((struct node*)Deref_Overload(prev_str))->next,
                        reinterpret_cast<uint64_t>(&(((struct node*)Deref_Overload(prev_str))->next))};
    Assign_Overload(prev_next_str, next_str);
    // Original Code
    //prev->next = next;
    // Update prev->next
    prev->next = (struct node*)prev_next_str.ptr;

    struct VoidStr next_prev_str = {(void*)((struct node*)Deref_Overload(next_str))->prev,
                        reinterpret_cast<uint64_t>(&(((struct node*)Deref_Overload(next_str))->prev))};
    Assign_Overload(next_prev_str, prev_str);
    // Original Code
    //next->prev = prev;
    // Update next->prev
    next->prev = (struct node*)next_prev_str.ptr;

    struct VoidStr pos_base_ptr_str = {(void*)((struct node*)Deref_Overload(pos_str))->base_ptr,
                        reinterpret_cast<uint64_t>(&((struct node*)Deref_Overload(pos_str))->base_ptr)};
    // Original Code
    //free(pos->base_ptr);
    free_Overload(pos_base_ptr_str);
    // FIXME: free doesn't change the pointer value itself right?
    // It only deallocates the memory underneath. 

    //free(pos);
    free_Overload(pos_str);
    
    //return prev;
    // As usual, prev_str has the updated values, just return it
    return prev_str;

}

void fn3(struct VoidStr array_str) {
    printf("fn3: array_str\n");
    // Create the local version of the variable
    struct node* array = (struct node*)array_str.ptr;
    uint64_t array_addr = reinterpret_cast<uint64_t>(&array);
    create_entry(array_addr, array_str.addr);
    array_str.addr = array_addr;

    for(unsigned int index = 0; index < ARRAY_SIZE; index++) {

        struct node* element;
        struct VoidStr element_str = {(void*)element, reinterpret_cast<uint64_t>(&element)};
        // We use array_str here for the work -- since we need to 
        // pass the metadata from that particular element of array_str
        // to element_str
        // FIXME: The metadata being passed here is the data associated
        // with array_str. This may or may not be correct in all cases.
        // Check with Peter
        // Assuming size of each element in node_array is equal to sizeof(struct node).
        // Should be correct
        Assign_Overload(element_str, AddressOp(ArrayRefOp(array_str, index, sizeof(struct node)/sizeof(char))));
        // Original Code
        // struct node* element = &array[index];
        // In this case, the variable element maintains data layout compatibility
        element = (struct node*)element_str.ptr;

        // If this element were to be passed to an external function, then
        // the output would still be correct
        
        // Create VoidStr variable for base_ptr
        struct VoidStr element_base_ptr_str = {(void*)((struct node*)Deref_Overload(element_str))->base_ptr,
                                            reinterpret_cast<uint64_t>(&((struct node*)Deref_Overload(element_str))->base_ptr)};
        // Nothing else to do here since no variable is created for base_ptr

        // Use element_str and base_ptr instead
        //printf("%d\n", element->base_ptr->get_var1());
        // FIXME: What we have lost here is that base_ptr belongs to element
        // and we are no longer going through two different pointers
        // Instead, we are going to the lowest level pointer directly
        // The necessary deref checks are done before this stage is reached
        // since element_str is checked
        // FIXME: The problem is that element might have been reassigned, in which
        // case element->base_ptr would also need to be updated... right?
        // The problem here is that any updates to element are not reflected here in
        // element_base_ptr_str
        // For that, we update the element_base_ptr_str just before the use!
        // In fact, we could do this simply using the original variables, since we 
        // keep them updated!
        element_base_ptr_str.ptr = element->base_ptr;
        element_base_ptr_str.addr = reinterpret_cast<uint64_t>(&element->base_ptr);
        // OR
        element_base_ptr_str.ptr = (void*)((struct node*)Deref_Overload(element_str))->base_ptr;
        element_base_ptr_str.addr = reinterpret_cast<uint64_t>(&((struct node*)Deref_Overload(element_str))->base_ptr);
        // This would be necessary for the second and higher level pointers, which
        // rely on the base pointers for their location
        printf("%d\n", ((class Base*)Deref_Overload(element_base_ptr_str))->get_var1());
    }
}

struct VoidStr set_bounds(uint64_t var_addr, uint64_t offset) {
    printf("set_bounds: var_addr: %lu, offset: %lu\n", var_addr, offset);
    // NOTE: var_addr is also the lower bound
    uint64_t lower_bound = var_addr;

    struct VoidStr temp;
    temp.ptr = reinterpret_cast<void*>(var_addr);
    
    uint64_t temp_ptr_addr = reinterpret_cast<uint64_t>(&temp.ptr);

    temp.addr = temp_ptr_addr;

    create_entry(temp_ptr_addr, lower_bound, offset);

    return temp;
}

void fn4_ext(struct node* head) {
    
    printf("fn4_ext: Printing linked list\n");
    unsigned int index = 0;
    struct node* start = head;
    while(start != NULL) {
        printf("%d: %d\n", ++index, start->node_ctr);
        start = start->next;
    };

    printf("Done with linked list\n");
}

void fn3_ext(struct node* node_array) {
    
    printf("fn3_ext: Printing array\n");
    for(unsigned int index = 0; index < LINKED_LIST_SIZE; index++) {
        printf("%d:%d\n", index, node_array[index].base_ptr->get_var1());
    }
    printf("Done with array\n");
}

int main() {
    printf("main\n");
    int *ptr;
    struct VoidStr ptr_str = {(void*)ptr, reinterpret_cast<uint64_t>(&ptr)};
    Assign_Overload(ptr_str, malloc_overload(PTR_SIZE*sizeof(int)));
    // Original Code
    //int *ptr = (int*)malloc(PTR_SIZE*sizeof(int));
    ptr = (int*)ptr_str.ptr;


    int *ptr2;
    struct VoidStr ptr2_str = {(void*)ptr2, reinterpret_cast<uint64_t>(&ptr2)};
    Assign_Overload(ptr2_str, malloc_overload(PTR2_SIZE*sizeof(int)));
    // Original Code
    //int *ptr2 = (int*)malloc(PTR2_SIZE*sizeof(int));
    ptr2 = (int*)ptr2_str.ptr;


    class Base base_obj;

    
    class Base* base_ptr;
    struct VoidStr base_ptr_str = {(void*)base_ptr, reinterpret_cast<uint64_t>(&base_ptr)};
    // NOTE: new shouldn't be replaced
    Assign_Overload(base_ptr_str, new_overload(new class Base, sizeof(class Base)));
    // Original code
    //class Base* base_ptr = new class Base;
    // Update pointer info
    base_ptr = (class Base*)base_ptr_str.ptr;

    
    //base_ptr->print();
    ((class Base*)Deref_Overload(base_ptr_str))->print();

    printf("Base Ptr print 1\n");

    class Base* base_ptr2;
    struct VoidStr base_ptr2_str = {(void*)base_ptr2, reinterpret_cast<uint64_t>(&base_ptr2)};
    Assign_Overload(base_ptr2_str, base_ptr_str);
    // Original code
    //class Base* base_ptr2 = base_ptr;
    // Update pointer info
    base_ptr2 = (class Base*)base_ptr2_str.ptr;


    //base_ptr->print();
    ((class Base*)Deref_Overload(base_ptr_str))->print();


    printf("Base Ptr print 2\n");

    class Derived1 der1_obj;
    // This is an interesting case
    // The bounds of this class object are available on the 
    // stack, and all we need to do is assign those bounds and 
    // whatever other metadata to the base_ptr. 
    // We simply use the set_bounds function
    // Since its a class object, its size is equal to the size of the class
    // Its lower bound is given by its stack address
    Assign_Overload(base_ptr_str, set_bounds(reinterpret_cast<uint64_t>(&der1_obj), sizeof(class Derived1)));
    // Original Code
    //base_ptr = &der1_obj;
    // Update pointer info
    base_ptr = (class Base*)base_ptr_str.ptr;
    printf("Base Ptr der1_obj assign\n");

    (dynamic_cast<class Derived1*>((class Base*)Deref_Overload(base_ptr_str)))->print();
    //(dynamic_cast<class Derived1*>(base_ptr))->print();

    printf("Base Ptr print 3\n");

    class Derived2* der2_ptr;
    struct VoidStr der2_ptr_str = {(void*)der2_ptr, reinterpret_cast<uint64_t>(&der2_ptr)};
    Assign_Overload(der2_ptr_str, new_overload(new class Derived2, sizeof(class Derived2)));
//    Assign_Overload(der2_ptr_str, malloc_overload(sizeof(class Derived2)));
    // Original Code
    //class Derived2* der2_ptr = new class Derived2;
    // Update pointer info
    der2_ptr = (class Derived2*)der2_ptr_str.ptr;

    printf("Derived2 ptr\n");
    
    Assign_Overload(base_ptr_str, der2_ptr_str);
    // Original Code
    //base_ptr = der2_ptr;    
    // Update pointer info
    base_ptr = (class Base*)base_ptr_str.ptr;

    (dynamic_cast<class Derived2*>((class Base*)Deref_Overload(base_ptr_str)))->print();
    //(dynamic_cast<class Derived2*>(base_ptr))->print();

    printf("Base_ptr->derived2 ptr print\n");

    // Removed dynamic casts... should be okay.
    Assign_Overload(der2_ptr_str, fn1(ptr_str, 'a', ptr2_str, der2_ptr_str, base_obj));
    // Original Code
    //der2_ptr = dynamic_cast<class Derived2*>(fn1(ptr, 'a', (float*)ptr2, der2_ptr, base_obj));
    // Update pointer info
    der2_ptr = (class Derived2*)der2_ptr_str.ptr;


    int* start_ptr;
    struct VoidStr start_ptr_str = {(void*)start_ptr, reinterpret_cast<uint64_t>(&start_ptr)};
    // This is where the metadata and the ptr value is carried over
    Assign_Overload(start_ptr_str, ptr_str);
    // Original Code
    //int* start_ptr = ptr;
    // Updating pointer info -- for the original ptr
    start_ptr = (int*)start_ptr_str.ptr;


    int* start_ptr2;
    struct VoidStr start_ptr2_str = {(void*)start_ptr2, reinterpret_cast<uint64_t>(&start_ptr2)};
    // This is where the metadata and the ptr value is carried over
    Assign_Overload(start_ptr2_str, ptr2_str);
    // Original Code
    //int* start_ptr2 = ptr2;
    // Updating pointer info -- for the original ptr
    start_ptr2 = (int*)start_ptr2_str.ptr;


    // Crossing the boundary of ptr. The condition should
    // be less than, not less than or equal to
    // ptr[PTR_SIZE] is an out-of-bounds access
    for(int index = 0; index <= (PTR_SIZE + OUT_OF_BOUNDS_EXCESS); index++) {
    
        *(int*)Deref_Overload(ptr_str) = index;
        // We don't need to update ptr here since the location that the pointer
        // points to hasn't changed
        //*ptr = index;

        IncrementOp(ptr_str, sizeof(int)/sizeof(char));
        // Here, pointer needs to be updated since the location pointed to
        // has changed
        // Original Code
        //ptr++;
        // Update pointer info
        ptr = (int*)ptr_str.ptr;

    }


    // Resetting ptr to start_ptr, so that it points to the beginning
    // of the allocation

    Assign_Overload(ptr_str, start_ptr_str);
    // Original Code
    //ptr = start_ptr;
    // Update pointer info
    // Remember, update the pointer from its associated structvar.ptr
    // Re-doing the expression might have side-effects and hence violate
    // program semantics
    ptr = (int*)ptr_str.ptr;
    


    // Printing what we wrote above
    for(int index = 0; index <= (PTR_SIZE + OUT_OF_BOUNDS_EXCESS); index++) {
        
        printf("ptr[%d]=%d\n", index, *((int*)Deref_Overload(ptr_str)));
        // the location that ptr points to hasn't changed, so no need
        // to update it. Basically, check if ptr is in the LHS, or has
        // a unary operator (like increment or decrement) and 
        // then update it, if necessary
        //printf("ptr[%d]=%d\n", index, *ptr);

        // This will increment the location pointed to by pointer, hence
        // the pointer info needs to be updated below.
        IncrementOp(ptr_str, sizeof(int)/sizeof(char));
        // Original Code
        //ptr++;
        // Update pointer info
        ptr = (int*)ptr_str.ptr;
    }
    
    printf("Linked list\n");

    printf("Head\n");
    // Linked list traversal
    struct node* head;
    struct VoidStr head_str = {(void*)head, reinterpret_cast<uint64_t>(&head)};
    // FIXME: new_overload isn't used properly here
    Assign_Overload(head_str, new_overload(new struct node, sizeof(struct node)));
    // Original code
    //struct node* head = new struct node;
    // Update pointer info
    head = (struct node*)head_str.ptr;

    // If we can't find a struct variable, associated with
    // the current pointer, we create one, and fill it up
    // This is how we should handle all var ref exps
    // and replace their uses with the struct var. 
    struct VoidStr head_base_ptr_str = {(void*)((struct node*)Deref_Overload(head_str))->base_ptr,
                                reinterpret_cast<uint64_t>(&(((struct node*)Deref_Overload(head_str))->base_ptr))};

    // FIXME: Check usage of malloc_overload here
    Assign_Overload(head_base_ptr_str, malloc_overload(sizeof(class Derived1)));
    // Original Code
    //head->base_ptr = (class Base*)malloc(sizeof(class Derived1));
    head->base_ptr = (class Base*)head_base_ptr_str.ptr;

    // node_ctr is not a pointer        
    head->node_ctr = 0;

    printf("Head next\n");

    struct VoidStr head_next_str = {(void*)((struct node*)Deref_Overload(head_str))->next,
                                reinterpret_cast<uint64_t>(&((struct node*)Deref_Overload(head_str))->next)};
    // FIXME: Fix Assign_Overload to handle this case
    Assign_Overload(head_next_str, NULL);
    // Original Code
    //head->next = NULL;
    // Update pointer info
    head->next = (struct node*)head_next_str.ptr;


    struct VoidStr head_prev_str = {(void*)((struct node*)Deref_Overload(head_str))->prev,
                                reinterpret_cast<uint64_t>(&((struct node*)Deref_Overload(head_str))->prev)};
    // FIXME: Fix Assign_Overload to handle this case
    Assign_Overload(head_prev_str, NULL);
    // Original Code
    //head->prev = NULL;
    // Update pointer info
    head->prev = (struct node*)head_prev_str.ptr;


    printf("Insert Node\n");


    struct node* new_node;
    struct VoidStr new_node_str = {(void*)new_node, reinterpret_cast<uint64_t>(&new_node)};
    Assign_Overload(new_node_str, insert_node(malloc_overload(sizeof(class Derived2)), head_str));
    // Original Code
    //struct node* new_node = insert_node((class Base*)malloc(sizeof(class Derived2)), head);
    // Update pointer info
    new_node = (struct node*)new_node_str.ptr;

    
    
    struct node* prev_node;
    struct VoidStr prev_node_str = {(void*)prev_node, reinterpret_cast<uint64_t>(&prev_node)};
    Assign_Overload(prev_node_str, new_node_str);
    // Original Code
    //struct node* prev_node = new_node;
    // Update pointer info
    prev_node = (struct node*)prev_node_str.ptr;

    printf("Insert Node Loop\n");
    // Create a linked list of size LINKED_LIST_SIZE -- already has two nodes -- so total size
    // would be LINKED_LIST_SIZE + 2
    for(unsigned int index = 0; index < LINKED_LIST_SIZE; index++) {
        

        class Base* temp;
        struct VoidStr temp_str = {(void*)temp, reinterpret_cast<uint64_t>(&temp)};
        Assign_Overload(temp_str, malloc_overload(sizeof(class Derived1)));
        // Original Code
        //class Base* temp = (class Base*)malloc(sizeof(class Derived1));
        // Update pointer info
        //temp = temp_str.ptr;

        //printf("sizeof(temp): %u\n", sizeof(*temp));
        printf("sizeof(temp): %u\n", sizeof(*(class Base*)Deref_Overload(temp_str)));

        Assign_Overload(new_node_str, insert_node(temp_str, prev_node_str));
        // Original Code
        //new_node = insert_node(temp, prev_node);
        // Update pointer info
        new_node = (struct node*)new_node_str.ptr;

        #if 0
        if(new_node->base_ptr != NULL) {
            printf("Not NULL: %u\n", new_node->base_ptr);
            new_node->base_ptr->print();
        }
        new_node->base_ptr->print();
        #endif
        
        Assign_Overload(prev_node_str, new_node_str);
        // Original Code
        //prev_node = new_node;
        // Update pointer info
        // NOTE: We shouldn't update the pointer using the same
        // RHS expression, as the original statement, since it might
        // have side-effects, and executing it twice might change program
        // semantics
        prev_node = (struct node*)prev_node_str.ptr;

    }
    
    
    // prev_node now points to the last node
    // lets delete from one before the last node
    //struct node* to_delete = prev_node->prev;

    // Since prev_node is up to date, we could use it here
    struct VoidStr prev_node_prev_str = {(void*)prev_node->prev, reinterpret_cast<uint64_t>(&prev_node->prev)}; 
    Assign_Overload(prev_node_str, prev_node_prev_str);
    // Original Code
    //prev_node = prev_node->prev;
    // Update pointer info
    // NOTE: This is a perfect case for why we shouldn't use the original 
    // expression .. doing prev_node = prev_node->prev would change the
    // program semantics, and possibly even case prev_node to be NULL when
    // it wasn't before!
    prev_node = (struct node*)prev_node_str.ptr;
    
    fn4_ext(head);

    printf("Delete Node Loop\n");
    for(unsigned int index = 0; index < LINKED_LIST_SIZE; index++) {
        #if 0
        dynamic_cast<class Base*>(prev_node->base_ptr)->print();
        #endif

        Assign_Overload(prev_node_str, delete_node(prev_node_str));
        // Original Code
        //prev_node = delete_node(prev_node);    
        // Update pointer info
        prev_node = (struct node*)prev_node_str.ptr;

    }

    printf("Asserting\n");
    // Should be left with two nodes
    // Verifying that
    // Original Code
    //assert(head != NULL);
    assert(Deref_Overload(head_str) != NULL);

    // We could use the head_str, and Deref_Overload on that as well
    // This might be more uniform usage, and probably easier to implement
    // head_next_str has already been declared. Update it.
    head_next_str.ptr = (void*)(head->next);
    head_next_str.addr = reinterpret_cast<uint64_t>(&head->next);
    // Original Code
    //assert(head->next != NULL);
    assert((struct node*)Deref_Overload(head_next_str) != NULL);

    // Update the head_next since its a second level pointer, before using
    // it below
    head_next_str.ptr = (void*)((struct node*)Deref_Overload(head_str))->next;
    head_next_str.addr = reinterpret_cast<uint64_t>(&((struct node*)Deref_Overload(head_str))->next);
    // No need to updated head pointer since it hasn't changed in this expr

    // NOTE: The critical thing here is that the value of head->next depends
    // on head's pointer value. Therefore, any change to head, which is reflected
    // in head_str, needs to be reflected in head_next_str. This doesn't happen
    // automatically since head_next_str was initialized at the first use, and head
    // could have changed after that.

    // Now that head_next_str is up-to-date, we can use it to find head_next_next
    struct VoidStr head_next_next_str = {(void*)((struct node*)Deref_Overload(head_next_str))->next,
                                    reinterpret_cast<uint64_t>(&((struct node*)Deref_Overload(head_next_str))->next)};
    
    // Original Code
    //assert(head->next->next == NULL);
    assert((struct node*)Deref_Overload(head_next_next_str) == NULL);

    printf("Prints\n");
    
    // node_ctr is a not a pointer, so no need to create a struct var for it
    // Original Code
    //printf("head: ctr: %d\n", head->node_ctr);
    printf("head: ctr: %d\n", ((struct node*)Deref_Overload(head_str))->node_ctr);


    // FIXME: We could optionally update head_next_str before using it here, to ensure
    // that any changes to head will be taken into account.
    // Original Code
    //printf("next: ctr: %d\n", head->next->node_ctr);
    printf("next: ctr: %d\n", ((struct node*)Deref_Overload(head_next_str))->node_ctr);

    #if 0
    // Array ref
    class Base** base_array = (class Base**)malloc(ARRAY_SIZE*sizeof(class Base*));
    printf("created base_array\n");

    for(unsigned int index = 0; index < ARRAY_SIZE; index++) {
        class Base* base_index = base_array[index];
        //base_index = (class Base*)malloc(sizeof(class Base));
        base_index = new class Base;
        assert(base_index);
        printf("setting var1\n");
        base_index->set_var1(index);
    }
    
    printf("allocated base indices\n");
    
    for(unsigned int index = 0; index < ARRAY_SIZE; index++) {
        class Base* base_index = base_array[index];
        printf("%d\n", base_index->get_var1());
    }
    #endif

    // malloc needs to be overloaded to find the metadata for this pointer.
    struct node* node_array;
    struct VoidStr node_array_str = {(void*)node_array, reinterpret_cast<uint64_t>(&node_array)};
    Assign_Overload(node_array_str, malloc_overload(ARRAY_SIZE*sizeof(struct node*)));
    // Original Code
    // struct node* node_array = (struct node*)malloc(ARRAY_SIZE*sizeof(struct node*));
    // Update pointer info
    node_array = (struct node*)node_array_str.ptr;

    printf("Setting node array\n");
    for(unsigned int index = 0; index < ARRAY_SIZE; index++) {
        
        struct node* element;    
        struct VoidStr element_str = {(void*)element, reinterpret_cast<uint64_t>(&element)};
        Assign_Overload(element_str, AddressOp(ArrayRefOp(node_array_str, index, (sizeof(struct node)/sizeof(char)))));
        // Original Code
        //struct node* element = &node_array[index];
        // Update pointer info
        element = (struct node*)element_str.ptr;


        
        struct VoidStr element_base_ptr_str = {(void*)element->base_ptr, reinterpret_cast<uint64_t>(&element->base_ptr)};
        // FIXME: not using new_overload properly here
        Assign_Overload(element_base_ptr_str, new_overload(new class Base, sizeof(class Base)));
        // Original Code
        //element->base_ptr = new class Base;
        // Update pointer info
        element->base_ptr = (class Base*)element_base_ptr_str.ptr;


        // We could update the element_base_ptr_str here, just to make sure
        // that its pointing to where element->base_ptr would've pointed to
        element_base_ptr_str.ptr = (void*)element->base_ptr;
        element_base_ptr_str.addr = reinterpret_cast<uint64_t>(&element->base_ptr);
        // Original Code    
        //element->base_ptr->set_var1(index);
        ((class Base*)Deref_Overload(element_base_ptr_str))->set_var1(index);
    }
    printf("Printing node array\n");

    fn3_ext(node_array);

    // Original Code    
    //fn3(node_array);    
    fn3(node_array_str);


    return 0;
}
