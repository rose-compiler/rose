// This causes a different number of typedefs to be generated
// Which causes a different mangled name (after processing via 
// SageInterface::generateUniqueName(), and so we don't get the
// merge that we might expect.  So we can't merge the 
// SgPointrType (since they have different names).  But then it
// seems that we delete part of the unmerged SgPointerType
// by mistake.  The problem is that we don't delete the 
// SgPointerType that is associated with the original 
// SgTypedefDeclaration that was deleted.
// It might be that the SgTypedefDeclaration shoudl not be merged 
// since the base_type will not evaluate to match (because it has 
// a difference in the typedef sequence lists).
// A FIX: might be to have the SgTypedefDeclaration's mangled name 
// use the unique name generated from its base_type (the 
// SgPointerType).

typedef char *__caddr_tXXX;

typedef char *__caddr_t;
