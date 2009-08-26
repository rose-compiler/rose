#ifndef ROSE_MEMORY_MAP_H
#define ROSE_MEMORY_MAP_H

/** A MemoryMap is an efficient mapping from virtual addresses to file (or other) offsets. The mapping can be built
 *  piecemeal and the data structure will coalesce adjacent memory areas (provided they are also adjacent in the file). If an
 *  attempt is made to define a mapping from one virtual address to multiple file offsets then an exception is raised. */
class MemoryMap {
public:
    /** A MemoryMap is composed of zero or more MapElements. Each MapElement describes the mapping from some contiguous
     *  virtual address space to some contiguous region of the file. */
    class MapElement {
    public:
        MapElement(rose_addr_t va, size_t size, rose_addr_t offset)
            : va(va), size(size), offset(offset) {}

        /** Returns the starting virtual address for this map element. */
        rose_addr_t get_va() const {return va;}

        /** Returns the size in bytes represented by the entire map element. */
        size_t get_size() const {return size;}

        /** Returns the starting offset for this map element. */
        rose_addr_t get_offset() const {return offset;}

        /** Returns the starting offset of the specified virtual address or throws an exception if the
         *  virtual address is not represented by this map element. */
        rose_addr_t get_va_offset(rose_addr_t va) const;

    private:
        friend class MemoryMap;
        rose_addr_t va;                 /* Virtual address for start of region */
        size_t size;                    /* Number of bytes in region */
        rose_addr_t offset;             /* File offset */
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

    MemoryMap() : sorted(false) {}
    MemoryMap(SgAsmGenericHeader *header) : sorted(false) {
        insertMappedSections(header);
    }

    /** Returns true only if @p a and @p b are separated by the same distance in memory as they are in the file.  If @p a and
     *  @p b overlap and this function returns false, then at least one virtual address is being mapped to two file offsets. */
    static bool consistent(const MapElement &a, const MapElement &b);

    /** Inserts mappings for all mapped sections reachable by the specified file header, including the file header itself if
     *  it is mapped. */
    void insertMappedSections(SgAsmGenericHeader*);

    /** Insert the specified section. If the section is not mapped then nothing is inserted. An exception is thrown if the
     *  operation would cause a virtual address to be mapped to multiple file offsets. Note that if an exception is
     *  thrown this MemoryMap might be modified anyway. */
    void insert(SgAsmGenericSection*);

    /** Insert the specified map element. */
    void insert(MapElement elmt);

    /** Erases parts of the mapping that correspond to the virtual addresses of the specified section.  If the section is not
     *  mapped then nothing is erased. It is possible to erase a unit of address space that was does not correspond precisely
     *  to a similar add() call. */
    void erase(SgAsmGenericSection*);

    /** Erase parts of the mapping that correspond to the specified virtual address range. The addresses to be erased don't
     *  necessarily need to correspond to a similar add() call; for instance, it's possible to add a large address space and
     *  then erase parts of it to make holes. */
    void erase(MapElement elmt);

    /** Search for the specified virtual address and return the map element that contains it. Returns null if the
     *  address is not mapped. */
    const MapElement* find(rose_addr_t va) const;

    /** Returns the currently defined map elements. */
    const std::vector<MapElement> &get_elements() const;

    /** Copies data from a contiguous region of the virtual address space into a user supplied buffer. The portion of the
     *  virtual address space to copy begins at @p start_va and continues for @p desired bytes. The data is copied from the
     *  @p src_buf to the @p dst_buf buffer. The return value is the number of bytes that were copied, which might be fewer
     *  than the number of bytes desired if the mapping does not include part of the address space requested. The part of @p
     *  dst_buf that is not read is zero filled. */
    size_t read(unsigned char *dst_buf, const unsigned char *src_buf, rose_addr_t start_va, size_t desired) const;

    /** Prints the contents of the map for debugging. */
    void dump(FILE*, const char *prefix="") const;

private:
    /* Mutable because the find() method, while conceptually const, might sort the elements. */
    mutable bool sorted;
    mutable std::vector<MapElement> elements; /*only lazily sorted*/
};

inline bool operator<(const MemoryMap::MapElement &a, const MemoryMap::MapElement &b) {
    return a.get_va() < b.get_va();
}

#endif
