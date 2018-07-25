#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#define PTR_SIZE 100
#define PTR2_SIZE 10
#define PTR3_SIZE 10
#define OUT_OF_BOUNDS_EXCESS 1


struct VoidStruct 
{
  void *L;
  void *H;
  uint64_t* lock_loc;
  uint64_t key;
}
;


class User_structed 
{
  public:
  struct VoidStruct tracking_struct; 
  public: struct VoidStruct user_ptr1_structed;
}
;


struct User{
    float* user_ptr1;
};

class Base_structed 
{
  public:
  struct VoidStruct tracking_struct;
  public: struct VoidStruct *ptr1_structed;
  struct VoidStruct *ptr2_structed;
  struct VoidStruct *ptr3_structed;
  struct VoidStruct *ptr4_structed;
  class User_structed *str_ptr1_structed;
  #if 0
  virtual void print() {
        int* print_ptr1;
        printf("This is base class\n");
    }
  #endif

}
;


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

class Derived1_structed : public Base_structed
{
  public:
  struct VoidStruct tracking_struct;
  public: struct VoidStruct *der1_ptr1_structed;
  struct VoidStruct *der1_ptr2_structed;
}
;


class Derived1 : public Base {
    unsigned int* der1_ptr1;
    float* der1_ptr2;
    public:
    void print() {
        int* print_ptr1;
        printf("This is Derived1 class\n");
    }
};

class Derived2_structed : public Derived1_structed
{
  public: 
  struct VoidStruct tracking_struct;
  public: struct VoidStruct *der2_ptr1_structed;
  struct VoidStruct *der2_ptr2_structed;
  class Base_structed *der2_base_ptr1_structed;
  class Derived1 der2_der1_obj_structed;
  
}
;


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
};

struct BasePtr_Struct {
    class Base* obj;
    class Base_structed* tracker;
};

struct Base_Struct {
    class Base obj;
    class Base_structed tracker;
};

struct Derived1Ptr_Struct {
    class Derived1* obj;
    class Derived1_structed *tracker;
};

struct Derived1_Struct {
    class Derived1 obj;
    class Derived1_structed tracker;
};

struct Derived2Ptr_Struct {
    class Derived2* obj;
    class Derived2_structed* tracker;
};

struct Derived2_Struct {
    class Derived2_structed obj;
    class Derived2_structed tracker;
};

struct VoidPtr_Struct {
    void* obj;
    struct VoidStruct* tracker;
};

struct BasePtr_Struct fn1(struct VoidPtr_Struct input1_structed, char input4, struct VoidPtr_Struct input2_structed,
                    struct Derived2Ptr_Struct input3_structed, struct Base_Struct input5_structed) {

    struct BasePtr_Struct baseptr_structed = {dynamic_cast<class Base*>(input3_structed.obj), 
                                              dynamic_cast<class Base_structed*>(input3_structed.tracker)};
    return baseptr_structed;
}

#if 0
struct ArgStruct fn1(struct ArgStruct input1_structed, char input4, struct ArgStruct input2_structed,
                    struct ArgStruct input3_structed, struct ArgStruct input5_structed) {

    return input3_structed;

}


class Base* fn1(int* input1,  char input4, float* input2, class Derived2* input3, class Base input5) {

    return dynamic_cast<class Base*>(input3);

}
#endif

void Check_Deref(void* ptr) {}

int main() {
    int *ptr = (int*)malloc(PTR_SIZE*sizeof(int));
    struct VoidStruct* ptr_structed;

    int *ptr2 = (int*)malloc(PTR2_SIZE*sizeof(int));
    struct VoidStruct* ptr2_structed;


    class Base base_obj;
    class Base_structed base_obj_structed;


    class Base* base_ptr = new class Base;
    class Base_structed* base_ptr_structed = new class Base_structed;

    Check_Deref(base_ptr_structed);
    base_ptr->print();

    class Base* base_ptr2 = base_ptr;
    class Base_structed* base_ptr2_structed = base_ptr_structed;

    Check_Deref(base_ptr_structed);
    base_ptr->print();
    


    class Derived1 der1_obj;
    class Derived1_structed der1_obj_structed;


    base_ptr = &der1_obj;
    base_ptr_structed = &der1_obj_structed;

    Check_Deref((static_cast<class Derived1_structed*>(base_ptr_structed)));
//    Check_Deref((dynamic_cast<class Derived1_structed*>(base_ptr_structed)));
    (dynamic_cast<class Derived1*>(base_ptr))->print();


    class Derived2* der2_ptr = new class Derived2;
    class Derived2_structed* der2_ptr_structed = new class Derived2_structed;


    base_ptr = der2_ptr;    
    base_ptr_structed = der2_ptr_structed;

    Check_Deref(static_cast<class Derived2_structed*>(base_ptr_structed));
//    Check_Deref(dynamic_cast<class Derived2_structed*>(base_ptr_structed));
    (dynamic_cast<class Derived2*>(base_ptr))->print();


    //der2_ptr = dynamic_cast<class Derived2*>(fn1(ptr, 'a', (float*)ptr2, der2_ptr, base_obj));

    // int*
    struct VoidPtr_Struct input1;
    input1.obj = ptr;
    input1.tracker = ptr_structed;

    // float*
    struct VoidPtr_Struct input2;
    input2.obj = (float*)ptr2;
    input2.tracker = ptr2_structed;

    // Derived2 arg
    struct Derived2Ptr_Struct der2Arg;
    der2Arg.obj = der2_ptr;
    der2Arg.tracker = der2_ptr_structed;
    // Base arg
    struct Base_Struct baseArg;
    baseArg.obj = base_obj;
    baseArg.tracker = base_obj_structed;


    struct BasePtr_Struct basePtr_Str;
    basePtr_Str  = fn1(input1, 'a', input2, der2Arg, baseArg);
    der2_ptr = dynamic_cast<class Derived2*>(basePtr_Str.obj);
    der2_ptr_structed = dynamic_cast<class Derived2_structed*>(basePtr_Str.tracker);
    


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
