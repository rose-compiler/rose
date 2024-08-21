#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/BinaryAnalysis/AddressIntervalSet.h>

#include "DataConversion.h"
#include "StatSerializer.h"                         // non-intrusive serialization of struct stat

#ifdef ROSE_IMPL
#include <SgAsmDwarfCompilationUnitList.h>
#include <SgAsmGenericSectionList.h>
#include <SgAsmGenericHeaderList.h>
#endif

/** Base class for binary files. */
class SgAsmGenericFile: public SgAsmExecutableFileFormat {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Section modification functions for @ref shiftExtend. */
    enum AddressSpace {
        ADDRSP_MEMORY = 0x0001,
        ADDRSP_FILE   = 0x0002,
        ADDRSP_ALL    = 0x0003
    };

    /** Elasticity argument for @ref shiftExtend. */
    enum Elasticity {
        ELASTIC_NONE  = 0,                          /**< Nothing is elastic; other parts of space are shifted. */
        ELASTIC_UNREF = 1,                          /**< Unreferenced address space is elastic. */
        ELASTIC_HOLE  = 2                           /**< Unreferenced and "hole" sections are elastic. */
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: DWARF debugging hiearchy. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmDwarfCompilationUnitList* dwarfInfo = nullptr;

    /** Property: Name of file. */
    [[using Rosebud: rosetta]]
    std::string name;

    /** Property: Read-only file descriptor.
     *
     *  Negative represents lack of valid file descriptor. */
    [[using Rosebud: rosetta, serialize()]]
    int fd = -1;

    [[using Rosebud: rosetta, mutators()]]
    struct stat sb;

    /** Property: Contents of the file. */
    [[using Rosebud: rosetta]]
    SgFileContentList data;

    /** Property: List of all headers in file.
     *
     *  This is stored as a pointer to another node that contains the list due to limitations of ROSETTA. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmGenericHeaderList* headers = createAndParent<SgAsmGenericHeaderList>(this);

    /** Property: Addresses unreferenced during parsing.
     *
     *  This is a pointer to a node that contains a list of pointers to the holes. It is done this way rather than storing
     *  the list directly, because of limitations of ROSETTA. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmGenericSectionList* holes = createAndParent<SgAsmGenericSectionList>(this);

    /** Property: Whether to truncate zeros on writes. */
    [[using Rosebud: rosetta]]
    bool truncateZeros = false;

    /** Property: Whether to track referenced areas during parsing. */
    [[using Rosebud: rosetta]]
    bool trackingReferences = true;

    /** Property: Addresses referenced during parsing. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::AddressIntervalSet referencedExtents;

    /** Property: If set, refuse to unparse file. */
    [[using Rosebud: rosetta]]
    bool neuter = false;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Non-property data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    mutable Rose::BinaryAnalysis::AddressIntervalSet *p_unreferenced_cache = nullptr;
    DataConverter *p_data_converter = nullptr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Loads file contents into memory. */
    SgAsmGenericFile* parse(std::string file_name);

    void reallocate();

    /** Mirror image of parsing an executable file.
     *
     *  The result (unless the AST has been modified) should be identical to the original file.  If the file's neuter
     *  property is true, then rather than creating a binary file, the output will contain a note indicating that the
     *  neuter property is set.  This is intended to prevent ASTs that represent malicious binaries from accidently being
     *  used to create the binary. */
    void unparse(std::ostream&) const;

    /** Extend the output file by writing the last byte if it hasn't been written yet. */
    void extendToEof(std::ostream&) const;

    /** Print basic info about the sections of a file. */
    void dump(FILE*) const;

    /** Print text file containing all known information about a binary file.
     *
     *  If in_cwd is set, then the file is created in the current working directory rather than the directory containing
     *  the binary file (the default is to create the file in the current working directory).  If @p ext is non-null then
     *  these characters are added to the end of the binary file name. The default null pointer causes the string ".dump"
     *  to be appended to the file name. */
    void dumpAll(bool in_cwd=true, const char *ext=NULL);

    /** Print text file containing all known information about a binary file. */
    void dumpAll(const std::string& dumpname);

    /** Returns the parts of the file that have never been referenced. */
    const Rose::BinaryAnalysis::AddressIntervalSet& get_unreferencedExtents() const;

    /** Marks part of a file as having been referenced if tracking references. */
    void markReferencedExtent(rose_addr_t start_rva, rose_addr_t size);

    /** Property: Data converter.
     *
     *  Function to encode/decode data as it's transferred to/from disk.  The default is to do no transformation.
     *
     * @{ */
    DataConverter* get_dataConverter() const;
    void set_dataConverter(DataConverter*);
    /** @} */

    /** Returns current size of file based on section with highest ending address. */
    rose_addr_t get_currentSize() const;

    /** Returns original size of file, based on file system. */
    rose_addr_t get_originalSize() const;

    /** Reads data from a file.
     *
     *  Reads up to @p size bytes of data from the file beginning at the specified byte offset (measured from the beginning
     *  of the file), placing the result in @p dst_buf, and returning the number of bytes read. If the number of bytes read
     *  is less than @p size then one of two things happen: if @p strict is true then an @ref
     *  SgAsmExecutableFileFormat::ShortRead exception is thrown; otherwise @p dst_buf is zero padded so that exactly @p
     *  size bytes are always initialized. */
    size_t readContent(rose_addr_t offset, void *dst_buf, rose_addr_t size, bool strict=true);

    /** Reads data from a file.
     *
     *  Reads up to @p size bytes of data starting at the specified (absolute) virtual address. The @p map specifies how virtual
     *  addresses are mapped to file offsets.  As bytes are read, if we encounter a virtual address that is not mapped we stop
     *  reading and do one of two things: if @p strict is set then a @ref Rose::BinaryAnalysis::MemoryMap::NotMapped exception is
     *  thrown; otherwise the rest of the @p dst_buf is zero filled and the number of bytes read (not filled) is returned. */
    size_t readContent(const Rose::BinaryAnalysis::MemoryMap::Ptr&, rose_addr_t va, void *dst_buf,
                       rose_addr_t size, bool strict=true);

    /** Reads a string from a file.
     *
     *  Returns the string stored at the specified (absolute) virtual address. The returned string contains the bytes beginning at
     *  the starting virtual address and continuing until we reach a NUL byte or an address which is not mapped. If we reach an
     *  address which is not mapped then one of two things happen: if @p strict is set then a @ref
     *  Rose::BinaryAnalysis::MemoryMap::NotMapped exception is thrown; otherwise the string is simply terminated. The returned
     *  string does not include the NUL byte. */
    std::string readContentString(const Rose::BinaryAnalysis::MemoryMap::Ptr&, rose_addr_t va, bool strict=true);

    /** Reads a string from a file.
     *
     *  Returns the NUL-terminated string stored at the specified relative virtual address. The returned string contains
     *  the bytes beginning at the specified starting file offset and continuing until we reach a NUL byte or an invalid
     *  file offset. If we reach an invalid file offset one of two things happen: if @p strict is set (the default) then an
     *  @ref SgAsmExecutableFileFormat::ShortRead exception is thrown; otherwise the string is simply terminated. The
     *  returned string does not include the NUL byte. */
    std::string readContentString(rose_addr_t abs_offset, bool strict=true);

    /** Property: Entire file contents. */
    const SgFileContentList& content() { return p_data; }

    /** Returns a vector that points to part of the file.
     *
     *  Returns a vector that points to part of the file content without actually ever reading or otherwise referencing the
     *  file content until the vector elements are referenced. If the desired extent falls entirely or partially outside
     *  the range of data known to the file then throw an @ref SgAsmExecutableFileFormat::ShortRead exception. This
     *  function never updates reference tracking lists for the file. */
    SgFileContentList content(rose_addr_t offset, rose_addr_t size);

    /** Returns list of all sections in the file that are memory mapped, including headers and holes. */
    SgAsmGenericSectionPtrList get_mappedSections() const;

    /** Returns list of all sections in the file (including headers, holes, etc). */
    SgAsmGenericSectionPtrList get_sections(bool include_holes=true) const;

    /** Returns sections having specified ID across all headers, including headers and holes. */
    SgAsmGenericSectionPtrList get_sectionsById(int id) const;

    /** Returns all sections having specified name across all headers, including headers and holes. */
    SgAsmGenericSectionPtrList get_sectionsByName(std::string, char sep='\0') const;

    /** Find sections by their offset.
     *
     *  Returns all sections that contain all of the specified portion of the file across all headers, including headers
     *  and holes. */
    SgAsmGenericSectionPtrList get_sectionsByOffset(rose_addr_t offset, rose_addr_t size) const;

    /** Find sections by address.
     *
     *  Returns all sections that are mapped to include the specified relative virtual address across all headers,
     *  including headers and holes. This uses the preferred mapping of the section rather than the actual mapping. */
    SgAsmGenericSectionPtrList get_sectionsByRva(rose_addr_t rva) const;

    /** Find sections by address.
     *
     *  Returns all sections that are mapped to include the specified virtual address across all headers, including headers
     *  and holes. This uses the preferred mapping rather than the actual mapping. */
    SgAsmGenericSectionPtrList get_sectionsByVa(rose_addr_t va) const;

    /** Find section with specified ID.
     *
     *  Returns the pointer to section with the specified ID across all headers only if there's exactly one match. Headers
     *  and holes are included in the results. */
    SgAsmGenericSection *get_sectionById(int id, size_t *nfound=0) const;

    /** Find section by name.
     *
     *  Returns pointer to the section with the specified name, or NULL if there isn't exactly one match. Any characters in
     *  the name after the first occurrence of SEP are ignored (default is NUL). For instance, if sep=='$' then the
     *  following names are all equivalent: .idata, .idata$, and .idata$1 */
    SgAsmGenericSection *get_sectionByName(const std::string&, char sep=0, size_t *nfound=0) const;

    /** Find section by file offset.
     *
     *  Returns single section that contains all of the specified portion of the file across all headers, including headers
     *  and holes. */
    SgAsmGenericSection *get_sectionByOffset(rose_addr_t offset, rose_addr_t size, size_t *nfound=0) const;

    /** Find section by address.
     *
     *  Returns single section that is mapped to include the specified relative virtual file address across all headers,
     *  including headers and holes. */
    SgAsmGenericSection *get_sectionByRva(rose_addr_t rva, size_t *nfound=0) const;

    /** Find section by address.
     *
     *  Returns single section that is mapped to include the specified virtual address across all headers. See also
     *  @ref get_bestSectionByVa. */
    SgAsmGenericSection *get_sectionByVa(rose_addr_t va, size_t *nfound=0) const;

    /** Find section by address.
     *
     *  Similar to @ref get_sectionByVa except when more than one section contains the specified virtual address this
     *  choose the "best" one. All candidates must map the virtual address to the same file address or else we fail (return
     *  null and number of candidates). See @ref bestSectionByVa for definition of "best". */
    SgAsmGenericSection *get_bestSectionByVa(rose_addr_t va, size_t *nfound=0) const;

    /** Definition for "best".
     *
     *  This is the definition of "best" as used by @ref get_bestSectionByVa and @ref
     *  SgAsmGenericHeader::get_bestSectionByVa.  The specified list of sections is scanned and the best one
     *  containing the specified virtual address is returned.  The operation is equivalent to the successive elimination of
     *  bad sections: first eliminate all sections that do not contain the virtual address.  If more than one remains,
     *  eliminate all but the smallest.  If two or more are tied in size and at least one has a name, eliminate those that
     *  don't have names.  If more than one section remains, return the section that is earliest in the specified list of
     *  sections.  Return the null pointer if no section contains the specified virtual address, or if any two sections
     *  that contain the virtual address map it to different parts of the underlying binary file. */
    static SgAsmGenericSection *bestSectionByVa(const SgAsmGenericSectionPtrList &sections, rose_addr_t va);

    /** Moves and enlarges a section.
     *
     *  Shifts (to a higher offset) and/or enlarges the specified section, S, taking all other sections into account. The
     *  positions of sections are based on their preferred virtual mappings rather than the actual mapping.
     *
     *  The neighborhood(S) is S itself and the set of all sections that overlap or are adjacent to the neighborhood of S,
     *  recursively.
     *
     *  The address space can be partitioned into three categories:
     *  @li Section: part of an address space that is referenced by an SgAsmGenericSection other than a "hole" section.
     *  @li Hole:    part of an address space that is referenced only by a "hole" section.
     *  @li Unref:   part of an address space that is not used by any section, including any "hole" section.
     *
     *  The last two categories define parts of the address space that can be optionally elastic--they expand or contract
     *  to take up slack or provide space for neighboring sections. This is controlled by the "elasticity" argument.
     *
     *  Note that when elasticity is ELASTIC_HOLE we simply ignore the "hole" sections, effectively removing their
     *  addresses from the range of addresses under consideration. This avoids complications that arise when a "hole"
     *  overlaps with a real section (due to someone changing offsets in an incompatible manner), but causes the hole
     *  offset and size to remain fixed.  (FIXME RPM 2008-10-20)
     *
     *  When section S is shifted by 'Sa' bytes and/or enlarged by 'Sn' bytes, other sections are affected as follows:
     *  @li Cat L:  Not affected
     *  @li Cat R: Shifted by Sa+Sn if they are in neighborhood(S). Otherwise the amount of shifting depends on the size of
     *      the hole right of neighborhood(S).
     *  @li Cat C:  Shifted Sa and enlarged Sn.
     *  @li Cat O:  If starting address are the same: Shifted Sa. If starting address not equal: Englarged Sa+Sn
     *  @li Cat I:  Shifted Sa, not enlarged
     *  @li Cat B:  Not shifted, but enlarged Sn
     *  @li Cat E:  Shifted Sa and enlarged Sn
     *
     *  Generally speaking, the AddressSpace argument should be SgAsmGenericFile::ADDRSP_ALL in order to adjust both file
     *  and memory offsets and sizes in a consistent manner.
     *
     *  To change the address and/or size of S without regard to other sections in the same file, use set_offset() and
     *  set_size() (for file address space) or set_mapped_preferred_rva() and set_mapped_size() (for memory address
     *  space).
     *
     * @{ */
    void shiftExtend(SgAsmGenericSection*, rose_addr_t sa, rose_addr_t sn, AddressSpace, Elasticity);
    void shiftExtend(SgAsmGenericSection *s, rose_addr_t sa, rose_addr_t sn);
    /** @} */

    /** File offset of next section.
     *
     *  Given a file address, return the file offset of the following section.  If there is no following section then
     *  return an address of -1 (when signed) */
    rose_addr_t get_nextSectionOffset(rose_addr_t offset);

    /** Adds a new hole to the file.
     *
     *  This is called implicitly by the hole constructor. */
    void addHole(SgAsmGenericSection*);

    /** Removes a hole from the list of holes in a file. */
    void removeHole(SgAsmGenericSection*);

    /** Find holes in file and create sections to fill them.
     *
     *  Synthesizes "hole" sections to describe the parts of the file that are not yet referenced by other sections.  Note
     *  that holes are used to represent parts of the original file data, before sections were modified by walking the AST
     *  (at this time it is not possible to create a hole outside the original file content). */
    void fillHoles();

    /** Deletes "hole" sections.
     *
     *  Undoes what @ref fillHoles did. */
    void unfillHoles();

    /** Adds a new header to the file.
     *
     *  This is called implicitly by the header constructor */
    void addHeader(SgAsmGenericHeader*);

    /** Removes a header from the header list in a file. */
    void removeHeader(SgAsmGenericHeader*);

    /** Returns the header for the specified format. */
    SgAsmGenericHeader *get_header(SgAsmExecutableFileFormat::ExecFamily);

    /** Return a string describing the file format.
     *
     *  This uses the last header so that files like PE, NE, LE, LX, etc. which also have a DOS header report the format of
     *  the second (PE, etc.) header rather than the DOS header. */
    const char *formatName() const;

protected:
    // Some extra clean-up actions on destruction
    virtual void destructorHelper() override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmDwarfCompilationUnitList* get_dwarf_info() const ROSE_DEPRECATED("use get_dwarfInfo");
    void set_dwarf_info(SgAsmDwarfCompilationUnitList*) ROSE_DEPRECATED("use set_dwarfInfo");
    bool get_truncate_zeros() const ROSE_DEPRECATED("use get_truncateZeros");
    void set_truncate_zeros(bool) ROSE_DEPRECATED("use set_truncateZeros");
    bool get_tracking_references() const ROSE_DEPRECATED("use get_trackingReferences");
    void set_tracking_references(bool) ROSE_DEPRECATED("use set_trackingReferences");
    const Rose::BinaryAnalysis::AddressIntervalSet& get_referenced_extents() const ROSE_DEPRECATED("use get_referencedExtents");
    void set_referenced_extents(const Rose::BinaryAnalysis::AddressIntervalSet&) ROSE_DEPRECATED("use set_referencedExtents");
    void extend_to_eof(std::ostream&) const ROSE_DEPRECATED("use extendToEof");
    void dump_all(bool in_cwd=true, const char *ext=NULL) ROSE_DEPRECATED("use dumpAll");
    void dump_all(const std::string& dumpname) ROSE_DEPRECATED("use dumpAll");
    const Rose::BinaryAnalysis::AddressIntervalSet& get_unreferenced_extents() const ROSE_DEPRECATED("use get_unreferencedExtents");
    void mark_referenced_extent(rose_addr_t, rose_addr_t) ROSE_DEPRECATED("use markReferencedExtent");
    DataConverter* get_data_converter() const ROSE_DEPRECATED("use get_dataConverter");
    void set_data_converter(DataConverter*) ROSE_DEPRECATED("use set_dataConverter");
    rose_addr_t get_current_size() const ROSE_DEPRECATED("use get_currentSize");
    rose_addr_t get_orig_size() const ROSE_DEPRECATED("use get_originalSize");
    size_t read_content(rose_addr_t, void*, rose_addr_t, bool=true) ROSE_DEPRECATED("use readContent");
    size_t read_content(const Rose::BinaryAnalysis::MemoryMap::Ptr&, rose_addr_t, void*, rose_addr_t, bool=true)
        ROSE_DEPRECATED("use readContent");
    std::string read_content_str(const Rose::BinaryAnalysis::MemoryMap::Ptr&, rose_addr_t, bool=true)
        ROSE_DEPRECATED("use readContentString");
    std::string read_content_str(rose_addr_t, bool=true) ROSE_DEPRECATED("use readContentString");
    SgAsmGenericSectionPtrList get_mapped_sections() const ROSE_DEPRECATED("use get_mappedSections");
    SgAsmGenericSectionPtrList get_sections_by_id(int id) const ROSE_DEPRECATED("use get_sectionById");
    SgAsmGenericSectionPtrList get_sections_by_name(std::string, char='\0') const ROSE_DEPRECATED("use get_sectionsByName");
    SgAsmGenericSectionPtrList get_sections_by_offset(rose_addr_t, rose_addr_t) const ROSE_DEPRECATED("use get_sectionsByOffset");
    SgAsmGenericSectionPtrList get_sections_by_rva(rose_addr_t) const ROSE_DEPRECATED("use get_sectionsByRva");
    SgAsmGenericSectionPtrList get_sections_by_va(rose_addr_t) const ROSE_DEPRECATED("use get_sectionsByVa");
    SgAsmGenericSection *get_section_by_id(int, size_t* = nullptr) const ROSE_DEPRECATED("use get_sectionById");
    SgAsmGenericSection *get_section_by_name(const std::string&, char=0, size_t* = nullptr) const
        ROSE_DEPRECATED("use get_sectionByName");
    SgAsmGenericSection *get_section_by_offset(rose_addr_t, rose_addr_t, size_t* = nullptr) const
        ROSE_DEPRECATED("use get_sectionByOffset");
    SgAsmGenericSection *get_section_by_rva(rose_addr_t, size_t* = nullptr) const ROSE_DEPRECATED("use get_sectionByRva");
    SgAsmGenericSection *get_section_by_va(rose_addr_t, size_t* = nullptr) const ROSE_DEPRECATED("use get_sectionByVa");
    SgAsmGenericSection *get_best_section_by_va(rose_addr_t, size_t* = nullptr) const ROSE_DEPRECATED("use get_bestSectionByVa");
    static SgAsmGenericSection *best_section_by_va(const SgAsmGenericSectionPtrList&, rose_addr_t)
        ROSE_DEPRECATED("use bestSectionByVa");
    void shift_extend(SgAsmGenericSection*, rose_addr_t, rose_addr_t, AddressSpace, Elasticity) ROSE_DEPRECATED("use shiftExtend");
    void shift_extend(SgAsmGenericSection*, rose_addr_t, rose_addr_t) ROSE_DEPRECATED("use shiftExtend");
    rose_addr_t get_next_section_offset(rose_addr_t) ROSE_DEPRECATED("use get_nextSectionOffset");
    void add_hole(SgAsmGenericSection*) ROSE_DEPRECATED("use addHole");
    void remove_hole(SgAsmGenericSection*) ROSE_DEPRECATED("use remoeHole");
    void fill_holes() ROSE_DEPRECATED("use fillHoles");
    void unfill_holes() ROSE_DEPRECATED("use unfillHoles");
    void add_header(SgAsmGenericHeader*) ROSE_DEPRECATED("use addHeader");
    void remove_header(SgAsmGenericHeader*) ROSE_DEPRECATED("use removeHeader");
    const char *format_name() const ROSE_DEPRECATED("use formatName");
};
