/** Contiguous region of a file.
 *
 *  This class represents any contiguous region of a file that serves a particular purpose. Not only are ELF and PE Sections
 *  represented by @ref SgAsmGenericSection, but also such things as file headers, segment and section tables, etc. */
class SgAsmGenericSection: public SgAsmExecutableFileFormat {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Reason for section's existence.
     *
     *  This is a union of all the section purposes from the various supported file formats.  However, at this time we're
     *  only interested in a few kinds of sections and therefore most will fall into the SP_OTHER category. */
    enum SectionPurpose {
        SP_UNSPECIFIED,                 /**< File format did not specify a reason and none could be determined. */
        SP_PROGRAM,                     /**< Program-supplied data, code, etc. */
        SP_HEADER,                      /**< Section contains a header for the executable file format. */
        SP_SYMTAB,                      /**< Symbol table. */
        SP_OTHER                        /**< File-specified purpose other than any given in this enum. */
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Property: File to which this section belongs. */
    [[using Rosebud: rosetta]]
    SgAsmGenericFile* file = nullptr;

    /** Property: File header that owns this section. */
    [[using Rosebud: rosetta]]
    SgAsmGenericHeader* header = nullptr;

    /** Property: Size of section in file in bytes.
     *
     *  This is the current file size of the section in bytes as it exists in the file.. The original size of the
     *  section (available when @ref parse is called for the function, but possibly updated while parsing) is available
     *  through the size of the original @ref p_data "data" property.
     *
     *  When ths size is set, some types of sections may do additional work. That additional work must not adjust the size
     *  of other sections or the mapping of any section (use @ref SgAsmGenericFile::resize to do that).
     *
     *  @{ */
    [[using Rosebud: rosetta, accessors(), mutators()]]
    rose_addr_t size = 0;

    rose_addr_t get_size() const;
    virtual void set_size(rose_addr_t);
    /** @} */

    /** Property: Offset to start of section in file.
     *
     *  This property holds the current file offset in bytes of this section. When setting the property, some types of
     *  sections may need to do perform additional work. That additional work must not adjust the offset of other sections,
     *  or the mapping of any section.
     *
     *  @{ */
    [[using Rosebud: rosetta, accessors(), mutators()]]
    rose_addr_t offset = 0; // Starting file offset of the section

    rose_addr_t get_offset() const;
    virtual void set_offset(rose_addr_t);
    /** @} */

    /** Property: Required file alignment.
     *
     *  Zero and one both imply byte alignment. */
    [[using Rosebud: rosetta]]
    rose_addr_t file_alignment = 0;

    /** Property: Original content of just this section.
     *
     *  Points into file's content. */
    [[using Rosebud: rosetta]]
    SgFileContentList data;

    /** Property: General contents of the section. */
    [[using Rosebud: rosetta]]
    SgAsmGenericSection::SectionPurpose purpose = SgAsmGenericSection::SP_UNSPECIFIED;

    /** Property: Whether section really exists.
     *
     *  If true, then this section was created by the format reader and not actually present in the file. */
    [[using Rosebud: rosetta]]
    bool synthesized = false;

    /** Property: Non-unique section ID or negative.
     *
     *  ELF files have unique id numbers. */
    [[using Rosebud: rosetta]]
    int id = -1;

    /** Property: Non-unique name of section.
     *
     *  The name may be the empty string. If you just want to change the name of a section use the existing name node and
     *  change its string value.  Assigning a new @ref SgAsmGenericString to the section also changes the parent of the
     *  specified string node.
     *
     *  @{ */
    [[using Rosebud: rosetta, accessors(), mutators()]]
    SgAsmGenericString* name = createAndParent<SgAsmBasicString>(this);

    SgAsmGenericString *get_name() const;
    void set_name(SgAsmGenericString *s);
    /** @} */

    /** Property: Abbreviated name.
     *
     *  Some sections have long names like "Import Address Table" that are cumbersome when they appear in assembly
     *  listings.  Therefore, each section may also have a short name.  Reading this property returns the short name
     *  if it's non-empty, otherwise the full name.
     *
     *  @{ */
    [[using Rosebud: rosetta, accessors(), mutators()]]
    std::string short_name;

    std::string get_short_name() const;
    void set_short_name(const std::string&);
    /** @} */

    /** Property: Relative virtual address where section prefers to be mapped.
     *
     *  This is the relative virtual address where the section @em prefers to be mapped. The RVA where the section will
     *  ultimately be mapped by the loader might be different due to file and/or memory alignment constraints and conflicts
     *  with other sections.
     *
     *  The virtual address is relative to the base address stored in the file header.
     *
     *  @{ */
    [[using Rosebud: rosetta, accessors(), mutators()]]
    rose_addr_t mapped_preferred_rva = 0;

    rose_addr_t get_mapped_preferred_rva() const;
    virtual void set_mapped_preferred_rva(rose_addr_t);
    /** @} */

    /** Property: Mapped size.
     *
     *  Size of section in bytes when it's mapped into virtual memory.
     *
     *  @{ */
    [[using Rosebud: rosetta, accessors(), mutators()]]
    rose_addr_t mapped_size = 0;

    rose_addr_t get_mapped_size() const;
    virtual void set_mapped_size(rose_addr_t);
    /** @} */

    /** Property: Alignment in virtual memory. */
    [[using Rosebud: rosetta]]
    rose_addr_t mapped_alignment = 0;

    /** Property: Whether mapped with read permission. */
    [[using Rosebud: rosetta]]
    bool mapped_rperm = false;

    /** Property: Whether mapped with write permission. */
    [[using Rosebud: rosetta]]
    bool mapped_wperm = false;

    /** Property: Whether mapped with execute permission. */
    [[using Rosebud: rosetta]]
    bool mapped_xperm = false;

    /** Property: Whether a section is known to contain code.
     *
     *  If a section is known to contain code then this data member is set.  Part of the process of parsing a binary file
     *  is to look at the symbol tables and if any function symbol points into a section then the entire section is marked
     *  as containing code.  This is necessary because ELF Object Files (*.o files) contain one ELF Section per function,
     *  none of which have a memory mapping specification.  By marking those sections as containing code, the LoaderELFObj
     *  class, knows that the section should be mapped to virtual memory for disassembly. */
    [[using Rosebud: rosetta]]
    bool contains_code = false;

    /** Property: Virtual address where ROSE maps this section.
     *
     *  The actual mapping is sometimes different than the preferred mapping indicated in the section table due to file
     *  and/or memory alignment constraints or conflicts with other sections.  The only place values are assigned to this
     *  data member is in the @ref BinaryLoader class and subclasses thereof.
     *
     *  The address corresponds to the latest call into the @ref BinaryLoader classes.  Depending on the loader employed,
     *  it's possible for a section to be mapped, this @c mapped_actual_va value to be set, and then some other section to
     *  be mapped over the top of all or part of the first section. In that case, the @c mapped_actual_va of the first
     *  section is not reset to zero.  The return value is not conditional upon @ref is_mapped since that predicate applies
     *  only to preferred mapping attributes. */
    [[using Rosebud: rosetta]]
    rose_addr_t mapped_actual_va = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Non-property data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    /* This is an optional local, writable pool for the p_data member. Normally a section will point into the pool
     * for its SgAsmGenericFile which is memory-mapped (read-only) from the actual file being parsed. The default
     * unparsing action is to write the original data back to the file. By allowing a section to allocate its own
     * pool for p_data we create a very easy way to get data into the unparsed file (the alternative is to derive
     * a new class and override the unparse() method). */
    unsigned char *local_data_pool;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Constructor linking object into ASt.
     *
     *  Section constructors set the optional section header relationship--a bidirectional link between this new section
     *  and its optional, single header.  This new section points to its header and the header contains a list that points
     *  to this new section.  The section-to-header part of the link is deleted by the default destructor by virtue of
     *  being a simple pointer, but we also need to delete the other half of the link in the destructors. */
    SgAsmGenericSection(SgAsmGenericFile*, SgAsmGenericHeader*);

    /** Prints info about offsets into known sections. */
    static void dump_containing_sections(FILE*, const std::string &prefix, rose_rva_t, const SgAsmGenericSectionPtrList&);

    /** Saves a reference to the original file data for a section based on the section's current offset and size.
     *
     *  Once this happens, changing the offset or size of the file will not affect the original data. The original data can
     *  be extended, however, by calling @ref extend, which is typically done during parsing. */
    void grab_content();

    // DQ (10/20/2010): Moved this function's definition to the source file.
    virtual SgAsmGenericSection* parse();

    /** Print some debugging info. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

    // Implemented in subclasses
    virtual bool reallocate() { return false; }

    /** Write a section back to the file.
     *
     *  This is the generic version that simply writes the content. Subclasses should override this. */
    virtual void unparse(std::ostream&) const;

    /** Write just the specified regions back to the file */
    void unparse(std::ostream&, const ExtentMap&) const;

    /** Write holes (unreferenced areas) back to the file */
    void unparse_holes(std::ostream&) const;

    /** Predicate determining whether this section is also a top-level file header.
     *
     *  Returns true (the associated @ref SgAsmGenericHeader pointer) if this section is a top-level file header, false
     *  (NULL) otherwise. */
    SgAsmGenericHeader *is_file_header();

    /** File offset for end of section. */
    rose_addr_t   get_end_offset() const;

    /** Extend a section by some number of bytes during the construction and/or parsing phase.
     *
     *  This is function is considered to be part of the parsing and construction of a section--it changes the part of the
     *  file that's considered the "original size" of the section. To adjust the size of a section after the executable
     *  file is parsed, see @ref SgAsmGenericFile::resize.  Sections are allowed to extend beyond the end of the file and
     *  the original data (the @ref get_data "data" property) is extended only up to the end of the file. */
    void extend(rose_addr_t nbytes);

    /** Write data to a file section.
     *
     *  @param f       Output steam to which to write
     *  @param offset  Byte offset relative to start of this section
     *  @param bufsize Size of @p buf in bytes
     *  @param buf     Buffer of bytes to be written
     *
     *  @returns Returns the section-relative byte offset for the first byte beyond what would have been written if all
     *  bytes of the buffer were written.
     *
     *  The buffer is allowed to extend past the end of the section as long as the part that extends beyond is all
     *  zeros. The zeros will not be written to the output file.  Furthermore, any trailing zeros that extend beyond the
     *  end of the file will not be written (end-of-file is determined by @ref SgAsmGenericFile::get_orig_size).
     *
     * @{ */
    rose_addr_t   write(std::ostream &f, rose_addr_t offset, size_t bufsize, const void *buf) const;
    rose_addr_t   write(std::ostream &f, rose_addr_t offset, const std::string &str) const;
    rose_addr_t   write(std::ostream &f, rose_addr_t offset, char c) const;
    rose_addr_t   write(std::ostream &f, rose_addr_t offset, const SgFileContentList &buf) const;
    rose_addr_t   write(std::ostream &f, rose_addr_t offset, const SgUnsignedCharList &buf) const;
    /** @} */

    /** Write an unsigned little-endian 128-bit value.
     *
     *  Encode an unsigned value as LEB128 and return the next offset. */
    rose_addr_t   write_uleb128(unsigned char*, rose_addr_t offset, uint64_t) const;

    /** Write a signed little-endian 128-bit value.
     *
     *  Encode an signed value as LEB128 and return the next offset. */
    rose_addr_t   write_sleb128(unsigned char*, rose_addr_t offset, int64_t) const;

    /** Reads data from a file.
     *
     *  Reads up to @p size bytes of data beginning at byte @p start_offset from the beginning of the file, placing the
     *  results in @p dst_buf and returning the number of bytes read. The return value could be smaller than @p size if the
     *  end-of-file is reached. If the return value is smaller than @p size then one of two things happen: if @p strict is
     *  set (the default) then an @ref SgAsmExecutableFileFormat::ShortRead exception is thrown; otherwise the @p dst_buf
     *  will be padded with zero bytes so that exactly @p size bytes of @p dst_buf are always initialized. */
    size_t read_content(rose_addr_t abs_offset, void *dst_buf, rose_addr_t size, bool strict=true);

    /** Reads data from a file.
     *
     *  Reads up to @p size bytes of data beginning at byte @p start (absolute or relative virtual address) in the mapped
     *  address space and placing the results in @p dst_buf and returning the number of bytes read. The return value could
     *  be smaller than @p size if the reading encounters virtual addresses that are not mapped.  When an unmapped virtual
     *  address is encountered the reading stops (even if subsequent virtual addresses are defined) and one of two things
     *  happen: if @p strict is set (the default) then an @ref MemoryMap::NotMapped exception is thrown, otherwise the @p
     *  dst_buf is padded with zeros so that all @p size bytes are initialized. The @p map is used to map virtual addresses
     *  to file offsets; if @p map is NULL then the map defined in the underlying file is used.
     *
     * @{ */
    size_t read_content(const Rose::BinaryAnalysis::MemoryMap::Ptr&, rose_addr_t start,  void *dst_buf,
                        rose_addr_t size, bool strict=true);
    size_t read_content(const Rose::BinaryAnalysis::MemoryMap::Ptr&, const rose_rva_t &start, void *dst_buf,
                        rose_addr_t size, bool strict=true);
    /** @} */

    /** Reads data from a file.
     *
     *  This behaves the same as @ref read_content except the starting offset is relative to the beginning of this section.
     *  Reading past the end of the section is not allowed and treated as a short read, and one of two things happen: if @p
     *  strict is set (the default) then an @ref SgAsmExecutableFileFormat::ShortRead exception is thrown, otherwise the
     *  result is zero padded so as to contain exactly @p size bytes. */
    size_t read_content_local(rose_addr_t rel_offset, void *dst_buf, rose_addr_t size, bool strict=true);

    /** Reads a string from the file.
     *
     *  The string begins at the specified virtual address and continues until the first NUL byte or until we reach an
     *  address that is not mapped. However, if @p strict is set (the default) and we reach an unmapped address then an
     *  @ref MemoryMap::NotMapped exception is thrown. The @p map defines the mapping from virtual addresses to file
     *  offsets; if @p map is NULL then the map defined in the underlying file is used. */
    std::string read_content_str(const Rose::BinaryAnalysis::MemoryMap::Ptr&, rose_addr_t va, bool strict=true);

    /** Reads a string from the file.
     *
     *  The string begins at the specified absolute or header-relative file offset and continues until the first NUL byte
     *  or end of file is reached. However, if @p strict is set (the default) and we reach the end-of-file then an @ref
     *  SgAsmExecutableFileFormat::ShortRead exception is thrown.
     *
     * @{ */
    std::string read_content_str(rose_addr_t abs_offset, bool strict=true);
    std::string read_content_str(const Rose::BinaryAnalysis::MemoryMap::Ptr &map, rose_rva_t rva, bool strict=true) {
        return read_content_str(map, rva.get_va(), strict);
    }
    /** @} */

    /** Reads a string from the file.
     *
     *  The string begins at the specified file offset relative to the start of this section and continues until the first
     *  NUL byte or the end of section is reached. However, if @p strict is set (the default) and we reach the
     *  end-of-section then an @ref SgAsmExecutableFileFormat::ShortRead exception is thrown. */
    std::string read_content_local_str(rose_addr_t rel_offset, bool strict=true);

    /** Reads content of a section and returns it as a container.
     *
     *  The returned container will always have exactly @p size byte.  If @p size bytes are not available in this section
     *  at the specified offset then the container will be zero padded. This method always behaves as a non-strict read. */
    SgUnsignedCharList read_content_local_ucl(rose_addr_t rel_offset, rose_addr_t size);

    /** Read a signed little-endian 128-bit value.
     *
     *  Extract a signed LEB128 value and adjust @p rel_offset according to how many bytes it occupied. If @p strict is set
     *  (the default) and the end of the section is reached then throw an @ref SgAsmExecutableFileFormat::ShortRead
     *  exception. Upon return, the @p rel_offset will be adjusted to point to the first byte after the LEB128 value. */
    int64_t read_content_local_sleb128(rose_addr_t *rel_offset, bool strict=true);

    /** Read an unsigned little-endian 128-bit value.
     *
     *  Extract an unsigned LEB128 value and adjust @p rel_offset according to how many bytes it occupied.  If @p strict is
     *  set (the default) and the end of the section is reached then throw an @ref SgAsmExecutableFileFormat::ShortRead
     *  exception. Upon return, the @p rel_offset will be adjusted to point to the first byte after the LEB128 value. */
    uint64_t read_content_local_uleb128(rose_addr_t *rel_offset, bool strict=true);

    /** Obtain a local, writable pool to hold content.
     *
     *  Sections typically point into the memory mapped, read-only file stored in the SgAsmGenericFile parent initialized
     *  by calling @ref grab_content (or indirectly by calling @ref parse).  This is also the same data which is, by
     *  default, written back out to the new file during @ref unparse.  Programs modify section content by either
     *  overriding the @ref unparse method or by modifying the @ref p_data "data" property. But in order to modify @ref
     *  p_data "data" we have to make sure that it's pointing to a read/write memory pool. This function replaces the
     *  read-only memory pool with a new one containing @p nbytes bytes of zeros. */
    unsigned char *writable_content(size_t nbytes);

    /** Returns a list of parts of a single section that have been referenced.
     *
     *  The offsets are relative to the start of the section. The tracking actually happens at the entire file level (see
     *  @ref SgAsmGenericFile::get_referenced_extents) and this function returns that same information but limits the
     *  results to this section, and returns section offsets rather than file offsets. */
    AddressIntervalSet get_referenced_extents() const;

    /** Returns a list of parts of a single section that have not been referenced.
     *
     *  The offsets are relative to the start of the section. The tracking actually happens at the entire file level
     *  (see @ref SgAsmGenericFile::get_unreferenced_extents) and this function returns that same information but
     *  limits the results to this section, and returns section offsets rather than file offsets. */
    AddressIntervalSet get_unreferenced_extents() const;

    /** Whether section desires to be mapped to memory.
     *
     *  This predicate is true iff this section has a non-zero mapped address and size. */
    bool is_mapped() const;

    /** Causes section to not be mapped to memory.
     *
     *  This method sets the mapped address and size to zero. */
    void clear_mapped();

    /** Base virtual address for a section.
     *
     *  Returns  zero if the section is not associated with a header.  This is just a convenience method to get the base
     *  virtual address of the file header that owns this section. */
    rose_addr_t get_base_va() const;

    /** Virtual address where section prefers to be mapped.
     *
     *  Returns (non-relative) virtual address if mapped, zero otherwise. See also, the @ref get_mapped_preferred_rva
     *  "mapped_preferred_rva" property. */
    rose_addr_t get_mapped_preferred_va() const;

    /** File offset for specified virtual address.
     *
     *  Returns the file offset associated with the virtual address of a mapped section. The @ref MemoryMap class is a
     *  better interface to this same information. */
    rose_addr_t get_va_offset(rose_addr_t va) const;

    /** File offset for specified relative virtual address.
     *
     *  Returns the file offset associated with the relative virtual address of a mapped section.  The @ref MemoryMap class
     *  is a better interface to this same information. */
    rose_addr_t get_rva_offset(rose_addr_t rva) const;

    /** Returns the file extent for the section.
     *
     *  The extent end points are determined by calling @ref get_offset and @ref get_size. */
    Extent get_file_extent() const;

    /** Returns the memory extent for a mapped section.
     *
     *  If the section is not mapped then offset and size will be zero. The return value is computed from the @ref
     *  get_mapped_preferred_rva "mapped_preferred_rva" and @ref get_mapped_size "mapped_size" properties. */
    Extent get_mapped_preferred_extent() const;

    /** Increase file offset and mapping address to satisfy alignment constraints.
     *
     *  This is typically done when initializing a new section. The constructor places the new section at the end of the
     *  file before it knows what the alignment constraints will be. The user should then set the alignment constraints
     *  (see @ref set_file_alignment "file_alignment" and @ref set_mapped_alignment "mapped_alignment" properties) and call
     *  this method.  This method must be called before any additional sections are appended to the file.
     *
     *  The file offset and memory mapping address are adjusted independently.
     *
     *  On the other hand, if additional sections are in the way, they must first be moved out of the way with the
     *  @ref SgAsmGenericFile::shift_extend method.
     *
     *  Returns true if the file offset and/or mapping address changed as a result of this call. */
    bool align();

protected:
    virtual void destructorHelper() override;
};
