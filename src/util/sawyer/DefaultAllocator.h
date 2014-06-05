#ifndef Sawyer_DefaultAllocator_H
#define Sawyer_DefaultAllocator_H

#include <cstddef>

namespace Sawyer {

class DefaultAllocator {
public:
    void *allocate(size_t size) {                       // hot
        return ::operator new(size);
    }

    void deallocate(void *addr, size_t size) {          // hot
        ::operator delete(addr);
    }
};

} // namespace

#endif
