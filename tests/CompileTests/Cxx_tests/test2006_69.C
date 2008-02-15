// Problem reported by Yarden

// Skip version 4.x gnu compilers
#if ( __GNUC__ == 3 )

// ROSE can not appear to detect this pragma (seemingly ignored by EDG)
#pragma 

// ROSE can not appear to detect this pragma (seemingly ignored by EDG)
#pragma 0

// ROSE can correctly recognize this pragma (since it includes a single character string)
#pragma s

// Demonstrate ident pragma (used to define strings?)
// #ident Must have associated string
#ident "this is an ident"
#pragma ident "this is a pragma"

// Some other pragmas special to some compilers (IBM, I think), but just treated as a typical string based pragma by ROSE
#pragma int_to_unsigned
#pragma intrinsic
#pragma unknown_control_flow

// These are just handled as string based pragmas within ROSE
#pragma ms_struct on
#pragma ms_struct off
#pragma ms_struct reset

// A made up pragma (correctly processed as a string based pragma by ROSE)
#pragma help

// Test IBM specific nopack option (ignored by GNU, and fails in EDG)
// #pragma pack(nopack)

// A packing pragma used for a variable declaration
#pragma pack(16)
struct { unsigned short a; } bVar1;

// A packing pragma used for a variable declaration
#pragma pack(16)
struct { unsigned short a; }
#pragma pack()
bVar2;

// Test packing pragma with variable declaration (pragma in non-typical location)
struct
#pragma pack(16)
   { unsigned short a; } bVar3;

// Test packing pragma with typedef
#pragma pack(1)
typedef struct { unsigned short a; } bType1;

// Test use of packing pragma embedded in typedef declaration
typedef
#pragma pack(1)
struct { unsigned short a; } bType2;

// Test use of packing pragma embedded in typedef declaration (pragma in non-typical location)
typedef
struct
#pragma pack(1)
 { unsigned short a; } bType3;

// Test use of packing pragma embedded in typedef declaration (pragma in non-typical location)
typedef
struct
#pragma pack(1)
 { unsigned short a; } 
#pragma pack()
bType4;

// Meaningless since there is nothing to apply the packing pragma to (ignored by EDG internally)!
#pragma pack(1)

#pragma pack(4)
struct A { unsigned short a; };
#pragma pack(push,8)
struct B1 { unsigned short a; };
struct B2 { unsigned short a; };
#pragma pack(pop)
struct C { unsigned short a; };
#pragma pack(push,1)
struct D { unsigned short a; };

#pragma pack(2)
struct F { unsigned short a; };

struct G { unsigned short a; };
#pragma pack(pop)

struct H { unsigned short a; };

struct I { unsigned short a; };

#pragma pack()
struct J { unsigned short a; };

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

