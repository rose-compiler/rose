
// error: cannot convert argument type `float (*)[4]' to parameter 1 type
// `float const (*)[4]'

// originally found in package xfree86.


// ----------------
// ERR-MATCH: cannot convert.*\Q(*)[4]\E

void bar(const float M[4][4]);
void bar2(const float (*M)[4]);

void foo() {
    float (*m)[4] = 0;

    bar((float (*)[4]) m);
    bar(m);

    bar2((float (*)[4]) m);
    bar2(m);
}


// ----------------
typedef int *ptr_to_int;
typedef ptr_to_int array4_of_ptr_to_int[4];
typedef array4_of_ptr_to_int *ptr_to_array4_of_ptr_to_int;

typedef int const *ptr_to_cint;
typedef ptr_to_cint array4_of_ptr_to_cint[4];
typedef array4_of_ptr_to_cint *ptr_to_array4_of_ptr_to_cint;


void callee(ptr_to_array4_of_ptr_to_cint param);

void caller()
{
  // EDG accepts with warning, GCC rejects, my understanding
  // of cppstd is it is not allowed, even though it would be
  // const-safe
  //ERROR(1): callee((ptr_to_array4_of_ptr_to_int)0);
}


// ----------------
typedef int array4_of_int[4];
typedef array4_of_int array5_of_array4_of_int[5];
typedef array5_of_array4_of_int *ptr_to_array5_of_array4_of_int;

typedef int const array4_of_cint[4];
typedef array4_of_cint array5_of_array4_of_cint[5];
typedef array5_of_array4_of_cint *ptr_to_array5_of_array4_of_cint;

void callee2(ptr_to_array5_of_array4_of_cint param);

void caller2()
{
  // ok to have an arbitrary number of array nestings
  callee2((ptr_to_array5_of_array4_of_int)0);
}


void callee3(array5_of_array4_of_cint param);

void caller3()
{
  array5_of_array4_of_int a;

  // both argument and parameter have their outermost level
  // of arrayness converted to pointers
  callee3(a);
}


// EOF
