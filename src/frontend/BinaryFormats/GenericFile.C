/* Generic Binary Files (SgAsmGenericFile and associated classes). */
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include "AsmUnparser_compat.h"
#include "MemoryMap.h"

#include <boost/version.hpp>
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

void
SgAsmGenericFile::ctor()
{
    ROSE_ASSERT(this != NULL);

    ROSE_ASSERT(p_fd == -1);
    ROSE_ASSERT(p_holes == NULL);
    ROSE_ASSERT(p_truncate_zeros == false);

    ROSE_ASSERT(p_headers == NULL);
    p_headers  = new SgAsmGenericHeaderList();
    ROSE_ASSERT(p_headers != NULL);
    p_headers->set_parent(this);

    ROSE_ASSERT(p_holes == NULL);
    p_holes  = new SgAsmGenericSectionList();
    ROSE_ASSERT(p_holes != NULL);
    p_holes->set_parent(this);
}

SgAsmGenericFile *
SgAsmGenericFile::parse(std::string fileName)
{
    ROSE_ASSERT(p_fd < 0); /*can call parse() only once per object*/

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
    DataConverter *dc = get_data_converter();
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

SgAsmGenericFile::~SgAsmGenericFile()
{
    /* AST child nodes have already been deleted if we're called from SageInterface::deleteAST() */

    /* Unmap and close */
    unsigned char *mapped = p_data.pool();
    if (mapped && p_data.size()>0)
        delete[] mapped;
    p_data.clear();

    if ( p_fd >= 0 )
        close(p_fd);
}

rose_addr_t
SgAsmGenericFile::get_orig_size() const
{
    return p_data.size();
}

rose_addr_t
SgAsmGenericFile::get_current_size() const
{
    rose_addr_t retval=0;
    SgAsmGenericSectionPtrList sections = get_sections();
    for (SgAsmGenericSectionPtrList::iterator i=sections.begin(); i!=sections.end(); ++i) {
        retval = std::max(retval, (*i)->get_end_offset());
    }
    return retval;
}

void
SgAsmGenericFile::mark_referenced_extent(rose_addr_t offset, rose_addr_t size)
{
    if (get_tracking_references()) {
        p_referenced_extents.insert(AddressInterval::baseSize(offset, size));
        delete p_unreferenced_cache;
        p_unreferenced_cache = NULL;
    }
}

const AddressIntervalSet &
SgAsmGenericFile::get_unreferenced_extents() const
{
    if (!p_unreferenced_cache) {
        p_unreferenced_cache = new AddressIntervalSet(p_referenced_extents);
        p_unreferenced_cache->invert(AddressInterval::baseSize(0, get_current_size()));
    }
    return *p_unreferenced_cache;
}

size_t
SgAsmGenericFile::read_content(rose_addr_t offset, void *dst_buf, rose_addr_t size, bool strict)
{
    size_t retval;
    if (offset+size <= p_data.size()) {
        retval = size;
    } else if (offset > p_data.size()) {
        if (strict)
            throw ShortRead(NULL, offset, size);
        retval = 0;
    } else {
        if (strict)
            throw ShortRead(NULL, p_data.size(), offset+size - (p_data.size()+offset));
        retval = p_data.size() - offset;
    }
    if (retval>0)
        memcpy(dst_buf, &(p_data[offset]), retval);
    if (get_tracking_references())
        mark_referenced_extent(offset, retval);
    memset((char*)dst_buf+retval, 0, size-retval);
    return retval;
}

size_t
SgAsmGenericFile::read_content(const MemoryMap::Ptr &map, rose_addr_t start_va, void *dst_buf, rose_addr_t size, bool strict)
{
    ROSE_ASSERT(map!=NULL);

    /* Note: This is the same algorithm as used by MemoryMap::read() except we do it here so that we have an opportunity
     *       to track the file byte references. */
    size_t ncopied = 0;
    while (ncopied < size) {
        rose_addr_t va = start_va + ncopied;
        size_t nread = map->at(va).limit(size-ncopied).singleSegment().read((uint8_t*)dst_buf+ncopied).size();
        if (0==nread) break;

        if (get_tracking_references()) {
            assert(map->at(va).exists());
            const MemoryMap::Node &me = *(map->at(va).findNode());
            if (me.value().buffer()->data()==&(get_data()[0])) {
                /* We are tracking file reads and this segment does, indeed, point into the file. */
                size_t file_offset = me.value().offset() + va - me.key().least();
                mark_referenced_extent(file_offset, nread);
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
SgAsmGenericFile::read_content_str(const MemoryMap::Ptr &map, rose_addr_t va, bool strict)
{
    static char *buf=NULL;
    static size_t nalloc=0;
    size_t nused=0;

    /* Note: reading one byte at a time might not be the most efficient way to do this, but it does cause the referenced bytes
     *       to be tracked very precisely. */
    while (1) {
        if (nused >= nalloc) {
            nalloc = std::max((size_t)32, 2*nalloc);
            buf = (char*)realloc(buf, nalloc);
            ROSE_ASSERT(buf!=NULL);
        }

        unsigned char byte;
        read_content(map, va+nused, &byte, 1, strict); /*might throw RvaSizeMap::NotMapped or return a NUL*/
        if (!byte)
            return std::string(buf, nused);
        buf[nused++] = byte;
    }
}

std::string
SgAsmGenericFile::read_content_str(rose_addr_t offset, bool strict)
{
    static char *buf=NULL;
    static size_t nalloc=0;
    size_t nused=0;

    /* Note: reading one byte at a time might not be the most efficient way to do this, but it does cause the referenced bytes
     *       to be tracked very precisely. */
    while (1) {
        if (nused >= nalloc) {
            nalloc = std::max((size_t)32, 2*nalloc);
            buf = (char*)realloc(buf, nalloc);
            ROSE_ASSERT(buf!=NULL);
        }

        unsigned char byte;
        read_content(offset+nused, &byte, 1, strict); /*might throw ShortRead or return a NUL*/
        if (!byte)
            return std::string(buf, nused);
        buf[nused++] = byte;
    }
}

SgFileContentList
SgAsmGenericFile::content(rose_addr_t offset, rose_addr_t size)
{
    if (offset+size <= p_data.size()) {
        return SgFileContentList(p_data, offset, size);
    } else {
        throw ShortRead(NULL, offset, size);
    }
}

void
SgAsmGenericFile::add_header(SgAsmGenericHeader *header)
{
    ROSE_ASSERT(p_headers!=NULL);
    p_headers->set_isModified(true);

#ifndef NDEBUG
    /* New header must not already be present. */
    for (size_t i=0; i< p_headers->get_headers().size(); i++) {
        ROSE_ASSERT(p_headers->get_headers()[i] != header);
    }
#endif
    header->set_parent(p_headers);
    p_headers->get_headers().push_back(header);
}

void
SgAsmGenericFile::remove_header(SgAsmGenericHeader *hdr)
{
    if (hdr!=NULL) {
        ROSE_ASSERT(p_headers  != NULL);
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
    ROSE_ASSERT(p_holes!=NULL);
    p_holes->set_isModified(true);

#ifndef NDEBUG
    /* New hole must not already be present. */
    for (size_t i=0; i< p_holes->get_sections().size(); i++) {
        ROSE_ASSERT(p_holes->get_sections()[i] != hole);
    }
#endif
    hole->set_parent(p_holes);
    p_holes->get_sections().push_back(hole);
}

void
SgAsmGenericFile::remove_hole(SgAsmGenericSection *hole)
{
    if (hole!=NULL) {
        ROSE_ASSERT(p_holes!=NULL);
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
    for (SgAsmGenericHeaderPtrList::iterator i=p_headers->get_headers().begin(); i!=p_headers->get_headers().end(); ++i) {
        if ((*i)->get_sections()!=NULL) {
            const SgAsmGenericSectionPtrList &recurse = (*i)->get_sections()->get_sections();
            retval.insert(retval.end(), recurse.begin(), recurse.end());
        }
    }
    return retval;
}

SgAsmGenericSectionPtrList
SgAsmGenericFile::get_mapped_sections() const
{
    SgAsmGenericSectionPtrList retval;
    SgAsmGenericSectionPtrList all = get_sections(true);
    for (size_t i=0; i<all.size(); i++) {
        if (all[i]->is_mapped())
            retval.push_back(all[i]);
    }
    return retval;
}

SgAsmGenericSectionPtrList
SgAsmGenericFile::get_sections_by_id(int id) const
{
    SgAsmGenericSectionPtrList retval;

    /* Holes */
    for (SgAsmGenericSectionPtrList::iterator i=p_holes->get_sections().begin(); i!=p_holes->get_sections().end(); ++i) {
        if ((*i)->get_id()==id)
            retval.push_back(*i);
    }

    /* Headers and their sections */
    for (SgAsmGenericHeaderPtrList::iterator i=p_headers->get_headers().begin(); i!=p_headers->get_headers().end(); ++i) {
        if ((*i)->get_id()==id)
            retval.push_back(*i);
        const SgAsmGenericSectionPtrList &recurse = (*i)->get_sections_by_id(id);
        retval.insert(retval.end(), recurse.begin(), recurse.end());
    }
    return retval;
}

SgAsmGenericSection *
SgAsmGenericFile::get_section_by_id(int id, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_id(id);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

SgAsmGenericSectionPtrList
SgAsmGenericFile::get_sections_by_name(std::string name, char sep/*or NUL*/) const
{
    SgAsmGenericSectionPtrList retval;

    /* Truncate name */
    if (sep) {
        size_t pos = name.find(sep);
        if (pos!=name.npos)
            name.erase(pos);
    }

    /* Holes */
    for (SgAsmGenericSectionPtrList::iterator i=p_holes->get_sections().begin(); i!=p_holes->get_sections().end(); ++i) {
        std::string secname = (*i)->get_name()->get_string();
        if (sep) {
            size_t pos = secname.find(sep);
            if (pos!=secname.npos)
                secname.erase(pos);
        }
        if (0==secname.compare(name))
            retval.push_back(*i);
    }

    /* Headers and their sections */
    for (SgAsmGenericHeaderPtrList::iterator i=p_headers->get_headers().begin(); i!=p_headers->get_headers().end(); ++i) {
        std::string secname = (*i)->get_name()->get_string();
        if (sep) {
            size_t pos = secname.find(sep);
            if (pos!=secname.npos)
                secname.erase(pos);
        }
        if (0==secname.compare(name))
            retval.push_back(*i);

        const SgAsmGenericSectionPtrList &recurse = (*i)->get_sections_by_name(name, sep);
        retval.insert(retval.end(), recurse.begin(), recurse.end());
    }
    return retval;
}

SgAsmGenericSection *
SgAsmGenericFile::get_section_by_name(const std::string &name, char sep/*or NUL*/, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_name(name, sep);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

SgAsmGenericSectionPtrList
SgAsmGenericFile::get_sections_by_offset(rose_addr_t offset, rose_addr_t size) const
{
    SgAsmGenericSectionPtrList retval;

    /* Holes */
    for (SgAsmGenericSectionPtrList::iterator i=p_holes->get_sections().begin(); i!=p_holes->get_sections().end(); ++i) {
        if (offset >= (*i)->get_offset() &&
            offset < (*i)->get_offset()+(*i)->get_size() &&
            offset-(*i)->get_offset() + size <= (*i)->get_size())
            retval.push_back(*i);
    }

    /* Headers and their sections */
    for (SgAsmGenericHeaderPtrList::iterator i=p_headers->get_headers().begin(); i!=p_headers->get_headers().end(); ++i) {
        if (offset >= (*i)->get_offset() &&
            offset < (*i)->get_offset()+(*i)->get_size() &&
            offset-(*i)->get_offset() + size <= (*i)->get_size())
            retval.push_back(*i);
        const SgAsmGenericSectionPtrList &recurse = (*i)->get_sections_by_offset(offset, size);
        retval.insert(retval.end(), recurse.begin(), recurse.end());
    }
    return retval;
}

SgAsmGenericSection *
SgAsmGenericFile::get_section_by_offset(rose_addr_t offset, rose_addr_t size, size_t *nfound) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_offset(offset, size);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

SgAsmGenericSectionPtrList
SgAsmGenericFile::get_sections_by_rva(rose_addr_t rva) const
{
    SgAsmGenericSectionPtrList retval;

    /* Holes (probably not mapped anyway) */
    for (SgAsmGenericSectionPtrList::iterator i=p_holes->get_sections().begin(); i!=p_holes->get_sections().end(); ++i) {
        if ((*i)->is_mapped() &&
            rva >= (*i)->get_mapped_preferred_rva() && rva < (*i)->get_mapped_preferred_rva() + (*i)->get_mapped_size())
            retval.push_back(*i);
    }

    /* Headers and their sections */
    for (SgAsmGenericHeaderPtrList::iterator i=p_headers->get_headers().begin(); i!=p_headers->get_headers().end(); ++i) {
        if ((*i)->is_mapped() &&
            rva >= (*i)->get_mapped_preferred_rva() && rva < (*i)->get_mapped_preferred_rva() + (*i)->get_mapped_size())
            retval.push_back(*i);
        const SgAsmGenericSectionPtrList &recurse = (*i)->get_sections_by_rva(rva);
        retval.insert(retval.end(), recurse.begin(), recurse.end());
    }
    return retval;
}

SgAsmGenericSection *
SgAsmGenericFile::get_section_by_rva(rose_addr_t rva, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_rva(rva);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

SgAsmGenericSectionPtrList
SgAsmGenericFile::get_sections_by_va(rose_addr_t va) const
{
    SgAsmGenericSectionPtrList retval;

    /* Holes (probably not mapped anyway) */
    for (SgAsmGenericSectionPtrList::iterator i=p_holes->get_sections().begin(); i!=p_holes->get_sections().end(); ++i) {
        rose_addr_t rva = va; /* Holes don't belong to any header and therefore have a zero base_va */
        if ((*i)->is_mapped() &&
            rva >= (*i)->get_mapped_preferred_rva() && rva < (*i)->get_mapped_preferred_rva() + (*i)->get_mapped_size())
            retval.push_back(*i);
    }

    /* Headers and their sections */
    for (SgAsmGenericHeaderPtrList::iterator i=p_headers->get_headers().begin(); i!=p_headers->get_headers().end(); ++i) {
        /* Headers probably aren't mapped, but just in case... */
        rose_addr_t rva = va; /* Headers don't belong to any header and therefore have a zero base_va */
        if ((*i)->is_mapped() &&
            rva >= (*i)->get_mapped_preferred_rva() && rva < (*i)->get_mapped_preferred_rva() + (*i)->get_mapped_size())
            retval.push_back(*i);

        /* Header sections */
        const SgAsmGenericSectionPtrList &recurse = (*i)->get_sections_by_va(va, true);
        retval.insert(retval.end(), recurse.begin(), recurse.end());
    }
    return retval;
}

SgAsmGenericSection *
SgAsmGenericFile::get_section_by_va(rose_addr_t va, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_va(va);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

SgAsmGenericSection *
SgAsmGenericFile::get_best_section_by_va(rose_addr_t va, size_t *nfound/*optional*/) const
{
    const SgAsmGenericSectionPtrList &candidates = get_sections_by_va(va);
    if (nfound)
        *nfound = candidates.size();
    return best_section_by_va(candidates, va);
}

SgAsmGenericSection *
SgAsmGenericFile::best_section_by_va(const SgAsmGenericSectionPtrList &sections, rose_addr_t va)
{
    SgAsmGenericSection *best = NULL;
    rose_addr_t file_offset = 0;
    for (SgAsmGenericSectionPtrList::const_iterator si=sections.begin(); si!=sections.end(); ++si) {
        SgAsmGenericSection *section = *si;
        if (!section->is_mapped() || va<section->get_mapped_actual_va() ||
            va>=section->get_mapped_actual_va()+section->get_mapped_size()) {
            // section does not contain virtual address
        } else if (!best) {
            best = section;
            file_offset = section->get_offset() + (va - section->get_mapped_actual_va());
        } else if (file_offset != section->get_offset() + (va - section->get_mapped_actual_va())) {
            return NULL; // error
        } else if (best->get_mapped_size() > section->get_mapped_size()) {
            best = section;
        } else if (best->get_name()->get_string().empty() && !section->get_name()->get_string().empty()) {
            best = section;
        } else {
            // prefer section defined earlier
        }
    }
    return best;
}

rose_addr_t
SgAsmGenericFile::get_next_section_offset(rose_addr_t offset)
{
    rose_addr_t found = ~(rose_addr_t)0;
    const SgAsmGenericSectionPtrList &sections = get_sections();
    for (SgAsmGenericSectionPtrList::const_iterator i=sections.begin(); i!=sections.end(); ++i) {
        if ((*i)->get_offset() >= offset && (*i)->get_offset() < found)
            found = (*i)->get_offset();
    }
    return found;
}

void
SgAsmGenericFile::shift_extend(SgAsmGenericSection *s, rose_addr_t sa, rose_addr_t sn, AddressSpace space, Elasticity elasticity)
{
    ROSE_ASSERT(s!=NULL);
    ROSE_ASSERT(s->get_file()==this);
    ROSE_ASSERT((space & (ADDRSP_FILE|ADDRSP_MEMORY)) != 0);

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
        sprintf(p, "SgAsmGenericFile::shift_extend[%" PRIuPTR "]: ", ncalls++);
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
    rose_addr_t align=1, aligned_sa, aligned_sasn;
    SgAsmGenericSectionPtrList neighbors, villagers;
    ExtentMap amap; /* address mappings for all extents */
    Extent sp;

    /* Get a list of all sections that may need to be adjusted. */
    SgAsmGenericSectionPtrList all;
    switch (elasticity) {
      case ELASTIC_NONE:
      case ELASTIC_UNREF:
        all = filespace ? get_sections() : get_mapped_sections();
        break;
      case ELASTIC_HOLE:
        all = filespace ? get_sections(false) : get_mapped_sections();
        break;
    }
    if (debug) {
        fprintf(stderr, "%s    Following sections are in 'all' set:\n", p);
        for (size_t i=0; i<all.size(); i++) {
            Extent ep;
            if (filespace) {
                ep = all[i]->get_file_extent();
            } else {
                ROSE_ASSERT(all[i]->is_mapped());
                ep = all[i]->get_mapped_preferred_extent();
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
            sp = s->get_file_extent();
        } else if (!memspace || !s->is_mapped()) {
            return; /*nothing to do*/
        } else {
            sp = s->get_mapped_preferred_extent();
        }

        /* Build address map */
        for (size_t i=0; i<all.size(); i++) {
            if (filespace) {
                amap.insert(all[i]->get_file_extent());
            } else {
                ROSE_ASSERT(all[i]->is_mapped());
                amap.insert(all[i]->get_mapped_preferred_extent());
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
                ap = a->get_file_extent();
            } else if (!a->is_mapped()) {
                continue;
            } else {
                ap = a->get_mapped_preferred_extent();
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
                Extent ap = filespace ? a->get_file_extent() : a->get_mapped_preferred_extent();
                rose_addr_t align = filespace ? a->get_file_alignment() : a->get_mapped_alignment();
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
                Extent ap = filespace ? a->get_file_extent() : a->get_mapped_preferred_extent();
                rose_addr_t align = filespace ? a->get_file_alignment() : a->get_mapped_alignment();
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
            Extent ap = filespace ? a->get_file_extent() : a->get_mapped_preferred_extent();
            if (strchr("RICE", ExtentMap::category(ap, sp))) {
                rose_addr_t x = filespace ? a->get_file_alignment() : a->get_mapped_alignment();
#if BOOST_VERSION < 106700
                align = boost::math::lcm(align, x?x:1); // deprecated in boost-1.69.0
#else
                align = boost::integer::lcm(align, x?x:1); // not present before boost-1.60.0
#endif
            }
        }
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
        SgAsmGenericSection *after_hole = NULL;
        Extent hp(0, 0);
        for (size_t i=0; i<villagers.size(); i++) {
            SgAsmGenericSection *a = villagers[i];
            Extent ap = filespace ? a->get_file_extent() : a->get_mapped_preferred_extent();
            if (!after_hole || ap.relaxed_first()<hp.relaxed_first()) {
                after_hole = a;
                hp = ap;
            }
        }
        ROSE_ASSERT(after_hole);
        ROSE_ASSERT(hp.relaxed_first() > nhs.relaxed_first()+nhs.size());
        rose_addr_t hole_size = hp.relaxed_first() - (nhs.relaxed_first()+nhs.size());
        if (debug) {
            fprintf(stderr, "%s    hole size = 0x%08" PRIx64 " (%" PRIu64 "); need 0x%08" PRIx64 " (%" PRIu64 "); %s\n",
                    p, hole_size, hole_size, aligned_sasn, aligned_sasn,
                    hole_size>=aligned_sasn ? "large enough" : "not large enough");
        }
        if (hole_size >= aligned_sasn) break;
        rose_addr_t need_more = aligned_sasn - hole_size;

        /* Hole is not large enough. We need to recursively move things that are right of our neighborhood, then recompute the
         * all-sections address map and neighborhood(S). */
        ROSE_ASSERT(0==pass); /*logic problem since the recursive call should have enlarged the hole enough*/
        if (debug) {
            fprintf(stderr, "%s    Calling recursively to increase hole size by 0x%08" PRIx64 " (%" PRIu64 ") bytes\n",
                    p, need_more, need_more);
        }
        shift_extend(after_hole, need_more, 0, space, elasticity);
        if (debug) fprintf(stderr, "%s    Returned from recursive call\n", p);
    }

    /* Consider sections that are in the same neighborhood as S */
    if (debug) fprintf(stderr, "%s    -- ADJUSTING --\n", p);
    bool resized_mem = false;
    for (size_t i=0; i<neighbors.size(); i++) {
        SgAsmGenericSection *a = neighbors[i];
        Extent ap = filespace ? a->get_file_extent() : a->get_mapped_preferred_extent();
        switch (ExtentMap::category(ap, sp)) {
          case 'L':
            break;
          case 'R':
            if (filespace) {
                a->set_offset(a->get_offset()+aligned_sasn);
            } else {
                a->set_mapped_preferred_rva(a->get_mapped_preferred_rva()+aligned_sasn);
            }
            break;
          case 'C': /*including S itself*/
          case 'E':
            if (filespace) {
                a->set_offset(a->get_offset()+aligned_sa);
                a->set_size(a->get_size()+sn);
                if (memspace && !resized_mem && a->is_mapped()) {
                    shift_extend(a, 0, sn, ADDRSP_MEMORY, elasticity);
                    resized_mem = true;
                }
            } else {
                a->set_mapped_preferred_rva(a->get_mapped_preferred_rva()+aligned_sa);
                a->set_mapped_size(a->get_mapped_size()+sn);
            }
            break;
          case 'O':
              if (ap.relaxed_first()==sp.relaxed_first()) {
                if (filespace) {
                    a->set_offset(a->get_offset()+aligned_sa);
                    a->set_size(a->get_size()+sn);
                } else {
                    a->set_mapped_preferred_rva(a->get_mapped_preferred_rva()+aligned_sa);
                    a->set_mapped_size(a->get_mapped_size()+sn);
                }
            } else {
                if (filespace) {
                    a->set_size(a->get_size()+aligned_sasn);
                    if (memspace && !resized_mem && a->is_mapped()) {
                        shift_extend(a, 0, aligned_sasn, ADDRSP_MEMORY, elasticity);
                        resized_mem = true;
                    }
                } else {
                    a->set_mapped_size(a->get_mapped_size()+aligned_sasn);
                }
            }
            break;
          case 'I':
            if (filespace) {
                a->set_offset(a->get_offset()+aligned_sa);
            } else {
                a->set_mapped_preferred_rva(a->get_mapped_preferred_rva()+aligned_sa);
            }
            break;
          case 'B':
            if (filespace) {
                a->set_size(a->get_size()+sn);
                if (memspace && !resized_mem && a->is_mapped()) {
                    shift_extend(a, 0, sn, ADDRSP_MEMORY, elasticity);
                    resized_mem = true;
                }
            } else {
                a->set_mapped_size(a->get_size()+sn);
            }
            break;
          default:
            ROSE_ASSERT(!"invalid extent category");
            break;
        }
        if (debug) {
            const char *space_name = filespace ? "file" : "mem";
            rose_addr_t x = filespace ? a->get_file_alignment() : a->get_mapped_alignment();
            fprintf(stderr, "%s   %4s-%c %c0x%08" PRIx64 " 0x%08" PRIx64 " 0x%08" PRIx64,
                    p, space_name, ExtentMap::category(ap, sp),
                    0==ap.relaxed_first()%(x?x:1)?' ':'!',
                    ap.relaxed_first(), ap.size(), ap.relaxed_first()+ap.size());
            Extent newap = filespace ? a->get_file_extent() : a->get_mapped_preferred_extent();
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
    if (!ext)
        ext = ".dump";
    std::string dump_name = get_name() + ext;
    if (in_cwd) {
        size_t slash = dump_name.find_last_of('/');
        if (slash!=dump_name.npos)
            dump_name.replace(0, slash+1, "");
    }
    dump_all(dump_name);
}

void
SgAsmGenericFile::dump_all(const std::string &dump_name)
{
    FILE *dumpFile = fopen(dump_name.c_str(), "wb");
    ROSE_ASSERT(dumpFile != NULL);
    try {
        // The file type should be the first; test harness depends on it
        fprintf(dumpFile, "%s\n", format_name());

        // A table describing the sections of the file
        dump(dumpFile);

        // Detailed info about each section
        const SgAsmGenericSectionPtrList &sections = get_sections();
        for (size_t i = 0; i < sections.size(); i++) {
            fprintf(dumpFile, "Section [%zd]:\n", i);
            ROSE_ASSERT(sections[i] != NULL);
            sections[i]->dump(dumpFile, "  ", -1);
        }

        /* Dump interpretations that point only to this file. */
        SgBinaryComposite *binary = SageInterface::getEnclosingNode<SgBinaryComposite>(this);
        ROSE_ASSERT(binary!=NULL);
        const SgAsmInterpretationPtrList &interps = binary->get_interpretations()->get_interpretations();
        for (size_t i=0; i<interps.size(); i++) {
            SgAsmGenericFilePtrList interp_files = interps[i]->get_files();
            if (interp_files.size()==1 && interp_files[0]==this) {
                std::string assembly = unparseAsmInterpretation(interps[i]);
                fputs(assembly.c_str(), dumpFile);
            }
        }

    } catch(...) {
        fclose(dumpFile);
        throw;
    }
    fclose(dumpFile);
}

void
SgAsmGenericFile::dump(FILE *f) const
{
    fprintf(f, "Encoding: %s\n", get_data_converter() ? escapeString(get_data_converter()->name()).c_str() : "none");

    SgAsmGenericSectionPtrList sections = get_sections();
    if (sections.size()==0) {
        fprintf(f, "No sections defined for file.\n");
        return;
    }

    /* Sort sections by offset (lowest to highest), then size (largest to smallest but zero-sized entries first) */
    for (size_t i = 1; i < sections.size(); i++) {
        for (size_t j=0; j<i; j++) {
            if (sections[j]->get_offset() == sections[i]->get_offset()) {
                rose_addr_t size_i = sections[i]->get_size();
                if (0==size_i) size_i = ~(rose_addr_t)0;
                rose_addr_t size_j = sections[j]->get_size();
                if (0==size_j) size_j = ~(rose_addr_t)0;
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
    rose_addr_t high_water = 0;
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
                section->get_file_alignment()==0 || section->get_offset()%section->get_file_alignment()==0?' ':'!',
                section->get_offset(), section->get_size(), section->get_offset()+section->get_size());

        /* Mapped addresses */
        if (section->is_mapped()) {
            fprintf(f, " %c0x%08" PRIx64 " 0x%08" PRIx64 " 0x%08" PRIx64 " 0x%08" PRIx64,
                    (section->get_mapped_alignment()==0 ||
                     section->get_mapped_preferred_rva()%section->get_mapped_alignment()==0?' ':'!'),
                    section->get_base_va(), section->get_mapped_preferred_rva(), section->get_mapped_size(),
                    section->get_mapped_preferred_rva()+section->get_mapped_size());
        } else {
            fprintf(f, " %*s", 4*11, "");
        }

        /* Permissions */
        if (section->is_mapped()) {
            fprintf(f, " %c%c%c ",
                    section->get_mapped_rperm()?'r':'-',
                    section->get_mapped_wperm()?'w':'-',
                    section->get_mapped_xperm()?'x':'-');
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
    if (high_water < get_current_size()) {
        overlap[2] = 'H';
    } else if (sections.back()->get_offset() + sections.back()->get_size() < get_current_size()) {
        overlap[2] = 'h';
    }
    fprintf(f, "  %3s 0x%08" PRIx64 "%*s EOF", overlap, get_current_size(), 76, "");
    if (get_current_size()!=p_data.size())
        fprintf(f, " (original EOF was 0x%08zx)", p_data.size());
    if (get_truncate_zeros())
        fputs(" [ztrunc]", f);
    fputc('\n', f);
    fprintf(f, "  --- ---------- ---------- ----------  ---------- ---------- ---------- ---------- ---- --- -----------------\n");

    /* Show what part of the file has not been referenced */
    AddressIntervalSet holes = get_unreferenced_extents();
    if (holes.size()>0) {
        fprintf(f, "These parts of the file have not been referenced during parsing:\n");
        BOOST_FOREACH (const AddressInterval &interval, holes.intervals()) {
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
    /* Get the list of file extents referenced by all file sections */
    ExtentMap refs;
    SgAsmGenericSectionPtrList sections = get_sections();
    for (SgAsmGenericSectionPtrList::iterator i=sections.begin(); i!=sections.end(); ++i) {
        refs.insert(Extent((*i)->get_offset(), (*i)->get_size()));
    }

    /* The hole extents are everything other than the sections */
    ExtentMap holes = refs.subtract_from(Extent(0, p_data.size()));

    /* Create the sections representing the holes */
    for (ExtentMap::iterator i=holes.begin(); i!=holes.end(); ++i) {
        Extent e = i->first;
        SgAsmGenericSection *hole = new SgAsmGenericSection(this, NULL);
        hole->set_offset(e.first());
        hole->set_size(e.size());
        hole->parse();
        hole->set_synthesized(true);
        hole->set_name(new SgAsmBasicString("hole"));
        hole->set_purpose(SgAsmGenericSection::SP_UNSPECIFIED);
        add_hole(hole);
    }
}

void
SgAsmGenericFile::unfill_holes()
{
    set_isModified(true);

    SgAsmGenericSectionPtrList to_delete = get_holes()->get_sections();
    for (size_t i=0; i<to_delete.size(); i++) {
        SgAsmGenericSection *hole = to_delete[i];
        SageInterface::deleteAST(hole);
    }

    /* Destructor for holes should have removed links to those holes. */
    ROSE_ASSERT(get_holes()->get_sections().size()==0);
}

void
SgAsmGenericFile::reallocate()
{
    bool reallocated;
    do {
        reallocated = false;

        /* holes */
        for (SgAsmGenericSectionPtrList::iterator i=p_holes->get_sections().begin(); i!=p_holes->get_sections().end(); ++i) {
            if ((*i)->reallocate())
                reallocated = true;
        }

        /* file headers (and indirectly, all that they reference) */
        for (SgAsmGenericHeaderPtrList::iterator i=p_headers->get_headers().begin(); i!=p_headers->get_headers().end(); ++i) {
            if ((*i)->reallocate())
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
    rose_addr_t remaining = get_current_size();
    unsigned char buf[4096];
    memset(buf, 0xaa, sizeof buf);
    while (remaining>=sizeof buf) {
        f.write((const char*)buf, sizeof buf);
        ROSE_ASSERT(f);
        remaining -= sizeof buf;
    }
    f.write((const char*)buf, remaining);
    ROSE_ASSERT(f);
#endif

    /* Write unreferenced sections (i.e., "holes") back to disk */
    for (SgAsmGenericSectionPtrList::iterator i=p_holes->get_sections().begin(); i!=p_holes->get_sections().end(); ++i)
        (*i)->unparse(f);

    /* Write file headers (and indirectly, all that they reference) */
    for (SgAsmGenericHeaderPtrList::iterator i=p_headers->get_headers().begin(); i!=p_headers->get_headers().end(); ++i)
        (*i)->unparse(f);

    /* Extend the file to the full size. The unparser will not write zero bytes at the end of a file because some files
     * actually use the fact that sections that extend past the EOF will be zero padded.  For the time being we'll extend the
     * file to its full size. */
    if (!get_truncate_zeros())
        extend_to_eof(f);
}

void
SgAsmGenericFile::extend_to_eof(std::ostream &f) const
{
    f.seekp(0, std::ios::end);
    if (f.tellp()<(off_t)get_current_size()) {
        f.seekp(get_current_size()-1);
        const char zero = '\0';
        f.write(&zero, 1);
    }
}


const char *
SgAsmGenericFile::format_name() const
{
    return p_headers->get_headers().back()->format_name();
}

SgAsmGenericHeader *
SgAsmGenericFile::get_header(SgAsmGenericFormat::ExecFamily efam)
{
    SgAsmGenericHeader *retval = NULL;
    for (size_t i = 0; i < p_headers->get_headers().size(); i++) {
        if (p_headers->get_headers()[i]->get_exec_format()->get_family() == efam) {
            ROSE_ASSERT(NULL == retval);
            retval = p_headers->get_headers()[i];
        }
    }

    return retval;
}

#endif
