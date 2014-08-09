/* This is a subset of in16.c */
struct AngEng
   {
   };

/* This is the simplest example of the bug. It is a bug because non-pointers of type shared are not transformed 
   Any uses of expressions of this type would be transformed, but are not shown in this test code.
 */
shared struct AngEng AE_ptr;

