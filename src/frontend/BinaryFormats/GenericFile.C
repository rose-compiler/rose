/* Generic Binary Files (SgAsmGenericFile and associated classes). */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/AST/Traversal.h>
#include <Rose/BinaryAnalysis/AddressInterval.h>
#include <Rose/BinaryAnalysis/AddressIntervalSet.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>

#include <boost/version.hpp>
#include <boost/scope_exit.hpp>
#if BOOST_VERSION < 106700
#include <boost/math/common_factor.hpp>                 // deprecated in 1.67.0
#else
#include <boost/integer/common_factor.hpp>
#endif
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Rose::Diagnostics; // for mlog, INFO, WARN, ERROR, FATAL, etc.

SgAsmGenericFile *
SgAsmGenericFile::parse(std::string fileName)
{
    ASSERT_require(p_fd < 0); /*can call parse() only once per object*/

    set_name(fileName);
    p_fd = open(fileName.c_str(), O_RDONLY);
    if (p_fd<0 || fstat(p_fd, &p_sb)<0) {
        std::string mesg = "could not open binary file \"" + StringUtility::cEscape(fileName) + "\"";
        throw FormatError(mesg + ": " + strerror(errno));
    }
    size_t nbytes = p_sb.st_size;

    /* To be more portable across operating systems, read the file into memory rather than mapping it. */
    unsigned char *mapped = new unsigned char[nbytes];
    if (!mapped)
        throw FormatError("could not allocate memory for binary file \"" + StringUtility::cEscape(fileName) + "\"");
    ssize_t nread = read(p_fd, mapped, nbytes);
    if (nread<0 || (size_t)nread!=nbytes)
    {
      delete [] mapped;
      throw FormatError("could not read entire binary file \"" + StringUtility::cEscape(fileName) + "\"");
    }

    /* Decode the memory if necessary */
    DataConverter *dc = get_dataConverter();
    if (dc) {
        unsigned char *new_mapped = dc->decode(mapped, &nbytes);
        if (new_mapped!=mapped) {
            delete[] mapped;
            mapped = new_mapped;
        }
    }

    /* Make file contents available through an STL vector without actually reading the file */
    p_data = SgFileContentList(mapped, nbytes);
    return this;
}

void
SgAsmGenericFile::destructorHelper() {
    /* AST child nodes have already been deleted if we're called from SageInterface::deleteAST() */

    /* Unmap and close */
    unsigned char *mapped = p_data.pool();
    if (mapped && p_data.size()>0)
        delete[] mapped;
    p_data.clear();

    if ( p_fd >= 0 )
        close(p_fd);
}

Address
SgAsmGenericFile::get_orig_size() const {
    return get_originalSize();
}

Address
SgAsmGenericFile::get_originalSize() const
{
    return p_data.size();
}

Address
SgAsmGenericFile::get_current_size() const
{
    return get_currentSize();
}

Address
SgAsmGenericFile::get_currentSize() const
{
    Address retval=0;
    SgAsmGenericSectionPtrList sections = get_sections();
    for (auto section: sections) {
        retval = std::max(retval, section->get_endOffset());
    }
    return retval;
}

void
SgAsmGenericFile::mark_referenced_extent(Address offset, Address size)
{
    markReferencedExtent(offset, size);
}

void
SgAsmGenericFile::markReferencedExtent(Address offset, Address size)
{
    if (get_trackingReferences()) {
        p_referencedExtents.insert(AddressInterval::baseSize(offset, size));
        delete p_unreferenced_cache;
        p_unreferenced_cache = nullptr;
    }
}

const AddressIntervalSet &
SgAsmGenericFile::get_unreferenced_extents() const
{
    return get_unreferencedExtents();
}

const AddressIntervalSet &
SgAsmGenericFile::get_unreferencedExtents() const
{
    if (!p_unreferenced_cache) {
        p_unreferenced_cache = new AddressIntervalSet(get_referencedExtents());
        p_unreferenced_cache->invert(AddressInterval::baseSize(0, get_currentSize()));
    }
    return *p_unreferenced_cache;
}

size_t
SgAsmGenericFile::read_content(Address offset, void *dst_buf, Address size, bool strict)
{
    return readContent(offset, dst_buf, size, strict);
}

size_t
SgAsmGenericFile::readContent(Address offset, void *dst_buf, Address size, bool strict)
{
    size_t retval;
    if (offset+size <= p_data.size()) {
        retval = size;
    } else if (offset > p_data.size()) {
        if (strict)
            throw ShortRead(nullptr, offset, size);
        retval = 0;
    } else {
        if (strict)
            throw ShortRead(nullptr, p_data.size(), offset+size - (p_data.size()+offset));
        retval = p_data.size() - offset;
    }
    if (retval>0)
        memcpy(dst_buf, &(p_data[offset]), retval);
    if (get_trackingReferences())
        markReferencedExtent(offset, retval);
    memset((char*)dst_buf+retval, 0, size-retval);
    return retval;
}

size_t
SgAsmGenericFile::read_content(const MemoryMap::Ptr &map, Address start_va, void *dst_buf, Address size, bool strict)
{
    return readContent(map, start_va, dst_buf, size, strict);
}

size_t
SgAsmGenericFile::readContent(const MemoryMap::Ptr &map, Address start_va, void *dst_buf, Address size, bool strict)
{
    /* Note: This is the same algorithm as used by MemoryMap::read() except we do it here so that we have an opportunity
     *       to track the file byte references. */
    size_t ncopied = 0;
    while (ncopied < size) {
        Address va = start_va + ncopied;
        size_t nread = map->at(va).limit(size-ncopied).singleSegment().read((uint8_t*)dst_buf+ncopied).size();
        if (0==nread) break;

        if (get_trackingReferences()) {
            assert(map->at(va).exists());
            const MemoryMap::Node &me = *(map->at(va).findNode());
            if (me.value().buffer()->data()==&(get_data()[0])) {
                /* We are tracking file reads and this segment does, indeed, point into the file. */
                size_t file_offset = me.value().offset() + va - me.key().least();
                markReferencedExtent(file_offset, nread);
            }
        }

        ncopied += nread;
    }

    if (ncopied<size) {
        if (strict)
            throw MemoryMap::NotMapped("SgAsmGenericFile::read_content() no mapping", map, start_va+ncopied);
        memset((char*)dst_buf+ncopied, 0, size-ncopied);                /*zero pad result if necessary*/
    }
    return ncopied;
}

std::string
SgAsmGenericFile::read_content_str(const MemoryMap::Ptr &map, Address va, bool strict)
{
    return readContentString(map, va, strict);
}

std::string
SgAsmGenericFile::readContentString(const MemoryMap::Ptr &map, Address va, bool strict)
{
    static char *buf=nullptr;
    static size_t nalloc=0;
    size_t nused=0;

    /* Note: reading one byte at a time might not be the most efficient way to do this, but it does cause the referenced bytes
     *       to be tracked very precisely. */
    while (1) {
        if (nused >= nalloc) {
            nalloc = std::max((size_t)32, 2*nalloc);
            buf = (char*)realloc(buf, nalloc);
            ASSERT_not_null(buf);
        }

        unsigned char byte;
        readContent(map, va+nused, &byte, 1, strict); /*might throw RvaSizeMap::NotMapped or return a NUL*/
        if (!byte)
            return std::string(buf, nused);
        buf[nused++] = byte;
    }
}

std::string
SgAsmGenericFile::read_content_str(Address offset, bool strict)
{
    return readContentString(offset, strict);
}

std::string
SgAsmGenericFile::readContentString(Address offset, bool strict)
{
    static char *buf=nullptr;
    static size_t nalloc=0;
    size_t nused=0;

    /* Note: reading one byte at a time might not be the most efficient way to do this, but it does cause the referenced bytes
     *       to be tracked very precisely. */
    while (1) {
        if (nused >= nalloc) {
            nalloc = std::max((size_t)32, 2*nalloc);
            buf = (char*)realloc(buf, nalloc);
            ASSERT_not_null(buf);
        }

        unsigned char byte;
        readContent(offset+nused, &byte, 1, strict); /*might throw ShortRead or return a NUL*/
        if (!byte)
            return std::string(buf, nused);
        buf[nused++] = byte;
    }
}

SgFileContentList
SgAsmGenericFile::content(Address offset, Address size)
{
    if (offset+size <= p_data.size()) {
        return SgFileContentList(p_data, offset, size);
    } else {
        throw ShortRead(nullptr, offset, size);
    }
}

void
SgAsmGenericFile::add_header(SgAsmGenericHeader *header)
{
    addHeader(header);
}

void
SgAsmGenericFile::addHeader(SgAsmGenericHeader *header)
{
    ASSERT_not_null(p_headers);
    p_headers->set_isModified(true);

#ifndef NDEBUG
    /* New header must not already be present. */
    for (size_t i=0; i< p_headers->get_headers().size(); i++) {
        ASSERT_require(p_headers->get_headers()[i] != header);
    }
#endif
    header->set_parent(p_headers);
    p_headers->get_headers().push_back(header);
}

void
SgAsmGenericFile::remove_header(SgAsmGenericHeader *hdr)
{
    removeHeader(hdr);
}

void
SgAsmGenericFile::removeHeader(SgAsmGenericHeader *hdr)
{
    if (hdr!=nullptr) {
        ASSERT_not_null(p_headers);
        SgAsmGenericHeaderPtrList::iterator i = find(p_headers->get_headers().begin(), p_headers->get_headers().end(), hdr);
        if (i != p_headers->get_headers().end()) {
            p_headers->get_headers().erase(i);
            p_headers->set_isModified(true);
        }
    }
}

void
SgAsmGenericFile::add_hole(SgAsmGenericSection *hole)
{
    addHole(hole);
}

void
SgAsmGenericFile::addHole(SgAsmGenericSection *hole)
{
    ASSERT_not_null(p_holes);
    p_holes->set_isModified(true);

#ifndef NDEBUG
    /* New hole must not already be present. */
    for (size_t i=0; i< p_holes->get_sections().size(); i++) {
        ASSERT_require(p_holes->get_sections()[i] != hole);
    }
#endif
    hole->set_parent(p_holes);
    p_holes->get_sections().push_back(hole);
}

void
SgAsmGenericFile::remove_hole(SgAsmGenericSection *hole)
{
    removeHole(hole);
}

void
SgAsmGenericFile::removeHole(SgAsmGenericSection *hole)
{
    if (hole!=nullptr) {
        ASSERT_not_null(p_holes);
        SgAsmGenericSectionPtrList::iterator i = find(p_holes->get_sections().begin(), p_holes->get_sections().end(), hole);
        if (i != p_holes->get_sections().end()) {
            p_holes->get_sections().erase(i);
            p_holes->set_isModified(true);
        }
    }
}

SgAsmGenericSectionPtrList
SgAsmGenericFile::get_sections(bool include_holes) const
{
    SgAsmGenericSectionPtrList retval;

    /* Start with headers and holes */
    retval.insert(retval.end(), p_headers->get_headers().begin(), p_headers->get_headers().end());
    if (include_holes)
        retval.insert(retval.end(), p_holes->get_sections().begin(), p_holes->get_sections().end());

    /* Add sections pointed to by headers. */
    for (auto header: p_headers->get_headers()) {
        if (header->get_sections() != nullptr) {
            const SgAsmGenericSectionPtrList &recurse = header->get_sections()->get_sections();
            retval.insert(retval.end(), recurse.begin(), recurse.end());
        }
    }
    return retval;
}

SgAsmGenericSectionPtrList
SgAsmGenericFile::get_mapped_sections() const
{
    return get_mappedSections();
}

SgAsmGenericSectionPtrList
SgAsmGenericFile::get_mappedSections() const
{
    SgAsmGenericSectionPtrList retval;
    SgAsmGenericSectionPtrList all = get_sections(true);
    for (size_t i=0; i<all.size(); i++) {
        if (all[i]->isMapped())
            retval.push_back(all[i]);
    }
    return retval;
}

SgAsmGenericSectionPtrList
SgAsmGenericFile::get_sections_by_id(int id) const
{
    return get_sectionsById(id);
}

SgAsmGenericSectionPtrList
SgAsmGenericFile::get_sectionsById(int id) const
{
    SgAsmGenericSectionPtrList retval;

    /* Holes */
    for (auto section: p_holes->get_sections()) {
        if (section->get_id()==id)
            retval.push_back(section);
    }

    /* Headers and their sections */
    for (auto header: p_headers->get_headers()) {
        if (header->get_id()==id)
            retval.push_back(header);
        const SgAsmGenericSectionPtrList &recurse = header->get_sectionsById(id);
        retval.insert(retval.end(), recurse.begin(), recurse.end());
    }
    return retval;
}

SgAsmGenericSection *
SgAsmGenericFile::get_section_by_id(int id, size_t *nfound/*optional*/) const
{
    return get_sectionById(id, nfound);
}

SgAsmGenericSection *
SgAsmGenericFile::get_sectionById(int id, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sectionsById(id);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : nullptr;
}

SgAsmGenericSectionPtrList
SgAsmGenericFile::get_sections_by_name(std::string name, char sep/*or NUL*/) const
{
    return get_sectionsByName(name, sep);
}

SgAsmGenericSectionPtrList
SgAsmGenericFile::get_sectionsByName(std::string name, char sep/*or NUL*/) const
{
    SgAsmGenericSectionPtrList retval;

    /* Truncate name */
    if (sep) {
        size_t pos = name.find(sep);
        if (pos!=name.npos)
            name.erase(pos);
    }

    /* Holes */
    for (auto section: p_holes->get_sections()) {
        std::string secname = section->get_name()->get_string();
        if (sep) {
            size_t pos = secname.find(sep);
            if (pos!=secname.npos)
                secname.erase(pos);
        }
        if (0==secname.compare(name))
            retval.push_back(section);
    }

    /* Headers and their sections */
    for (auto header: p_headers->get_headers()) {
        std::string secname = header->get_name()->get_string();
        if (sep) {
            size_t pos = secname.find(sep);
            if (pos!=secname.npos)
                secname.erase(pos);
        }
        if (0==secname.compare(name))
            retval.push_back(header);

        const SgAsmGenericSectionPtrList &recurse = header->get_sectionsByName(name, sep);
        retval.insert(retval.end(), recurse.begin(), recurse.end());
    }
    return retval;
}

SgAsmGenericSection *
SgAsmGenericFile::get_section_by_name(const std::string &name, char sep/*or NUL*/, size_t *nfound/*optional*/) const
{
    return get_sectionByName(name, sep, nfound);
}

SgAsmGenericSection *
SgAsmGenericFile::get_sectionByName(const std::string &name, char sep/*or NUL*/, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sectionsByName(name, sep);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : nullptr;
}

SgAsmGenericSectionPtrList
SgAsmGenericFile::get_sections_by_offset(Address offset, Address size) const
{
    return get_sectionsByOffset(offset, size);
}

SgAsmGenericSectionPtrList
SgAsmGenericFile::get_sectionsByOffset(Address offset, Address size) const
{
    SgAsmGenericSectionPtrList retval;

    /* Holes */
    for (auto section: p_holes->get_sections()) {
        if (offset >= section->get_offset() &&
            offset < section->get_offset()+section->get_size() &&
            offset-section->get_offset() + size <= section->get_size())
            retval.push_back(section);
    }

    /* Headers and their sections */
    for (auto header: p_headers->get_headers()) {
        if (offset >= header->get_offset() &&
            offset < header->get_offset()+header->get_size() &&
            offset-header->get_offset() + size <= header->get_size())
            retval.push_back(header);
        const SgAsmGenericSectionPtrList &recurse = header->get_sectionsByOffset(offset, size);
        retval.insert(retval.end(), recurse.begin(), recurse.end());
    }
    return retval;
}

SgAsmGenericSection *
SgAsmGenericFile::get_section_by_offset(Address offset, Address size, size_t *nfound) const
{
    return get_sectionByOffset(offset, size, nfound);
}

SgAsmGenericSection *
SgAsmGenericFile::get_sectionByOffset(Address offset, Address size, size_t *nfound) const
{
    SgAsmGenericSectionPtrList possible = get_sectionsByOffset(offset, size);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : nullptr;
}

SgAsmGenericSectionPtrList
SgAsmGenericFile::get_sections_by_rva(Address rva) const
{
    return get_sectionsByRva(rva);
}

SgAsmGenericSectionPtrList
SgAsmGenericFile::get_sectionsByRva(Address rva) const
{
    SgAsmGenericSectionPtrList retval;

    /* Holes (probably not mapped anyway) */
    for (auto section: p_holes->get_sections()) {
        if (section->isMapped() &&
            rva >= section->get_mappedPreferredRva() && rva < section->get_mappedPreferredRva() + section->get_mappedSize())
            retval.push_back(section);
    }

    /* Headers and their sections */
    for (auto header: p_headers->get_headers()) {
        if (header->isMapped() &&
            rva >= header->get_mappedPreferredRva() && rva < header->get_mappedPreferredRva() + header->get_mappedSize())
            retval.push_back(header);
        const SgAsmGenericSectionPtrList &recurse = header->get_sectionsByRva(rva);
        retval.insert(retval.end(), recurse.begin(), recurse.end());
    }
    return retval;
}

SgAsmGenericSection *
SgAsmGenericFile::get_section_by_rva(Address rva, size_t *nfound/*optional*/) const
{
    return get_sectionByRva(rva, nfound);
}

SgAsmGenericSection *
SgAsmGenericFile::get_sectionByRva(Address rva, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sectionsByRva(rva);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : nullptr;
}

SgAsmGenericSectionPtrList
SgAsmGenericFile::get_sections_by_va(Address va) const
{
    return get_sectionsByVa(va);
}

SgAsmGenericSectionPtrList
SgAsmGenericFile::get_sectionsByVa(Address va) const
{
    SgAsmGenericSectionPtrList retval;

    /* Holes (probably not mapped anyway) */
    for (auto section: p_holes->get_sections()) {
        Address rva = va; /* Holes don't belong to any header and therefore have a zero base_va */
        if (section->isMapped() &&
            rva >= section->get_mappedPreferredRva() && rva < section->get_mappedPreferredRva() + section->get_mappedSize())
            retval.push_back(section);
    }

    /* Headers and their sections */
    for (auto header: p_headers->get_headers()) {
        /* Headers probably aren't mapped, but just in case... */
        Address rva = va; /* Headers don't belong to any header and therefore have a zero base_va */
        if (header->isMapped() &&
            rva >= header->get_mappedPreferredRva() && rva < header->get_mappedPreferredRva() + header->get_mappedSize())
            retval.push_back(header);

        /* Header sections */
        const SgAsmGenericSectionPtrList &recurse = header->get_sectionsByVa(va, true);
        retval.insert(retval.end(), recurse.begin(), recurse.end());
    }
    return retval;
}

SgAsmGenericSection *
SgAsmGenericFile::get_section_by_va(Address va, size_t *nfound/*optional*/) const
{
    return get_sectionByVa(va, nfound);
}

SgAsmGenericSection *
SgAsmGenericFile::get_sectionByVa(Address va, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sectionsByVa(va);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : nullptr;
}

SgAsmGenericSection *
SgAsmGenericFile::get_best_section_by_va(Address va, size_t *nfound/*optional*/) const
{
    return get_bestSectionByVa(va, nfound);
}

SgAsmGenericSection *
SgAsmGenericFile::get_bestSectionByVa(Address va, size_t *nfound/*optional*/) const
{
    const SgAsmGenericSectionPtrList &candidates = get_sectionsByVa(va);
    if (nfound)
        *nfound = candidates.size();
    return bestSectionByVa(candidates, va);
}

SgAsmGenericSection *
SgAsmGenericFile::best_section_by_va(const SgAsmGenericSectionPtrList &sections, Address va)
{
    return bestSectionByVa(sections, va);
}

SgAsmGenericSection *
SgAsmGenericFile::bestSectionByVa(const SgAsmGenericSectionPtrList &sections, Address va)
{
    SgAsmGenericSection *best = nullptr;
    Address file_offset = 0;
    for (SgAsmGenericSectionPtrList::const_iterator si=sections.begin(); si!=sections.end(); ++si) {
        SgAsmGenericSection *section = *si;
        if (!section->isMapped() || va<section->get_mappedActualVa() ||
            va>=section->get_mappedActualVa()+section->get_mappedSize()) {
            // section does not contain virtual address
        } else if (!best) {
            best = section;
            file_offset = section->get_offset() + (va - section->get_mappedActualVa());
        } else if (file_offset != section->get_offset() + (va - section->get_mappedActualVa())) {
            return nullptr; // error
        } else if (best->get_mappedSize() > section->get_mappedSize()) {
            best = section;
        } else if (best->get_name()->get_string().empty() && !section->get_name()->get_string().empty()) {
            best = section;
        } else {
            // prefer section defined earlier
        }
    }
    return best;
}

Address
SgAsmGenericFile::get_next_section_offset(Address offset)
{
    return get_nextSectionOffset(offset);
}

Address
SgAsmGenericFile::get_nextSectionOffset(Address offset)
{
    Address found = ~(Address)0;
    const SgAsmGenericSectionPtrList &sections = get_sections();
    for (auto section: sections) {
        if (section->get_offset() >= offset && section->get_offset() < found)
            found = section->get_offset();
    }
    return found;
}

void
SgAsmGenericFile::shift_extend(SgAsmGenericSection *s, Address sa, Address sn) {
    shiftExtend(s, sa, sn);
}

void
SgAsmGenericFile::shiftExtend(SgAsmGenericSection *s, Address sa, Address sn) {
    shiftExtend(s, sa, sn, ADDRSP_ALL, ELASTIC_UNREF);
}

void
SgAsmGenericFile::shift_extend(SgAsmGenericSection *s, Address sa, Address sn, AddressSpace space, Elasticity elasticity)
{
    shiftExtend(s, sa, sn, space, elasticity);
}

void
SgAsmGenericFile::shiftExtend(SgAsmGenericSection *s, Address sa, Address sn, AddressSpace space, Elasticity elasticity)
{
    ASSERT_not_null(s);
    ASSERT_require(s->get_file()==this);
    ASSERT_require((space & (ADDRSP_FILE|ADDRSP_MEMORY)) != 0);

    const bool debug = false;
    static size_t ncalls=0;
    char p[256];

    if (debug) {
        const char *space_s="unknown";
        if (space & ADDRSP_FILE) {
            space_s = "file";
        } else if (space & ADDRSP_MEMORY) {
            space_s = "memory";
        }
        snprintf(p, 256, "SgAsmGenericFile::shiftExtend[%" PRIuPTR "]: ", ncalls++);
        fprintf(stderr, "%s    -- START --\n", p);
        fprintf(stderr, "%s    S = [%d] \"%s\"\n", p, s->get_id(), s->get_name()->get_string(true).c_str());
        fprintf(stderr, "%s    %s Sa=0x%08" PRIx64 " (%" PRIu64 "), Sn=0x%08" PRIx64 " (%" PRIu64 ")\n",
                p, space_s, sa, sa, sn, sn);
        fprintf(stderr, "%s    elasticity = %s\n", p, (ELASTIC_NONE==elasticity ? "none" :
                                                       ELASTIC_UNREF==elasticity ? "unref" :
                                                       ELASTIC_HOLE==elasticity ? "unref+holes" :
                                                       "unknown"));
    }

    /* No-op case */
    if (0==sa && 0==sn) {
        if (debug) {
            fprintf(stderr, "%s    No change necessary.\n", p);
            fprintf(stderr, "%s    -- END --\n", p);
        }
        return;
    }

    bool filespace = (space & ADDRSP_FILE)!=0;
    bool memspace = (space & ADDRSP_MEMORY)!=0;
    Address align=1, aligned_sa, aligned_sasn;
    SgAsmGenericSectionPtrList neighbors, villagers;
    ExtentMap amap; /* address mappings for all extents */
    Extent sp;

    /* Get a list of all sections that may need to be adjusted. */
    SgAsmGenericSectionPtrList all;
    switch (elasticity) {
      case ELASTIC_NONE:
      case ELASTIC_UNREF:
        all = filespace ? get_sections() : get_mappedSections();
        break;
      case ELASTIC_HOLE:
        all = filespace ? get_sections(false) : get_mappedSections();
        break;
    }
    if (debug) {
        fprintf(stderr, "%s    Following sections are in 'all' set:\n", p);
        for (size_t i=0; i<all.size(); i++) {
            Extent ep;
            if (filespace) {
                ep = all[i]->get_fileExtent();
            } else {
                ASSERT_require(all[i]->isMapped());
                ep = all[i]->get_mappedPreferredExtent();
            }
            fprintf(stderr, "%s        0x%08" PRIx64 " 0x%08" PRIx64 " 0x%08" PRIx64 " [%d] \"%s\"\n",
                    p, ep.relaxed_first(), ep.size(), ep.relaxed_first()+ep.size(), all[i]->get_id(),
                    all[i]->get_name()->get_string(true).c_str());
        }
    }

    for (size_t pass=0; pass<2; pass++) {
        if (debug) {
            fprintf(stderr, "%s    -- %s --\n",
                    p, 0==pass?"FIRST PASS":"SECOND PASS (after making a larger hole)");
        }

        /* S offset and size in file or memory address space */
        if (filespace) {
            sp = s->get_fileExtent();
        } else if (!memspace || !s->isMapped()) {
            return; /*nothing to do*/
        } else {
            sp = s->get_mappedPreferredExtent();
        }

        /* Build address map */
        for (size_t i=0; i<all.size(); i++) {
            if (filespace) {
                amap.insert(all[i]->get_fileExtent());
            } else {
                ASSERT_require(all[i]->isMapped());
                amap.insert(all[i]->get_mappedPreferredExtent());
            }
        }
        if (debug) {
            fprintf(stderr, "%s    Address map:\n", p);
            amap.dump_extents(stderr, (std::string(p)+"        ").c_str(), "amap");
            fprintf(stderr, "%s    Extent of S:\n", p);
            fprintf(stderr, "%s        start=0x%08" PRIx64 " size=0x%08" PRIx64 " end=0x%08" PRIx64 "\n",
                    p, sp.relaxed_first(), sp.size(), sp.relaxed_first()+sp.size());
        }

        /* Neighborhood (nhs) of S is a single extent. However, if S is zero size then nhs might be empty.  The neighborhood of
         * S is S plus all sections that overlap with S and all sections that are right-contiguous with S. */
        ExtentMap nhs_map;
        for (ExtentMap::iterator amapi=amap.begin(); amapi!=amap.end(); ++amapi) {
            if (amapi->first.relaxed_first() <= sp.relaxed_first()+sp.size() &&
                amapi->first.relaxed_first()+amapi->first.size() > sp.relaxed_first())
                nhs_map.insert(amapi->first, amapi->second);
        }
        if (debug) {
            fprintf(stderr, "%s    Neighborhood of S:\n", p);
            nhs_map.dump_extents(stderr, (std::string(p)+"        ").c_str(), "nhs_map");
        }
        Extent nhs;
        if (nhs_map.size()>0) {
            assert(nhs_map.nranges()==1);
            nhs = nhs_map.begin()->first;
        } else {
            nhs = sp;
        }

        /* What sections are in the neighborhood (including S), and right of the neighborhood? */
        neighbors.clear(); /*sections in neighborhood*/
        neighbors.push_back(s);
        villagers.clear(); /*sections right of neighborhood*/
        if (debug)
            fprintf(stderr, "%s    Ignoring left (L) sections:\n", p);
        for (size_t i=0; i<all.size(); i++) {
            SgAsmGenericSection *a = all[i];
            if (a==s) continue; /*already pushed onto neighbors*/
            Extent ap;
            if (filespace) {
                ap = a->get_fileExtent();
            } else if (!a->isMapped()) {
                continue;
            } else {
                ap = a->get_mappedPreferredExtent();
            }
            switch (ExtentMap::category(ap, nhs)) {
              case 'L':
                if (debug)
                    fprintf(stderr, "%s        L 0x%08" PRIx64 " 0x%08" PRIx64 " 0x%08" PRIx64 " [%d] \"%s\"\n",
                            p, ap.relaxed_first(), ap.size(), ap.relaxed_first()+ap.size(),
                            a->get_id(), a->get_name()->get_string(true).c_str());
                break;
              case 'R':
                  if (ap.relaxed_first()==nhs.relaxed_first()+nhs.size() && 0==ap.size()) {
                    /* Empty sections immediately right of the neighborhood of S should actually be considered part of the
                     * neighborhood rather than right of it. */
                    neighbors.push_back(a);
                } else if (elasticity!=ELASTIC_NONE) {
                    /* If holes are elastic then treat things right of the hole as being part of the right village; otherwise
                     * add those sections to the neighborhood of S even though they fall outside 'nhs' (it's OK because this
                     * partitioning of sections is the only thing we use 'nhs' for anyway. */
                    villagers.push_back(a);
                } else if ('L'==ExtentMap::category(ap, sp)) {
                    /*ignore sections left of S*/
                } else {
                    neighbors.push_back(a);
                }
                break;
              default:
                if ('L'!=ExtentMap::category(ap, sp)) /*ignore sections left of S*/
                    neighbors.push_back(a);
                break;
            }
        }
        if (debug) {
            fprintf(stderr, "%s    Neighbors:\n", p);
            for (size_t i=0; i<neighbors.size(); i++) {
                SgAsmGenericSection *a = neighbors[i];
                Extent ap = filespace ? a->get_fileExtent() : a->get_mappedPreferredExtent();
                Address align = filespace ? a->get_fileAlignment() : a->get_mappedAlignment();
                char cat = ExtentMap::category(ap, sp);
                fprintf(stderr, "%s        %c %c0x%08" PRIx64 " 0x%08" PRIx64 " 0x%08" PRIx64,
                        p, cat, 0==ap.relaxed_first() % (align?align:1) ? ' ' : '!',
                        ap.relaxed_first(), ap.size(), ap.relaxed_first()+ap.size());
                if (strchr("RICE", cat)) {
                    fprintf(stderr, " align=0x%08" PRIx64, align);
                } else {
                    fputs("                 ", stderr);
                }
                fprintf(stderr, " [%2d] \"%s\"\n", a->get_id(), a->get_name()->get_string(true).c_str());
            }
            if (villagers.size()>0) fprintf(stderr, "%s    Villagers:\n", p);
            for (size_t i=0; i<villagers.size(); i++) {
                SgAsmGenericSection *a = villagers[i];
                Extent ap = filespace ? a->get_fileExtent() : a->get_mappedPreferredExtent();
                Address align = filespace ? a->get_fileAlignment() : a->get_mappedAlignment();
                fprintf(stderr, "%s        %c %c0x%08" PRIx64 " 0x%08" PRIx64 " 0x%08" PRIx64,
                        p, ExtentMap::category(ap, sp), /*cat should always be R*/
                        0==ap.relaxed_first() % (align?align:1) ? ' ' : '!',
                        ap.relaxed_first(), ap.size(), ap.relaxed_first()+ap.size());
                fputs("                 ", stderr);
                fprintf(stderr, " [%2d] \"%s\"\n", a->get_id(), a->get_name()->get_string(true).c_str());
            }
        }

        /* Adjust Sa to satisfy all alignment constraints in neighborhood(S) for sections that will move (cats R, I, C, and E). */
        align = 1;
        for (size_t i=0; i<neighbors.size(); i++) {
            SgAsmGenericSection *a = neighbors[i];
            Extent ap = filespace ? a->get_fileExtent() : a->get_mappedPreferredExtent();
            if (strchr("RICE", ExtentMap::category(ap, sp))) {
                Address x = filespace ? a->get_fileAlignment() : a->get_mappedAlignment();
#if BOOST_VERSION < 106700
                align = boost::math::lcm(align, x?x:1); // deprecated in boost-1.69.0
#else
                align = boost::integer::lcm(align, x?x:1); // not present before boost-1.60.0
#endif
            }
        }
        ASSERT_require(align != 0);
        aligned_sa = (sa/align + (sa%align?1:0))*align;
        aligned_sasn = ((sa+sn)/align + ((sa+sn)%align?1:0))*align;
        if (debug) {
            fprintf(stderr, "%s    Alignment LCM = 0x%08" PRIx64 " (%" PRIu64 ")\n", p, align, align);
            fprintf(stderr, "%s    Aligned Sa    = 0x%08" PRIx64 " (%" PRIu64 ")\n", p, aligned_sa, aligned_sa);
            fprintf(stderr, "%s    Aligned Sa+Sn = 0x%08" PRIx64 " (%" PRIu64 ")\n", p, aligned_sasn, aligned_sasn);
        }

        /* Are there any sections to the right of neighborhood(S)? If so, find the one with the lowest start address and use
         * that to define the size of the hole right of neighborhood(S). */
        if (0==villagers.size()) break;
        SgAsmGenericSection *after_hole = nullptr;
        Extent hp(0, 0);
        for (size_t i=0; i<villagers.size(); i++) {
            SgAsmGenericSection *a = villagers[i];
            Extent ap = filespace ? a->get_fileExtent() : a->get_mappedPreferredExtent();
            if (!after_hole || ap.relaxed_first()<hp.relaxed_first()) {
                after_hole = a;
                hp = ap;
            }
        }
        ASSERT_not_null(after_hole);
        ASSERT_require(hp.relaxed_first() > nhs.relaxed_first()+nhs.size());
        Address hole_size = hp.relaxed_first() - (nhs.relaxed_first()+nhs.size());
        if (debug) {
            fprintf(stderr, "%s    hole size = 0x%08" PRIx64 " (%" PRIu64 "); need 0x%08" PRIx64 " (%" PRIu64 "); %s\n",
                    p, hole_size, hole_size, aligned_sasn, aligned_sasn,
                    hole_size>=aligned_sasn ? "large enough" : "not large enough");
        }
        if (hole_size >= aligned_sasn) break;
        Address need_more = aligned_sasn - hole_size;

        /* Hole is not large enough. We need to recursively move things that are right of our neighborhood, then recompute the
         * all-sections address map and neighborhood(S). */
        ASSERT_require(0==pass); /*logic problem since the recursive call should have enlarged the hole enough*/
        if (debug) {
            fprintf(stderr, "%s    Calling recursively to increase hole size by 0x%08" PRIx64 " (%" PRIu64 ") bytes\n",
                    p, need_more, need_more);
        }
        shiftExtend(after_hole, need_more, 0, space, elasticity);
        if (debug) fprintf(stderr, "%s    Returned from recursive call\n", p);
    }

    /* Consider sections that are in the same neighborhood as S */
    if (debug) fprintf(stderr, "%s    -- ADJUSTING --\n", p);
    bool resized_mem = false;
    for (size_t i=0; i<neighbors.size(); i++) {
        SgAsmGenericSection *a = neighbors[i];
        Extent ap = filespace ? a->get_fileExtent() : a->get_mappedPreferredExtent();
        switch (ExtentMap::category(ap, sp)) {
          case 'L':
            break;
          case 'R':
            if (filespace) {
                a->set_offset(a->get_offset()+aligned_sasn);
            } else {
                a->set_mappedPreferredRva(a->get_mappedPreferredRva()+aligned_sasn);
            }
            break;
          case 'C': /*including S itself*/
          case 'E':
            if (filespace) {
                a->set_offset(a->get_offset()+aligned_sa);
                a->set_size(a->get_size()+sn);
                if (memspace && !resized_mem && a->isMapped()) {
                    shiftExtend(a, 0, sn, ADDRSP_MEMORY, elasticity);
                    resized_mem = true;
                }
            } else {
                a->set_mappedPreferredRva(a->get_mappedPreferredRva()+aligned_sa);
                a->set_mappedSize(a->get_mappedSize()+sn);
            }
            break;
          case 'O':
              if (ap.relaxed_first()==sp.relaxed_first()) {
                if (filespace) {
                    a->set_offset(a->get_offset()+aligned_sa);
                    a->set_size(a->get_size()+sn);
                } else {
                    a->set_mappedPreferredRva(a->get_mappedPreferredRva()+aligned_sa);
                    a->set_mappedSize(a->get_mappedSize()+sn);
                }
            } else {
                if (filespace) {
                    a->set_size(a->get_size()+aligned_sasn);
                    if (memspace && !resized_mem && a->isMapped()) {
                        shiftExtend(a, 0, aligned_sasn, ADDRSP_MEMORY, elasticity);
                        resized_mem = true;
                    }
                } else {
                    a->set_mappedSize(a->get_mappedSize()+aligned_sasn);
                }
            }
            break;
          case 'I':
            if (filespace) {
                a->set_offset(a->get_offset()+aligned_sa);
            } else {
                a->set_mappedPreferredRva(a->get_mappedPreferredRva()+aligned_sa);
            }
            break;
          case 'B':
            if (filespace) {
                a->set_size(a->get_size()+sn);
                if (memspace && !resized_mem && a->isMapped()) {
                    shiftExtend(a, 0, sn, ADDRSP_MEMORY, elasticity);
                    resized_mem = true;
                }
            } else {
                a->set_mappedSize(a->get_size()+sn);
            }
            break;
          default:
            mlog[FATAL] << "invalid extent category";
            exit(1);
        }
        if (debug) {
            const char *space_name = filespace ? "file" : "mem";
            Address x = filespace ? a->get_fileAlignment() : a->get_mappedAlignment();
            fprintf(stderr, "%s   %4s-%c %c0x%08" PRIx64 " 0x%08" PRIx64 " 0x%08" PRIx64,
                    p, space_name, ExtentMap::category(ap, sp),
                    0==ap.relaxed_first()%(x?x:1)?' ':'!',
                    ap.relaxed_first(), ap.size(), ap.relaxed_first()+ap.size());
            Extent newap = filespace ? a->get_fileExtent() : a->get_mappedPreferredExtent();
            fprintf(stderr, " -> %c0x%08" PRIx64 " 0x%08" PRIx64 " 0x%08" PRIx64,
                    0==newap.relaxed_first()%(x?x:1)?' ':'!',
                    newap.relaxed_first(), newap.size(), newap.relaxed_first()+newap.size());
            fprintf(stderr, " [%2d] \"%s\"\n", a->get_id(), a->get_name()->get_string(true).c_str());
        }
    }
    if (debug) fprintf(stderr, "%s    -- END --\n", p);
}

void
SgAsmGenericFile::dump_all(bool in_cwd, const char *ext)
{
    dumpAll(in_cwd, ext);
}

void
SgAsmGenericFile::dumpAll(bool in_cwd, const char *ext)
{
    if (!ext)
        ext = ".dump";
    std::string dump_name = get_name() + ext;
    if (in_cwd) {
        size_t slash = dump_name.find_last_of('/');
        if (slash!=dump_name.npos)
            dump_name.replace(0, slash+1, "");
    }
    dumpAll(dump_name);
}

void
SgAsmGenericFile::dump_all(const std::string &dump_name)
{
    dumpAll(dump_name);
}

void
SgAsmGenericFile::dumpAll(const std::string &dump_name)
{
    FILE *dumpFile = fopen(dump_name.c_str(), "wb");
    if (!dumpFile)
        throw Rose::Exception("cannot open \"" + StringUtility::cEscape(dump_name) + "\" for writing");
    BOOST_SCOPE_EXIT(&dumpFile) {
        fclose(dumpFile);
    } BOOST_SCOPE_EXIT_END;

    // The file type should be the first; test harness depends on it
    fprintf(dumpFile, "%s\n", formatName());

    // A table describing the sections of the file
    dump(dumpFile);

    // Detailed info about each section
    const SgAsmGenericSectionPtrList &sections = get_sections();
    for (size_t i = 0; i < sections.size(); i++) {
        fprintf(dumpFile, "Section [%zd]:\n", i);
        ASSERT_not_null(sections[i]);
        sections[i]->dump(dumpFile, "  ", -1);
    }

    // Dump interpretations that point only to this file.
    //
    // [Robb Matzke 2024-10-18]: There are better ways of getting an assembly listing (e.g., Rose::BinaryAnalysis::Unparser) but
    // they don't naively traverse the AST like the old method that was here, or the method that is here now.
    SgBinaryComposite *binary = SageInterface::getEnclosingNode<SgBinaryComposite>(this);
    ASSERT_not_null(binary);
    const SgAsmInterpretationPtrList &interps = binary->get_interpretations()->get_interpretations();
    for (size_t i=0; i<interps.size(); i++) {
        SgAsmGenericFilePtrList interp_files = interps[i]->get_files();
        if (interp_files.size()==1 && interp_files[0]==this) {
            AST::Traversal::forwardPre<SgAsmInstruction>(interps[i], [&dumpFile](SgAsmInstruction *insn) {
                const std::string assembly = insn->toString() + "\n";
                fputs(assembly.c_str(), dumpFile);
            });
        }
    }
}

void
SgAsmGenericFile::dump(FILE *f) const
{
    fprintf(f, "Encoding: %s\n", get_dataConverter() ? escapeString(get_dataConverter()->name()).c_str() : "none");

    SgAsmGenericSectionPtrList sections = get_sections();
    if (sections.size()==0) {
        fprintf(f, "No sections defined for file.\n");
        return;
    }

    /* Sort sections by offset (lowest to highest), then size (largest to smallest but zero-sized entries first) */
    for (size_t i = 1; i < sections.size(); i++) {
        for (size_t j=0; j<i; j++) {
            if (sections[j]->get_offset() == sections[i]->get_offset()) {
                Address size_i = sections[i]->get_size();
                if (0==size_i) size_i = ~(Address)0;
                Address size_j = sections[j]->get_size();
                if (0==size_j) size_j = ~(Address)0;
                if (size_j < size_i) {
                    SgAsmGenericSection *x = sections[j];
                    sections[j] = sections[i];
                    sections[i] = x;
                }
            } else if (sections[j]->get_offset() > sections[i]->get_offset()) {
                SgAsmGenericSection *x = sections[j];
                sections[j] = sections[i];
                sections[i] = x;
            }
        }
    }

    /* Print results */
    fprintf(f, "File sections:\n");
    fprintf(f, "  Flg File-Addr  File-Size  File-End    Base-VA    Start-RVA  Virt-Size  End-RVA    Perm  ID Name\n");
    fprintf(f, "  --- ---------- ---------- ----------  ---------- ---------- ---------- ---------- ---- --- -----------------\n");
    Address high_water = 0;
    for (size_t i=0; i<sections.size(); i++) {
        SgAsmGenericSection *section = sections[i];

        /* Does section overlap with any other (before or after)? */
        char overlap[4] = "   "; /* status characters: overlap prior, overlap subsequent, hole */
        for (size_t j=0; overlap[0]==' ' && j<i; j++) {
            if (sections[j]->get_offset()+sections[j]->get_size() > section->get_offset()) {
                overlap[0] = '<';
            }
        }
        for (size_t j=i+1; overlap[1]==' ' && j<sections.size(); j++) {
            if (section->get_offset()+section->get_size() > sections[j]->get_offset()) {
                overlap[1] = '>';
            }
        }

        /* Is there a hole before section[i]? */
        if (high_water < section->get_offset()) {
            overlap[2] = 'H'; /* truly unaccounted region of the file */
        } else if (i>0 && sections[i-1]->get_offset()+sections[i-1]->get_size() < section->get_offset()) {
            overlap[2] = 'h'; /* unaccounted only if overlaps are not allowed */
        }
        high_water = std::max(high_water, section->get_offset() + section->get_size());
        fprintf(f, "  %3s", overlap);

        /* File addresses */
        fprintf(f, "%c0x%08" PRIx64 " 0x%08" PRIx64 " 0x%08" PRIx64,
                section->get_fileAlignment()==0 || section->get_offset()%section->get_fileAlignment()==0?' ':'!',
                section->get_offset(), section->get_size(), section->get_offset()+section->get_size());

        /* Mapped addresses */
        if (section->isMapped()) {
            fprintf(f, " %c0x%08" PRIx64 " 0x%08" PRIx64 " 0x%08" PRIx64 " 0x%08" PRIx64,
                    (section->get_mappedAlignment()==0 ||
                     section->get_mappedPreferredRva()%section->get_mappedAlignment()==0?' ':'!'),
                    section->get_baseVa(), section->get_mappedPreferredRva(), section->get_mappedSize(),
                    section->get_mappedPreferredRva()+section->get_mappedSize());
        } else {
            fprintf(f, " %*s", 4*11, "");
        }

        /* Permissions */
        if (section->isMapped()) {
            fprintf(f, " %c%c%c ",
                    section->get_mappedReadPermission()?'r':'-',
                    section->get_mappedWritePermission()?'w':'-',
                    section->get_mappedExecutePermission()?'x':'-');
        } else {
            fputs("     ", f);
        }

        /* Section ID, name */
        if (section->get_id()>=0) {
            fprintf(f, " %3d", section->get_id());
        } else {
            fputs("    ", f);
        }
        fprintf(f, " %s\n", section->get_name()->get_string(true).c_str());
    }

    char overlap[4] = "   ";
    if (high_water < get_currentSize()) {
        overlap[2] = 'H';
    } else if (sections.back()->get_offset() + sections.back()->get_size() < get_currentSize()) {
        overlap[2] = 'h';
    }
    fprintf(f, "  %3s 0x%08" PRIx64 "%*s EOF", overlap, get_currentSize(), 76, "");
    if (get_currentSize() != p_data.size()) {
        fprintf(f, " (original EOF was 0x%08zx)", p_data.size());
    }
    if (get_truncateZeros()) {
        fputs(" [ztrunc]", f);
    }
    fputc('\n', f);
    fprintf(f, "  --- ---------- ---------- ----------  ---------- ---------- ---------- ---------- ---- --- -----------------\n");

    /* Show what part of the file has not been referenced */
    AddressIntervalSet holes = get_unreferencedExtents();
    if (holes.size()>0) {
        fprintf(f, "These parts of the file have not been referenced during parsing:\n");
        for (const AddressInterval &interval : holes.intervals()) {
            std::ostringstream ss;
            using namespace StringUtility;
            ss <<"    " <<toHex(interval.least()) <<" + " <<toHex(interval.size()) <<" = " <<toHex(interval.greatest()+1) <<"\n";
            fputs(ss.str().c_str(), f);
        }
    }
}

void
SgAsmGenericFile::fill_holes()
{
    fillHoles();
}

void
SgAsmGenericFile::fillHoles()
{
    /* Get the list of file extents referenced by all file sections */
    ExtentMap refs;
    SgAsmGenericSectionPtrList sections = get_sections();
    for (auto section: sections) {
        refs.insert(Extent(section->get_offset(), section->get_size()));
    }

    /* The hole extents are everything other than the sections */
    ExtentMap holes = refs.subtract_from(Extent(0, p_data.size()));

    /* Create the sections representing the holes */
    for (ExtentMap::iterator i=holes.begin(); i!=holes.end(); ++i) {
        Extent e = i->first;
        SgAsmGenericSection *hole = new SgAsmGenericSection(this, nullptr);
        hole->set_offset(e.first());
        hole->set_size(e.size());
        hole->parse();
        hole->set_synthesized(true);
        hole->set_name(new SgAsmBasicString("hole"));
        hole->set_purpose(SgAsmGenericSection::SP_UNSPECIFIED);
        addHole(hole);
    }
}

void
SgAsmGenericFile::unfill_holes()
{
    unfillHoles();
}

void
SgAsmGenericFile::unfillHoles()
{
    set_isModified(true);

    SgAsmGenericSectionPtrList to_delete = get_holes()->get_sections();
    for (size_t i=0; i<to_delete.size(); i++) {
        SgAsmGenericSection *hole = to_delete[i];
        SageInterface::deleteAST(hole);
    }

    /* Destructor for holes should have removed links to those holes. */
    ASSERT_require(get_holes()->get_sections().size()==0);
}

void
SgAsmGenericFile::reallocate()
{
    bool reallocated;
    do {
        reallocated = false;

        /* holes */
        for (auto section: p_holes->get_sections()) {
            if (section->reallocate())
                reallocated = true;
        }

        /* file headers (and indirectly, all that they reference) */
        for (auto header: p_headers->get_headers()) {
            if (header->reallocate())
                reallocated = true;
        }
    } while (reallocated);
}

void
SgAsmGenericFile::unparse(std::ostream &f) const
{
    if (get_neuter()) {
        f.seekp(0);
        f <<"NOTE: ROSE is refusing to create a binary file for this AST.\n"
          <<"      See SgAsmGenericFile::set_neuter() for details.\n";
        return;
    }

#if 0
    /* This is only for debugging -- fill the file with something other than zero so we have a better chance of making sure
     * that all data is written back to the file, including things that are zero. */
    Address remaining = get_currentSize();
    unsigned char buf[4096];
    memset(buf, 0xaa, sizeof buf);
    while (remaining>=sizeof buf) {
        f.write((const char*)buf, sizeof buf);
        remaining -= sizeof buf;
    }
    f.write((const char*)buf, remaining);
#endif

    /* Write unreferenced sections (i.e., "holes") back to disk */
    for (auto section: p_holes->get_sections()) {
        section->unparse(f);
    }

    /* Write file headers (and indirectly, all that they reference) */
    for (auto header: p_headers->get_headers()) {
        header->unparse(f);
    }

    /* Extend the file to the full size. The unparser will not write zero bytes at the end of a file because some files
     * actually use the fact that sections that extend past the EOF will be zero padded.  For the time being we'll extend the
     * file to its full size. */
    if (!get_truncateZeros()) {
        extendToEof(f);
    }
}

void
SgAsmGenericFile::extend_to_eof(std::ostream &f) const
{
    return extendToEof(f);
}

void
SgAsmGenericFile::extendToEof(std::ostream &f) const
{
    f.seekp(0, std::ios::end);
    if (f.tellp() < (off_t)get_currentSize()) {
        f.seekp(get_currentSize()-1);
        const char zero = '\0';
        f.write(&zero, 1);
    }
}

const char*
SgAsmGenericFile::format_name() const
{
    return formatName();
}

const char*
SgAsmGenericFile::formatName() const
{
    return p_headers->get_headers().back()->formatName();
}

SgAsmGenericHeader *
SgAsmGenericFile::get_header(SgAsmGenericFormat::ExecFamily efam)
{
    SgAsmGenericHeader *retval = nullptr;
    for (size_t i = 0; i < p_headers->get_headers().size(); i++) {
        if (p_headers->get_headers()[i]->get_executableFormat()->get_family() == efam) {
            ASSERT_require(nullptr == retval);
            retval = p_headers->get_headers()[i];
        }
    }

    return retval;
}

SgAsmDwarfCompilationUnitList*
SgAsmGenericFile::get_dwarf_info() const {
    return get_dwarfInfo();
}

void
SgAsmGenericFile::set_dwarf_info(SgAsmDwarfCompilationUnitList *x) {
    set_dwarfInfo(x);
}

bool
SgAsmGenericFile::get_truncate_zeros() const {
    return get_truncateZeros();
}

void
SgAsmGenericFile::set_truncate_zeros(bool x) {
    set_truncateZeros(x);
}

bool
SgAsmGenericFile::get_tracking_references() const {
    return get_trackingReferences();
}

void
SgAsmGenericFile::set_tracking_references(bool x) {
    set_trackingReferences(x);
}

const AddressIntervalSet&
SgAsmGenericFile::get_referenced_extents() const {
    return get_referencedExtents();
}

void
SgAsmGenericFile::set_referenced_extents(const AddressIntervalSet &x) {
    set_referencedExtents(x);
}

DataConverter*
SgAsmGenericFile::get_data_converter() const {
    return get_dataConverter();
}

DataConverter*
SgAsmGenericFile::get_dataConverter() const {
    return p_data_converter;
}

void
SgAsmGenericFile::set_data_converter(DataConverter* dc) {
    set_dataConverter(dc);
}

void
SgAsmGenericFile::set_dataConverter(DataConverter *dc) {
    p_data_converter = dc;
}

#endif
