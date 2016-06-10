// This is a clang specific issue (other compilers don't have a prolem with this).
// The issue is that using a weakref requires using the alias attribute as well
// they can be specified in any order as well).

void foobar_16a();
static void foobar_16() __attribute__ ((alias("foobar_16a"))) __attribute__((weakref));
