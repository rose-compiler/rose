
// Test of a function pointer which returns a function pointer!
// typedef void *(*functionPointer)(void *);

// long form of declaration of pointer to function
// void *(*functionPointer_A)(void *);

// Since foo is not previously defined the "struct" is required!
// typedef struct foo *(*functionPointer_B)(void *);

// Put the secondary type declaration into the return type
// typedef struct foobar *(*functionPointer_C)(void *);
typedef struct foobar *(*foobarFunctionPointer)();

#if __cplusplus
foobar* foobarPointer = 0L;
#else
struct foobar* foobarPointer = 0L;
#endif

struct foobar {};

#if 0
// Put the secondary type declaration into the parameter list
struct foobar *(*functionPointer_D)(struct foobar2 *);
foobar2* foobar2Pointer = 0L;

// A function pointer which returns a function pointer as a return type
functionPointer functionPointerUsingTypedef;

// Function pointer declarations (not using typedef)
functionPointer (*functionPointerReturningFunctionPointer)(void *) = 0L;

// Long form of pointer to function returning function pointer as return type
// void* (*functionPointerReturnType)(void *) (*functionPointerReturningFunctionPointer)(void *) = 0L;

// void* (*functionPointerReturnType)(void *) (*altFunctionPointerReturningFunctionPointer)(void *);
#endif
