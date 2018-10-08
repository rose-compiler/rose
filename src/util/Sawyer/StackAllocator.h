// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef SAWYER_STACK_ALLOCATOR
#define SAWYER_STACK_ALLOCATOR

#include <Sawyer/Sawyer.h>

namespace Sawyer {

/** Stack-like allocator.
 *
 *  This allocator allocates single elements of POD type from a stack-like pool. The allocator is intentionally not thread safe
 *  and is intended to be used in multi-threaded applications where each thread is allocating and freeing elements in a
 *  stack-like order from its own allocator. */
template<typename T>
class StackAllocator {
public:
    typedef T Value;                                    /**< Type of values being allocated. */

private:
    const size_t elmtsPerBlock_;                        // elements per large block of memory requested
    std::list<T*> blocks_;                              // front() is the most recent block from which an element is allocated
    size_t available_;                                  // number of elements available in most recent block
    std::list<T*> freeBlocks_;                          // blocks that aren't being used currently

public:
    explicit StackAllocator(size_t elmtsPerBlock)
        : elmtsPerBlock_(elmtsPerBlock), available_(0) {}

    ~StackAllocator() {
        prune();
        BOOST_FOREACH (T *ptr, blocks_)
            delete[] ptr;
        blocks_.clear();
        available_ = 0;
    }

    /** Free unused large blocks. */
    void prune() {
        BOOST_FOREACH (T *ptr, freeBlocks_)
            delete[] ptr;
        freeBlocks_.clear();
    }

    /** Reserve space for objects.  Reserves space for @p nElmts elements of type @p T and returns a pointer to the first
     *  one. Although this reserves space for objects, the memory is not yet usable to store an object. The next allocation
     *  request will give back the same address as returned here, unless the allocated is reverted to an earlier address in the
     *  mean time. */
    T* reserve(size_t nElmts) {
        if (nElmts > available_) {
            ASSERT_require(nElmts <= elmtsPerBlock_);
            allocateBlock();
        }
        return blocks_.front() + (elmtsPerBlock_ - available_);
    }

    /** Allocate one element. If this allocation is below the amount previously reserved then it is guaranteed to be adjacent
     *  to the previous allocation. */
    T* allocNext() {
        if (0 == available_)
            allocateBlock();
        return blocks_.front() + (elmtsPerBlock_ - available_--);
    }

    /** Free all elements back to the specified element.  Reverts the state of this allocator so that the specified @p ptr
     *  would be the return value from @ref allocNext. If this results in whole large blocks becoming unused they are not
     *  returned to the system (call @ref prune if that's desired). */
    void revert(T *ptr) {
        ASSERT_not_null(ptr);
        while (!blocks_.empty() && (ptr < blocks_.front() || ptr >= blocks_.front() + elmtsPerBlock_))
            freeBlock();
        ASSERT_always_forbid2(blocks_.empty(), "bad address or previously reverted");
        available_ = elmtsPerBlock_ - (ptr - blocks_.front());
    }

private:
    void allocateBlock() {
        if (freeBlocks_.empty()) {
            blocks_.insert(blocks_.begin(), new T[elmtsPerBlock_]);
        } else {
            blocks_.insert(blocks_.begin(), freeBlocks_.front());
            freeBlocks_.erase(freeBlocks_.begin());
        }
        available_ = elmtsPerBlock_;
    }

    void freeBlock() {
        ASSERT_forbid(blocks_.empty());
        freeBlocks_.insert(freeBlocks_.begin(), blocks_.front());
        blocks_.erase(blocks_.begin());
        available_ = 0;
    }
};

} // namespace

#endif
