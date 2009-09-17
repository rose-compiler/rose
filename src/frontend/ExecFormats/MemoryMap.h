#ifndef ROSE_MEMORY_MAP_H
#define ROSE_MEMORY_MAP_H

#include <sys/mman.h> /*mmap include for PROT_READ, PROT_WRITE, PROT_EXEC, and PROT_NONE*/

/** A MemoryMap is an efficient mapping from virtual addresses to file (or other) offsets. The mapping can be built
 *  piecemeal and the data structure will coalesce adjacent memory areas (provided they are also adjacent in the file). If an
 *  attempt is made to define a mapping from one virtual address to multiple file offsets then an exception is raised. */
class MemoryMap {
public:
    /** A MemoryMap is composed of zero or more MapElements. Each map element describes a mapping from contiguous virtual
     *  addresses to contiguous file/memory bytes. A map element can point to a buffer supplied by the caller or a buffer
     *  allocated and managed by the MemoryMap itself. MemoryMap-managed buffers are used for anonymous maps where the backing
     *  store is initialized to all zero bytes rather than to values supplied by the caller.
     *
     *  If the user supplies a const pointer base address to the MapElement constructor then any call to MemoryMap::write()
     *  will fail when attempting to write to that map element.  The is_read_only() method returns true if the user supplied a
     *  const base address to the constructor.
     *
     *  The map element also tracks what permissions would be used if the memory were actually mapped for real. These
     *  permissions are bit flags PROT_EXEC, PROT_READ, PROT_WRITE, and PROT_NONE from <sys/mman.h>. The presence or absence
     *  of the PROT_WRITE bit here has no relation to the is_read_only() value -- it is legal for ROSE to write new values
     *  to a memory location that is mapped without PROT_WRITE, but not to a memory location where is_read_only() is true. */
    class MapElement {
    public:
        MapElement()
            : va(0), size(0), base(NULL), offset(0), read_only(false), mapperms(PROT_NONE), anonymous(NULL) {}
        
        MapElement(const MapElement &other) {
            init(other);
        }

        MapElement &operator=(const MapElement &other) {
            nullify();
            init(other);
            return *this;
        }

        ~MapElement() {
            nullify();
        }

        /** Creates a mapping relative to a memory buffer. */
        MapElement(rose_addr_t va, size_t size, void *base, rose_addr_t offset, unsigned perms=PROT_NONE)
            : va(va), size(size), base(base), offset(offset), read_only(false), mapperms(perms), anonymous(NULL) {}

        /** Create a mapping relative to a read-only memory buffer. The MemoryMap will coalesce adjacent elements having the
         *  same base when possible, but never elements having different bases. */
        MapElement(rose_addr_t va, size_t size, const void *base, rose_addr_t offset, unsigned perms=PROT_NONE)
            : va(va), size(size), base(const_cast<void*>(base)), offset(offset), read_only(true), mapperms(perms), anonymous(NULL)
            {}

        /** Creates an anonymous mapping where all addresses of the mapping are initially contain zero bytes. Note that the
         *  base address is not assigned until a write attempt is made. */
        MapElement(rose_addr_t va, size_t size, unsigned perms=PROT_NONE)
            : va(va), size(size), base(NULL), offset(0), read_only(false), mapperms(perms), anonymous(new size_t) {
            *anonymous = 0; /*no storage allocated yet for 'base'*/
        }

        /** Returns the starting virtual address for this map element. */
        rose_addr_t get_va() const {
            return va;
        }

        /** Returns the size in bytes represented by the entire map element. */
        size_t get_size() const {
            return size;
        }

        /** Returns true if the map element is anonymous. */
        bool is_anonymous() const {
            return anonymous!=NULL;
        }

        /** Returns true if the map points to read-only memory. This attribute is orthogonal to the mapping permissions
         *  returned by get_mapperms(). */
        bool is_read_only() const {
            return read_only;
        }

        /** Returns mapping permissions. The mapping permissions are orthogonal to is_read_only(). */
        unsigned get_mapperms() const {
            return mapperms;
        }

        /** Returns the buffer to which the offset applies. */
        void *get_base() const {
            return base;
        }

        /** Returns the starting offset for this map element. */
        rose_addr_t get_offset() const {
            return offset;
        }

        /** Returns the starting offset of the specified virtual address or throws an exception if the
         *  virtual address is not represented by this map element. */
        rose_addr_t get_va_offset(rose_addr_t va) const;

        /** Returns true if this element is consistent with the @p other element. Consistent map elements can be merged when
         *  they are adjacent or overlapping with one another. Elements are not consistent if they have different base
         *  addresses or different permissions.  If the base addresses are the same, elements are not consistent if the
         *  difference in starting virtual addresses is not equal to the difference in offsets. */
        bool consistent(const MapElement &other) const;

        /** Attempts to merge the @p other element with this one.  Returns true if the elements can be merged; false if they
         *  cannot. If the two elements overlap but are inconsistent then an Inconsistent exception is thrown. */
        bool merge(const MapElement &other);

    private:
        friend class MemoryMap;

        /* Initialize this element using data from another element. */
        void init(const MapElement &other) {
            va = other.va;
            size = other.size;
            anonymous = other.anonymous;
            base = other.base;
            offset = other.offset;
            read_only = other.read_only;
            mapperms = other.mapperms;
            if (anonymous && *anonymous>0)
                (*anonymous)++;
        }

        /** Make this a null mapping, releasing any anonymous memory that might be referenced. */
        void nullify() {
            if (anonymous  && *anonymous==1) {
                delete anonymous;
                delete[] (uint8_t*)base;
            } else if (anonymous && *anonymous>1) {
                (*anonymous)--;
            }
            va = 0;
            size = 0;
            anonymous = NULL;
            base = NULL;
            offset = 0;
            read_only = 0;
            mapperms = PROT_NONE;
        }

        /** Helper function for merge() when this and @p other element are both anonymous. This method will allocate storage
         *  for this anonymous element if necessary and initialize it with the contents of the @p other element. The @p
         *  oldsize argument is the size of this element before we merged it with the other. */
        void merge_anonymous(const MapElement &other, size_t oldsize);

        rose_addr_t va;                 /**< Virtual address for start of region */
        size_t size;                    /**< Number of bytes in region */
        mutable void *base;             /**< The buffer to which 'offset' applies */
        rose_addr_t offset;             /**< Offset with respect to 'base' */
        bool read_only;                 /**< If set then write() is not allowed */
        unsigned mapperms;              /**< Mapping permissions (PROT_READ, PROT_WRITE, and PROT_EXEC from <sys/mman.h>) */

        /** If non-null then the element describes an anonymous mapping, one that is initially all zero.  The 'base' data
         *  member in this case will initially be NULL and will be allocated when a MemoryMap::write() modifies the anonymous
         *  region. When 'base' is allocated, then anonymous will point to a one and will be incremented each time the element
         *  is copied and decremented when the element is overwritten or destroyed.  Each allocated 'base' will have it's own
         *  allocated reference counter. */
        size_t *anonymous;
    };

    /** Exceptions for MemoryMap operations. */
    struct Exception {
        Exception(const MemoryMap *map)
            : map(map) {}
        const MemoryMap *map;           /**< Map that caused the exception if the map is available (null otherwise). */
    };

    /** Exception for an inconsistent mapping. The @p a and @p b are the map elements that are in conflict. For an insert()
     *  operation, the @p a is the element being inserted and @p b is the existing element that's in conflict. Note that the
     *  map may have already been partly modified before the exception is thrown [FIXME: RPM 2009-08-20]. */
    struct Inconsistent : public Exception {
        Inconsistent(const MemoryMap *map, const MapElement &a, const MapElement &b)
            : Exception(map), a(a), b(b) {}
        MapElement a, b;
    };

    /** Exception for when we try to access a virtual address that isn't mapped. */
    struct NotMapped : public Exception {
        NotMapped(const MemoryMap *map, rose_addr_t va)
            : Exception(map), va(va) {}
        rose_addr_t va;
    };

    /** Exception thrown by find_free() when there's not enough free space left. */
    struct NoFreeSpace : public Exception {
        NoFreeSpace(const MemoryMap *map, size_t size)
            : Exception(map), size(size) {}
        size_t size;
    };

    MemoryMap() : sorted(false) {}

    /** Insert the specified map element. */
    void insert(MapElement elmt);

    /** Erase parts of the mapping that correspond to the specified virtual address range. The addresses to be erased don't
     *  necessarily need to correspond to a similar add() call; for instance, it's possible to add a large address space and
     *  then erase parts of it to make holes. */
    void erase(MapElement elmt);

    /** Search for the specified virtual address and return the map element that contains it. Returns null if the
     *  address is not mapped. */
    const MapElement* find(rose_addr_t va) const;

    /** Search for free space in the mapping.  This is done by looking for the lowest possible address not less than @p
     *  start_va and with the specified alignment where there are at least @p size free bytes. */
    rose_addr_t find_free(rose_addr_t start_va, size_t size, rose_addr_t mem_alignment=1) const;

    /** Returns the currently defined map elements. */
    const std::vector<MapElement> &get_elements() const;

    /** Prunes the map elements by removing those for which @p predicate returns true. */
    void prune(bool(*predicate)(const MapElement&));

    /** Copies data from a contiguous region of the virtual address space into a user supplied buffer. The portion of the
     *  virtual address space to copy begins at @p start_va and continues for @p desired bytes. The data is copied into the @p
     *  dst_buf buffer. The return value is the number of bytes that were copied, which might be fewer than the number of
     *  bytes desired if the mapping does not include part of the address space requested. The @p dst_buf bytes that do not
     *  correpond to mapped virtual addresses will be zero filled. */
    size_t read(void *dst_buf, rose_addr_t start_va, size_t desired) const;

    /** Copies data from a supplied buffer into the specified virtual addresses.  If part of the destination address space is
     *  not mapped, then all bytes up to that location are copied and no additional bytes are copied.  The write is also
     *  aborted early if a map element is marked read-only.  The return value is the number of bytes copied. */
    size_t write(const void *src_buf, rose_addr_t start_va, size_t size) const;

    /** Prints the contents of the map for debugging. */
    void dump(FILE*, const char *prefix="") const;

private:
    /* Mutable because some constant methods might sort the elements. */
    mutable bool sorted;
    mutable std::vector<MapElement> elements; /*only lazily sorted*/
};

inline bool operator<(const MemoryMap::MapElement &a, const MemoryMap::MapElement &b) {
    return a.get_va() < b.get_va();
}

#endif
