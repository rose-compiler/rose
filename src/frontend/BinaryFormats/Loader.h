#ifndef ROSE_LOADER_H
#define ROSE_LOADER_H

/* Increase ADDR if necessary to make it a multiple of ALMNT */
#define ALIGN_UP(ADDR,ALMNT)       ((((ADDR)+(ALMNT)-1)/(ALMNT))*(ALMNT))
#define ALIGN_DN(ADDR,ALMNT)       (((ADDR)/(ALMNT))*(ALMNT))


/** This class is the interface for binary file loaders.
 *
 *  A Loader is reponsible for constructing a MemoryMap describing a
 *  mapping from virtual memory to file content and emulates the actions taken by real-world loaders. Each real-world loader
 *  will be represented as a subclass of Loader. The loaders are registered with ROSE by calling register_loader(), making
 *  them available to modules such as the instruction disassembler.
 *
 *  For example, given a Windows PE executable containing a DOS executable and a PE executable, this is how one would create a
 *  mapping to describe all the code-containing sections that are part of the PE executable:
 *
 *  @code
 *  SgAsmPEFileHeader *pe_header = ...;
 *  Loader *loader = Loader::find_loader(pe_header); // Get the best loader for PE
 *  MemoryMap *map = loader->map_code_sections(pe_header);
 *  @endcode
 *
 *  The high-level functions, map_all_sections(), map_code_sections(), map_executable_sections(), map_writable_sections(), and
 *  possibly others in the future each have three overloaded versions: one that selects sections from the entire file, one
 *  that selects sections belonging to a particular binary header, and one that selects sections from a specified list.  These
 *  are all declared virtual so that a subclass can provide new definitions, although normally only the last version (the one
 *  that takes a list of sections) is overridden since the other two are defined in terms of it.
 *
 *  The high-level functions then call create_map(), supplying a local class using the Selector interface to describe which
 *  specific sections contribute to the final mapping and how they contribute.  The sections are sorted (see order_sections())
 *  and then each contributing section is then run through the align_values() method to satisfy alignment constraints, choose
 *  new virtual addresses, and/or eliminate the section from consideration.
 *
 *  Finally, once values are aligned addresses are added to or removed from the MemoryMap that is eventually returned.  For
 *  each section that is added to the memory map, the section's starting address is assigned with
 *  SgAsmGenericSection::set_mapped_actual_rva().
 */
class Loader {
public:
    Loader(): p_debug(NULL) { ctor(); }
    virtual ~Loader() {}

    /*------------------------------------------------------------------------------------------------------------------------
     * Methods for registering and looking up loader implementations.
     *------------------------------------------------------------------------------------------------------------------------*/
public:

    /** Class method to register a loader.  This allows users to define loaders at runtime and register them with ROSE.  When
     *  loader functionality is needed by the parser, disassembler, etc., ROSE will try each loader in the reverse order they
     *  were registered until one indicates that it can handle the request. A loader indicates it can handle a request when its
     *  can_handle() method returns true.  The loader's built into ROSE are also registered automatically by ROSE. */
    static void register_subclass(Loader*);

    /** Class method to find a registered loader supporting the specified file header.
     *
     *  This method will call the can_handle()
     *  method of each registered loader, in reverse order of their registration, until one returns true. */
    static Loader *find_loader(SgAsmGenericHeader*);

    /** Indicates whether a loader can handle a specific type of file.
     *
     *  Returns true if the loader is able to handle the specified file header.  For instance, the LoaderELFObj loader would
     *  return true if the header is for an ELF object file. The generic Loader advertises that it can handle all types of
     *  file headers, so subclasses should definitely override this. */
    virtual bool can_handle(SgAsmGenericHeader*) { return true; }

    /** Class method to register ROSE built-in loaders. This is called automatically by register_subclass(). */
    static void initclass();

    /*------------------------------------------------------------------------------------------------------------------------
     * High-level functions for mapping.
     *------------------------------------------------------------------------------------------------------------------------*/
public:

    /** Creates a map containing all mappable sections in the file. If @p map is non-null then it will be modified in place
     *  and returned, otherwise a new map is created. */
    virtual MemoryMap *map_all_sections(MemoryMap *map, SgAsmGenericFile *file, bool allow_overmap=true) {
        return map_all_sections(map, file->get_sections(), allow_overmap);
    }

    /** Creates a map containing all mappable sections in a file header. If @p map is non-null then it will be modified in place
     *  and returned, otherwise a new map is created. */
    virtual MemoryMap *map_all_sections(MemoryMap *map, SgAsmGenericHeader *fhdr, bool allow_overmap=true) {
        return map_all_sections(map, fhdr->get_sections()->get_sections(), allow_overmap);
    }

    /** Creates a map containing the specified sections (if they are mapped). If @p map is non-null then it will be modified
     *  in place and returned, otherwise a new map is created. */
    virtual MemoryMap *map_all_sections(MemoryMap*, const SgAsmGenericSectionPtrList &sections, bool allow_overmap=true);



    /** Creates a map for all code-containing sections in the file. If @p map is non-null then it will be modified in place
     *  and returned, otherwise a new map is created. */
    virtual MemoryMap *map_code_sections(MemoryMap *map, SgAsmGenericFile *file, bool allow_overmap=true) {
        return map_code_sections(map, file->get_sections(), allow_overmap);
    }

    /** Creates a map for all code-containing sections reachable from the file header. If @p map is non-null then it will be
     *  modified in place and returned, otherwise a new map is created. */
    virtual MemoryMap *map_code_sections(MemoryMap *map, SgAsmGenericHeader *fhdr, bool allow_overmap=true) {
        return map_code_sections(map, fhdr->get_sections()->get_sections(), allow_overmap);
    }

    /** Creates a map for all code-containing sections from the specified list. If @p map is non-null then it will be
     *  modified in place and returned, otherwise a new map is created. */
    virtual MemoryMap *map_code_sections(MemoryMap*, const SgAsmGenericSectionPtrList &sections, bool allow_overmap=true);



    /** Creates a map for all executable sections in the file. If @p map is non-null then it will be
     *  modified in place and returned, otherwise a new map is created. */
    virtual MemoryMap *map_executable_sections(MemoryMap *map, SgAsmGenericFile *file, bool allow_overmap=true) {
        return map_executable_sections(map, file->get_sections(), allow_overmap);
    }

    /** Creates a map for all executable sections reachable from the file header. If @p map is non-null then it will be
     *  modified in place and returned, otherwise a new map is created. */
    virtual MemoryMap *map_executable_sections(MemoryMap *map, SgAsmGenericHeader *fhdr, bool allow_overmap=true) {
        return map_executable_sections(map, fhdr->get_sections()->get_sections(), allow_overmap);
    }

    /** Creates a map for all executable sections from the specified list. If @p map is non-null then it will be
     *  modified in place and returned, otherwise a new map is created. */
    virtual MemoryMap *map_executable_sections(MemoryMap*, const SgAsmGenericSectionPtrList &sections, bool allow_overmap=true);



    /** Creates a map for all writable sections in the file. If @p map is non-null then it will be
     *  modified in place and returned, otherwise a new map is created. */
    virtual MemoryMap *map_writable_sections(MemoryMap *map, SgAsmGenericFile *file, bool allow_overmap=true) {
        return map_writable_sections(map, file->get_sections(), allow_overmap);
    }

    /** Creates a map for all writable sections reachable from the file header. If @p map is non-null then it will be
     *  modified in place and returned, otherwise a new map is created. */
    virtual MemoryMap *map_writable_sections(MemoryMap *map, SgAsmGenericHeader *fhdr, bool allow_overmap=true) {
        return map_writable_sections(map, fhdr->get_sections()->get_sections(), allow_overmap);
    }

    /** Creates a map for all writable sections from the specified list. If @p map is non-null then it will be
     *  modified in place and returned, otherwise a new map is created. */
    virtual MemoryMap *map_writable_sections(MemoryMap*, const SgAsmGenericSectionPtrList &sections, bool allow_overmap=true);

    /*------------------------------------------------------------------------------------------------------------------------
     * Selectors
     *------------------------------------------------------------------------------------------------------------------------*/
public:

    enum Contribution {
        CONTRIBUTE_NONE,                /**< Section does not contribute to final mapping. */
        CONTRIBUTE_ADD,                 /**< Section is added to the mapping. */
        CONTRIBUTE_SUB                  /**< Section is subtracted from the mapping. */
    };

    /** The interface for deciding whether a section contributes to a mapping.
     *
     *  A Selector is used to decide whether a section should contribute to the mapping, and whether that contribution should
     *  be additive or subtractive.  Any section that contributes to a mapping will be passed through the align_values()
     *  method, which has an opportunity to veto the section's contribution. The Selector virtual class will be subclassed for
     *  various kinds of selections. */
    class Selector {
    public:
        virtual ~Selector() {}
        virtual Contribution contributes(SgAsmGenericSection*) = 0;
    };

    /*------------------------------------------------------------------------------------------------------------------------
     * Helper methods. These are declared virtual but are often not overridden by subclasses.
     *------------------------------------------------------------------------------------------------------------------------*/
public:

    /** Computes memory mapping addresses for a section.
     *
     *  Operating systems generally have some alignment constraints for how a file is mapped to virtual memory, and these
     *  correspond to the page size. For example, see the man page for Unix mmap().  There are three parts to the information
     *  returned:
     *
     *  First, we need to know the entire range of file offsets that are mapped. On Unix systems only entire pages of a file
     *  can be mapped. If the section begins with a partial page then we align the starting offset downward. Likewise if the
     *  section ends with a partial page we extend the range.  Both of these adjustments can increase the number of file bytes
     *  that are mapped.  The starting offset and file size are returned through the @p offset and @p file_size arguments.
     *
     *  Second, we need to know the entire range of virtual addresses that are affected.  Again, at least on Unix systems,
     *  only entire pages of memory can be mapped. We therefore make the same adjustments to memory as we did to the file,
     *  aligning the starting virtual address downward and the ending address upward. Both of these adjustments can affect the
     *  size of the virtual address region affected.  The starting address and memory size are returned through the @p va and
     *  @p mem_size arguments.  When the returned memory size is larger than the returned file size then the mapping will
     *  include an anonymous region (memory that is initialized to zero rather than file contents).
     *
     *  Finally, we need to indicate where the first byte of the section is located in memory. If we needed to make
     *  adjustments to the first affected virtual address then the start-of-section will not be the same as the first affected
     *  virtual address.  The start-of-section virtual address is the return value of this function.
     *
     *  Additionally, this function can veto a section mapping by returning a zero memory size.  This can happen when the
     *  Selector selects a section indiscriminantly (as with map_all_sections()) but there is not enough information to
     *  determine where the section should be mapped.  On the other hand, it can also define a mapping for a section that
     *  doesn't have any mapping attributes (for instance, the ELF object file loader, LoaderELFObj, maps function text
     *  sections that are marked as "not mapped" in the ELF container by choosing free regions of virtual memory using the @p
     *  current argument that contains the most up-to-date mapping). */
    virtual rose_addr_t align_values(SgAsmGenericSection*, Contribution,
                                     rose_addr_t *va,     rose_addr_t *mem_size,
                                     rose_addr_t *offset, rose_addr_t *file_size,
                                     const MemoryMap *current);

    /** Sort sections for mapping.
     *
     *  Given a list of sections, sort the sections according to the order they should contribute to the mapping.  For
     *  instance, ELF Segments should contribute to the mapping before ELF Sections.
     *
     *  This method can also be used as a first-line for excluding sections that meet certain criteria. It is called by
     *  create_map() before sections are passed through the Selector object.  The default implementation excludes any section
     *  that was synthesized by the binary parser, keeping only those that were created due to the parser encountering a
     *  description of the section in some kind of table. */
    virtual SgAsmGenericSectionPtrList order_sections(const SgAsmGenericSectionPtrList&);

    /** Creates a memory map containing sections that satisfy some constraint.
     *
     *  The sections are mapped in the order specified by order_sections(), contribute to the final mapping according to the
     *  specified Selector, and are aligned according to the rules in align_values().  If @p allow_overmap is set (the
     *  default) then any section that contributes to the map in an additive manner will first have its virtual address space
     *  removed from the map in order to prevent a MemoryMap::Inconsistent exception; otherwise a MemoryMap::Inconsistent
     *  exception will be thrown when a single virtual address would map to two different source bytes, such as two different
     *  offsets in a file.
     *
     *  If a memory map is supplied, then it will be modified in place and returned; otherwise a new map is allocated. */
    virtual MemoryMap *create_map(MemoryMap *map, const SgAsmGenericSectionPtrList&, Selector*, bool allow_overmap=true);

    /*------------------------------------------------------------------------------------------------------------------------
     * Configuration methods.
     *------------------------------------------------------------------------------------------------------------------------*/

    /** Turns debugging output on or off. If @p f is null then debugging is turned off, otherwise debugging output is sent to
     *  the specified file.  The default can be changed at the top of the Loader.C file. */
    void set_debug(FILE *f) { p_debug=f; }

    /** Returns debugging status. If debugging is off a null pointer is returned, otherwise the file to which debugging
     *  information is being emitted is returned. */
    FILE *get_debug() const { return p_debug; }

private:
    void ctor();
    static std::vector<Loader*> p_registered;
    FILE *p_debug;
};

#endif
