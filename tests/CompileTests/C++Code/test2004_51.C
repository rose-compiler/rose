// DQ (7/20/2004):
// This code is derived from test2003_08.[hC] and demonstrates a bug in the
// current version of the EDG/SAGE connection or elsewhere within ROSE.
// We are fixing the island test and this is leading to several other problems,
// like this one!


// This tests a member function pointer
class A { };
typedef void (A::*PointerToMemberFunctionType)();

// A tag is build internally by EDG
struct
   {
     int b;
   } B1;

// Use of explicit tag in autonomous type in a variable declaration
struct Btag2
   {
     int b;
   } B2;

// Use of a reference to the autonomous type inside the type (in a variable declaration)
struct Btag3
   {
     Btag3* b;
   } B3;

// trivial case of a typedef
typedef int INTEGER;

// Tagless autonomous type in a typedef
typedef struct
   {
     int a;
   } A1;

// Explicit use of tag in autonomous type used in a typedef
typedef struct Atag2
   {
     int a;
   } A2;

// recursive reference to the tag of an autonomous type in a typedef
typedef struct Atag3
   {
     Atag3* a;
   } A3;

// Declaration of variables using both the tag of an autonomous 
// type and the typedef name of the same autonomous type
Atag3 B;
A3 C;

// DQ (4/7/2003): This will still fail because the 2nd part of the type 
// needs to be unparsed to get the "[1]" and the definition follows along 
// by accedent.
// Atag3 B[1];

// Nested typedef using the tag of the autonomous type
typedef struct Atag4
   {
     typedef Atag4 Btag;
   } A4;

// Nested typedef and references to the tag of an autonomous type in a typedef
typedef struct Atag5
   {
     typedef Atag5 Btag;
     typedef Atag5 * AtagPtr;
     typedef Atag5 & AtagRef;
   } A5;

