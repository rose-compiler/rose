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




int main() {
    int *ptr = (int*)malloc(PTR_SIZE*sizeof(int));
    struct VoidStr ptr_structed;
    ptr_structed.ptr2ptr = &ptr;
    ptr_structed.L = (void*)ptr;
    ptr_structed.H = (void*)(ptr + PTR_SIZE*sizeof(int));
    find_lock(ptr_structed);


    int *ptr2 = (int*)malloc(PTR2_SIZE*sizeof(int));
    struct VoidStr ptr2_structed;
    ptr2_structed.ptr2ptr = &ptr2;
    ptr2_structed.L = (void*)ptr2;
    ptr2_structed.H = (void*)(ptr2 + PTR2_SIZE*sizeof(int));
    find_lock(ptr2_structed);

    class Base base_obj;
    class Base* base_ptr = new class Base;
    struct VoidStr base_ptr_structed;
    base_ptr_structed.ptr2ptr = &base_ptr;
    base_ptr_structed.L = (void*)base_ptr;
    base_ptr_structed.H = (void*)(base_ptr + sizeof(class Base));
    find_lock(base_ptr_structed);

    //base_ptr->print();
    // This Deref will return the original pointer in
    // void type
    ((class Base*)Deref(base_ptr_structed))->print();


    class Base* base_ptr2 = base_ptr;
    struct VoidStr base_ptr2_structed;
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
    (dynamic_cast<Derived2*>((class Base*)Deref(base_ptr_structed))->print());


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

