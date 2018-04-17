
struct Type { int x; };

// The first part of the type is: "struct tag { int x; }"
// And there is no 2nd part!
struct tag { int x; } Var;

// mixing unnamed types in typedefs with arrays
typedef struct { int x; } structArrayType [100];
typedef enum { No,Yes }   enumArrayType [100];

// The first part of the type is: "struct { int x; }"
// and the second part is: "[100]"
// but we have to be careful to extract the base type from the SgArrayType
struct { int x; } ArrayVar [100];

// DQ (3/10/2007): I think this make this example more complex in a relavant way
struct { int x; } anotherArrayVar [100],struct_abc;

enum {  _No, _Yes } enumArrayVar [100];
enum { __No,__Yes } anotherEnumArrayVar [100],enum_abc;


typedef struct { int x; } struct_a,*struct_b;

#if __cplusplus
// This case fails in C (using "-rose:C" option)
typedef enum { ___No,___Yes } enum_a,*enum_b;
#else
typedef enum enum_tag { ___No,___Yes } enum_a,*enum_b;
#endif
