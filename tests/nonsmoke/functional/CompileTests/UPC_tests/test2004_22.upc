/* Example UPC code (using UPC keywords to test use of UPC with C++) */

  /* Example UPC variable declaration */
shared int* integer;

// DQ (9/19/2010): Enable this when testing use of UPC with C++ (UPC++)
#ifdef ROSE_USE_NEW_EDG_INTERFACE // FIXME: This doesn't work yet
struct A
#else
class A
#endif
   {
     int x;
   };

int main()
   {
  // int x = MYTHREAD;
     return 0;
   }
