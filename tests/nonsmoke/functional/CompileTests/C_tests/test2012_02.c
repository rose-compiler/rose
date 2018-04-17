#if 0
// Original code:
typedef union
   {
     struct __pthread_mutex_s
        {

        } __data;
   } pthread_mutex_t;
#endif

#if 0
// This defines Y in global scope and does not premit it's redifinition in "struct X"
struct Y
   {
  // int x2;
   };
#endif

// Simplest version of failing code:
struct X
   {
  // int x1;

  // Note that for C this will be in the global scope (but have a parent in "struct X"
  // Concepts of scope are different between C and C++.
  // This must be a variable declaration for this code to fail.
     struct Y
        {
       // int x2;
        } B;
   };

// Y yyy;

