/* Test thread local storage modifier (__thread) 
 * Liao 9/18/2009
 * */

// TOO (2/14/11): thrifty compiler (gcc 3.4.4) does not
// support thread-local storage
#if __GNUC__ >= 4
__thread int b;
extern __thread int a;
static __thread int c;
#endif
