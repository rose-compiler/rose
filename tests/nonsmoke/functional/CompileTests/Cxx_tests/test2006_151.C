// This test code demonstrated the declaration of "struct b"
// inside of "struct a" making it unclear at initial declaration 
// if it is in scope of "struct a" or global scope.  The
// defining declaration is later seen in global scope so
// the declaration is in global socpe and the non-defining
// declaration is fixed up to reflect this.  This was handled
// correctly by EDG for C++, but is a bug in EDG 3.3 for C
// we now correct for this bug by resetting the non-defining
// declaration's scope when we see the defining declaration 
// (and have correctly nailed down its scope).

// A potential problem is that there may be many references to
// "struct b" before we see the defining declaration and we can 
// only reach back to correct the scope of the first non-defining 
// declaration.

// struct b;

struct a {
   struct b *bp;

// Redundent references to "struct b" share the same first non-defineding declaration in ROSE.
   struct b *bpx;

// Note that this forward reference to "struct b" is ignored by EDG and thus not represented 
// in ROSE.  Interestingly this is insufficent to define the scope of "struct b" (need the 
// defining declaration for that).
   struct b;

// This will cause the subsequent references to "struct b" to be output with qualification!
// struct b {};

// Redundent references to "struct b" share the same first non-defineding declaration in ROSE.
   struct b *bpy;
};

// Defining declaration of "struct b" (at this point we know the scope)
typedef struct b
   {
     int i;
   } BDEF;


int main(int argc, char **argv)
   {
  // Variables of type "struct b"
     struct b b1 = {3};
     struct a a1 = {&b1};

     int j = (a1.bp)->i;

     return 0;
   }

