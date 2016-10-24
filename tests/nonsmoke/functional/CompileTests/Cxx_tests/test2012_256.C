inline void* operator new(__SIZE_TYPE__, void* __p)  { return __p; }

class MotionItem {};

void foo()
   {
     void* x = 0L;

  // This is a call to new with "placement" arguments
     MotionItem* item2 = new (x) MotionItem();
   };

