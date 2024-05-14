/* Generic Sections. SgAsmGenericSection serves as a base class for all binary file formats that divide a file into contiguous
 * parts.  The SgAsmGenericSection class describes such a part. Most binary formats will subclass this. */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include "stringify.h"
#include <Rose/BinaryAnalysis/AddressInterval.h>
#include <Rose/BinaryAnalysis/AddressIntervalSet.h>
#include <Rose/BinaryAnalysis/Hexdump.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>
#include <Rose/Diagnostics.h>

using namespace Rose;
using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;

SgAsmGenericSection::SgAsmGenericSection(SgAsmGenericFile *ef, SgAsmGenericHeader *hdr) {
    initializeProperties();

    ASSERT_not_null(ef);
    p_file = ef;
    p_size = 1;
    p_offset = ef->get_currentSize();

    /* Add this section to the header's section list */
    if (hdr)
        hdr->addSection(this);
}

/* Destructor must remove section/header link */
void
SgAsmGenericSection::destructorHelper()
{
    SgAsmGenericFile* ef = get_file();
    SgAsmGenericHeader* hdr = get_header();

    /* See constructor comment. This deletes both halves of the header/section link. */
    if (hdr) {
        hdr->removeSection(this);
        set_header(nullptr);
    }

    /* See comment in ROSETTA/src/binaryInstruction.C.  We need to explicitly delete the section name. */
    if (p_name) {
        SageInterface::deleteAST(p_name);
        p_name = nullptr;
    }
    
    /* FIXME: holes should probably be their own class, which would make the file/hole bidirectional linking more like the
     *        header/section bidirectional links (RPM 2008-09-02) */
    if (ef) {
        ef->removeHole(this);
    }

    /* Delete children */
    p_file = nullptr; // deleted by SageInterface::deleteAST()

    /* If the section has allocated its own local pool for the p_data member (rather than pointing into the SgAsmGenericFile)
     * then free that now. */
    if (local_data_pool != nullptr) {
        free(local_data_pool);
        local_data_pool = nullptr;
    }
}

SgAsmGenericSection*
SgAsmGenericSection::parse() {
     grabContent();
     return this;
}

/* Increase file offset and mapping rva to satisfy constraints */
bool
SgAsmGenericSection::align()
{
    bool changed = false;

    if (get_fileAlignment()>0) {
        rose_addr_t old_offset = get_offset();
        rose_addr_t new_offset = alignUp(old_offset, get_fileAlignment());
        set_offset(new_offset);
        changed = changed ? true : (old_offset!=new_offset);
    }

    if (isMapped() && get_mappedAlignment()>0) {
        rose_addr_t old_rva = get_mappedPreferredRva();
        rose_addr_t new_rva = alignUp(old_rva, get_mappedAlignment());
        set_mappedPreferredRva(new_rva);
        changed = changed ? true : (old_rva!=new_rva);
    }

    return changed;
}

void
SgAsmGenericSection::grab_content()
{
    grabContent();
}

void
SgAsmGenericSection::grabContent()
{
    SgAsmGenericFile *ef = get_file();
    ASSERT_not_null(ef);

    if (get_offset()<=ef->get_originalSize()) {
        if (get_offset()+get_size()<=ef->get_originalSize()) {
            p_data = ef->content(get_offset(), get_size());
        } else {
            p_data = ef->content(get_offset(), ef->get_originalSize()-get_offset());
        }
    }
}

unsigned char *
SgAsmGenericSection::writable_content(size_t nbytes)
{
    return writableContent(nbytes);
}

unsigned char *
SgAsmGenericSection::writableContent(size_t nbytes)
{
    if (local_data_pool != nullptr) {
        free(local_data_pool);
    }
    local_data_pool = (unsigned char*)calloc(nbytes, 1);
    memset(local_data_pool, 0, nbytes);
    p_data = SgSharedVector<unsigned char>(local_data_pool, nbytes);
    return &(p_data[0]);
}

void
SgAsmGenericSection::set_name(SgAsmGenericString *s)
{
    if (s!=p_name) {
        if (p_name) {
            p_name->set_parent(nullptr);
            SageInterface::deleteAST(p_name);
        }
        p_name = s;
        if (p_name)
            p_name->set_parent(this);
        set_isModified(true);
    }
}

std::string
SgAsmGenericSection::get_short_name() const
{
    return get_shortName();
}

std::string
SgAsmGenericSection::get_shortName() const
{
    if (p_shortName.empty())
        return get_name() ? get_name()->get_string() : std::string();
    return p_shortName;
}

void
SgAsmGenericSection::set_short_name(const std::string &name)
{
    set_shortName(name);
}

rose_addr_t
SgAsmGenericSection::get_size() const
{
    return p_size;
}

void
SgAsmGenericSection::set_size(rose_addr_t size)
{
    if (p_size!=size)
        set_isModified(true);
    p_size = size;
}

rose_addr_t
SgAsmGenericSection::get_offset() const
{
    return p_offset;
}

void
SgAsmGenericSection::set_offset(rose_addr_t offset)
{
    if (p_offset!=offset)
        set_isModified(true);
    p_offset = offset;
}

rose_addr_t
SgAsmGenericSection::get_end_offset() const
{
    return get_endOffset();
}

rose_addr_t
SgAsmGenericSection::get_endOffset() const
{
    return get_offset() + get_size();
}

Extent
SgAsmGenericSection::get_file_extent() const
{
    return get_fileExtent();
}

Extent
SgAsmGenericSection::get_fileExtent() const
{
    return Extent(get_offset(), get_size());
}

bool
SgAsmGenericSection::is_mapped() const
{
    return isMapped();
}

bool
SgAsmGenericSection::isMapped() const
{
    return (get_mappedPreferredRva()!=0 || get_mappedSize()!=0 ||
            get_mappedReadPermission() || get_mappedWritePermission() || get_mappedExecutePermission());
}

void
SgAsmGenericSection::clear_mapped()
{
    clearMapped();
}

void
SgAsmGenericSection::clearMapped()
{
    set_mappedSize(0);
    set_mappedPreferredRva(0);
    set_mappedActualVa(0);
    set_mappedReadPermission(false);
    set_mappedWritePermission(false);
    set_mappedExecutePermission(false);
}

rose_addr_t
SgAsmGenericSection::get_mapped_size() const
{
    return get_mappedSize();
}

void
SgAsmGenericSection::set_mapped_size(rose_addr_t size)
{
    set_mappedSize(size);
}

void
SgAsmGenericSection::set_mappedSize(rose_addr_t size)
{
    ASSERT_not_null(this);
    if (get_mappedSize()!=size)
        set_isModified(true);
    p_mappedSize = size;
}

rose_addr_t
SgAsmGenericSection::get_mapped_preferred_rva() const {
    return get_mappedPreferredRva();
}

void
SgAsmGenericSection::set_mapped_preferred_rva(rose_addr_t a)
{
    set_mappedPreferredRva(a);
}

void
SgAsmGenericSection::set_mappedPreferredRva(rose_addr_t a)
{
    ASSERT_not_null(this);
    if (get_mappedPreferredRva()!=a)
        set_isModified(true);
    p_mappedPreferredRva = a;
}

rose_addr_t
SgAsmGenericSection::get_mapped_preferred_va() const
{
    return get_mappedPreferredVa();
}

rose_addr_t
SgAsmGenericSection::get_mappedPreferredVa() const
{
    ASSERT_not_null(this);
    if (isMapped())
        return get_baseVa() + get_mappedPreferredRva();
    return 0;
}

rose_addr_t
SgAsmGenericSection::get_base_va() const
{
    return get_baseVa();
}

rose_addr_t
SgAsmGenericSection::get_baseVa() const
{
    ASSERT_not_null(this);

    if (isSgAsmGenericHeader(this))
        return isSgAsmGenericHeader(this)->get_baseVa();

    SgAsmGenericHeader *hdr = get_header();
    return hdr ? hdr->get_baseVa() : 0;
}

Extent
SgAsmGenericSection::get_mapped_preferred_extent() const
{
    return get_mappedPreferredExtent();
}

Extent
SgAsmGenericSection::get_mappedPreferredExtent() const
{
    ASSERT_not_null(this);
    return Extent(get_mappedPreferredRva(), get_mappedSize());
}

size_t
SgAsmGenericSection::read_content(rose_addr_t start_offset, void *dst_buf, rose_addr_t size, bool strict)
{
    return readContent(start_offset, dst_buf, size, strict);
}

size_t
SgAsmGenericSection::readContent(rose_addr_t start_offset, void *dst_buf, rose_addr_t size, bool strict)
{
    SgAsmGenericFile *file = get_file();
    ASSERT_not_null(file);
    return file->readContent(start_offset, dst_buf, size, strict);
}

size_t
SgAsmGenericSection::read_content(const MemoryMap::Ptr &map, rose_addr_t start_va, void *dst_buf, rose_addr_t size, bool strict)
{
    return readContent(map, start_va, dst_buf, size, strict);
}

size_t
SgAsmGenericSection::readContent(const MemoryMap::Ptr &map, rose_addr_t start_va, void *dst_buf, rose_addr_t size, bool strict)
{
    SgAsmGenericFile *file = get_file();
    ASSERT_not_null(file);
    return file->readContent(map, start_va, dst_buf, size, strict);
}

size_t
SgAsmGenericSection::read_content(const MemoryMap::Ptr &map, const RelativeVirtualAddress &start, void *dst_buf,
                                  rose_addr_t size, bool strict) {
    return readContent(map, start, dst_buf, size, strict);
}

size_t
SgAsmGenericSection::readContent(const MemoryMap::Ptr &map, const RelativeVirtualAddress &start, void *dst_buf,
                                 rose_addr_t size, bool strict) {
    return readContent(map, *start.va(), dst_buf, size, strict);
}

size_t
SgAsmGenericSection::read_content_local(rose_addr_t start_offset, void *dst_buf, rose_addr_t size, bool strict)
{
    return readContentLocal(start_offset, dst_buf, size, strict);
}

size_t
SgAsmGenericSection::readContentLocal(rose_addr_t start_offset, void *dst_buf, rose_addr_t size, bool strict)
{
    size_t retval;
    SgAsmGenericFile *file = get_file();
    ASSERT_not_null(file);
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

    file->readContent(get_offset()+start_offset, dst_buf, retval, true);
    memset((char*)dst_buf+retval, 0, size-retval);
    return retval;
}

SgUnsignedCharList
SgAsmGenericSection::read_content_local_ucl(rose_addr_t rel_offset, rose_addr_t size)
{
    return readContentLocalUcl(rel_offset, size);
}

SgUnsignedCharList
SgAsmGenericSection::readContentLocalUcl(rose_addr_t rel_offset, rose_addr_t size)
{
    SgUnsignedCharList retval;
    unsigned char *buf = new unsigned char[size];
    readContentLocal(rel_offset, buf, size, false); /*zero pads; never throws*/
    for (size_t i=0; i<size; i++)
        retval.push_back(buf[i]);
    delete[] buf;
    return retval;
}

std::string
SgAsmGenericSection::read_content_str(const MemoryMap::Ptr &map, rose_addr_t start_va, bool strict)
{
    return readContentString(map, start_va, strict);
}

std::string
SgAsmGenericSection::readContentString(const MemoryMap::Ptr &map, rose_addr_t start_va, bool strict)
{
    SgAsmGenericFile *file = get_file();
    ASSERT_not_null(file);
    return file->readContentString(map, start_va, strict);
}

std::string
SgAsmGenericSection::read_content_str(rose_addr_t abs_offset, bool strict)
{
    return readContentString(abs_offset, strict);
}

std::string
SgAsmGenericSection::readContentString(rose_addr_t abs_offset, bool strict)
{
    SgAsmGenericFile *file = get_file();
    ASSERT_not_null(file);
    return file->readContentString(abs_offset, strict);
}

std::string
SgAsmGenericSection::readContentString(const Rose::BinaryAnalysis::MemoryMap::Ptr &map, RelativeVirtualAddress rva, bool strict) {
    return readContentString(map, *rva.va(), strict);
}

std::string
SgAsmGenericSection::read_content_local_str(rose_addr_t rel_offset, bool strict)
{
    return readContentLocalString(rel_offset, strict);
}

std::string
SgAsmGenericSection::readContentLocalString(rose_addr_t rel_offset, bool strict)
{
    std::string retval;
    while (1) {
        char ch;
        if (readContentLocal(rel_offset+retval.size(), &ch, 1, strict)) {
            if ('\0'==ch)
                return retval;
            retval += ch;
        } else {
            return retval;
        }
    }
}

uint64_t
SgAsmGenericSection::read_content_local_uleb128(rose_addr_t *rel_offset, bool strict)
{
    return readContentLocalUleb128(rel_offset, strict);
}

uint64_t
SgAsmGenericSection::readContentLocalUleb128(rose_addr_t *rel_offset, bool strict)
{
    int shift=0;
    uint64_t retval=0;
    while (1) {
        unsigned char byte;
        readContentLocal(*rel_offset, &byte, 1, strict);
        *rel_offset += 1;
        ASSERT_require(shift<64);
        retval |= (byte & 0x7f) << shift;
        shift += 7;
        if (0==(byte & 0x80))
            break;
    }
    return retval;
}

int64_t
SgAsmGenericSection::read_content_local_sleb128(rose_addr_t *rel_offset, bool strict)
{
    return readContentLocalSleb128(rel_offset, strict);
}

int64_t
SgAsmGenericSection::readContentLocalSleb128(rose_addr_t *rel_offset, bool strict)
{
    int shift=0;
    int64_t retval=0;
    while (1) {
        unsigned char byte;
        readContentLocal(*rel_offset, &byte, 1, strict);
        *rel_offset += 1;
        ASSERT_require(shift<64);
        retval |= (byte & 0x7f) << shift;
        shift += 7;
        if (0==(byte & 0x80))
            break;
    }
    retval = (retval << (64-shift)) >> (64-shift); /*sign extend*/
    return retval;
}

rose_addr_t
SgAsmGenericSection::write(std::ostream &f, rose_addr_t offset, size_t bufsize, const void *buf) const
{
    size_t nwrite;

    ASSERT_not_null(this);

    /* Don't write past end of section */
    if (offset>=get_size()) {
        nwrite = 0;
    } else if (offset+bufsize<=get_size()) {
        nwrite = bufsize;
    } else {
        nwrite = get_size() - offset;
    }

    /* Don't write past end of current EOF if we can help it. */
    f.seekp(0, std::ios::end);
    rose_addr_t filesize = f.tellp();
    while (nwrite>0 && 0==((const char*)buf)[nwrite-1] && get_offset()+offset+nwrite>filesize)
        --nwrite;

    /* Write bytes to file. This is a good place to set a break point if you're trying to figure out what section is writing
     * to a particular file address. For instance, if byte 0x7c is incorrect in the unparsed file you would set a conditional
     * breakpoint for o<=0x7c && o+nwrite>0x7c */
    off_t o = get_offset() + offset;
    f.seekp(o);
    f.write((const char*)buf, nwrite);

    /* Check that truncated data is all zero and fail if it isn't */
    for (size_t i=nwrite; i<bufsize; i++) {
        if (((const char*)buf)[i]) {
            char mesg[1024];
            snprintf(mesg, 1024, "non-zero value truncated: buf[0x%zx]=0x%02x", i, ((const unsigned char*)buf)[i]);
            mlog[ERROR] <<"SgAsmGenericSection::write: error: " <<mesg
                        <<" in [" <<get_id() <<"] \"" <<get_name()->get_string(true) <<"\"\n"
                        <<"    section is at file offset " <<StringUtility::addrToString(get_offset())
                        <<" size " <<StringUtility::plural(get_size(), "bytes") <<"\n"
                        <<"    write " <<StringUtility::plural(bufsize, "bytes")
                        <<" at section offset " <<StringUtility::addrToString(offset) <<"\n";
            HexdumpFormat hf;
            hf.prefix = "      ";
            hexdump(mlog[ERROR], get_offset()+offset, (const unsigned char*)buf, bufsize, hf);
            mlog[ERROR] <<"\n";
            throw SgAsmGenericFile::ShortWrite(this, offset, bufsize, mesg);
        }
    }

    return offset+bufsize;
}

rose_addr_t
SgAsmGenericSection::write(std::ostream &f, rose_addr_t offset, const SgFileContentList &buf) const
{
    if (0==buf.size()) {
        return 0;
    }
    return write(f, offset, buf.size(), &(buf[0]));
}

rose_addr_t
SgAsmGenericSection::write(std::ostream &f, rose_addr_t offset, const SgUnsignedCharList &buf) const
{
    if (0==buf.size()) {
        return 0;
    }
    return write(f, offset, buf.size(), (void*)&(buf[0]));
}

rose_addr_t
SgAsmGenericSection::write(std::ostream &f, rose_addr_t offset, const std::string &str) const
{
    return write(f, offset, str.size(), &(str[0]));
}

rose_addr_t
SgAsmGenericSection::write(std::ostream &f, rose_addr_t offset, char c) const
{
    return write(f, offset, 1, &c);
}

rose_addr_t
SgAsmGenericSection::write_uleb128(unsigned char *buf, rose_addr_t offset, uint64_t val) const
{
    return writeUleb128(buf, offset, val);
}

rose_addr_t
SgAsmGenericSection::writeUleb128(unsigned char *buf, rose_addr_t offset, uint64_t val) const
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

rose_addr_t
SgAsmGenericSection::write_sleb128(unsigned char *buf, rose_addr_t offset, int64_t val) const
{
    return writeSleb128(buf, offset, val);
}

rose_addr_t
SgAsmGenericSection::writeSleb128(unsigned char *buf, rose_addr_t offset, int64_t val) const
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

AddressIntervalSet
SgAsmGenericSection::get_referenced_extents() const
{
    return get_referencedExtents();
}

AddressIntervalSet
SgAsmGenericSection::get_referencedExtents() const
{
    if (0==get_size())
        return AddressIntervalSet();

    AddressIntervalSet retval;
    AddressInterval segment = AddressInterval::baseSize(get_offset(), get_size());
    const AddressIntervalSet &fileExtents = get_file()->get_referencedExtents();
    for (const AddressInterval &interval : fileExtents.intervals()) {
        if (segment.contains(interval)) {
            retval.insert(AddressInterval::baseSize(interval.least()-get_offset(), interval.size()));
        } else if (interval.isLeftOf(segment) || interval.isRightOf(segment)) {
            // no overlap
        } else if (interval.contains(segment)) {
            retval.insert(AddressInterval::baseSize(0, get_size()));
            break;                                      // no point in continuing since we've referenced whole segment now
        } else if (interval.least() < segment.least()) {
            retval.insert(AddressInterval::baseSize(0, interval.least()+interval.size()-get_offset()));
        } else if (interval.greatest() > segment.greatest()) {
            retval.insert(AddressInterval::baseSize(interval.least()-get_offset(), get_offset()+get_size()-interval.least()));
        } else {
            ASSERT_not_reachable("invalid extent overlap category");
        }
    }
    return retval;
}

AddressIntervalSet
SgAsmGenericSection::get_unreferenced_extents() const
{
    return get_unreferencedExtents();
}

AddressIntervalSet
SgAsmGenericSection::get_unreferencedExtents() const
{
    AddressIntervalSet set = get_referencedExtents();
    set.invert(AddressInterval::baseSize(0, get_size()));
    return set;
}

void
SgAsmGenericSection::extend(rose_addr_t size)
{
    ASSERT_not_null(get_file());
    ROSE_ASSERT(get_file()->get_trackingReferences()); /*can only be called during the parsing phase*/
    rose_addr_t new_size = get_size() + size;

    /* Ending file address for section using new size, limited by total file size */
    rose_addr_t new_end = std::min(get_file()->get_originalSize(), get_offset()+new_size);
    if (get_offset()<=new_end) {
        p_data.resize(new_end-get_offset());
    } else {
        ROSE_ASSERT(0==p_data.size());
    }

    if (p_size!=new_size)
        set_isModified(true);
    p_size = new_size;
}

SgAsmGenericHeader *
SgAsmGenericSection::is_file_header()
{
    return isFileHeader();
}

SgAsmGenericHeader *
SgAsmGenericSection::isFileHeader()
{
    try {
        SgAsmGenericHeader *retval = dynamic_cast<SgAsmGenericHeader*>(this);
        return retval;
    } catch(...) {
        return nullptr;
    }
}

void
SgAsmGenericSection::unparse(std::ostream &f) const
{
    /* Most subclasses should override this method */
    write(f, 0, p_data);
}

void
SgAsmGenericSection::unparse(std::ostream &f, const ExtentMap &map) const
{
    for (ExtentMap::const_iterator i=map.begin(); i!=map.end(); ++i) {
        Extent e = i->first;
        assert(e.first()+e.size() <= get_size());
        const unsigned char *extent_data;
        if (e.first() >= p_data.size()) {
            extent_data = nullptr;
        } else if (e.first() + e.size() > p_data.size()) {
            extent_data = &p_data[e.first()];
        } else {
            extent_data = &p_data[e.first()];
        }
        if (extent_data)
            write(f, e.first(), e.size(), extent_data);
    }
}

void
SgAsmGenericSection::unparse_holes(std::ostream &f) const
{
    unparseHoles(f);
}

void
SgAsmGenericSection::unparseHoles(std::ostream&) const
{
#if 0 /*DEBUGGING*/
    ExtentMap holes = get_unreferencedExtents();
    fprintf(stderr, "Section \"%s\", 0x%" PRIx64 " bytes\n", get_name()->get_string(true).c_str(), get_size());
    holes.dump_extents(stderr, "  ", "");
#endif
//    unparse(f, get_unreferencedExtents());
}

rose_addr_t
SgAsmGenericSection::get_rva_offset(rose_addr_t rva) const
{
    return get_rvaOffset(rva);
}

rose_addr_t
SgAsmGenericSection::get_rvaOffset(rose_addr_t rva) const
{
    return get_vaOffset(rva + get_baseVa());
}

rose_addr_t
SgAsmGenericSection::get_va_offset(rose_addr_t va) const
{
    return get_vaOffset(va);
}

rose_addr_t
SgAsmGenericSection::get_vaOffset(rose_addr_t va) const
{
    ROSE_ASSERT(isMapped());
    ROSE_ASSERT(va >= get_baseVa());
    rose_addr_t rva = va - get_baseVa();
    ROSE_ASSERT(rva >= get_mappedPreferredRva());
    return get_offset() + (rva - get_mappedPreferredRva());
}

void
SgAsmGenericSection::dump_containing_sections(FILE *f, const std::string &prefix, RelativeVirtualAddress rva,
                                              const SgAsmGenericSectionPtrList &slist) {
    return dumpContainingSections(f, prefix, rva, slist);
}

void
SgAsmGenericSection::dumpContainingSections(FILE *f, const std::string &prefix, RelativeVirtualAddress rva,
                                            const SgAsmGenericSectionPtrList &slist)
{
    for (size_t i=0; i<slist.size(); i++) {
        SgAsmGenericSection *s = slist[i];
        if (s->isMapped() && rva>=s->get_mappedPreferredRva() && rva<s->get_mappedPreferredRva()+s->get_mappedSize()) {
            rose_addr_t offset = rva - s->get_mappedPreferredRva();
            fprintf(f, "%-*s   is 0x%08" PRIx64 " (%" PRIu64 ") bytes into section [%d] \"%s\"\n",
                    (int)DUMP_FIELD_WIDTH, prefix.c_str(), offset, offset, s->get_id(), s->get_name()->get_string(true).c_str());
        }
    }
}

void
SgAsmGenericSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, 4096, "%sSection[%zd].", prefix, idx);
    } else {
        snprintf(p, 4096, "%sSection.", prefix);
    }
    
    const int w = std::max(size_t{1}, DUMP_FIELD_WIDTH - strlen(p));

    fprintf(f, "%s%-*s = \"%s\"",                      p, w, "name",        p_name->get_string(true).c_str());
    if (!p_shortName.empty())
        fprintf(f, " (%s)", p_shortName.c_str());
    fprintf(f, "\n");
    fprintf(f, "%s%-*s = %d\n",                          p, w, "id",          p_id);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 " (%" PRIu64 ") bytes into file\n", p, w, "offset", p_offset, p_offset);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 " (%" PRIu64 ") bytes\n",           p, w, "size", get_size(), get_size());
    if (0==get_fileAlignment()) {
        fprintf(f, "%s%-*s = not specified\n", p, w, "file_alignment");
    } else {
        fprintf(f, "%s%-*s = 0x%08" PRIx64 " (%" PRIu64 ") %s\n", p, w, "file_alignment", 
                get_fileAlignment(), get_fileAlignment(),
                0==get_offset()%get_fileAlignment()?"satisfied":"NOT SATISFIED");
    }
    fprintf(f, "%s%-*s = %s\n",                          p, w, "synthesized", p_synthesized?"yes":"no");
    if (p_header) {
        fprintf(f, "%s%-*s = \"%s\"\n",                  p, w, "header",      p_header->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = not associated\n",          p, w, "header");
    }
    
    std::string purpose = stringifySgAsmGenericSectionSectionPurpose(p_purpose);
    fprintf(f, "%s%-*s = %s\n", p, w, "purpose", purpose.c_str());

    if (isMapped()) {
        fprintf(f, "%s%-*s = rva=0x%08" PRIx64 ", size=%" PRIu64 " bytes\n",
                p, w, "mapped",  get_mappedPreferredRva(), get_mappedSize());
        if (0==get_mappedAlignment()) {
            fprintf(f, "%s%-*s = not specified\n", p, w, "mapped_alignment");
        } else {
            fprintf(f, "%s%-*s = 0x%08" PRIx64 " (%" PRIu64 ") %s\n", p, w, "mapped_alignment", 
                    get_mappedAlignment(), get_mappedAlignment(),
                    0==get_mappedPreferredRva()%get_mappedAlignment()?"satisfied":"NOT SATISFIED");
        }
        fprintf(f, "%s%-*s = %c%c%c\n", p, w, "permissions",
                get_mappedReadPermission()?'r':'-', get_mappedWritePermission()?'w':'-', get_mappedExecutePermission()?'x':'-');
    } else {
        fprintf(f, "%s%-*s = <not mapped>\n",    p, w, "mapped");
    }

    fprintf(f, "%s%-*s = %s\n", p, w, "contains_code", get_containsCode()?"true":"false");
    fprintf(f, "%s%-*s = 0x%08" PRIx64 " (%" PRIu64 ") \n", p, w, "mapped_actual_va", get_mappedActualVa(), get_mappedActualVa());

    // DQ (8/31/2008): Output the contents if this not derived from (there is likely a 
    // better implementation if the hexdump function was a virtual member function).
    if (variantT() == V_SgAsmGenericSection) {
        hexdump(f, 0, std::string(p)+"data at ", p_data);
    }
}

rose_addr_t
SgAsmGenericSection::get_file_alignment() const {
    return get_fileAlignment();
}

void
SgAsmGenericSection::set_file_alignment(rose_addr_t x) {
    set_fileAlignment(x);
}

rose_addr_t
SgAsmGenericSection::get_mapped_alignment() const {
    return get_mappedAlignment();
}

void
SgAsmGenericSection::set_mapped_alignment(rose_addr_t x) {
    set_mappedAlignment(x);
}

bool
SgAsmGenericSection::get_mapped_rperm() const {
    return get_mappedReadPermission();
}

void
SgAsmGenericSection::set_mapped_rperm(bool x) {
    set_mappedReadPermission(x);
}

bool
SgAsmGenericSection::get_mapped_wperm() const {
    return get_mappedWritePermission();
}

void
SgAsmGenericSection::set_mapped_wperm(bool x) {
    set_mappedWritePermission(x);
}

bool
SgAsmGenericSection::get_mapped_xperm() const {
    return get_mappedExecutePermission();
}

void
SgAsmGenericSection::set_mapped_xperm(bool x) {
    set_mappedExecutePermission(x);
}

bool
SgAsmGenericSection::get_contains_code() const {
    return get_containsCode();
}

void
SgAsmGenericSection::set_contains_code(bool x) {
    set_containsCode(x);
}

rose_addr_t
SgAsmGenericSection::get_mapped_actual_va() const {
    return get_mappedActualVa();
}

void
SgAsmGenericSection::set_mapped_actual_va(rose_addr_t x) {
    set_mappedActualVa(x);
}

#endif
