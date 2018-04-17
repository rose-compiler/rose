// Example from Xerces Base64.cpp
class MemoryManager
   {
     public:
          virtual void* allocate(unsigned long size) = 0;
   };


static void* getExternalMemory(  MemoryManager* const allocator, unsigned long const   sizeToAllocate)
   {
  // return allocator ? allocator->allocate(sizeToAllocate) : ::operator new(sizeToAllocate);
  // allocator->allocate(sizeToAllocate);
  // ::operator new(sizeToAllocate);

     allocator ? allocator->allocate(sizeToAllocate) : ::operator new(sizeToAllocate);

     return 0L;
   }
