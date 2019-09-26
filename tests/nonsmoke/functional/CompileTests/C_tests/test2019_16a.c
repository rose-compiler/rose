

// A.c:
typedef struct b_class* B_ID;
int main() {
  return 0;
}

#if 0
// B.c:
typedef struct b_class
{
} B_CLASS;

#endif

/*
Good.sh:
rose-cc B.c A.c

Bad.sh:
rose-cc A.c B.c

This is a reproducer for a new assertion failure apparently introduced recently.  It requires multiple files to demostrate.  It looks like passing multiple VxWorks files to any rose tool will cause this assertion failure. 

It should fail like this:

rose-cc[3612] 2.06629s Rose[FATAL]: assertion failed:
rose-cc[3612] 2.06653s Rose[FATAL]:   ../../../../../../src/frontend/CxxFrontend/EDG/edgRose/edgRose.C:22227
rose-cc[3612] 2.06679s Rose[FATAL]:   SgDeclarationStatement* EDG_ROSE_Translation::convert_struct_primary(a_type_ptr, SgClassDeclaration::class_types, SgClassDeclaration*)
rose-cc[3612] 2.06692s Rose[FATAL]:   required: temp_defining_classDeclaration->get_scope() == originalDeclarationScope
Aborted (core dumped) 

Order matters in this case, so I provided 2 scripts, good.sh and bad.sh .  bad.sh is the one that fails. gcc can compile either one.
*/


