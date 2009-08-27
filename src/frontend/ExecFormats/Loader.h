#ifndef ROSE_LOADER_H
#define ROSE_LOADER_H

/* Increase ADDR if necessary to make it a multiple of ALMNT */
#define ALIGN_UP(ADDR,ALMNT)       ((((ADDR)+(ALMNT)-1)/(ALMNT))*(ALMNT))
#define ALIGN_DN(ADDR,ALMNT)       (((ADDR)/(ALMNT))*(ALMNT))


/** This class is the interface for executable loaders. A loader is reponsible for mapping file sections into memory and
 *  emulates the actions taken by real-world loaders. Each real-world loader will be represented as a subclass of Loader. The
 *  loaders are registered with ROSE by calling register_loader(), making them available to modules such as the instruction
 *  disassembler. For example, see the LoaderPE class.
 *
 *  For example, given a Windows PE executable containing a DOS executable and a PE executable, this is how one would create a
 *  mapping to describe all the code-containing sections that are part of the PE executable:
 *
 *  @code
 *  SgAsmPEFileHeader *pe_header = ...;
 *  Loader *loader = Loader::find_loader(pe_header); // Get the best loader for PE
 *  ROSE_ASSERT(loader!=NULL);
 *  const SgAsmGenericSectionPtrList pe_sections = pe_header->get_sections()->get_sections();
 *  MemoryMap *map = loader->map_code_sections(pe_sections);
 *  @endcode
 */
class Loader {
public:
    Loader(): p_debug(NULL) {}
    virtual ~Loader() {}

    /*------------------------------------------------------------------------------------------------------------------------
     * Methods for registering and looking up loader implementations.
     *------------------------------------------------------------------------------------------------------------------------*/

    /** Class method to register a loader.  This allows users to define loaders at runtime and register them with ROSE.  When
     *  loader functionality is needed by the parser, disassembler, etc., ROSE will try each loader in the reverse order they
     *  were registered until one indicates that it can handle the request. */
    static void register_subclass(Loader*);

    /** Class method to find a registered loader supporting the specified file header. */
    static Loader *find_loader(SgAsmGenericHeader*);

    /** Returns true if the loader is able to handle the specified file header.  For instance, WindowsVista loader would
     *  return true if the header is for a PE file. The generic Loader advertises that it can handle all types of file headers,
     *  so subclasses should definitely override this. */
    virtual bool can_handle(SgAsmGenericHeader*) { return true; }

    /** Class method to register ROSE built-in loaders. This is called automatically by register_subclass(). */
    static void initclass();

    /*------------------------------------------------------------------------------------------------------------------------
     * High-level functions for mapping.
     *------------------------------------------------------------------------------------------------------------------------*/

    /** Creates a map containing all mappable sections in the file. */
    virtual MemoryMap *map_all_sections(SgAsmGenericFile *file, bool allow_overmap=true) {
        return map_all_sections(file->get_sections(), allow_overmap);
    }

    /** Creates a map containing the specified sections (if they are mapped). */
    virtual MemoryMap *map_all_sections(const SgAsmGenericSectionPtrList &sections, bool allow_overmap=true);

    /** Creates a map for all code-containing sections in the file. */
    virtual MemoryMap *map_code_sections(SgAsmGenericFile *file, bool allow_overmap=true) {
        return map_code_sections(file->get_sections(), allow_overmap);
    }

    /** Creates a map for all code-containing sections from the specified list. */
    virtual MemoryMap *map_code_sections(const SgAsmGenericSectionPtrList &sections, bool allow_overmap=true);

    /** Creates a map for all executable sections in the file. */
    virtual MemoryMap *map_executable_sections(SgAsmGenericFile *file, bool allow_overmap=true) {
        return map_executable_sections(file->get_sections(), allow_overmap);
    }

    /** Creates a map for all executable sections from the specified list. */
    virtual MemoryMap *map_executable_sections(const SgAsmGenericSectionPtrList &sections, bool allow_overmap=true);

    /** Creates a map for all writable sections in the file. */
    virtual MemoryMap *map_writable_sections(SgAsmGenericFile *file, bool allow_overmap=true) {
        return map_writable_sections(file->get_sections(), allow_overmap);
    }

    /** Creates a map for all writable sections from the specified list. */
    virtual MemoryMap *map_writable_sections(const SgAsmGenericSectionPtrList &sections, bool allow_overmap=true);

    /*------------------------------------------------------------------------------------------------------------------------
     * Selectors
     *------------------------------------------------------------------------------------------------------------------------*/
    enum Contribution {
        CONTRIBUTE_NONE,                /**< Section does not contribute to final mapping. */
        CONTRIBUTE_ADD,                 /**< Section is added to the mapping. */
        CONTRIBUTE_SUB                  /**< Section is subtracted from the mapping. */
    };

    /** A Selector is used to decide whether a section should contribute to the mapping, and whether that contribution should
     *  be additive or subtractive.  The Selector virtual class will be subclassed for various kinds of selections. */
    class Selector {
    public:
        virtual ~Selector() {}
        virtual Contribution contributes(SgAsmGenericSection*) = 0;
    };

    /*------------------------------------------------------------------------------------------------------------------------
     * Helper methods. These are declared virtual but are often not overridden by subclasses.
     *------------------------------------------------------------------------------------------------------------------------*/

    /** Given a section, return the aligned mapping address, offset, and size. The version defined in the loader class uses
     *  the section memory and file alignment values, aligning the virtual address and file offset downward and adjusting the
     *  size upward (however, we make no attempt to align the size since it's not clear whether it should be aligned according
     *  to file constraints, memory constraints, or both. */
    virtual void align_values(SgAsmGenericSection*, rose_addr_t *va, rose_addr_t *size, rose_addr_t *offset);

    /** Returns the list of sections in the file in the order they would be mapped.  This function makes no distinction between
     *  sections that would ultimately be selected and those that wouldn't. In other words, the order that sections are mapped
     *  is independent of the rules (Selector) determining how the section contributes to the mapping. */
    virtual SgAsmGenericSectionPtrList order_sections(const SgAsmGenericSectionPtrList&);

    /** Creates a memory map containing sections that satisfy some constraint. The sections are mapped in the order specified
     *  by order_sections() and contribute to the final mapping according to the specified Selector.  If @p allow_overmap is
     *  set (the default) then any section that contributes to the map in an additive manner will first have its virtual
     *  address space removed from the map in order to prevent a MemoryMap::Inconsistent exception. */
    virtual MemoryMap *create_map(const SgAsmGenericSectionPtrList&, Selector*, bool allow_overmap=true);

    /*------------------------------------------------------------------------------------------------------------------------
     * Configuration methods.
     *------------------------------------------------------------------------------------------------------------------------*/

    /** Turns debugging output on or off.  If @p f is null then debugging is turned off, otherwise debugging output is sent to
     *  the specified file. */
    void set_debug(FILE *f) { p_debug=f; }

    /** Returns debugging status.  If debugging is off a null pointer is returned, otherwise the file to which debugging
     *  information is being emitted is returned. */
    FILE *get_debug() const { return p_debug; }

private:
    static std::vector<Loader*> p_registered;
    FILE *p_debug;
};

#endif
