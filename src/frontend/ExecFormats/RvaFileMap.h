#ifndef ROSE_RVA_FILE_MAP_H
#define ROSE_RVA_FILE_MAP_H

/** An RvaFileMap is an efficient mapping from relative virtual addresses to file offsets. The mapping can be built piecemeal
 *  and the data structure will coalesce adjacent memory areas (provided they are also adjacent in the file). If an attempt is
 *  made to define a mapping from one virtual address to multiple file offsets then an exception is raised. We also store a
 *  base virtual address to make computations that use non-relative virtual addresses easier. */
class RvaFileMap {
public:
    /** An RvaFileMap is composed of zero or more MapElements. Each MapElement describes the mapping from some contiguous
     *  relative virtual address space to some contiguous region of the file. */
    class MapElement {
    public:
        MapElement(rose_addr_t rva, size_t size, rose_addr_t offset)
            : rva(rva), size(size), offset(offset) {}
        rose_addr_t get_rva() const {return rva;}
        size_t get_size() const {return size;}
        rose_addr_t get_offset() const {return offset;}
    private:
        friend class RvaFileMap;
        rose_addr_t rva;                /* Virtual address for start of region */
        size_t size;                    /* Number of bytes in region */
        rose_addr_t offset;             /* File offset */
    };

    /** This object is thrown if an attempt is made to define a mapping from one relative virtual address to more than one
     *  file offset.  The "a" and "b" data members are the two mappings in conflict. */
    struct Exception {
        Exception(const MapElement &a, const MapElement &b)
            : a(a), b(b) {}
        MapElement a, b;
    };

    RvaFileMap()
        : sorted(false), base_va(0) {}
    RvaFileMap(SgAsmGenericHeader *header)
        : sorted(false), base_va(0) {
        insertMappedSections(header);
    }

    /** Returns true only if @p a and @p b are separated by the same distance in memory as they are in the file.  If @p a and
     *  @p b overlap and this function returns false, then at least one virtual address is being mapped to two file offsets. */
    static bool consistent(const MapElement &a, const MapElement &b);

    /** Inserts mappings for all mapped sections reachable by the specified file header, including the file header itself if
     *  it is mapped. */
    void insertMappedSections(SgAsmGenericHeader*);

    /** Insert the specified section. If the section is not mapped then nothing is inserted. An exception is thrown if the
     *  operation would cause a relative virtual address to be mapped to multiple file offsets. Note that if an exception is
     *  thrown this RvaFileMap might be modified anyway. */
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

    /** Search for the specified relative virtual address and return the map element that contains it. Returns null if the
     *  address is not mapped. */
    const MapElement* findRVA(rose_addr_t rva) const;

    /** Search for the specified virtual address and return the map element that contains it. Note that the map element has a
     *  _relative_ virtual address rather than a virtual address, and in order to get the virtual address the caller must add
     *  the value returned by the RvaFileMap::get_base_va() method. */
    const MapElement* findVA(rose_addr_t va) const {
        ROSE_ASSERT(va>=base_va);
        return findRVA(va-base_va);
    }

    /** Returns the currently defined map elements. */
    const std::vector<MapElement> &get_elements() const;

    /** Returns the base virtual address used to calculate the relative virtual addresses in the mappings. */
    rose_addr_t get_base_va() const {return base_va;}

    /** Copies data from a contiguous region of the virtual address space into a user supplied buffer. The portion of the
     *  virtual address space to copy begins at @p start_va and continues for @p desired bytes. The data is copied from the
     *  @p src_buf to the @p dst_buf buffers. The return value is the number of bytes that were copied, which might be fewer
     *  than the number of bytes desired if the mapping does not include part of the address space requested. */
    size_t read(unsigned char *dst_buf, const unsigned char *src_buf, rose_addr_t start_va, size_t desired) const;

private:
    /* Mutable because the find() method, while conceptually const, might sort the elements. */
    mutable bool sorted;
    mutable std::vector<MapElement> elements; /*only lazily sorted*/
    rose_addr_t base_va;
};

inline bool operator<(const RvaFileMap::MapElement &a, const RvaFileMap::MapElement &b) {
    return a.get_rva() < b.get_rva();
}

#endif
