#include <Rose/BinaryAnalysis/Address.h>
#include <Rose/BinaryAnalysis/ByteOrder.h>
#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>
#include <sageContainer.h>
#include <SgAsmGenericFormat.h>

#ifdef ROSE_IMPL
#include <SgAsmGenericDLLList.h>
#include <SgAsmGenericSectionList.h>
#endif

class SgAsmGenericDLL;
class SgAsmGenericDLLList;

/** Base class for container file headers. */
class SgAsmGenericHeader: public SgAsmGenericSection {
public:
    /** Property: General info about the executable format. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmGenericFormat* executableFormat = createAndParent<SgAsmGenericFormat>(this);

    /** Property: Optional magic number in file byte order. */
    [[using Rosebud: rosetta, large]]
    SgCharList magic;

    /** Property: Machine for which this header and its sections, etc. was compiled. */
    [[using Rosebud: rosetta]]
    SgAsmGenericFormat::InsSetArchitecture isa = SgAsmGenericFormat::ISA_UNSPECIFIED;

    /** Property: Base virtual address used by all relative virtual addresses. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address baseVa = 0;

    /** Property: Code entry point wrt base virtual address. */
    [[using Rosebud: rosetta, large]]
    SgRVAList entryRvas;

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

    virtual bool reallocate() override;

    /** Unparse headers and all they point to */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging info. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

    /** Returns the name of the file format. */
    virtual const char *formatName() const;

    /** Add a new DLL to the header DLL list */
    void addDll(SgAsmGenericDLL *dll);

    /** Vector of dynamically loaded libraries. */
    std::vector<SgAsmGenericDLL*>& get_dlls();

    /** Returns the RVA of the first entry point.
     *
     *  The return value is relative to the header's base virtual address. If there are no entry points defined then
     *  returns a zero RVA. */
    Rose::BinaryAnalysis::Address get_entryRva() const;

    /** Append an RVA to the list of entry points. */
    void addEntryRva(const Rose::BinaryAnalysis::RelativeVirtualAddress&);

    /* Convenience functions */
    Rose::BinaryAnalysis::ByteOrder::Endianness get_sex() const;
    size_t get_wordSize() const;

    /** Adds a new section to the header.
     *
     *  This is called implicitly by the section constructor. */
    void addSection(SgAsmGenericSection*);

    /** Removes a secton from the header's section list. */
    void removeSection(SgAsmGenericSection*);

    /** Returns the list of sections that are memory mapped */
    SgAsmGenericSectionPtrList get_mappedSections() const;

    /** Returns sections in this header that have the specified ID. */
    SgAsmGenericSectionPtrList get_sectionsById(int id) const;

    /** Returns sections in this header that have the specified name.
     *
     *  If @p sep is a non-null string then ignore any part of name at and after @p sep. */
    SgAsmGenericSectionPtrList get_sectionsByName(std::string, char sep=0) const;

    /** Returns sectons in this header that contain all of the specified portion of the file. */
    SgAsmGenericSectionPtrList get_sectionsByOffset(Rose::BinaryAnalysis::Address offset, Rose::BinaryAnalysis::Address size) const;

    /** Returns sections that have a preferred mapping that includes the specified relative virtual address. */
    SgAsmGenericSectionPtrList get_sectionsByRva(Rose::BinaryAnalysis::Address rva) const;

    /** Returns sections having a preferred or actual mapping that includes the specified virtual address.
     *
     *  If @p use_preferred is set, then the condition is evaluated by looking at the section's preferred mapping,
     *  otherwise the actual mapping is used.  If an actual mapping is used, the specified virtual address must be part of
     *  the actual mapped section, not merely in the memory region that was also mapped to satisfy alignment
     *  constraints. */
    SgAsmGenericSectionPtrList get_sectionsByVa(Rose::BinaryAnalysis::Address va, bool use_preferred) const;

    /** Returns single section in this header that has the specified ID. */
    SgAsmGenericSection *get_sectionById(int id, size_t *nfound=0) const;

    /** Returns single section in this header that has the specified name. */
    SgAsmGenericSection *get_sectionByName(const std::string&, char sep=0, size_t *nfound=0) const;

    /** Returns single section in this header that contains all of the specified portion of the file. */
    SgAsmGenericSection *get_sectionByOffset(Rose::BinaryAnalysis::Address offset, Rose::BinaryAnalysis::Address size,
                                             size_t *nfound=0) const;

    /** Returns the single section having a preferred mapping that includes the specified relative virtual address.
     *
     *  If there are no sections or multiple sections satisfying this condition then a null pointer is returned. */
    SgAsmGenericSection *get_sectionByRva(Rose::BinaryAnalysis::Address rva, size_t *nfound=0) const;

    /** Returns the section having a preferred or actual mapping that includes the specified virtual address.
     *
     *  If @p use_preferred is set, then the condition is evaluated by looking at the section's preferred mapping,
     *  otherwise the actual mapping is used. If an actual mapping is used, the specified virtual address must be part of
     *  the actual mapped section, not merely in the memory region that was also mapped to satisfy alignment constraints.
     *  If there are no sections or multiple sections satisfying this condition then a null pointer is returned. */
    SgAsmGenericSection *get_sectionByVa(Rose::BinaryAnalysis::Address va, bool use_preferred, size_t *nfound=0) const;

    /** Like SgAsmGenericFile::get_best_section_by_va() except considers only sections defined in this header. */
    SgAsmGenericSection *get_bestSectionByVa(Rose::BinaryAnalysis::Address va, bool use_preferred, size_t *nfound=0) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmGenericFormat* get_exec_format() const ROSE_DEPRECATED("use get_executableFormat");
    void set_exec_format(SgAsmGenericFormat*) ROSE_DEPRECATED("use set_executableFormat");
    Rose::BinaryAnalysis::Address get_base_va() const ROSE_DEPRECATED("use get_baseVa");
    void set_base_va(Rose::BinaryAnalysis::Address) ROSE_DEPRECATED("use set_baseVa");
    SgRVAList& get_entry_rvas() ROSE_DEPRECATED("use get_entryRvas");
    const SgRVAList& get_entry_rvas() const ROSE_DEPRECATED("use get_entryRvas");
    void set_entry_rvas(const SgRVAList&) ROSE_DEPRECATED("use set_entryRvas");
    virtual const char *format_name() const ROSE_DEPRECATED("use formatName");
    void add_dll(SgAsmGenericDLL*) ROSE_DEPRECATED("use addDll");
    Rose::BinaryAnalysis::Address get_entry_rva() const ROSE_DEPRECATED("use get_entryRva");
    void add_entry_rva(const Rose::BinaryAnalysis::RelativeVirtualAddress&) ROSE_DEPRECATED("use addEntryRva");
    size_t get_word_size() const ROSE_DEPRECATED("use get_wordSize");
    void add_section(SgAsmGenericSection*) ROSE_DEPRECATED("use addSection");
    void remove_section(SgAsmGenericSection*) ROSE_DEPRECATED("use removeSection");
    SgAsmGenericSectionPtrList get_mapped_sections() const ROSE_DEPRECATED("use get_mappedSections");
    SgAsmGenericSectionPtrList get_sections_by_id(int) const ROSE_DEPRECATED("use get_sectionsById");
    SgAsmGenericSectionPtrList get_sections_by_name(std::string, char=0) const ROSE_DEPRECATED("use get_sectionsByName");
    SgAsmGenericSectionPtrList get_sections_by_offset(Rose::BinaryAnalysis::Address, Rose::BinaryAnalysis::Address) const
        ROSE_DEPRECATED("use get_sectionsByOffset");
    SgAsmGenericSectionPtrList get_sections_by_rva(Rose::BinaryAnalysis::Address) const ROSE_DEPRECATED("use get_sectionsByRva");
    SgAsmGenericSectionPtrList get_sections_by_va(Rose::BinaryAnalysis::Address, bool) const
        ROSE_DEPRECATED("use get_sectionsByVa");
    SgAsmGenericSection *get_section_by_id(int, size_t* = nullptr) const ROSE_DEPRECATED("use get_sectionById");
    SgAsmGenericSection *get_section_by_name(const std::string&, char=0, size_t* = nullptr) const
        ROSE_DEPRECATED("use get_sectionByName");
    SgAsmGenericSection *get_section_by_offset(Rose::BinaryAnalysis::Address, Rose::BinaryAnalysis::Address,
                                               size_t* = nullptr) const ROSE_DEPRECATED("use get_sectionByOffset");
    SgAsmGenericSection *get_section_by_rva(Rose::BinaryAnalysis::Address, size_t* = nullptr) const
        ROSE_DEPRECATED("use get_sectionByRva");
    SgAsmGenericSection *get_section_by_va(Rose::BinaryAnalysis::Address, bool, size_t* = nullptr) const
        ROSE_DEPRECATED("use get_sectionByVa");
    SgAsmGenericSection *get_best_section_by_va(Rose::BinaryAnalysis::Address, bool, size_t* = nullptr) const
        ROSE_DEPRECATED("use get_bestSectionByVa");
};
