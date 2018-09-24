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

struct FloatStruct {
    float* ptr2obj;
    float* L;
    float* H;
};

struct UIntStruct {
    unsigned int* ptr2obj;
    unsigned int* L;
    unsigned int* H;
};

struct IntStruct {
    int* ptr2obj;
    int* L;
    int* H;
};


struct User_structed {
    struct FloatStruct user_ptr1_structed;
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

class Base_structed{
    struct UIntStruct ptr1_structed;
    struct UIntStruct ptr2_structed;
    unsigned int var1;
    char var2;
    float var3;
    struct FloatStruct ptr3_structed;
    struct UIntStruct  ptr4_structed;
    struct User_structed str_ptr1_structed;
    public:
    virtual void print() {
        printf("This is base_structed class\n");
    }
};

struct Base_structed_ptr {
    class Base_structed* ptr2obj;
    class Base_structed* L;
    class Base_structed* H;
};


class Derived1 : public Base {
    unsigned int* der1_ptr1;
    float* der1_ptr2;
    public:
    void print() {
        printf("This is Derived1 class\n");
    }
};

class Derived1_structed : public Base_structed {
    struct UIntStruct der1_ptr1_structed;
    struct FloatStruct der1_ptr2_structed;
    public:
    void print() {
        printf("This is Derived1_structed class\n");
    }
};

struct Derived1_structed_ptr {
    class Derived1_structed* ptr2obj;
    class Derived1_structed* L;
    class Derived1_structed* H;
};

class Derived2 : public Derived1 {
    unsigned int* der2_ptr1;
    float* der2_ptr2;
    public:
    void print() {
        printf("This is Derived2 class\n");
    }
};

class Derived2_structed : public Derived1_structed {
    struct UIntStruct der2_ptr1_structed;
    struct FloatStruct der2_ptr2_structed;
    public:
    void print() {
        printf("This is Derived2_structed class\n");
    }
};

struct Derived2_structed_ptr {
    class Derived2_structed* ptr2obj;
    class Derived2_structed* L;
    class Derived2_structed* H;
};

int main() {
    int *ptr = (int*)malloc(PTR_SIZE*sizeof(int));
    int *ptr2 = (int*)malloc(PTR2_SIZE*sizeof(int));

    struct IntStruct ptr_structed = (Int_Void_Cast(malloc_wrap(PTR_SIZE*sizeof(int))));
    struct IntStruct ptr2_structed = (Int_Void_Cast(malloc_wrap(PTR2_SIZE*sizeof(int))));


    class Base base_obj;
    class Base_structed base_obj_structed;


    class Base* base_ptr = new class Base;
    struct Base_structed_ptr base_ptr_structed_ptr = (Base_structed_ptr_Void_Cast(malloc_wrap(sizeof(class Base_Structed))));    


    base_ptr->print();
    base_ptr_structed_ptr->print();


    class Base* base_ptr2 = base_ptr;
    struct Base_structed_ptr base_ptr2_structed_ptr = base_ptr_structed_ptr;


    base_ptr->print();
    base_ptr_structed_ptr->print();

    class Derived1 der1_obj;
    class Derived1_structed der1_obj_structed;

    base_ptr = &der1_obj;
    base_ptr_structed_ptr = &der1_obj_structed; // Needs to be overloaded 

    (dynamic_cast<Derived1*>(base_ptr))->print();
    Derived1_structed_ptr_Base_structed_ptr_Dynamic_Cast(base_ptr_structed_ptr)->print();

    class Derived2* der2_ptr = new class Derived2;
    struct Derived2_structed_ptr der2_ptr_structed_ptr = (Derived2_structed_ptr_Void_Cast(malloc_wrap(sizeof(class Derived2_structed))));

    base_ptr = der2_ptr;
    base_ptr_structed_ptr = der2_ptr_structed_ptr; // Should be automatically handled -- struct to struct copy

    (dynamic_cast<Derived2*>(base_ptr))->print();
    Derived2_structed_ptr_Base_structed_ptr_Dynamic_Cast(base_ptr_structed_ptr)->print();

    
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
    
    return 1;
}
