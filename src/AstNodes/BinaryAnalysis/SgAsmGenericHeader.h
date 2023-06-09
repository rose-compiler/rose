#include <Rose/BinaryAnalysis/ByteOrder.h>

/** Base class for container file headers. */
class SgAsmGenericHeader: public SgAsmGenericSection {
    /** Property: General info about the executable format. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmGenericFormat* exec_format = createAndParent<SgAsmGenericFormat>(this);

    /** Property: Optional magic number in file byte order. */
    [[using Rosebud: rosetta, large]]
    SgCharList magic;

    /** Property: Machine for which this header and its sections, etc. was compiled. */
    [[using Rosebud: rosetta]]
    SgAsmGenericFormat::InsSetArchitecture isa = SgAsmGenericFormat::ISA_UNSPECIFIED;

    /** Property: Base virtual address used by all relative virtual addresses. */
    [[using Rosebud: rosetta]]
    rose_addr_t base_va = 0;

    /** Property: Code entry point wrt base virtual address. */
    [[using Rosebud: rosetta, large]]
    SgRVAList entry_rvas;

    /** Property: List of dynamically linked libraries. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmGenericDLLList* dlls = createAndParent<SgAsmGenericDLLList>(this);

    /** Property: List of file sections. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmGenericSectionList* sections = createAndParent<SgAsmGenericSectionList>(this);

public:
    /** Constructor.
     *
     *  Headers (@ref SgAsmGenericHeader and derived classes) set the file/header relationship--a bidirectional link
     *  between this new header and the single file that contains this new header. This new header points to its file and
     *  the file contains a list that points to this new header. The header-to-file half of the link is deleted by the
     *  default destructor by virtue of being a simple pointer, but we also need to delete the other half of the link in
     *  the destructors. */
    explicit SgAsmGenericHeader(SgAsmGenericFile*);

    /** Allow all sections to reallocate themselves */
    virtual bool reallocate() override;

    /** Unparse headers and all they point to */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging info. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

    /** Returns the name of the file format. */
    virtual const char *format_name() const;

    /** Add a new DLL to the header DLL list */
    void add_dll(SgAsmGenericDLL *dll);

    /** Vector of dynamically loaded libraries. */
    std::vector<SgAsmGenericDLL*>& get_dlls() {
        ROSE_ASSERT(p_dlls != NULL);
        return p_dlls->get_dlls();
    }

    /** Returns the RVA of the first entry point.
     *
     *  The return value is relative to the header's base virtual address. If there are no entry points defined then
     *  returns a zero RVA. */
    rose_addr_t get_entry_rva() const;

    /** Append an RVA to the list of entry points. */
    void add_entry_rva(const rose_rva_t &rva) {
        p_entry_rvas.push_back(rva);
    }

    /* Convenience functions */
    Rose::BinaryAnalysis::ByteOrder::Endianness get_sex() const;
    size_t get_word_size() const;

    /** Adds a new section to the header.
     *
     *  This is called implicitly by the section constructor. */
    void add_section(SgAsmGenericSection*);

    /** Removes a secton from the header's section list. */
    void remove_section(SgAsmGenericSection*);

    /** Returns the list of sections that are memory mapped */
    SgAsmGenericSectionPtrList get_mapped_sections() const;

    /** Returns sections in this header that have the specified ID. */
    SgAsmGenericSectionPtrList get_sections_by_id(int id) const;

    /** Returns sections in this header that have the specified name.
     *
     *  If @p sep is a non-null string then ignore any part of name at and after @p sep. */
    SgAsmGenericSectionPtrList get_sections_by_name(std::string, char sep=0) const;

    /** Returns sectons in this header that contain all of the specified portion of the file. */
    SgAsmGenericSectionPtrList get_sections_by_offset(rose_addr_t offset, rose_addr_t size) const;

    /** Returns sections that have a preferred mapping that includes the specified relative virtual address. */
    SgAsmGenericSectionPtrList get_sections_by_rva(rose_addr_t rva) const;

    /** Returns sections having a preferred or actual mapping that includes the specified virtual address.
     *
     *  If @p use_preferred is set, then the condition is evaluated by looking at the section's preferred mapping,
     *  otherwise the actual mapping is used.  If an actual mapping is used, the specified virtual address must be part of
     *  the actual mapped section, not merely in the memory region that was also mapped to satisfy alignment
     *  constraints. */
    SgAsmGenericSectionPtrList get_sections_by_va(rose_addr_t va, bool use_preferred) const;

    /** Returns single section in this header that has the specified ID. */
    SgAsmGenericSection *get_section_by_id(int id, size_t *nfound=0) const;

    /** Returns single section in this header that has the specified name. */
    SgAsmGenericSection *get_section_by_name(const std::string&, char sep=0, size_t *nfound=0) const;

    /** Returns single section in this header that contains all of the specified portion of the file. */
    SgAsmGenericSection *get_section_by_offset(rose_addr_t offset, rose_addr_t size, size_t *nfound=0) const;

    /** Returns the single section having a preferred mapping that includes the specified relative virtual address.
     *
     *  If there are no sections or multiple sections satisfying this condition then a null pointer is returned. */
    SgAsmGenericSection *get_section_by_rva(rose_addr_t rva, size_t *nfound=0) const;

    /** Returns the section having a preferred or actual mapping that includes the specified virtual address.
     *
     *  If @p use_preferred is set, then the condition is evaluated by looking at the section's preferred mapping,
     *  otherwise the actual mapping is used. If an actual mapping is used, the specified virtual address must be part of
     *  the actual mapped section, not merely in the memory region that was also mapped to satisfy alignment constraints.
     *  If there are no sections or multiple sections satisfying this condition then a null pointer is returned. */
    SgAsmGenericSection *get_section_by_va(rose_addr_t va, bool use_preferred, size_t *nfound=0) const;

    /** Like SgAsmGenericFile::get_best_section_by_va() except considers only sections defined in this header. */
    SgAsmGenericSection *get_best_section_by_va(rose_addr_t va, bool use_preferred, size_t *nfound=0) const;

protected:
    virtual void destructorHelper() override;
};
