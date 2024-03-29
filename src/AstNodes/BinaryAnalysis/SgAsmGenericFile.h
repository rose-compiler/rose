#include <Rose/BinaryAnalysis/MemoryMap.h>
#include "DataConversion.h"
#include "StatSerializer.h"                         // non-intrusive serialization of struct stat

/** Base class for binary files. */
class SgAsmGenericFile: public SgAsmExecutableFileFormat {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Section modification functions for @ref shift_extend. */
    enum AddressSpace {
        ADDRSP_MEMORY = 0x0001,
        ADDRSP_FILE   = 0x0002,
        ADDRSP_ALL    = 0x0003
    };

    /** Elasticity argument for @ref shift_extend. */
    enum Elasticity {
        ELASTIC_NONE  = 0,                          /**< Nothing is elastic; other parts of space are shifted. */
        ELASTIC_UNREF = 1,                          /**< Unreferenced address space is elastic. */
        ELASTIC_HOLE  = 2                           /**< Unreferenced and "hole" sections are elastic. */
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Property: DWARF debugging hiearchy. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmDwarfCompilationUnitList* dwarf_info = nullptr;

    /** Property: Name of file. */
    [[using Rosebud: rosetta]]
    std::string name;

    /** Property: Read-only file descriptor.
     *
     *  Negative represents lack of valid file descriptor. */
    [[using Rosebud: rosetta, no_serialize]]
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
    bool truncate_zeros = false;

    /** Property: Whether to track referenced areas during parsing. */
    [[using Rosebud: rosetta]]
    bool tracking_references = true;

    /** Property: Addresses referenced during parsing. */
    [[using Rosebud: rosetta]]
    AddressIntervalSet referenced_extents;

    /** Property: If set, refuse to unparse file. */
    [[using Rosebud: rosetta]]
    bool neuter = false;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Non-property data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    mutable AddressIntervalSet *p_unreferenced_cache = nullptr;
    DataConverter *p_data_converter = nullptr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Loads file contents into memory. */
    SgAsmGenericFile* parse(std::string file_name);

    /** Call this before unparsing to make sure everything is consistent. */
    void reallocate();

    /** Mirror image of parsing an executable file.
     *
     *  The result (unless the AST has been modified) should be identical to the original file.  If the file's neuter
     *  property is true, then rather than creating a binary file, the output will contain a note indicating that the
     *  neuter property is set.  This is intended to prevent ASTs that represent malicious binaries from accidently being
     *  used to create the binary. */
    void unparse(std::ostream&) const;

    /** Extend the output file by writing the last byte if it hasn't been written yet. */
    void extend_to_eof(std::ostream&) const;

    /** Print basic info about the sections of a file. */
    void dump(FILE*) const;

    /** Print text file containing all known information about a binary file.
     *
     *  If in_cwd is set, then the file is created in the current working directory rather than the directory containing
     *  the binary file (the default is to create the file in the current working directory).  If @p ext is non-null then
     *  these characters are added to the end of the binary file name. The default null pointer causes the string ".dump"
     *  to be appended to the file name. */
    void dump_all(bool in_cwd=true, const char *ext=NULL);

    /** Print text file containing all known information about a binary file. */
    void dump_all(const std::string& dumpname);

    /** Returns the parts of the file that have never been referenced. */
    const AddressIntervalSet& get_unreferenced_extents() const;

    /** Marks part of a file as having been referenced if tracking references. */
    void mark_referenced_extent(rose_addr_t start_rva, rose_addr_t size);

    /** Property: Data converter.
     *
     *  Function to encode/decode data as it's transferred to/from disk.  The default is to do no transformation.
     *
     * @{ */
    void set_data_converter(DataConverter* dc) {p_data_converter=dc;}
    DataConverter* get_data_converter() const {return p_data_converter;}
    /** @} */

    /** Returns current size of file based on section with highest ending address. */
    rose_addr_t get_current_size() const;

    /** Returns original size of file, based on file system. */
    rose_addr_t get_orig_size() const;

    /** Reads data from a file.
     *
     *  Reads up to @p size bytes of data from the file beginning at the specified byte offset (measured from the beginning
     *  of the file), placing the result in @p dst_buf, and returning the number of bytes read. If the number of bytes read
     *  is less than @p size then one of two things happen: if @p strict is true then an @ref
     *  SgAsmExecutableFileFormat::ShortRead exception is thrown; otherwise @p dst_buf is zero padded so that exactly @p
     *  size bytes are always initialized. */
    size_t read_content(rose_addr_t offset, void *dst_buf, rose_addr_t size, bool strict=true);

    /** Reads data from a file.
     *
     *  Reads up to @p size bytes of data starting at the specified (absolute) virtual address. The @p map specifies how
     *  virtual addresses are mapped to file offsets.  As bytes are read, if we encounter a virtual address that is not
     *  mapped we stop reading and do one of two things: if @p strict is set then a @ref MemoryMap::NotMapped exception is
     *  thrown; otherwise the rest of the @p dst_buf is zero filled and the number of bytes read (not filled) is
     *  returned. */
    size_t read_content(const Rose::BinaryAnalysis::MemoryMap::Ptr&, rose_addr_t va, void *dst_buf,
                        rose_addr_t size, bool strict=true);

    /** Reads a string from a file.
     *
     *  Returns the string stored at the specified (absolute) virtual address. The returned string contains the bytes
     *  beginning at the starting virtual address and continuing until we reach a NUL byte or an address which is not
     *  mapped. If we reach an address which is not mapped then one of two things happen: if @p strict is set then a @ref
     *  MemoryMap::NotMapped exception is thrown; otherwise the string is simply terminated. The returned string does not
     *  include the NUL byte. */
    std::string read_content_str(const Rose::BinaryAnalysis::MemoryMap::Ptr&, rose_addr_t va, bool strict=true);

    /** Reads a string from a file.
     *
     *  Returns the NUL-terminated string stored at the specified relative virtual address. The returned string contains
     *  the bytes beginning at the specified starting file offset and continuing until we reach a NUL byte or an invalid
     *  file offset. If we reach an invalid file offset one of two things happen: if @p strict is set (the default) then an
     *  @ref SgAsmExecutableFileFormat::ShortRead exception is thrown; otherwise the string is simply terminated. The
     *  returned string does not include the NUL byte. */
    std::string read_content_str(rose_addr_t abs_offset, bool strict=true);

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
    SgAsmGenericSectionPtrList get_mapped_sections() const;

    /** Returns list of all sections in the file (including headers, holes, etc). */
    SgAsmGenericSectionPtrList get_sections(bool include_holes=true) const;

    /** Returns sections having specified ID across all headers, including headers and holes. */
    SgAsmGenericSectionPtrList get_sections_by_id(int id) const;

    /** Returns all sections having specified name across all headers, including headers and holes. */
    SgAsmGenericSectionPtrList get_sections_by_name(std::string, char sep='\0') const;

    /** Find sections by their offset.
     *
     *  Returns all sections that contain all of the specified portion of the file across all headers, including headers
     *  and holes. */
    SgAsmGenericSectionPtrList get_sections_by_offset(rose_addr_t offset, rose_addr_t size) const;

    /** Find sections by address.
     *
     *  Returns all sections that are mapped to include the specified relative virtual address across all headers,
     *  including headers and holes. This uses the preferred mapping of the section rather than the actual mapping. */
    SgAsmGenericSectionPtrList get_sections_by_rva(rose_addr_t rva) const;

    /** Find sections by address.
     *
     *  Returns all sections that are mapped to include the specified virtual address across all headers, including headers
     *  and holes. This uses the preferred mapping rather than the actual mapping. */
    SgAsmGenericSectionPtrList get_sections_by_va(rose_addr_t va) const;

    /** Find section with specified ID.
     *
     *  Returns the pointer to section with the specified ID across all headers only if there's exactly one match. Headers
     *  and holes are included in the results. */
    SgAsmGenericSection *get_section_by_id(int id, size_t *nfound=0) const;

    /** Find section by name.
     *
     *  Returns pointer to the section with the specified name, or NULL if there isn't exactly one match. Any characters in
     *  the name after the first occurrence of SEP are ignored (default is NUL). For instance, if sep=='$' then the
     *  following names are all equivalent: .idata, .idata$, and .idata$1 */
    SgAsmGenericSection *get_section_by_name(const std::string&, char sep=0, size_t *nfound=0) const;

    /** Find section by file offset.
     *
     *  Returns single section that contains all of the specified portion of the file across all headers, including headers
     *  and holes. */
    SgAsmGenericSection *get_section_by_offset(rose_addr_t offset, rose_addr_t size, size_t *nfound=0) const;

    /** Find section by address.
     *
     *  Returns single section that is mapped to include the specified relative virtual file address across all headers,
     *  including headers and holes. */
    SgAsmGenericSection *get_section_by_rva(rose_addr_t rva, size_t *nfound=0) const;

    /** Find section by address.
     *
     *  Returns single section that is mapped to include the specified virtual address across all headers. See also
     *  @ref get_best_section_by_va. */
    SgAsmGenericSection *get_section_by_va(rose_addr_t va, size_t *nfound=0) const;

    /** Find section by address.
     *
     *  Similar to @ref get_section_by_va except when more than one section contains the specified virtual address this
     *  choose the "best" one. All candidates must map the virtual address to the same file address or else we fail (return
     *  null and number of candidates). See @ref best_section_by_va for definition of "best". */
    SgAsmGenericSection *get_best_section_by_va(rose_addr_t va, size_t *nfound=0) const;

    /** Definition for "best".
     *
     *  This is the definition of "best" as used by @ref get_best_section_by_va and @ref
     *  SgAsmGenericHeader::get_best_section_by_va.  The specified list of sections is scanned and the best one
     *  containing the specified virtual address is returned.  The operation is equivalent to the successive elimination of
     *  bad sections: first eliminate all sections that do not contain the virtual address.  If more than one remains,
     *  eliminate all but the smallest.  If two or more are tied in size and at least one has a name, eliminate those that
     *  don't have names.  If more than one section remains, return the section that is earliest in the specified list of
     *  sections.  Return the null pointer if no section contains the specified virtual address, or if any two sections
     *  that contain the virtual address map it to different parts of the underlying binary file. */
    static SgAsmGenericSection *best_section_by_va(const SgAsmGenericSectionPtrList &sections, rose_addr_t va);

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
    void shift_extend(SgAsmGenericSection*, rose_addr_t sa, rose_addr_t sn, AddressSpace, Elasticity);
    void shift_extend(SgAsmGenericSection *s, rose_addr_t sa, rose_addr_t sn) {
        shift_extend(s, sa, sn, ADDRSP_ALL, ELASTIC_UNREF);
    }
    /** @} */

    /** File offset of next section.
     *
     *  Given a file address, return the file offset of the following section.  If there is no following section then
     *  return an address of -1 (when signed) */
    rose_addr_t get_next_section_offset(rose_addr_t offset);

    /** Adds a new hole to the file.
     *
     *  This is called implicitly by the hole constructor. */
    void add_hole(SgAsmGenericSection*);

    /** Removes a hole from the list of holes in a file. */
    void remove_hole(SgAsmGenericSection*);

    /** Find holes in file and create sections to fill them.
     *
     *  Synthesizes "hole" sections to describe the parts of the file that are not yet referenced by other sections.  Note
     *  that holes are used to represent parts of the original file data, before sections were modified by walking the AST
     *  (at this time it is not possible to create a hole outside the original file content). */
    void fill_holes();

    /** Deletes "hole" sections.
     *
     *  Undoes what @ref fill_holes did. */
    void unfill_holes();

    /** Adds a new header to the file.
     *
     *  This is called implicitly by the header constructor */
    void add_header(SgAsmGenericHeader*);

    /** Removes a header from the header list in a file. */
    void remove_header(SgAsmGenericHeader*);

    /** Returns the header for the specified format. */
    SgAsmGenericHeader *get_header(SgAsmExecutableFileFormat::ExecFamily);

    /** Return a string describing the file format.
     *
     *  This uses the last header so that files like PE, NE, LE, LX, etc. which also have a DOS header report the format of
     *  the second (PE, etc.) header rather than the DOS header. */
    const char *format_name() const;

protected:
    // Some extra clean-up actions on destruction
    virtual void destructorHelper() override;
};
