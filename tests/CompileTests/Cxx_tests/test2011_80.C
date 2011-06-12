// struct b {};
// struct b;

struct a {
   struct b *bp;

// Redundent references to "struct b" share the same first non-defineding declaration in ROSE.
// struct b *bpx;

// Note that this forward reference to "struct b" is ignored by EDG and thus not represented 
// in ROSE.  Interestingly this is insufficent to define the scope of "struct b" (need the 
// defining declaration for that).
   struct b;

// This will cause the subsequent references to "struct b" to be output with qualification!
   struct b {};

// Redundent references to "struct b" share the same first non-defineding declaration in ROSE.
// struct b *bpy;
};

// struct b {};
