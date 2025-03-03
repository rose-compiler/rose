#include <Rose/BinaryAnalysis/Address.h>
#include <Rose/BinaryAnalysis/AddressIntervalSet.h>

/** Base class for string tables. */
[[Rosebud::abstract]]
class SgAsmGenericStrtab: public SgAsmExecutableFileFormat {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    typedef std::vector<class SgAsmStringStorage*> referenced_t;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Section storing this string table. */
    [[using Rosebud: rosetta]]
    SgAsmGenericSection* container = nullptr;

    /** Property: String storage list. */
    [[using Rosebud: rosetta]]
    SgAsmGenericStrtab::referenced_t storageList;

    /** Property: Free space list.
     *
     *  This list stores space which is available for new strings.
     *
     *  @{ */
    [[using Rosebud: rosetta, large, mutators()]]
    Rose::BinaryAnalysis::AddressIntervalSet freeList;
    /** @} */

    /** Property: Space that should never be freed. */
    [[using Rosebud: rosetta]]
    SgAsmStringStorage* dontFree = nullptr;

    /** Property: Number of strings freed thus far. */
    [[using Rosebud: rosetta]]
    size_t numberFreed = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Constructs table inside file section. */
    explicit SgAsmGenericStrtab(SgAsmGenericSection *container);

    /** Parse a string table. */
    virtual SgAsmGenericStrtab* parse() {return this;}

    /** Print some debugging info. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

    /** Constructs an SgAsmStoredString from an offset into this string table. */
    SgAsmStoredString *createString(Rose::BinaryAnalysis::Address offset, bool shared);

    /** Free area of this string table that corresponds to the string currently stored.
     *
     *  Use this in preference to the offset/size version of free() when possible. */
    void free(SgAsmStringStorage*);

    /** Add a range of bytes to the free list after subtracting areas that are referenced by other strings.
     *
     *  For instance, an ELF string table can have "main" and "domain" sharing storage. If we free the "domain" string then
     *  only "do" should be added to the free list. */
    void free(Rose::BinaryAnalysis::Address offset, Rose::BinaryAnalysis::Address size); /*mark part of table as free*/

    /** Free all strings so they will be reallocated later.
     *
     *  This is more efficient than calling @ref free for each storage object. If @p blow_away_holes is true then any areas
     *  that are unreferenced in the string table will be marked as referenced and added to the free list. */
    void freeAllStrings(bool blow_away_holes=false);

    virtual void allocateOverlap(SgAsmStringStorage*);

    /** Allocates storage for strings that have been modified but not allocated.
     *
     *  First try to fit unallocated strings into free space. Any that are left will cause the string table to be
     *  extended. Returns true if the reallocation would potentially affect some other section. If @p shrink is true then
     *  release address space that's no longer needed at the end of the table. */
    bool reallocate(bool shrink);


    //These should be pure virtual but ROSETTA apparently doesn't support that (RPM 2008-10-03)
    virtual SgAsmStringStorage *createStorage(Rose::BinaryAnalysis::Address /*offset*/, bool /*shared*/);
    virtual Rose::BinaryAnalysis::Address get_storageSize(const SgAsmStringStorage*);
    virtual void rebind(SgAsmStringStorage*, Rose::BinaryAnalysis::Address);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    const referenced_t& get_storage_list() const ROSE_DEPRECATED("use get_storageList");
    void set_storage_list(const referenced_t&) ROSE_DEPRECATED("use set_storageList");
    const Rose::BinaryAnalysis::AddressIntervalSet& get_freelist() const ROSE_DEPRECATED("use get_freeList");
    Rose::BinaryAnalysis::AddressIntervalSet& get_freelist() ROSE_DEPRECATED("use set_freeList");
    SgAsmStringStorage* get_dont_free() const ROSE_DEPRECATED("use get_dontFree");
    void set_dont_free(SgAsmStringStorage*) ROSE_DEPRECATED("use set_dontFree");
    size_t get_num_freed() const ROSE_DEPRECATED("use get_numberFreed");
    void set_num_freed(size_t) ROSE_DEPRECATED("use set_numberFreed");
    SgAsmStoredString *create_string(Rose::BinaryAnalysis::Address, bool) ROSE_DEPRECATED("use createString");
    void free_all_strings(bool=false) ROSE_DEPRECATED("use freeAllStrings");
    virtual void allocate_overlap(SgAsmStringStorage*) ROSE_DEPRECATED("use allocateOverlap");
    virtual SgAsmStringStorage *create_storage(Rose::BinaryAnalysis::Address, bool) ROSE_DEPRECATED("use createStorage");
    virtual Rose::BinaryAnalysis::Address get_storage_size(const SgAsmStringStorage*) ROSE_DEPRECATED("use get_storageSize");
};
