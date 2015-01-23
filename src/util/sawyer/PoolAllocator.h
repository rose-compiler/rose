#ifndef Sawyer_PoolAllocator_H
#define Sawyer_PoolAllocator_H

#include <boost/foreach.hpp>
#include <boost/static_assert.hpp>
#include <boost/cstdint.hpp>
#include <list>
#include <sawyer/Assert.h>
#include <sawyer/Interval.h>
#include <sawyer/IntervalMap.h>
#include <sawyer/Sawyer.h>
#include <vector>

namespace Sawyer {

/** Small object allocation from memory pools.
 *
 *  This class manages allocation and deallocation of small objects from pools. This allocator has pools available for a
 *  variety of small object sizes, or falls back to the global <code>new</code> and <code>delete</code> operators for larger
 *  objects.  Each pool contains zero or more large chunks of contiguous memory from which storage for the small objects are
 *  obtained. The following template parameters control the number and sizes of pools:
 *
 *  @li @p smallestCell is the size in bytes of the smallest cells.  An "cell" is a small unit of storage and may be larger
 *      than what is requested when allocating memory for an object.  The value must be at least as large as a pointer.
 *      Memory requests that are smaller than a cell lead to internal fragmentation.
 *  @li @p sizeDelta is the difference in size in bytes between the cells of two neighboring pools.  The value must be
 *      positive. When looking for a pool that will satisfy an allocation request, the allocator chooses the pool having the
 *      smallest cells that are at least as large as the request.
 *  @li @p nPools is the total number of pools, each having a different size of cells. If pools are numbered zero through
 *      @em n then the size of cells is \f$\|cell_i\| = \|cell_0\| + i \Delta\f$
 *  @li @p chunkSize is the size of each chunk in bytes.  Chunks are the unit of allocation requested from the runtime. All
 *      chunks in the allocator are the same size regardless of the cell size, and this may lead to external
 *      fragmentation&mdash;extra space at the end of a chunk that is not large enough for a complete cell.  The chunk size
 *      should be large in relation to the largest cell size (that's the whole point of pool allocation).
 *
 *  The @ref PoolAllocator typedef provides reasonable template arguments.
 *
 *  When a pool allocator is copied, only its settings are copied, not the pools.  Since containers typically copy their
 *  constructor-provided allocators, each container will have its own pools even if one provides the same pool to all the
 *  constructors.  See @ref ProxyAllocator for a way to avoid this, and to allow different containers to share the same
 *  allocator.
 *
 *  Deleting a pool allocator deletes all its pools, which deletes all the chunks, which deallocates memory that might be in
 *  use by objects allocated from this allocator.  In other words, don't destroy the allocator unless you're willing that the
 *  memory for any objects in use will suddenly be freed without even calling the destructors for those objects. */
template<size_t smallestCell, size_t sizeDelta, size_t nPools, size_t chunkSize>
class PoolAllocatorBase {
public:
    enum { SMALLEST_CELL = smallestCell };
    enum { SIZE_DELTA = sizeDelta };
    enum { N_POOLS = nPools };
    enum { CHUNK_SIZE = chunkSize };

private:

    // Singly-linked list of cells (units of object backing store) that are not being used by the caller.
    struct FreeCell { FreeCell *next; };

    typedef Sawyer::Container::Interval<boost::uint64_t> ChunkAddressInterval;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Basic unit of allocation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    class Chunk {
        unsigned char data_[chunkSize];
    public:
        BOOST_STATIC_ASSERT(chunkSize >= sizeof(FreeCell));

        FreeCell* fill(size_t cellSize) {               // create a free list for this chunk
            ASSERT_require(cellSize >= sizeof(FreeCell));
            ASSERT_require(cellSize <= chunkSize);
            FreeCell *retval = NULL;
            size_t n = chunkSize / cellSize;
            for (size_t i=n; i>0; --i) {                // free list in address order is easier to debug at no extra expense
                FreeCell *cell = reinterpret_cast<FreeCell*>(data_+(i-1)*cellSize);
                cell->next = retval;
                retval = cell;
            }
            return retval;
        }

        ChunkAddressInterval extent() const {
            return ChunkAddressInterval::hull(reinterpret_cast<boost::uint64_t>(data_),
                                              reinterpret_cast<boost::uint64_t>(data_+chunkSize-1));
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Interesting info about a chunk
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    struct ChunkInfo {
        const Chunk *chunk;
        size_t nUsed;
        ChunkInfo(): chunk(NULL), nUsed(0) {}
        ChunkInfo(const Chunk *chunk, size_t nUsed): chunk(chunk), nUsed(nUsed) {}
        bool operator==(const ChunkInfo &other) const {
            return chunk==other.chunk && nUsed==other.nUsed;
        }
    };

    typedef Sawyer::Container::IntervalMap<ChunkAddressInterval, ChunkInfo> ChunkInfoMap;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Pool of single-sized cells; collection of chunks
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class Pool {
        size_t cellSize_;
        FreeCell *freeList_;
        std::list<Chunk*> chunks_;
    public:
        Pool(): cellSize_(0), freeList_(NULL) {}        // needed by std::vector
        Pool(size_t cellSize): cellSize_(cellSize), freeList_(NULL) {}

    public:
        ~Pool() {
            for (typename std::list<Chunk*>::iterator ci=chunks_.begin(); ci!=chunks_.end(); ++ci)
                delete *ci;
        }

        bool isEmpty() const { return chunks_.empty(); }

        // Obtains the cell at the front of the free list, allocating more space if necessary.
        void* aquire() {                                // hot
            if (!freeList_) {
                Chunk *chunk = new Chunk;
                chunks_.push_back(chunk);
                freeList_ = chunk->fill(cellSize_);
            }
            ASSERT_not_null(freeList_);
            FreeCell *cell = freeList_;
            freeList_ = freeList_->next;
            cell->next = NULL;                          // optional
            return cell;
        }

        // Returns an cell to the front of the free list.
        void release(void *cell) {                      // hot
            ASSERT_not_null(cell);
            FreeCell *freedCell = reinterpret_cast<FreeCell*>(cell);
            freedCell->next = freeList_;
            freeList_ = freedCell;
        }

        // Information about each chunk
        ChunkInfoMap chunkInfo() const {
            ChunkInfoMap map;
            BOOST_FOREACH (const Chunk* chunk, chunks_)
                map.insert(chunk->extent(), ChunkInfo(chunk, chunkSize / cellSize_));
            for (FreeCell *cell=freeList_; cell!=NULL; cell=cell->next) {
                typename ChunkInfoMap::ValueIterator found = map.find(reinterpret_cast<boost::uint64_t>(cell));
                ASSERT_require2(found!=map.values().end(), "each freelist item must be some chunk cell");
                ASSERT_require2(found->nUsed > 0, "freelist must be consistent with chunk capacities");
                --found->nUsed;
            }
            return map;
        }

        // Free unused chunks
        void vacuum() {
            ChunkInfoMap map = chunkInfo();

            // Create a new free list that doesn't have any cells that belong to chunks that are about to be deleted
            FreeCell *cell = freeList_, *next = NULL;
            freeList_ = NULL;
            for (/*void*/; cell!=NULL; cell=next) {
                next = cell->next;
                boost::uint64_t cellAddr = reinterpret_cast<boost::uint64_t>(cell);
                if (map[cellAddr].nUsed != 0) {
                    cell->next = freeList_;
                    freeList_ = cell;
                }
            }

            // Delete chunks that have no used cells.
            typename std::list<Chunk*>::iterator iter = chunks_.begin();
            while (iter!=chunks_.end()) {
                Chunk *chunk = *iter;
                boost::uint64_t cellAddr = chunk->extent().least(); // any cell will do
                if (map[cellAddr].nUsed == 0) {
                    delete chunk;
                    iter = chunks_.erase(iter);
                } else {
                    ++iter;
                }
            }
        }

        size_t showInfo(std::ostream &out) const {
            const size_t nCells = chunkSize / cellSize_;
            size_t totalUsed=0;
            ChunkInfoMap cim = chunkInfo();
            BOOST_FOREACH (const ChunkInfo &info, cim.values()) {
                out <<"  chunk " <<info.chunk <<"\t" <<info.nUsed <<"/" <<nCells <<"\t= " <<100.0*info.nUsed/nCells <<"%\n";
                totalUsed += info.nUsed;
            }
            return totalUsed;
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Private data members and methods
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    std::vector<Pool> pools_;

    // Called by constructors
    void init() {
        pools_.reserve(nPools);
        for (size_t i=0; i<nPools; ++i)
            pools_.push_back(Pool(cellSize(i)));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Construction
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Default constructor. */
    PoolAllocatorBase() {
        init();
    }

    /** Copy constructor.
     *
     *  Copying an allocator does not copy its pools, but rather creates a new allocator that is empty but has the same
     *  settings as the source allocator. */
    PoolAllocatorBase(const PoolAllocatorBase&) {
        init();
    }

private:
    // Assignment is nonsensical
    PoolAllocatorBase& operator=(const PoolAllocatorBase&);

public:
    /** Destructor.
     *
     *  Destroying a pool allocator destroys all its pools, which means that any objects that use storage managed by this pool
     *  will have their storage deleted. */
    virtual ~PoolAllocatorBase() {}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Public methods
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Pool number for a request size.
     *
     *  The return value is a pool number assuming that an infinite number of pools is available.  In practice, if the return
     *  value is greater than or equal to the @p nPools template argument then allocation is handled by the global "new"
     *  operator rather than this allocator. */
    static size_t poolNumber(size_t size) {
        return size <= smallestCell ? 0 : (size - smallestCell + sizeDelta - 1) / sizeDelta;
    }

    /** Size of each cell for a given pool.
     *
     *  Returns the number of bytes per cell for the given pool. */
    static size_t cellSize(size_t poolNumber) {
        return smallestCell + poolNumber * sizeDelta;
    }

    /** Number of cells per chunk.
     *
     *  Returns the number of cells contained in each chunk of the specified pool. */
    static size_t nCells(size_t poolNumber) {
        return chunkSize / cellSize(poolNumber);
    }

    /** Allocate one object of specified size.
     *
     *  Allocates one cell from an allocation pool, using the pool with the smallest-sized cells that are large enough to
     *  satisfy the request.  If the request is larger than that available from any pool then the global "new" operator is
     *  used.
     *
     *  The requested size must be positive.
     *
     *  @sa DefaultAllocator::allocate */
    void *allocate(size_t size) {                       // hot
        ASSERT_require(size>0);
        size_t pn = poolNumber(size);
        return pn < nPools ? pools_[pn].aquire() : ::operator new(size);
    }

    /** Number of objects allocated and reserved.
     *
     *  Returns a pair containing the number of objects currently allocated in the pool, and the number of objects that the
     *  pool can hold (including those that are allocated) before the pool must request more memory from the system. */
    std::pair<size_t, size_t> nAllocated() const {
        size_t nAllocated = 0, nReserved = 0;
        for (size_t pn=0; pn<nPools; ++pn) {
            ChunkInfoMap cim = pools_[pn].chunkInfo();
            nReserved += nCells(pn) * cim.nIntervals();
            BOOST_FOREACH (const ChunkInfo &info, cim.values()) {
                nAllocated += info.nUsed;
            }
        }
        return std::make_pair(nAllocated, nReserved);
    }
    
    /** Deallocate an object of specified size.
     *
     *  The @p addr must be an object address that was previously returned by the @ref allocate method and which hasn't been
     *  deallocated in the interim.  The @p size must be the same as the argument passed to the @ref allocate call that
     *  returned this address.
     *
     *  @sa DefaultAllocator::deallocate */
    void deallocate(void *addr, size_t size) {          // hot
        ASSERT_not_null(addr);
        ASSERT_require(size>0);
        size_t pn = poolNumber(size);
        if (pn < nPools) {
            pools_[pn].release(addr);
        } else {
            ::operator delete(addr);
        }
    }

    /** Delete unused chunks.
     *
     *  A pool allocator is optimized for the utmost performance when allocating and deallocating small objects, and therefore
     *  does minimal bookkeeping and does not free chunks.  This method traverses the free lists to discover which chunks have
     *  no cells in use, removes those cells from the free list, and frees the chunk. */
    void vacuum() {
        for (size_t pn=0; pn<nPools; ++pn)
            pools_[pn].vacuum();
    }

    /** Print pool allocation information.
     *
     *  Prints some interesting information about each chunk of each pool. The output will be multiple lines. */
    void showInfo(std::ostream &out) const {
        for (size_t pn=0; pn<nPools; ++pn) {
            if (!pools_[pn].isEmpty()) {
                out <<"  pool #" <<pn <<"; cellSize = " <<cellSize(pn) <<" bytes:\n";
                size_t nUsed = pools_[pn].showInfo(out);
                out <<"    total objects in use: " <<nUsed <<"\n";
            }
        }
    }
};

/** Small object allocation from memory pools.
 *
 *  See @ref PoolAllocatorBase for details. */
typedef PoolAllocatorBase<sizeof(void*), 4, 32, 40960> PoolAllocator;

} // namespace
#endif
