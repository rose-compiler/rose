/* Generic Sections. SgAsmGenericSection serves as a base class for all binary file formats that divide a file into contiguous
 * parts.  The SgAsmGenericSection class describes such a part. Most binary formats will subclass this. */

#include "sage3basic.h"
#include "stringify.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>


/** Section constructors set the optional section header relationship--a bidirectional link between this new section and its
 *  optional, single header.  This new section points to its header and the header contains a list that points to this new
 *  section.  The section-to-header part of the link is deleted by the default destructor by virtue of being a simple pointer,
 *  but we also need to delete the other half of the link in the destructors. */
void
SgAsmGenericSection::ctor(SgAsmGenericFile *ef, SgAsmGenericHeader *hdr)
{
    ROSE_ASSERT(ef != NULL);

    ROSE_ASSERT(p_name==NULL);
    p_name = new SgAsmBasicString("");

    /* Add this section to the header's section list */
    if (hdr)
        hdr->add_section(this);
}

/* Destructor must remove section/header link */
SgAsmGenericSection::~SgAsmGenericSection()
{
    SgAsmGenericFile* ef = get_file();
    SgAsmGenericHeader *hdr = get_header();

    /* See constructor comment. This deletes both halves of the header/section link. */
    if (hdr) {
        hdr->remove_section(this);
        set_header(NULL);
    }
    
    /* FIXME: holes should probably be their own class, which would make the file/hole bidirectional linking more like the
     *        header/section bidirectional links (RPM 2008-09-02) */
    ef->remove_hole(this);

    /* Delete children */
    /*not a child*/;     p_file   = NULL;
    delete p_name;       p_name   = NULL;

    /* If the section has allocated its own local pool for the p_data member (rather than pointing into the SgAsmGenericFile)
     * then free that now. */
    if (local_data_pool!=NULL) {
        free(local_data_pool);
        local_data_pool = NULL;
    }
}

/** Increase file offset and mapping address to satisfy alignment constraints. This is typically done when initializing a new
 *  section. The constructor places the new section at the end of the file before it knows what the alignment constraints will
 *  be. The user should then set the alignment constraints (see set_file_alignment() and set_mapped_alignment()) and call this
 *  method.  This method must be called before any additional sections are appended to the file.
 *
 *  The file offset and memory mapping address are adjusted independently.
 *
 *  On the other hand, if additional sections are in the way, they must first be moved out of the way with the
 *  SgAsmGenericFile::shift_extend() method.
 *
 *  Returns true if the file offset and/or mapping address changed as a result of this call. */
bool
SgAsmGenericSection::align()
{
    bool changed = false;

    if (get_file_alignment()>0) {
        rose_addr_t old_offset = get_offset();
        rose_addr_t new_offset = ALIGN_UP(old_offset, get_file_alignment());
        set_offset(new_offset);
        changed = changed ? true : (old_offset!=new_offset);
    }

    if (is_mapped() && get_mapped_alignment()>0) {
        rose_addr_t old_rva = get_mapped_preferred_rva();
        rose_addr_t new_rva = ALIGN_UP(old_rva, get_mapped_alignment());
        set_mapped_preferred_rva(new_rva);
        changed = changed ? true : (old_rva!=new_rva);
    }

    return changed;
}

/** Saves a reference to the original file data for a section based on the sections current offset and size. Once we do this,
 *  changing the offset or size of the file will not affect the original data. The original data can be extended, however, by
 *  calling SgAsmGenericSection::extend(), which is typically done during parsing. */
void
SgAsmGenericSection::grab_content()
{
    SgAsmGenericFile *ef = get_file();
    ROSE_ASSERT(ef);

    if (get_offset()<=ef->get_orig_size()) {
        if (get_offset()+get_size()<=ef->get_orig_size()) {
            p_data = ef->content(get_offset(), get_size());
        } else {
            p_data = ef->content(get_offset(), ef->get_orig_size()-get_offset());
        }
    }
}

/** Sections typically point into the memory mapped, read-only file stored in the SgAsmGenericFile parent initialized by
 *  calling grab_content() (or indirectly by calling parse()).  This is also the same data which is, by default, written back
 *  out to the new file during unparse().  Programs modify section content by either overriding the unparse() method or by
 *  modifying the p_data values. But in order to modify p_data we have to make sure that it's pointing to a read/write memory
 *  pool. This function replaces the read-only memory pool with a new one containing @p nbytes bytes of zeros. */
unsigned char *
SgAsmGenericSection::writable_content(size_t nbytes)
{
    if (local_data_pool!=NULL)
        free(local_data_pool);
    local_data_pool = (unsigned char*)calloc(nbytes, 1);
    p_data = SgSharedVector<unsigned char>(local_data_pool, nbytes);
    return &(p_data[0]);
}

/** Accessors for section name. Setting the section name makes the SgAsmGenericString node a child of the section. */
SgAsmGenericString *
SgAsmGenericSection::get_name() const 
{
    return p_name;
}
void
SgAsmGenericSection::set_name(SgAsmGenericString *s)
{
    if (s!=p_name) {
        if (p_name) {
            p_name->set_parent(NULL);
            delete p_name;
        }
        p_name = s;
        if (p_name)
            p_name->set_parent(this);
        set_isModified(true);
    }
}

/** Returns the current file size of the section in bytes. The original size of the section (available when parse() is called
 * for the function, but possibly updated while parsing) is available through the size of the original data: p_data.size() */
rose_addr_t
SgAsmGenericSection::get_size() const
{
    return p_size;
}

/** Adjust the current size of a section. This is virtual because some sections may need to do something special. This function
 * should not adjust the size of other sections, or the mapping of any section (see SgAsmGenericFile::resize() for that). */
void
SgAsmGenericSection::set_size(rose_addr_t size)
{
    if (p_size!=size)
        set_isModified(true);
    p_size = size;
}

/** Returns current file offset of section in bytes. */
rose_addr_t
SgAsmGenericSection::get_offset() const
{
    return p_offset;
}

/** Adjust the current offset of a section. This is virtual because some sections may need to do something special. This
 * function should not adjust the offset of other sections, or the mapping of any section. */
void
SgAsmGenericSection::set_offset(rose_addr_t offset)
{
    if (p_offset!=offset)
        set_isModified(true);
    p_offset = offset;
}

/** Returns starting byte offset in the file */
rose_addr_t
SgAsmGenericSection::get_end_offset() const
{
    return get_offset() + get_size();
}

/** Returns the file extent for the section */
ExtentPair
SgAsmGenericSection::get_file_extent() const 
{
    return ExtentPair(get_offset(), get_size());
}

/** Returns whether section desires to be mapped to memory */
bool
SgAsmGenericSection::is_mapped() const
{
    return (get_mapped_preferred_rva()!=0 || get_mapped_size()!=0 ||
            get_mapped_rperm() || get_mapped_wperm() || get_mapped_xperm());
}

/** Causes section to not be mapped to memory. */
void
SgAsmGenericSection::clear_mapped()
{
    set_mapped_size(0);
    set_mapped_preferred_rva(0);
    set_mapped_actual_va(0);
    set_mapped_rperm(false);
    set_mapped_wperm(false);
    set_mapped_xperm(false);
}

/** Returns mapped size of section. */
rose_addr_t
SgAsmGenericSection::get_mapped_size() const
{
    ROSE_ASSERT(this != NULL);
    return p_mapped_size;
}

/** Resizes a mapped section without consideration of other sections that might be mapped. See also
 *  SgAsmGenericFile::mapped_resize(). */
void
SgAsmGenericSection::set_mapped_size(rose_addr_t size)
{
    ROSE_ASSERT(this != NULL);
    if (p_mapped_size!=size)
        set_isModified(true);
    p_mapped_size = size;
}

/** Returns relative virtual address w.r.t., base address of header */
rose_addr_t
SgAsmGenericSection::get_mapped_preferred_rva() const
{
    ROSE_ASSERT(this != NULL);
    return p_mapped_preferred_rva;
}

/** Moves a mapped section without consideration of other sections that might be mapped. */
void
SgAsmGenericSection::set_mapped_preferred_rva(rose_addr_t a)
{
    ROSE_ASSERT(this != NULL);
    if (p_mapped_preferred_rva!=a)
        set_isModified(true);
    p_mapped_preferred_rva = a;
}

/** Returns (non-relative) virtual address if mapped, zero otherwise. */
rose_addr_t
SgAsmGenericSection::get_mapped_preferred_va() const
{
    ROSE_ASSERT(this != NULL);
    if (is_mapped())
        return get_base_va() + get_mapped_preferred_rva();
    return 0;
}

/** Returns base virtual address for a section, or zero if the section is not associated with a header. */
rose_addr_t
SgAsmGenericSection::get_base_va() const
{
    ROSE_ASSERT(this != NULL);
    SgAsmGenericHeader *hdr = get_header();
    return hdr ? hdr->get_base_va() : 0;
}

/** Returns the memory extent for a mapped section. If the section is not mapped then offset and size will be zero */
ExtentPair
SgAsmGenericSection::get_mapped_preferred_extent() const
{
    ROSE_ASSERT(this != NULL);
    return ExtentPair(get_mapped_preferred_rva(), get_mapped_size());
}

/** Reads data from a file. Reads up to @p size bytes of data beginning at byte @p start_offset from the beginning of the file,
 *  placing the results in @p dst_buf and returning the number of bytes read. The return value could be smaller than @p size
 *  if the end-of-file is reached. If the return value is smaller than @p size then one of two things happen: if @p strict is
 *  set (the default) then an SgAsmExecutableFileFormat::ShortRead exception is thrown; otherwise the @p dst_buf will be
 *  padded with zero bytes so that exactly @p size bytes of @p dst_buf are always initialized. */
size_t
SgAsmGenericSection::read_content(rose_addr_t start_offset, void *dst_buf, rose_addr_t size, bool strict)
{
    SgAsmGenericFile *file = get_file();
    ROSE_ASSERT(file!=NULL);
    return file->read_content(start_offset, dst_buf, size, strict);
}

/** Reads data from a file. Reads up to @p size bytes of data beginning at byte @p start_rva in the mapped address space and
 *  placing the results in @p dst_buf and returning the number of bytes read. The return value could be smaller than @p size
 *  if the reading encounters virtual addresses that are not mapped.  When an unmapped virtual address is encountered the
 *  reading stops (even if subsequent virtual addresses are defined) and one of two things happen: if @p strict is set (the
 *  default) then an MemoryMap::NotMapped exception is thrown, otherwise the @p dst_buf is padded with zeros so that all @p
 *  size bytes are initialized. The @p map is used to map virtual addresses to file offsets; if @p map is NULL then the map
 *  defined in the underlying file is used. */
size_t
SgAsmGenericSection::read_content(const MemoryMap *map, rose_addr_t start_rva, void *dst_buf, rose_addr_t size, bool strict)
{
    SgAsmGenericFile *file = get_file();
    ROSE_ASSERT(file!=NULL);
    SgAsmGenericHeader *hdr = get_header();
    ROSE_ASSERT(hdr!=NULL);
    rose_addr_t va = hdr->get_base_va() + start_rva;
    return file->read_content(map, va, dst_buf, size, strict);
}

/** Reads data from a file. This behaves the same as read_content() except the @p start_offset is relative to the beginning of
 *  this section.   Reading past the end of the section is not allowed and treated as a short read, and one of two things
 *  happen: if @p strict is set (the default) then an SgAsmExecutableFileFormat::ShortRead exception is thrown, otherwise the
 *  result is zero padded so as to contain exactly @p size bytes. */
size_t
SgAsmGenericSection::read_content_local(rose_addr_t start_offset, void *dst_buf, rose_addr_t size, bool strict)
{
    size_t retval;
    SgAsmGenericFile *file = get_file();
    ROSE_ASSERT(file!=NULL);
    if (start_offset > get_size()) {
        if (strict)
            throw ShortRead(this, start_offset, size);
        retval = 0;
    } else if (start_offset+size > get_size()) {
        if (strict)
            throw ShortRead(this, get_size(), start_offset+size-get_size());
        retval = get_size() - start_offset;
    } else {
        retval = size;
    }

    file->read_content(get_offset()+start_offset, dst_buf, retval, true);
    memset((char*)dst_buf+retval, 0, size-retval);
    return retval;
}

/** Reads content of a section and returns it as a container.  The returned container will always have exactly @p size byte.
 *  If @p size bytes are not available in this section at the specified offset then the container will be zero padded. */
SgUnsignedCharList
SgAsmGenericSection::read_content_local_ucl(rose_addr_t rel_offset, rose_addr_t size)
{
    SgUnsignedCharList retval;
    unsigned char *buf = new unsigned char[size];
    read_content_local(rel_offset, buf, size, false); /*zero pads; never throws*/
    for (size_t i=0; i<size; i++)
        retval.push_back(buf[i]);
    delete[] buf;
    return retval;
}

/** Reads a string from the file. The string begins at the specified virtual address and continues until the first NUL byte or
 *  until we reach an address that is not mapped. However, if @p strict is set (the default) and we reach an unmapped address
 *  then an MemoryMap::NotMapped exception is thrown. The @p map defines the mapping from virtual addresses to file offsets;
 *  if @p map is NULL then the map defined in the underlying file is used. */
std::string
SgAsmGenericSection::read_content_str(const MemoryMap *map, rose_addr_t start_rva, bool strict)
{
    SgAsmGenericFile *file = get_file();
    ROSE_ASSERT(file!=NULL);
    SgAsmGenericHeader *hdr = get_header();
    ROSE_ASSERT(hdr!=NULL);
    rose_addr_t va = hdr->get_base_va() + start_rva;
    return file->read_content_str(map, va, strict);
}

/** Reads a string from the file. The string begins at the specified absolute file offset and continues until the first NUL
 *  byte or end of file is reached. However, if @p strict is set (the default) and we reach the end-of-file then an
 *  SgAsmExecutableFileFormat::ShortRead exception is thrown. */
std::string
SgAsmGenericSection::read_content_str(rose_addr_t abs_offset, bool strict)
{
    SgAsmGenericFile *file = get_file();
    ROSE_ASSERT(file!=NULL);
    return file->read_content_str(abs_offset, strict);
}

/** Reads a string from the file. The string begins at the specified file offset relative to the start of this section and
 *  continues until the first NUL byte or the end of section is reached. However, if @p strict is set (the default) and we
 *  reach the end-of-section then an SgAsmExecutableFileFormat::ShortRead exception is thrown. */
std::string
SgAsmGenericSection::read_content_local_str(rose_addr_t rel_offset, bool strict)
{
    static char *buf=NULL;
    static size_t nalloc=0;
    size_t nused=0;

    while (1) {
        if (nused >= nalloc) {
            nalloc = std::max((size_t)32, 2*nalloc);
            buf = (char*)realloc(buf, nalloc);
            ROSE_ASSERT(buf!=NULL);
        }

        unsigned char byte;
        read_content_local(rel_offset+nused, &byte, 1, strict);
        if (!byte)
            return std::string(buf, nused);
        buf[nused++] = byte;
    }
}

/** Extract an unsigned LEB128 value and adjust @p rel_offset according to how many bytes it occupied.  If @p strict is set
 *  (the default) and the end of the section is reached then throw an SgAsmExecutableFileFormat::ShortRead exception. Upon
 *  return, the @p rel_offset will be adjusted to point to the first byte after the LEB128 value. */
uint64_t
SgAsmGenericSection::read_content_local_uleb128(rose_addr_t *rel_offset, bool strict)
{
    int shift=0;
    uint64_t retval=0;
    while (1) {
        unsigned char byte;
        read_content_local(*rel_offset, &byte, 1, strict);
        *rel_offset += 1;
        ROSE_ASSERT(shift<64);
        retval |= (byte & 0x7f) << shift;
        shift += 7;
        if (0==(byte & 0x80))
            break;
    }
    return retval;
}

/** Extract a signed LEB128 value and adjust @p rel_offset according to how many bytes it occupied. If @p strict is set (the
 *  default) and the end of the section is reached then throw an SgAsmExecutableFileFormat::ShortRead exception. Upon return,
 *  the @p rel_offset will be adjusted to point to the first byte after the LEB128 value. */
int64_t
SgAsmGenericSection::read_content_local_sleb128(rose_addr_t *rel_offset, bool strict)
{
    int shift=0;
    int64_t retval=0;
    while (1) {
        unsigned char byte;
        read_content_local(*rel_offset, &byte, 1, strict);
        *rel_offset += 1;
        ROSE_ASSERT(shift<64);
        retval |= (byte & 0x7f) << shift;
        shift += 7;
        if (0==(byte & 0x80))
            break;
    }
    retval = (retval << (64-shift)) >> (64-shift); /*sign extend*/
    return retval;
}

/** Write data to a file section.
 *
 *   @param f       Output steam to which to write
 *   @param offset  Byte offset relative to start of this section
 *   @param bufsize Size of @p buf in bytes
 *   @param buf     Buffer of bytes to be written
 *
 *  @returns Returns the section-relative byte offset for the first byte beyond what would have been written if all bytes
 *  of the buffer were written.
 *
 *  The buffer is allowed to extend past the end of the section as long as the part that extends beyond is all zeros. The
 *  zeros will not be written to the output file.  Furthermore, any trailing zeros that extend beyond the end of the file will
 *  not be written (end-of-file is determined by SgAsmGenericFile::get_orig_size()) */
rose_addr_t
SgAsmGenericSection::write(std::ostream &f, rose_addr_t offset, size_t bufsize, const void *buf) const
{
    size_t nwrite, nzero;

    ROSE_ASSERT(this != NULL);

    /* Don't write past end of section */
    if (offset>=get_size()) {
        nwrite = 0;
        nzero  = bufsize;
    } else if (offset+bufsize<=get_size()) {
        nwrite = bufsize;
        nzero = 0;
    } else {
        nwrite = get_size() - offset;
        nzero = bufsize - nwrite;
    }

    /* Don't write past end of current EOF if we can help it. */
    f.seekp(0, std::ios::end);
    rose_addr_t filesize = f.tellp();
    while (nwrite>0 && 0==((const char*)buf)[nwrite-1] && get_offset()+offset+nwrite>filesize)
        --nwrite;

    /* Write bytes to file. This is a good place to set a break point if you're trying to figure out what section is writing
     * to a particular file address. For instance, if byte 0x7c is incorrect in the unparsed file you would set a conditional
     * breakpoint for o<=0x7c && o+nwrite>0x7c */
    ROSE_ASSERT(f);
    off_t o = get_offset() + offset;
    f.seekp(o);
    ROSE_ASSERT(f);
    f.write((const char*)buf, nwrite);
    ROSE_ASSERT(f);

    /* Check that truncated data is all zero and fail if it isn't */
    for (size_t i=nwrite; i<bufsize; i++) {
        if (((const char*)buf)[i]) {
            char mesg[1024];
            sprintf(mesg, "non-zero value truncated: buf[0x%zx]=0x%02x", i, ((const unsigned char*)buf)[i]);
            fprintf(stderr, "SgAsmGenericSection::write: error: %s", mesg);
            fprintf(stderr, " in [%d] \"%s\"\n", get_id(), get_name()->c_str());
            fprintf(stderr, "    section is at file offset 0x%08"PRIx64" (%"PRIu64"), size 0x%"PRIx64" (%"PRIu64") bytes\n", 
                    get_offset(), get_offset(), get_size(), get_size());
            fprintf(stderr, "      ");
            HexdumpFormat hf;
            hf.prefix = "      ";
            hexdump(stderr, get_offset()+offset, (const unsigned char*)buf, bufsize, hf);
            fprintf(stderr, "\n");
            throw SgAsmGenericFile::ShortWrite(this, offset, bufsize, mesg);
        }
    }

    return offset+bufsize;
}

/* See related method above */
rose_addr_t
SgAsmGenericSection::write(std::ostream &f, rose_addr_t offset, const SgFileContentList &buf) const
{
    if (0==buf.size())
        return 0;
    return write(f, offset, buf.size(), &(buf[0]));
}

/* See related method above */
rose_addr_t
SgAsmGenericSection::write(std::ostream &f, rose_addr_t offset, const SgUnsignedCharList &buf) const
{
    if (0==buf.size())
        return 0;
    return write(f, offset, buf.size(), (void*)&(buf[0]));
}

/* See related method above. */
rose_addr_t
SgAsmGenericSection::write(std::ostream &f, rose_addr_t offset, const std::string &str) const
{
    return write(f, offset, str.size(), &(str[0]));
}

/* See related method above. */
rose_addr_t
SgAsmGenericSection::write(std::ostream &f, rose_addr_t offset, char c) const
{
    return write(f, offset, 1, &c);
}

/** Encode an unsigned value as LEB128 and return the next offset. */
rose_addr_t
SgAsmGenericSection::write_uleb128(unsigned char *buf, rose_addr_t offset, uint64_t val) const
{
    if (val==0) {
        buf[offset++] = 0;
    } else {
        while (val) {
            unsigned char byte = val & 0x7f;
            val >>= 7;
            if (val!=0)
                byte |= 0x80;
            buf[offset++] = byte;
        }
    }
    return offset;
}

/** Encode a signed value as LEB128 and return the next offset. */
rose_addr_t
SgAsmGenericSection::write_sleb128(unsigned char *buf, rose_addr_t offset, int64_t val) const
{
    if (val==0) {
        buf[offset++] = 0;
    } else if (val==-1) {
        buf[offset++] = 0x7f;
    } else {
        while (val!=0 && val!=-1) {
            unsigned char byte = (uint64_t)val & 0x7f;
            val >>= 7; /*sign extending*/
            if (val!=0 && val!=-1)
                byte |= 0x80;
            buf[offset++] = byte;
        }
    }
    return offset;
}

/** Returns a list of parts of a single section that have been referenced.  The offsets are relative to the start of the
 *  section. */
ExtentMap
SgAsmGenericSection::get_referenced_extents() const
{
    ExtentMap retval;
    ExtentPair s(get_offset(), get_size());
    const ExtentMap &file_extents = get_file()->get_referenced_extents();
    for (ExtentMap::const_iterator i=file_extents.begin(); i!=file_extents.end(); i++) {
        switch (ExtentMap::category(*i, s)) {
            case 'C': /*congruent*/
            case 'I': /*extent is inside section*/
                retval.insert(i->first-get_offset(), i->second);
                break;
            case 'L': /*extent is left of section*/
            case 'R': /*extent is right of section*/
                break;
            case 'O': /*extent contains all of section*/
                retval.insert(0, get_size());
                break;
            case 'B': /*extent overlaps with beginning of section*/
                retval.insert(0, i->first+i->second - get_offset());
                break;
            case 'E': /*extent overlaps with end of section*/
                retval.insert(i->first-get_offset(), get_offset()+get_size() - i->first);
                break;
            default:
                ROSE_ASSERT(!"invalid extent overlap category");
        }
    }
    return retval;
}

ExtentMap
SgAsmGenericSection::get_unreferenced_extents() const
{
    return get_referenced_extents().subtract_from(0, get_size()); /*complement*/
}

/** Extend a section by some number of bytes during the construction and/or parsing phase. This is function is considered to
 *  be part of the parsing and construction of a section--it changes the part of the file that's considered the "original
 *  size" of the section. To adjust the size of a section after the executable file is parsed, see SgAsmGenericFile::resize().
 *  Sections are allowed to extend beyond the end of the file and the original data (p_data) is extended only up to the end
 *  of the file. */
void
SgAsmGenericSection::extend(rose_addr_t size)
{
    ROSE_ASSERT(get_file() != NULL);
    ROSE_ASSERT(get_file()->get_tracking_references()); /*can only be called during the parsing phase*/
    rose_addr_t new_size = get_size() + size;

    /* Ending file address for section using new size, limited by total file size */
    rose_addr_t new_end = std::min(get_file()->get_orig_size(), get_offset()+new_size);
    if (get_offset()<=new_end) {
        p_data.resize(new_end-get_offset());
    } else {
        ROSE_ASSERT(0==p_data.size());
    }

    if (p_size!=new_size)
        set_isModified(true);
    p_size = new_size;
}

/** True (the SgAsmGenericHeader pointer) if this section is also a top-level file header, false (NULL) otherwise. */
SgAsmGenericHeader *
SgAsmGenericSection::is_file_header()
{
    try {
        SgAsmGenericHeader *retval = dynamic_cast<SgAsmGenericHeader*>(this);
        return retval;
    } catch(...) {
        return NULL;
    }
}

/** Write a section back to the file. This is the generic version that simply writes the content. Subclasses should override
 *  this. */
void
SgAsmGenericSection::unparse(std::ostream &f) const
{
#if 0
    /* FIXME: for now we print the names of all sections we dump using this method. Eventually most of these sections will
     *        have subclasses that override this method. */
    fprintf(stderr, "SgAsmGenericSection::unparse(FILE*) for section [%d] \"%s\"\n", id, name.c_str());
#endif

    write(f, 0, p_data);
}

/** Write just the specified regions back to the file */
void
SgAsmGenericSection::unparse(std::ostream &f, const ExtentMap &map) const
{
    for (ExtentMap::const_iterator i=map.begin(); i!=map.end(); ++i) {
        ROSE_ASSERT((*i).first+(*i).second <= get_size());
        const unsigned char *extent_data;
        size_t nwrite;
        if ((*i).first >= p_data.size()) {
            extent_data = NULL;
            nwrite = 0;
        } else if ((*i).first + (*i).second > p_data.size()) {
            extent_data = &p_data[(*i).first];
            nwrite = p_data.size() - (*i).first;
        } else {
            extent_data = &p_data[(*i).first];
            nwrite = (*i).second;
        }
        if (extent_data)
            write(f, (*i).first, (*i).second, extent_data);
    }
}

/** Write holes (unreferenced areas) back to the file */
void
SgAsmGenericSection::unparse_holes(std::ostream &f) const
{
#if 0 /*DEBUGGING*/
    ExtentMap holes = get_unreferenced_extents();
    fprintf(stderr, "Section \"%s\", 0x%"PRIx64" bytes\n", get_name()->c_str(), get_size());
    holes.dump_extents(stderr, "  ", "");
#endif
//    unparse(f, get_unreferenced_extents());
}

/** Returns the file offset associated with the relative virtual address of a mapped section.
 *
 *  NOTE: The MemoryMap class is a better interface to this same information. [RPM 2009-09-09] */
rose_addr_t
SgAsmGenericSection::get_rva_offset(rose_addr_t rva) const
{
    return get_va_offset(rva + get_base_va());
}

/** Returns the file offset associated with the virtual address of a mapped section.
 *
 *  NOTE: The MemoryMap class is a better interface to this same information. [RPM 2009-09-09] */
rose_addr_t
SgAsmGenericSection::get_va_offset(rose_addr_t va) const
{
    ROSE_ASSERT(is_mapped());
    ROSE_ASSERT(va >= get_base_va());
    rose_addr_t rva = va - get_base_va();
    ROSE_ASSERT(rva >= get_mapped_preferred_rva());
    return get_offset() + (rva - get_mapped_preferred_rva());
}

/** Class method that prints info about offsets into known sections */
void
SgAsmGenericSection::dump_containing_sections(FILE *f, const std::string &prefix, rose_rva_t rva,
                                              const SgAsmGenericSectionPtrList &slist)
{
    for (size_t i=0; i<slist.size(); i++) {
        SgAsmGenericSection *s = slist[i];
        if (s->is_mapped() && rva>=s->get_mapped_preferred_rva() && rva<s->get_mapped_preferred_rva()+s->get_mapped_size()) {
            rose_addr_t offset = rva - s->get_mapped_preferred_rva();
            fprintf(f, "%-*s   is 0x%08"PRIx64" (%"PRIu64") bytes into section [%d] \"%s\"\n",
                    DUMP_FIELD_WIDTH, prefix.c_str(), offset, offset, s->get_id(), s->get_name()->c_str());
        }
    }
}

/* Print some debugging info */
void
SgAsmGenericSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sSection.", prefix);
    }
    
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = \"%s\"\n",                      p, w, "name",        p_name->c_str());
    fprintf(f, "%s%-*s = %d\n",                          p, w, "id",          p_id);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") bytes into file\n", p, w, "offset", p_offset, p_offset);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") bytes\n",           p, w, "size", get_size(), get_size());
    if (0==get_file_alignment()) {
        fprintf(f, "%s%-*s = not specified\n", p, w, "file_alignment");
    } else {
        fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") %s\n", p, w, "file_alignment", 
                get_file_alignment(), get_file_alignment(),
                0==get_offset()%get_file_alignment()?"satisfied":"NOT SATISFIED");
    }
    fprintf(f, "%s%-*s = %s\n",                          p, w, "synthesized", p_synthesized?"yes":"no");
    if (p_header) {
        fprintf(f, "%s%-*s = \"%s\"\n",                  p, w, "header",      p_header->get_name()->c_str());
    } else {
        fprintf(f, "%s%-*s = not associated\n",          p, w, "header");
    }
    
    std::string purpose = stringifySgAsmGenericSectionSectionPurpose(p_purpose);
    fprintf(f, "%s%-*s = %s\n", p, w, "purpose", purpose.c_str());

    if (is_mapped()) {
        fprintf(f, "%s%-*s = rva=0x%08"PRIx64", size=%"PRIu64" bytes\n", p, w, "mapped",  p_mapped_preferred_rva, p_mapped_size);
        if (0==get_mapped_alignment()) {
            fprintf(f, "%s%-*s = not specified\n", p, w, "mapped_alignment");
        } else {
            fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") %s\n", p, w, "mapped_alignment", 
                    get_mapped_alignment(), get_mapped_alignment(),
                    0==get_mapped_preferred_rva()%get_mapped_alignment()?"satisfied":"NOT SATISFIED");
        }
        fprintf(f, "%s%-*s = %c%c%c\n", p, w, "permissions",
                get_mapped_rperm()?'r':'-', get_mapped_wperm()?'w':'-', get_mapped_xperm()?'x':'-');
    } else {
        fprintf(f, "%s%-*s = <not mapped>\n",    p, w, "mapped");
    }

    fprintf(f, "%s%-*s = %s\n", p, w, "contains_code", get_contains_code()?"true":"false");
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") \n", p, w, "mapped_actual_va", p_mapped_actual_va, p_mapped_actual_va);

    // DQ (8/31/2008): Output the contents if this not derived from (there is likely a 
    // better implementation if the hexdump function was a virtual member function).
    if (variantT() == V_SgAsmGenericSection) {
        hexdump(f, 0, std::string(p)+"data at ", p_data);
    }
}
