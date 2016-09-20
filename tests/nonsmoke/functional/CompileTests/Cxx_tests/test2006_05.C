/*
The following code:
inline void* operator new(unsigned int, void* __p)  { return __p; }

class MotionItem{
};

void foo(){
        void* x;
        MotionItem* item = new (x)
          MotionItem();
};
gives the following error:
rose_test2.C: In function `void foo()':
rose_test2.C:17: error: `xMotionItem' has not been declared

*/

// The first parameter must be size_t (but size_t is unsigned int, so this works 
// DQ (7/21/2006): at least on 32 bit systems, but use __SIZE_TYPE__ instead for portablity).
// inline void* operator new(unsigned int, void* __p)  { return __p; }
inline void* operator new(__SIZE_TYPE__, void* __p)  { return __p; }

class MotionItem{
};

void foo()
   {
     void* x = 0L;

  // This is a more conventional call to the new operator
     MotionItem* item1 = new MotionItem();

  // This is a call to new with "placement" arguments
     MotionItem* item2 = new (x) MotionItem();

  // Allocates 100 int and returns a pointer to an int (SgTypeInt)
     int *array1 = new int(100);

  // Allocates an array 100 ints and returns a pointer to an array of int (SgArrayType)
     int *array2 = new int[100];

   };

