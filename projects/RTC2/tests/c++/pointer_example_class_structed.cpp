#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <utility>
#include <iostream>

#define PTR_SIZE 100
#define PTR2_SIZE 10
#define PTR3_SIZE 10
#define OUT_OF_BOUNDS_EXCESS 1

class Base;


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

// INSERTED CODE

// VoidStruct is used for all pointers within the program.
struct VoidStruct {
    void* obj; // Only useful for pointers to objects
    void* L;
    void* H;
    uint64_t* lock_loc;
    uint64_t key;
};

// These "structed" versions of original classes/structs
// are used to track pointers within their corresponding
// classes and structs
struct User_structed {
    struct VoidStruct user_ptr1_structed;
};

class Base_structed{
    struct VoidStruct ptr1_structed;
    struct VoidStruct ptr2_structed;
    struct VoidStruct ptr3_structed;
    struct VoidStruct ptr4_structed;
    struct VoidStruct str_ptr1_structed;
};

class Derived1_structed : public Base_structed {
    struct VoidStruct der1_ptr1_structed;
    struct VoidStruct der1_ptr2_structed;
};

class Derived2_structed : public Derived1_structed {
    struct VoidStruct der2_ptr1_structed;
    struct VoidStruct der2_ptr2_structed;
};

//typedef std::pair<class Base*, class Base_structed> Basepair;
typedef std::pair<void*, struct VoidStruct> ptrStrpair;

// Original idea -- Use std pairs for return values. Keep original parameters, and add structed
// versions
// Problem: Passing the return value of one function, directly as the argument into
// another function
// Solution: Parameters to a function are also pairs. Split the pairs for use within the
// function

ptrStrpair fn1(ptrStrpair input1_pair, char input4, ptrStrpair input2_pair,
                ptrStrpair input3_pair) {

    int* input1 = (int*)input1_pair.first;
    struct VoidStruct input1_structed = input1_pair.second;
    float* input2 = (float*)input2_pair.first;
    struct VoidStruct input2_structed = input2_pair.second;
    class Derived2* input3 = (class Derived2*)input3_pair.first;
    struct VoidStruct input3_structed = input3_pair.second;

    return std::make_pair(dynamic_cast<class Base*>(input3), input3_structed);
}


#if 0
Basepair fn1(int* input1, struct VoidStruct input1_structed,
            char input4,
            float* input2, struct VoidStruct input2_structed,
            class Derived2* input3, struct VoidStruct input3_structed) {

    return std::make_pair(dynamic_cast<class Base*>(input3), dynamic_cast<class Base_structed>(input3_structed));
}
#endif
// END



class Base* fn1(int* input1,  char input4, float* input2, class Derived2* input3) {

    return dynamic_cast<class Base*>(input3);

}

uint64_t* find_lock(void* ptr) { 
    uint64_t* lock_loc = (uint64_t*)malloc(sizeof(uint64_t)); 
    *lock_loc = 5; // Some value as an example
    return lock_loc;
}

void Check_Deref(struct VoidStruct str, void* ptr) { 
    return; 
}



int main() {
    
    printf("part 1 - ptr\n");
    // These statements might need to be atomic in a parallel program
    int *ptr = (int*)malloc(PTR_SIZE*sizeof(int));
    struct VoidStruct ptr_structed;
    ptr_structed.obj = NULL; // Not a pointer to an object
    ptr_structed.L = ptr;
    ptr_structed.H = ptr + PTR_SIZE*sizeof(int);    
    ptr_structed.lock_loc = find_lock(ptr);
    ptr_structed.key = *ptr_structed.lock_loc; 
    
    printf("part 2 - ptr2\n");
    // These statements might need to be atomic in a parallel program
    int *ptr2 = (int*)malloc(PTR2_SIZE*sizeof(int));
    struct VoidStruct ptr2_structed;
    ptr2_structed.obj = NULL; // Not a pointer to an object
    ptr2_structed.L = ptr2;
    ptr2_structed.H = ptr2 + PTR_SIZE*sizeof(int);    
    ptr2_structed.lock_loc = find_lock(ptr2);
    ptr2_structed.key = *ptr2_structed.lock_loc; 

    printf("part 3 - base_obj\n");
    // VoidStruct is only used for pointers. For objects of 
    // classes, we use the structed_version of the class 
    // 
    class Base base_obj;
    class Base_structed base_obj_structed;
    // FIXME: Will need to get the initializations of the
    // variables in base_obj into base_obj_structed

    printf("part 4 - base_ptr\n");
    class Base* base_ptr = new class Base;
    // This struct handles all the pointer checking for
    // the base_ptr and all the members of the object
    // pointed to by base_ptr
    // This is a pointer, so use VoidStruct
    struct VoidStruct base_ptr_structed;
    // This is a pointer to an object, so we associate
    // obj with an object of the structed version
    // The VoidStructs within base_ptr_structed will 
    // track the pointers within base_ptr
    base_ptr_structed.obj = new class Base_structed;
    // This is pointer tracking info for the base_ptr
    base_ptr_structed.L = base_ptr;
    base_ptr_structed.H = base_ptr + sizeof(class Base);
    base_ptr_structed.lock_loc = find_lock(base_ptr);
    base_ptr_structed.key = *base_ptr_structed.lock_loc;


    printf("part 5 - base_ptr_deref\n");
    // Pass the VoidStruct to be checked for the current
    // dereference to Check_Deref
    Check_Deref(base_ptr_structed, base_ptr);
    base_ptr->print();

    printf("part 6 - base_ptr2\n");
    class Base* base_ptr2 = base_ptr;
    // Since base_ptr2 is a pointer, we use
    // VoidStruct and assign obj to object
    // of type base_ptr2_structed
    struct VoidStruct base_ptr2_structed;
    base_ptr2_structed = base_ptr_structed;

    printf("part 7 - base_ptr_deref\n");
    // Pass the VoidStruct to be checked for the current
    // dereference to Check_Deref
    Check_Deref(base_ptr_structed, base_ptr);
    base_ptr->print();

    printf("part 8 - der1_obj\n");
    // Class object -- so created a structed version of
    // the same
    class Derived1 der1_obj;
    class Derived1_structed der1_obj_structed;

    printf("part 9 - base_ptr_der1_obj\n");
    // Here, a pointer is being reassigned. 
    // 1. This involves
    // copying over the data from the RHS object to the
    // the structed version of the object within base_ptr
    //
    // 2. And re-assigning correct values for spatial and
    // temporal checks for base_ptr_structed
    base_ptr = &der1_obj;
    // Always -- L, H, lock_loc and key will refer to the 
    // original pointer. 
    // obj refers to the structed version of the pointer
    // to the object
    base_ptr_structed.obj = &der1_obj_structed;
    base_ptr_structed.L = &der1_obj;
    base_ptr_structed.H = &der1_obj + sizeof(der1_obj);
    base_ptr_structed.lock_loc = find_lock(&der1_obj);
    base_ptr_structed.key = *base_ptr_structed.lock_loc;
    

    printf("part 10 - base_ptr_Derived1_deref\n");
    // FIXME: Not sure about this one. Should it be
    // cast somewhere? This would be virtual table
    // checking??
    Check_Deref(base_ptr_structed, base_ptr);
    (dynamic_cast<Derived1*>(base_ptr))->print();


    printf("part 11 - der2_ptr\n");
    class Derived2* der2_ptr = new class Derived2;
    // Pointer -- so use VoidStruct
    struct VoidStruct der2_ptr_structed;
    // obj - will be used to track the pointers within
    // the Derived2
    der2_ptr_structed.obj = new class Derived2_structed;
    // L, H, lock_loc and key will check the original
    // der2_ptr
    der2_ptr_structed.L = der2_ptr;
    der2_ptr_structed.H = der2_ptr + sizeof(class Derived2);
    der2_ptr_structed.lock_loc = find_lock(der2_ptr);
    der2_ptr_structed.key = *der2_ptr_structed.lock_loc;


    printf("part 12 - base_ptr_der2_ptr\n");
    base_ptr = der2_ptr;    
    // Pointer to pointer assignment => VoidStruct to VoidStruct
    // copy
    base_ptr_structed = der2_ptr_structed;


    printf("part 13 - base_ptr_deref\n");
    // FIXME: Same issue as above.
    Check_Deref(base_ptr_structed, base_ptr);
    (dynamic_cast<class Derived2*>(base_ptr))->print();

    
    printf("part 14 - fn1\n");
//    der2_ptr = dynamic_cast<class Derived2*>(fn1(ptr, 'a', (float*)ptr2, der2_ptr));
    ptrStrpair arg1 = std::make_pair((void*)ptr, ptr_structed);
    // arg2 is not a pointer
    ptrStrpair arg3 = std::make_pair((void*)(float*)ptr2, ptr2_structed);
    ptrStrpair arg4 = std::make_pair((void*)der2_ptr, der2_ptr_structed);
    ptrStrpair ret;
    // Need to update der2_ptr_structed with the return value from fn1. The
    // original return value is the first one in the pair
    der2_ptr = dynamic_cast<class Derived2*>(ret = fn1(arg1, 'a', arg3, arg4), der2_ptr_structed = ret.second, (class Base*)ret.first);
    

    printf("part 15 - start_ptr\n");
    int* start_ptr = ptr;
    struct VoidStruct start_ptr_structed;
    start_ptr_structed = ptr_structed;

    printf("part 16 - start_ptr2\n");
    int* start_ptr2 = ptr2;
    struct VoidStruct start_ptr2_structed;
    start_ptr2_structed = ptr2_structed;

    printf("part 17 - for loop 1\n");
    // Crossing the boundary of ptr. The condition should
    // be less than, not less than or equal to
    // ptr[PTR_SIZE] is an out-of-bounds access
    for(int index = 0; index <= (PTR_SIZE + OUT_OF_BOUNDS_EXCESS); index++) {


        Check_Deref(ptr_structed, ptr);
        *ptr = index;

        // FIXME: Should we do a spatial check here?
        ptr++;
    }

    printf("part 18 - ptr_start_ptr\n");
    // Resetting ptr to start_ptr, so that it points to the beginning
    // of the allocation
    ptr = start_ptr;
    ptr_structed = start_ptr_structed;


    printf("part 19 - for loop 2\n");
    // Printing what we wrote above
    for(int index = 0; index <= (PTR_SIZE + OUT_OF_BOUNDS_EXCESS); index++) {
    

        // Using comma operator to insert the Check_Deref
        printf("ptr[%d]=%d\n", index, (Check_Deref(ptr_structed, ptr), *ptr));
        
        // FIXME: Should we do a spatial check here?
        ptr++;
    }

    printf("part 20 - done\n");
    return 1;
}
