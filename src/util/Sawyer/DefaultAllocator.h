// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          github.com:matzke1/sawyer.




#ifndef Sawyer_DefaultAllocator_H
#define Sawyer_DefaultAllocator_H

#include <cstddef>
#include <Sawyer/Sawyer.h>

namespace Sawyer {

/** Default allocator.
 *
 *  This is the default allocator used by all Sawyer container classes. It simply delegates to the normal C++ global
 *  <code>new</code> and <code>delete</code> allocator. */
class DefaultAllocator {
public:
    /** Allocate memory.
     *
     *  Allocates a block of suitably aligned memory for an object that is @p size bytes.  The actual block may be slightly
     *  larger. The return value is the address of the new block. Allocators may return a null pointer or throw an exception
     *  when the request cannot be satisfied. */
    void *allocate(size_t size) {                       // hot
        return ::operator new(size);
    }

    /** Deallocate memory.
     *
     *  Deallocates a block of memory that was allocated by the @ref allocate method and which has not been deallocated in the
     *  mean time.  The @p addr and @p size must be the address returned by @ref allocate and the size that was used to request
     *  that block. */
    void deallocate(void *addr, size_t size) {          // hot
        ::operator delete(addr);
    }
};

/** Allocator proxy.
 *
 *  Allocators are copied by value when containers are created, which means that each container would normally have its own
 *  allocator.  However, sometimes, like for pool allocators, performance can be better if multiple containers could share an
 *  allocator.  An allocator proxy allows containers to share allocators by having the proxy reference the allocator.  When the
 *  proxy is copied, the new copy continues to point to the original allocator.
 *
 *  Example usage:
 *
 * @code
 *  Sawyer::PoolAllocator pool; // one pool of memory for all graphs
 *  typedef Sawyer::ProxyAllocator<Sawyer::PoolAllocator> PoolProxy;
 *  typedef Sawyer::Container::Graph<int, int, PoolProxy> Graph;
 *
 *  Graph g1(pool);
 *  Graph g2(pool);
 * @endcode */
template<class Allocator>
class ProxyAllocator {
    Allocator &allocator_;
public:
    /** Constructor.
     *
     *  The constructor stores a reference to the @p allocator within the new proxy object.  All copies of the proxy will refer
     *  to the same allocator. */
    ProxyAllocator(Allocator &allocator): allocator_(allocator) {} // implicit

    /** Allocate memory.
     *
     *  Allocates new memory by delegating to the underlying allocator.
     *
     *  @sa DefaultAllocator::allocate */
    void *allocate(size_t size) { return allocator_.allocate(size); }

    /** Deallocate memory.
     *
     *  Deallocates memory by delegating to the underlying allocator.
     *
     *  @sa DefaultAllocator::deallocate */
    void deallocate(void *addr, size_t size) { allocator_.deallocate(addr, size); }
};

} // namespace

#endif
