#ifndef Sawyer_SmallObject_H
#define Sawyer_SmallObject_H

#include <sawyer/PoolAllocator.h>
#include <sawyer/Sawyer.h>

namespace Sawyer {

/** Small object support.
 *
 *  Small objects that inherit from this class will use a pool allocator instead of the global allocator. */
class SAWYER_EXPORT SmallObject {
#include <sawyer/WarningsOff.h>
    static PoolAllocator allocator_;
#include <sawyer/WarningsRestore.h>
public:
    /** Return the pool allocator for this class. */
    static PoolAllocator& poolAllocator() { return allocator_; }

    static void *operator new(size_t size) { return allocator_.allocate(size); }
    static void operator delete(void *ptr, size_t size) { allocator_.deallocate(ptr, size); }
};

} // namespace
#endif
