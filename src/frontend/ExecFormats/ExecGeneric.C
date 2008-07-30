/* Copyright 2008 Lawrence Livermore National Security, LLC */

#include "ExecELF.h"
#include "ExecLE.h"
#include "ExecNE.h"
#include "ExecPE.h"

#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <algorithm>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

namespace Exec {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExecFormat
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Print some debugging info */
void
ExecFormat::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096], sbuf[256];
    const char *s;
    if (idx>=0) {
        sprintf(p, "%sExecFormat[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sExecFormat.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    switch (family) {
      case FAMILY_UNSPECIFIED:  s = "unspecified";                          break;
      case FAMILY_DOS:          s = "Microsoft DOS";                        break;
      case FAMILY_ELF:          s = "Executable and Linking Format (ELF)";  break;
      case FAMILY_LE:           s = "Microsoft Linear Executable (LE)";     break;
      case FAMILY_LX:           s = "OS/2 Extended Linear Executable (LX)"; break;
      case FAMILY_NE:           s = "Microsoft New Executable (NE)";        break;
      case FAMILY_PE:           s = "Microsoft Portable Executable (PE)";   break;
      default:
        sprintf(sbuf, "%u", family);
        s = sbuf;
        break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "family", s);

    switch (purpose) {
      case PURPOSE_UNSPECIFIED:   s = "unspecified";                       break;
      case PURPOSE_OTHER:         s = "other";                             break;
      case PURPOSE_EXECUTABLE:    s = "executable program";                break;
      case PURPOSE_LIBRARY:       s = "library (shared or relocatable)";   break;
      case PURPOSE_CORE_DUMP:     s = "post mortem image (core dump)";     break;
      case PURPOSE_OS_SPECIFIC:   s = "operating system specific purpose"; break;
      case PURPOSE_PROC_SPECIFIC: s = "processor specific purpose";        break;
      default:
        sprintf(sbuf, "%u", purpose);
        s = sbuf;
        break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "purpose", s);

    switch (sex) {
      case ORDER_UNSPECIFIED:     s = "unspecified";   break;
      case ORDER_LSB:             s = "little-endian"; break;
      case ORDER_MSB:             s = "big-endian";    break;
      default:
        sprintf(sbuf, "%u", sex);
        s = sbuf;
        break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "sex", s);

    fprintf(f, "%s%-*s = %u (%scurrent)\n", p, w, "version", version, is_current_version?"":"not-");
    
    switch (abi) {
      case ABI_UNSPECIFIED: s = "unspecified";        break;
      case ABI_86OPEN:      s = "86Open Common IA32"; break;
      case ABI_AIX:         s = "AIX";                break;
      case ABI_ARM:         s = "ARM architecture";   break;
      case ABI_FREEBSD:     s = "FreeBSD";            break;
      case ABI_HPUX:        s = "HP/UX";              break;
      case ABI_IRIX:        s = "IRIX";               break;
      case ABI_HURD:        s = "GNU/Hurd";           break;
      case ABI_LINUX:       s = "GNU/Linux";          break;
      case ABI_MODESTO:     s = "Novell Modesto";     break;
      case ABI_MONTEREY:    s = "Monterey project";   break;
      case ABI_MSDOS:       s = "Microsoft DOS";      break;
      case ABI_NT:          s = "Windows NT";         break;
      case ABI_NETBSD:      s = "NetBSD";             break;
      case ABI_SOLARIS:     s = "Sun Solaris";        break;
      case ABI_SYSV:        s = "SysV R4";            break;
      case ABI_TRU64:       s = "Compaq TRU64 UNIX";  break;
      default:
        sprintf(sbuf, "%u", abi);
        s = sbuf;
        break;
    }
    fprintf(f, "%s%-*s = %s\n",  p, w, "ABI",      s);
    fprintf(f, "%s%-*s = %u\n",  p, w, "ABIvers",  abi_version);
    fprintf(f, "%s%-*s = %zu\n", p, w, "wordsize", word_size);
}
    
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExecFile
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructs by mapping file contents into memory */
void
ExecFile::ctor(std::string fileName)
{
    if ((fd=open(fileName.c_str(), O_RDONLY))<0 || fstat64(fd, &sb)<0) {
        std::string mesg = "Could not open binary file";
        throw FormatError(mesg + ": " + strerror(errno));
    }
    if (NULL==(data=(unsigned char*)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0))) {
        std::string mesg = "Could not mmap binary file";
        throw FormatError(mesg + ": " + strerror(errno));
    }
}

/* Destructs by closing and unmapping the file and destroying all sections, headers, etc. */
ExecFile::~ExecFile() 
{
    /* Delete subclasses before super classes (e.g., ExecHeader before ExecSection) */
    while (headers.size()) {
        ExecHeader *header = headers.back();
        headers.pop_back();
        delete header;
    }
    while (sections.size()) {
        ExecSection *section = sections.back();
        sections.pop_back();
        delete section;
    }
    
    /* Unmap and close */
    if (data)
        munmap(data, sb.st_size);
    if (fd>=0)
        close(fd);
}

/* Adds a new header to the file. This is called implicitly by the header constructor */
void
ExecFile::add_header(ExecHeader *header) 
{
#ifndef NDEBUG
    /* New header must not already be present. */
    for (size_t i=0; i<headers.size(); i++) {
        ROSE_ASSERT(headers[i]!=header);
    }
#endif
    headers.push_back(header);
}

/* Adds a new section to the file. This is called implicitly by the section constructor. */
void
ExecFile::add_section(ExecSection *section)
{
#ifndef NDEBUG
    /* New section must not already be present. */
    for (size_t i=0; i<sections.size(); i++) {
        ROSE_ASSERT(sections[i]!=section);
    }
#endif
    sections.push_back(section);
}

/* Returns the pointer to the first section with the specified ID. */
ExecSection *
ExecFile::get_section_by_id(int id)
{
    for (std::vector<ExecSection*>::iterator i=sections.begin(); i!=sections.end(); i++) {
        if ((*i)->get_id() == id) {
            return *i;
        }
    }
    return NULL;
}

/* Returns pointer to the first section with the specified name. Any characters in the name after the first occurrence of SEP
 * are ignored (default is NUL). For instance, if sep=='$' then the following names are all equivalent: .idata, .idata$,
 * .idata$1 */
ExecSection *
ExecFile::get_section_by_name(std::string name, char sep)
{
    if (sep) {
        size_t pos = name.find(sep);
        if (pos!=name.npos)
            name.erase(pos);
    }
    
    for (std::vector<ExecSection*>::iterator i=sections.begin(); i!=sections.end(); i++) {
        if (0==(*i)->get_name().compare(name))
            return *i;
    }
    return NULL;
}

/* Returns a vector of sections that contain the specified portion of the file */
std::vector<ExecSection*>
ExecFile::get_sections_by_offset(addr_t offset, addr_t size)
{
    std::vector<ExecSection*> retval;
    for (std::vector<ExecSection*>::iterator i=sections.begin(); i!=sections.end(); i++) {
        ExecSection *section = *i;
        if (offset >= section->get_offset() &&
            offset < section->get_offset()+section->get_size() &&
            offset-section->get_offset() + size <= section->get_size())
            retval.push_back(section);
    }
    return retval;
}

/* Returns a vector of sections that are mapped to the specified RVA */
std::vector<ExecSection*>
ExecFile::get_sections_by_rva(addr_t rva)
{
    std::vector<ExecSection*> retval;
    for (std::vector<ExecSection*>::iterator i=sections.begin(); i!=sections.end(); i++) {
        ExecSection *section = *i;
        if (section->is_mapped() && rva>=section->get_mapped_rva() && rva<section->get_mapped_rva()+section->get_mapped_size()) {
            retval.push_back(section);
        }
    }
    return retval;
}

/* Returns a vector of sections that are mapped to the specified virtual address (VA). Sections are mapped by relative virtual
 * addresses (RVAs) that are based on the base VA of the section's file header. If the section is mapped but has no associated
 * file header then we assume zero for the base VA. */
std::vector<ExecSection*>
ExecFile::get_sections_by_va(addr_t va)
{
    std::vector<ExecSection*> retval;
    for (std::vector<ExecSection*>::iterator i=sections.begin(); i!=sections.end(); i++) {
        ExecSection *section = *i;
        if (section->is_mapped()) {
            ExecHeader *hdr = section->get_header();
            addr_t base_va = hdr ? hdr->get_base_va() : 0;
            if (va>=base_va+section->get_mapped_rva() && va<base_va+section->get_mapped_rva()+section->get_mapped_size()) {
                retval.push_back(section);
            }
        }
    }
    return retval;
}

/* Given a file address, return the file offset of the following section(s). If there is no following section then return an
 * address of -1 (when signed) */
addr_t
ExecFile::get_next_section_offset(addr_t offset)
{
    addr_t found = ~(addr_t)0;
    for (std::vector<ExecSection*>::iterator i=sections.begin(); i!=sections.end(); i++) {
        if ((*i)->get_offset() >= offset && (*i)->get_offset() < found)
            found = (*i)->get_offset();
    }
    return found;
}

/* Print basic info about the sections of a file */
void
ExecFile::dump(FILE *f)
{
    if (sections.size()==0) {
        fprintf(f, "No sections defined for file.\n");
        return;
    }
    
    /* Sort sections by offset and size */
    std::vector<ExecSection*> sections = this->sections;
    for (size_t i=1; i<sections.size(); i++) {
        for (size_t j=0; j<i; j++) {
            if (sections[j]->get_offset() == sections[i]->get_offset()) {
                if (sections[j]->get_size() > sections[i]->get_size()) {
                    ExecSection *x = sections[j];
                    sections[j] = sections[i];
                    sections[i] = x;
                }
            } else if (sections[j]->get_offset() > sections[i]->get_offset()) {
                ExecSection *x = sections[j];
                sections[j] = sections[i];
                sections[i] = x;
            }
        }
    }
    
    /* Print results */
    fprintf(f, "File sections:\n");
    fprintf(f, "  Flg Offset     Size       End         ID Perm Section Name\n");
    fprintf(f, "  --- ---------- ---------- ----------  -- ---- -------------------------------------\n");
    addr_t high_water = 0;
    for (size_t i=0; i<sections.size(); i++) {
        ExecSection *section = sections[i];
        
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
        
        fprintf(f, "  %3s 0x%08"PRIx64" 0x%08"PRIx64" 0x%08"PRIx64, 
                overlap, section->get_offset(), section->get_size(), section->get_offset()+section->get_size());
        if (section->get_id()>=0) {
            fprintf(f, "  %2d", section->get_id());
        } else {
            fputs("    ", f);
        }
        if (section->is_mapped()) {
#if 0
            fprintf(f, " %c%c%c ",
                    section->get_readable()  ?'r':'-',
                    section->get_writable()  ?'w':'-', 
                    section->get_executable()?'x':'-');
#else
            fputc(' ', f);
            bool b = section->get_rperm();
            fputc(b ? 'r' : '-', f);
            b = section->get_wperm();
            fputc(b ? 'w' : '-', f);
            b = section->get_eperm();
            fputc(b ? 'x' : '-', f);
            fputc(' ', f);
#endif
        } else {
            fputs("     ", f);
        }
        fprintf(f, " %s\n", section->get_name().c_str());
    }

    char overlap[4] = "   ";
    if (high_water < (addr_t)sb.st_size) {
        overlap[2] = 'H';
    } else if (sections.back()->get_offset() + sections.back()->get_size() < (addr_t)sb.st_size) {
        overlap[2] = 'h';
    }
    fprintf(f, "  %3s 0x%08"PRIx64"                                EOF\n", overlap, (addr_t)sb.st_size);
    fprintf(f, "  --- ---------- ---------- ----------  -- ---- -------------------------------------\n");
}

/* Synthesizes sections to describe the parts of the file that are not yet referenced by other sections. */
void
ExecFile::fill_holes()
{

    /* Find the holes and store their extent info */
    ExecSection::ExtentVector extents;
    addr_t offset = 0;
    while (offset < (addr_t)sb.st_size) {
        std::vector<ExecSection*> sections = get_sections_by_offset(offset, 0); /*all sections at this file offset*/
        
        /* Find the maximum ending offset */
        addr_t end_offset = 0;
        for (size_t i=0; i<sections.size(); i++) {
            addr_t tmp = sections[i]->get_offset() + sections[i]->get_size();
            if (tmp>end_offset)
                end_offset = tmp;
        }
        ROSE_ASSERT(end_offset <= (addr_t)sb.st_size);
        
        /* Is there a hole here? */
        if (end_offset<=offset) {
            end_offset = get_next_section_offset(offset+1);
            if (end_offset==(addr_t)-1)
                end_offset = sb.st_size;
            extents.push_back(ExecSection::ExtentPair(offset, end_offset-offset));
        }
        
        /* Advance */
        offset = end_offset;
    }

    /* Create the sections representing the holes */
    for (size_t i=0; i<extents.size(); i++) {
        ExecSection *section = new ExecSection(this, extents[i].first, extents[i].second);
        section->set_synthesized(true);
        section->set_name("hole");
        section->set_purpose(SP_UNSPECIFIED);
        section->congeal();
    }
}

/* Deletes "hole" sections */
void
ExecFile::unfill_holes()
{
    /* Get a list of holes */
    std::vector<ExecSection*> holes;
    for (size_t i=0; i<sections.size(); i++) {
        if (sections[i]->get_id()<0 && sections[i]->get_name().compare("hole")==0)
            holes.push_back(sections[i]);
    }

    /* Destroy the holes, removing them from the "sections" vector */
    for (std::vector<ExecSection*>::iterator it=holes.begin(); it!=holes.end(); it++)
        delete *it;
}

/* Mirror image of parsing an executable file. The result should be identical to the original file. */
void
ExecFile::unparse(const char *filename)
{
    FILE *f = fopen(filename, "w");
    ROSE_ASSERT(f);

#if 0
    /* This is only for debugging -- fill the file with something other than zero so we have a better chance of making sure
     * that all data is written back to the file, including things that are zero. */
    addr_t remaining = sb.st_size;
    unsigned char fill=0xaa, buf[4096];
    memset(buf, fill, sizeof buf);
    while (remaining>=sizeof buf) {
        fwrite(buf, sizeof buf, 1, f);
        remaining -= sizeof buf;
    }
    fwrite(buf, remaining, 1, f);
#endif

    /* Write unreferenced sections (i.e., "holes") back to disk */
    for (size_t i=0; i<sections.size(); i++) {
        if (sections[i]->get_id()<0 && sections[i]->get_name().compare("hole")==0)
            sections[i]->unparse(f);
    }
    
    /* Write file headers (and indirectly, all that they reference) */
    for (size_t i=0; i<headers.size(); i++) {
        headers[i]->unparse(f);
    }
}

/* Return a string describing the file format. We use the last header so that files like PE, NE, LE, LX, etc. which also have
 * a DOS header report the format of the second (PE, etc.) header rather than the DOS header. */
const char *
ExecFile::format_name()
{
    return headers.back()->format_name();
}

/* Returns the header for the specified format. */
ExecHeader *
ExecFile::get_header(ExecFamily efam)
{
    ExecHeader *retval=NULL;
    for (size_t i=0; i<headers.size(); i++) {
        if (headers[i]->get_exec_format().family==efam) {
            ROSE_ASSERT(NULL==retval);
            retval = headers[i];
        }
    }
    return retval;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExecSection
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
ExecSection::ctor(ExecFile *ef, addr_t offset, addr_t size)
{
    ROSE_ASSERT(ef);
    if (offset > ef->get_size() || offset+size > ef->get_size())
        throw ShortRead(NULL, offset, size);
    
    this->file = ef;
    this->offset = offset;
    this->size = size;
    this->data = ef->content() + offset;

    /* The section is added to the file's list of sections. It may also be added to other lists by the caller. The destructor
     * will remove it from the file's list but does not know about other lists. */
    ef->add_section(this);
}

/* Destructor must remove the section from its parent file's section list */
ExecSection::~ExecSection()
{
    if (file) {
        std::vector<ExecSection*> &sections = file->get_sections();
        std::vector<ExecSection*>::iterator i = sections.begin();
        while (i!=sections.end()) {
            if (*i==this) {
                i = sections.erase(i);
            } else {
                i++;
            }
        }
    }
}

/* Returns ptr to content at specified offset after ensuring that the required amount of data is available. */
const unsigned char *
ExecSection::content(addr_t offset, addr_t size)
{
    if (offset > this->size || offset+size > this->size)
        throw ShortRead(this, offset, size);
    if (!congealed && size>0)
        referenced.insert(std::make_pair(offset, offset+size));
    return data + offset;
}

/* Copies the specified part of the section into a buffer. Any part of the selected area that is outside the domain of the
 * section will be filled with zero (in contrast to the two-argument version that throws an exception */
void
ExecSection::content(addr_t offset, addr_t size, void *buf)
{
    if (offset >= this->size) {
        memset(buf, 0, size);
    } else if (offset+size > this->size) {
        addr_t nbytes = this->size - offset;
        memcpy(buf, data+offset, nbytes);
        memset((char*)buf+nbytes, 0, size-nbytes);
        if (!congealed)
            referenced.insert(std::make_pair(offset, offset+nbytes));
    } else {
        memcpy(buf, data+offset, size);
        if (!congealed)
            referenced.insert(std::make_pair(offset, offset+size));
    }
}

/* Returns ptr to a NUL-terminated string */
const char *
ExecSection::content_str(addr_t offset)
{
    const char *ret = (const char*)content(offset, 0);
    size_t nchars=0;
    while (offset+nchars<size && ret[nchars]) nchars++;
    nchars++; /*NUL*/
    if (offset+nchars>size)
        throw ShortRead(this, offset, nchars);
    if (!congealed)
        referenced.insert(std::make_pair(offset, offset+nchars));
    return ret;
}

/* Congeal the references to find the unreferenced areas. Once the references are congealed calling content() and
 * content_str() will not affect references. This allows us to read the unreferenced areas without turning them into
 * referenced areas. */
const ExecSection::ExtentVector &
ExecSection::congeal()
{

    if (!congealed) {
        holes.clear();
        addr_t old_end = 0;
        for (RefMap::iterator it=referenced.begin(); it!=referenced.end(); it++) {
            ExtentPair value = *it;
            ROSE_ASSERT(value.first <= value.second);
            if (value.first > old_end)
                holes.push_back(std::make_pair(old_end, value.first));
            if (value.second > old_end)
                old_end = value.second;
        }
        if (size > old_end)
            holes.push_back(std::make_pair(old_end, size));
        referenced.clear();
        congealed = true;
    }
    return holes;
}

/* Uncongeal the holes */
const ExecSection::RefMap &
ExecSection::uncongeal()
{
    if (congealed) {
        referenced.clear();
        addr_t old_end = 0;
        for (ExtentVector::iterator it=holes.begin(); it!=holes.end(); it++) {
            ExtentPair value = *it;
            ROSE_ASSERT(value.first >= old_end);
            ROSE_ASSERT(value.first <= value.second);
            if (value.first > old_end)
                referenced.insert(std::make_pair(old_end, value.first));
            if (value.second > old_end)
                old_end = value.second;
        }
        if (size > old_end)
            referenced.insert(std::make_pair(old_end, size));
        congealed = false;
        holes.clear();
    }
    return referenced;
}

/* Extend a section by some number of bytes. */
void
ExecSection::extend(addr_t size)
{
    ROSE_ASSERT(file);
    if (offset + this->size + size > file->get_size())
        throw ShortRead(this, offset+this->size, size);
    this->size += size;
}

/* Like extend() but is more relaxed at the end of the file: if extending the section would cause it to go past the end of the
 * file then it is extended to the end of the file and no exception is thrown. */
void
ExecSection::extend_up_to(addr_t size)
{
    ROSE_ASSERT(file);
    if (offset + this->size + size > file->get_size()) {
        ROSE_ASSERT(this->offset <= file->get_size());
        this->size = file->get_size() - this->offset;
    } else {
        this->size += size;
    }
}

/* True (the ExecHeader pointer) if this section is also a top-level file header, false (NULL) otherwise. */
ExecHeader *
ExecSection::is_file_header()
{
    try {
        ExecHeader *retval = dynamic_cast<ExecHeader*>(this);
        return retval;
    } catch(...) {
        return NULL;
    }
}

/* Write a section back to the file. This is the generic version that simply writes the content. Subclasses should override
 * this. */
void
ExecSection::unparse(FILE *f)
{
#if 0
    /* FIXME: for now we print the names of all sections we dump using this method. Eventually most of these sections will
     *        have subclasses that override this method. */
    fprintf(stderr, "Exec::ExecSection::unparse(FILE*) for section [%d] \"%s\"\n", id, name.c_str());
#endif

    int status = fseek(f, offset, SEEK_SET);
    ROSE_ASSERT(status>=0);
    size_t nwrite = fwrite(data, 1, size, f);
    ROSE_ASSERT(nwrite==size);
}

/* Write just the specified regions back to the file */
void
ExecSection::unparse(FILE *f, const ExtentVector &ev)
{
    for (size_t i=0; i<ev.size(); i++) {
        ExtentPair p = ev[i];
        ROSE_ASSERT(p.first<=p.second);
        ROSE_ASSERT(p.second<=size);
        addr_t extent_offset = p.first;
        addr_t extent_size   = p.second - p.first;
        const unsigned char *extent_data = content(extent_offset, extent_size);
        int status = fseek(f, offset + extent_offset, SEEK_SET);
        ROSE_ASSERT(status>=0);
        size_t nwrite = fwrite(extent_data, 1, extent_size, f);
        ROSE_ASSERT(nwrite==extent_size);
    }
}

/* Write holes (unreferenced areas) back to the file */
void
ExecSection::unparse_holes(FILE *f)
{
    bool was_congealed = congealed;
    unparse(f, congeal());
    if (!was_congealed)
        uncongeal();
}

/* Print some debugging info */
void
ExecSection::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096], sbuf[256];
    const char *s;
    if (idx>=0) {
        sprintf(p, "%sExecSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sExecSection.", prefix);
    }
    
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = \"%s\"\n",                      p, w, "name",        name.c_str());
    fprintf(f, "%s%-*s = %d\n",                          p, w, "id",          id);
    fprintf(f, "%s%-*s = %" PRId64 " bytes into file\n", p, w, "offset",      offset);
    fprintf(f, "%s%-*s = %" PRId64 " bytes\n",           p, w, "size",        size);
    fprintf(f, "%s%-*s = %s\n",                          p, w, "synthesized", synthesized?"yes":"no");
    if (header) {
        fprintf(f, "%s%-*s = \"%s\"\n",                  p, w, "header",      header->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = not associated\n",          p, w, "header");
    }
    
    switch (purpose) {
      case SP_UNSPECIFIED: s = "not specified"; break;
      case SP_PROGRAM:     s = "program-supplied data/code/etc"; break;
      case SP_HEADER:      s = "executable format header";       break;
      case SP_SYMTAB:      s = "symbol table";                   break;
      case SP_OTHER:       s = "other";                          break;
      default:
        sprintf(sbuf, "%u", purpose);
        s = sbuf;
        break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "purpose", s);

    if (mapped) {
        fprintf(f, "%s%-*s = rva=0x%08"PRIx64", size=%"PRIu64" bytes\n", p, w, "mapped",  mapped_rva, mapped_size);
        fprintf(f, "%s%-*s = %c%c%c\n", p, w, "permissions",
                rperm?'r':'-', wperm?'w':'-', eperm?'x':'-');
    } else {
        fprintf(f, "%s%-*s = <not mapped>\n",    p, w, "mapped");
    }

    /* Show holes based on what's been referenced so far */
    fprintf(f, "%s%-*s = %s\n", p, w, "congealed", congealed?"true":"false");
    bool was_congealed = congealed;
    const ExtentVector &holes = congeal();
    for (size_t i=0; i<holes.size(); i++) {
        ExtentPair extent = holes[i];
        addr_t hole_size = extent.second - extent.first;
        fprintf(f, "%s%-*s = [%zu] at %"PRIu64", %"PRIu64" bytes\n", p, w, "hole", i, extent.first, hole_size);
    }
    if (!was_congealed)
        uncongeal();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExecHeader
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
ExecHeader::ctor(ExecFile *ef, addr_t offset, addr_t size)
{
    set_synthesized(true);
    set_purpose(SP_HEADER);
    ef->add_header(this);
}

/* Destructor must remove the header from its parent file's headers list. */
ExecHeader::~ExecHeader() 
{
    if (file) {
        std::vector<ExecHeader*> &headers = file->get_headers();
        std::vector<ExecHeader*>::iterator i=headers.begin();
        while (i!=headers.end()) {
            if (*i==this) {
                i = headers.erase(i);
            } else {
                i++;
            }
        }
    }
}

/* Add a new DLL to the header DLL list */
void
ExecHeader::add_dll(ExecDLL *dll)
{
#ifndef NDEBUG
    for (size_t i=0; i<dlls.size(); i++) {
        ROSE_ASSERT(dlls[i]!=dll); /*duplicate*/
    }
#endif
    dlls.push_back(dll);
}


/* Add a new symbol to the symbol table */
void
ExecHeader::add_symbol(ExecSymbol *symbol)
{
#ifndef NDEBUG
    for (size_t i=0; i<symbols.size(); i++) {
        ROSE_ASSERT(symbols[i]!=symbol); /*duplicate*/
    }
#endif
    symbols.push_back(symbol);
}

/* Print some debugging info */
void
ExecHeader::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sExecHeader[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sExecHeader.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    ExecSection::dump(f, p, -1);
    exec_format.dump(f, p, -1);

    fprintf(f, "%s%-*s = %s\n", p, w, "target",   "<FIXME>");

    fprintf(f, "%s%-*s = \"", p, w, "magic");
    for (size_t i=0; i<magic.size(); i++) {
        switch (magic[i]) {
          case '\\': fputs("\\\\", f); break;
          case '\n': fputs("\\n", f); break;
          case '\r': fputs("\\r", f); break;
          case '\t': fputs("\\t", f); break;
          default:
            if (isprint(magic[i])) {
                fputc(magic[i], f);
            } else {
                fprintf(f, "\\%03o", (unsigned)magic[i]);
            }
            break;
        }
    }
    fputs("\"\n", f);
    
    /* Base virtual address and entry addresses */
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n", p, w, "base_va",   base_va);
    fprintf(f, "%s%-*s = %zu entry points\n", p, w, "entry_rva.size", entry_rvas.size());
    for (size_t i=0; i<entry_rvas.size(); i++) {
        fprintf(f, "%s%-*s = [%zu] 0x%08"PRIx64, p, w, "entry_rva", i, entry_rvas[i]);
        std::vector<ExecSection*> sections = get_file()->get_sections_by_rva(entry_rvas[i]);
        for (size_t j=0; j<sections.size(); j++) {
            fprintf(f, "%s in section [%d] \"%s\"", j?"\n    also":"",  sections[j]->get_id(), sections[j]->get_name().c_str());
        }
        fputc('\n', f);
    }

    fprintf(f, "%s%-*s = %zu entries\n", p, w, "ExecDLL.size", dlls.size());
    for (size_t i=0; i<dlls.size(); i++)
        dlls[i]->dump(f, p, i);

    fprintf(f, "%s%-*s = %zu entries\n", p, w, "ExecSymbol.size", symbols.size());
    for (size_t i=0; i<symbols.size(); i++)
        symbols[i]->dump(f, p, i);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dynamically linked libraries
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Print some debugging info */
void
ExecDLL::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sExecDLL[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sExecDLL.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = \"%s\"\n", p, w, "name", name.c_str());
    for (size_t i=0; i<funcs.size(); i++)
        fprintf(f, "%s%-*s = [%zd] \"%s\"\n", p, w, "func", i, funcs[i].c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Symbols and symbol tables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Print some debugging info */
void
ExecSymbol::dump(FILE *f, const char *prefix, ssize_t idx) 
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sExecSymbol[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sExecSymbol.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = \"%s\"\n", p, w, "name", name.c_str());

    const char *s_def_state = NULL;
    switch (def_state) {
      case SYM_UNDEFINED: s_def_state = "undefined"; break;
      case SYM_TENTATIVE: s_def_state = "tentative"; break;
      case SYM_DEFINED:   s_def_state = "defined";   break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "def_state", s_def_state);

    const char *s_bind = NULL;
    switch (binding) {
      case SYM_NO_BINDING: s_bind = "no-binding"; break;
      case SYM_LOCAL:      s_bind = "local";      break;
      case SYM_GLOBAL:     s_bind = "global";     break;
      case SYM_WEAK:       s_bind = "weak";       break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "binding", s_bind);
    
    const char *s_type = NULL;
    switch (type) {
      case SYM_NO_TYPE:  s_type = "no-type";  break;
      case SYM_DATA:     s_type = "data";     break;
      case SYM_FUNC:     s_type = "function"; break;
      case SYM_SECTION:  s_type = "section";  break;
      case SYM_FILE:     s_type = "file";     break;
      case SYM_TLS:      s_type = "thread";   break;
      case SYM_REGISTER: s_type = "register"; break;
      case SYM_ARRAY:    s_type = "array";    break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "type", s_type);
    if (bound) {
        fprintf(f, "%s%-*s = [%d] \"%s\" @%"PRIu64", %"PRIu64" bytes\n", p, w, "bound",
                bound->get_id(), bound->get_name().c_str(), bound->get_offset(), bound->get_size());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "bound");
    }
    
    fprintf(f, "%s%-*s = 0x%08"PRIx64, p, w, "value", value);
    if (value>9) {
        fprintf(f, " (unsigned)%"PRIu64, value);
        if ((int64_t)value<0) fprintf(f, " (signed)%"PRId64, (int64_t)value);
    }
    fputc('\n', f);

    fprintf(f, "%s%-*s = %"PRIu64" bytes\n", p, w, "size", size);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Works like hexdump -C to display N bytes of DATA */
void
hexdump(FILE *f, addr_t base_addr, const char *prefix, const unsigned char *data, size_t n)
{
    for (size_t i=0; i<n; i+=16) {
        fprintf(f, "%s0x%08"PRIx64, prefix, base_addr+i);
        for (size_t j=0; j<16; j++) {
            if (8==j) fputc(' ', f);
            if (i+j<n) {
                fprintf(f, " %02x", data[i+j]);
            } else {
                fputs("   ", f);
            }
        }
        fprintf(f, "  |");
        for (size_t j=0; j<16 && i+j<n; j++) {
            if (isprint(data[i+j])) {
                fputc(data[i+j], f);
            } else {
                fputc('.', f);
            }
        }
        fputs("|\n", f);
    }
}

// FIXME: This cut-n-pasted version of Exec::ELF::parse() is out-of-date (rpm 2008-07-10)
/* Top-level binary executable file parser. Given the name of a file, open the file, detect the format, parse the file,
 * and return information about the file. */
void
parseBinaryFormat(const std::string & name, SgAsmFile* asmFile)
{
     ExecFile *ef = new ExecFile(name.c_str());
    
     asmFile->set_name(name);

     if (ELF::is_ELF(ef))
        {
          ELF::parseBinaryFormat(ef,asmFile);
        }
       else
        {
          if (PE::is_PE(ef))
             {
               PE::parseBinaryFormat(ef,asmFile);
             }
            else
             {
               delete ef;
               throw FormatError("unrecognized file format");
             }
        }

  // return ef;
}

// DQ (6/15/2008): Old function name (confirmed to not be called in ROSE)
/* Top-level binary executable file parser. Given the name of a file, open the file, detect the format, parse the file,
 * and return information about the file. */
ExecFile *
parse(const char *name)
{
    ExecFile *ef = new ExecFile(name);
    
    if (ELF::is_ELF(ef)) {
        ELF::parse(ef);
    } else if (PE::is_PE(ef)) {
        PE::parse(ef);
    } else if (NE::is_NE(ef)) {
        NE::parse(ef);
    } else if (LE::is_LE(ef)) { /*or LX*/
        LE::parse(ef);
    } else if (DOS::is_DOS(ef)) {
        /* Must be after PE and NE all PE and NE files are also DOS files */
        DOS::parse(ef);
    } else {
        delete ef;
        /* Use file(1) to try to figure out the file type to report in the exception */
        int child_stdout[2];
        pipe(child_stdout);
        pid_t pid = fork();
        if (0==pid) {
            close(0);
            dup2(child_stdout[1], 1);
            close(child_stdout[0]);
            close(child_stdout[1]);
            execlp("file", "file", "-b", name, NULL);
            exit(1);
        } else if (pid>0) {
            char buf[4096];
            memset(buf, 0, sizeof buf);
            read(child_stdout[0], buf, sizeof buf);
            buf[sizeof(buf)-1] = '\0';
            if (char *nl = strchr(buf, '\n')) *nl = '\0'; /*keep only first line w/o LF*/
            waitpid(pid, NULL, 0);
            char mesg[64+sizeof buf];
            sprintf(mesg, "unrecognized file format: %s", buf);
            throw FormatError(mesg);
        } else {
            throw FormatError("unrecognized file format");
        }
    }
    return ef;
}

}; /*namespace*/
