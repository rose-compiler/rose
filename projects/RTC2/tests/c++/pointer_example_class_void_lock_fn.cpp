#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PTR_SIZE 100
#define PTR2_SIZE 10
#define PTR3_SIZE 10
#define OUT_OF_BOUNDS_EXCESS 1

struct User{
    float* user_ptr1;
};

struct VoidStr{
    void **ptr2ptr;
    void *L;
    void *H;
    void *lock; // Should this be an array of pointers?
    uint64_t key; // This should also be an array, if the multiple locks are required per pointer
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
        printf("This is base class\n");
    }
};

class Derived1 : public Base {
    unsigned int* der1_ptr1;
    float* der1_ptr2;
    public:
    void print() {
        printf("This is Derived1 class\n");
    }
};

class Derived2 : public Derived1 {
    unsigned int* der2_ptr1;
    float* der2_ptr2;
    public:
    void print() {
        printf("This is Derived2 class\n");
    }
};

class Base* fn1(int* input1, char input4, float* input2, class Derived2* input3) {

    struct VoidStr input1_structed;
    Reassign(input1_structed, popStack());
    struct VoidStr input2_structed;
    Reassign(input2_structed, popStack());
    struct VoidStr input3_structed;
    Reassign(input3_structed, popStack());

    pushStack(input3_structed);

    return dynamic_cast<class Base*>((class Derived2*)Deref(input3_structed));        

}

// This creates a mapping between the pointer and
// its corresponding struct. This map could be maintained
// at a local scope to allow for automatic cleanup
create_entry(void* ptr, VoidStr ptr_structed) {
}

int main() {
    int *ptr = (int*)malloc(PTR_SIZE*sizeof(int));
    struct VoidStr ptr_structed;
    create_entry(ptr, ptr_structed);
    ptr_structed.ptr2ptr = &ptr;
    ptr_structed.L = (void*)ptr;
    ptr_structed.H = (void*)(ptr + PTR_SIZE*sizeof(int));
    ptr_structed.lock = find_lock(ptr);
    ptr_structed.key = *ptr_structed.lock;


    int *ptr2 = (int*)malloc(PTR2_SIZE*sizeof(int));
    struct VoidStr ptr2_structed;
    create_entry(ptr2, ptr2_structed);
    ptr2_structed.ptr2ptr = &ptr2;
    ptr2_structed.L = (void*)ptr2;
    ptr2_structed.H = (void*)(ptr2 + PTR2_SIZE*sizeof(int));
    ptr2_structed.lock = find_lock(ptr2);
    ptr2_structed.key = *ptr2_structed.lock;

    class Base base_obj;
    class Base* base_ptr = new class Base;
    struct VoidStr base_ptr_structed;
    create_entry(base_ptr, base_ptr_structed);
    base_ptr_structed.ptr2ptr = &base_ptr;
    base_ptr_structed.L = (void*)base_ptr;
    base_ptr_structed.H = (void*)(base_ptr + sizeof(class Base));
    base_ptr_structed.lock = find_lock(base_ptr);
    base_ptr_structed.key = *base_ptr_structed.lock;

    // Create structs for each of the pointers within class Base
    // And this has to be done recursively so that we catch all pointers
    // This is in line with the idea that each pointer gets its own struct
    // which tracks all the info required
    // An example
    struct VoidStr base_ptr1_structed;
    // FIXME: This might not be a valid call since ptr1 might be private 
    // an inaccesible from here
    create_entry(base_ptr->ptr1, base_ptr1_structed); 

    struct VoidStr base_ptr2_structed;
    // FIXME: This might not be a valid call since ptr1 might be private 
    // an inaccesible from here
    create_entry(base_ptr->ptr2, base_ptr2_structed);


    // The problem gets worse when the constructor is called and these 
    // pointers are initialized through the constructor


    //base_ptr->print();
    // This Deref will return the original pointer in
    // void type
    ((class Base*)Deref(base_ptr_structed))->print();


    class Base* base_ptr2 = base_ptr;
    struct VoidStr base_ptr2_structed;
    // FIXME: Pointers to base_ptr2 need to have their own
    // struct variables
    // Reassign is required since the locks would need
    // to be updated as well -- and this could be a 
    // duplication of an array of pointers which won't happen
    // in a simple struct to struct copy
    Reassign(base_ptr2_structed,base_ptr_structed);


    //base_ptr->print();
    // This Deref will get return the original pointer
    // in the correct type 
    ((class Base*)Deref(base_ptr_structed))->print();

    class Derived1 der1_obj;

    // base_ptr = &der1_obj; 
    // We do the reassignment through our function 
    // the operation so that we know what is being assigned
    // and we update the lower and upper bounds
    // We also update the locks in this function
    Reassign(base_ptr_structed, &der1_obj);


    //(dynamic_cast<Derived1*>(base_ptr))->print();
    (dynamic_cast<Derived1*>(Deref(base_ptr_structed))->print());

    class Derived2* der2_ptr = new class Derived2;
    struct VoidStr der2_ptr_structed;
    der2_ptr_structed.ptr2ptr = &der2_ptr;
    der2_ptr_structed.L = der2ptr;
    der2_ptr_structed.H = sizeof(class Derived2);
    find_lock(der2_ptr_structed);

    //base_ptr = der2_ptr;    
    Reassign(base_ptr_structed, der2_ptr_structed);
    
    //(dynamic_cast<Derived2*>(base_ptr))->print();
    (dynamic_cast<class Derived2*>((class Base*)Deref(base_ptr_structed))->print());

    // This is a pointer reassignment. The problem with this particular example is that the actual contents
    // of the struct which is returned, are lost -- and only the pointer is transferred. This scheme 
    // therefore, does not work
    Reassign(der2_ptr_structed, dynamic_cast<class Derived2*>(push_stack(der2_ptr_structed), push_stack(ptr2_structed), push_stack(ptr_structed), (der2_ptr = fn1(ptr, 'a', (float*)ptr2, der2_ptr)), (class Base*)Deref(pop_Stack())));


    int* start_ptr = ptr;
    struct VoidStr start_ptr_structed;
    start_ptr_structed.ptr2ptr = &start_ptr;
    start_ptr_structed.L = (void*)start_ptr;
    start_ptr_structed.H = (void*)(start_ptr + sizeof(int));
    find_lock(start_ptr_structed);

    int* start_ptr2 = ptr2;
    struct VoidStr start_ptr2_structed;
    start_ptr2_structed.ptr2ptr = &start_ptr2;
    start_ptr2_structed.L = (void*)start_ptr2;
    start_ptr2_structed.H = (void*)(start_ptr2 + sizeof(int));
    find_lock(start_ptr2_structed);

    // Crossing the boundary of ptr. The condition should
    // be less than, not less than or equal to
    // ptr[PTR_SIZE] is an out-of-bounds access
    for(int index = 0; index <= (PTR_SIZE + OUT_OF_BOUNDS_EXCESS); index++) {
        //*ptr = index;
        // Deref returns the right type of variable
        *((int*)Deref(ptr_structed)) = index;
        //ptr++;
        // Increment becomes addition of 1
        // This function updates the L, H and lock
        // of LHS with those of RHS
        Reassign(ptr_structed, Add(ptr_structed, 1));
    }


    // Resetting ptr to start_ptr, so that it points to the beginning
    // of the allocation

    //ptr = start_ptr;
    Reassign(ptr_structed, start_ptr_structed);

    // Printing what we wrote above
    for(int index = 0; index <= (PTR_SIZE + OUT_OF_BOUNDS_EXCESS); index++) {
        
        //printf("ptr[%d]=%d\n", index, *ptr);
        printf("ptr[%d]=%d\n", index, *((int*)(Deref(ptr_structed))));
        //ptr++;
        Reassign(ptr_structed, Add(ptr_structed, 1));
    }
    
    return 1;
}

