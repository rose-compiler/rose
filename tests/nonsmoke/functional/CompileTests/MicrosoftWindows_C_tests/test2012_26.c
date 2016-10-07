
/* For at least C code, ROSE drops the static keyword on the function.
// This causes linking problems for multi-file projects using the same
// static functions acorss multiple files.  Linker reports multiply 
// defined symbols. Note that this is awkward to test without using a
// multiple file test code.
*/
static void foo();

static void foo()
   {
   }

