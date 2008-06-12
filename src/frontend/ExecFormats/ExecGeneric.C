/* Copyright 2008 Lawrence Livermore National Security, LLC */

#include "ExecELF.h"
#include "ExecPE.h"

#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <algorithm>
#include <fcntl.h>
#include <sys/mman.h>

namespace Exec {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExecFormat
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Print some debugging info */
void
ExecFormat::dump(FILE *f, const char *prefix)
{
    char p[4096], sbuf[256];
    const char *s;
    sprintf(p, "%sExecFormat.", prefix);
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    switch (family) {
      case FAMILY_UNSPECIFIED: s = "unspecified"; break;
      case FAMILY_ELF:         s = "ELF";         break;
      case FAMILY_DOS:         s = "DOS";         break;
      case FAMILY_PE:          s = "PE";          break;
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
ExecFile::ExecFile(std::string fileName)
{
    if ((fd=open(fileName.c_str(), O_RDONLY))<0 || fstat(fd, &sb)<0) {
        throw FormatError("Could not open binary file: " + fileName);
    }
    if (NULL==(data=(unsigned char*)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0))) {
        throw FormatError("Could not mmap binary file: " + fileName);
    }
}

/* Returns file size in bytes */
addr_t
ExecFile::size()
{
    ROSE_ASSERT(fd>=0);
    return sb.st_size;
}

/* Closes and unmaps the file */
void
ExecFile::close() 
{
    if (data)
        munmap(data, sb.st_size);
    if (fd>=0)
        ::close(fd);
}

/* Returns vector of file headers. This is a subset of the vector of file sections. */
std::vector<ExecHeader*>
ExecFile::get_headers()
{
    std::vector<ExecHeader*> retval;
    for (size_t i=0; i<sections.size(); i++) {
        ExecHeader *hdr=NULL;
        try {
            hdr = dynamic_cast<ExecHeader*>(sections[i]);
        } catch(...) {/*void*/}
        if (hdr)
            retval.push_back(hdr);
    }
    return retval;
}

/* Returns vector of all sections (including file headers) defined in the file. */
std::vector<ExecSection*>
ExecFile::get_sections()
{
    return sections;
}

/* Returns vector of all segments across all sections */
std::vector<ExecSegment*>
ExecFile::get_segments()
{
    std::vector<ExecSegment*> retval;
    for (std::vector<ExecSection*>::iterator sec_i=sections.begin(); sec_i!=sections.end(); sec_i++) {
        std::vector<ExecSegment*> segments = (*sec_i)->get_segments();
        retval.insert(retval.end(), segments.begin(), segments.end());
    }
    return retval;
}

/* Returns the pointer to the section with the specified ID. */
ExecSection *
ExecFile::lookup_section_id(int id)
{
    for (std::vector<ExecSection*>::iterator i=sections.begin(); i!=sections.end(); i++) {
        if ((*i)->get_id() == id) {
            return *i;
        }
    }
    return NULL;
}

/* Returns pointer to first section with specified name. */
ExecSection *
ExecFile::lookup_section_name(const std::string &name)
{
    for (std::vector<ExecSection*>::iterator i=sections.begin(); i!=sections.end(); i++) {
        if (0==(*i)->get_name().compare(name))
            return *i;
    }
    return NULL;
}

/* Returns a vector of sections that contain the specified portion of the file */
std::vector<ExecSection*>
ExecFile::lookup_section_offset(addr_t offset, addr_t size)
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
ExecFile::lookup_section_rva(addr_t rva)
{
    std::vector<ExecSection*> retval;
    for (std::vector<ExecSection*>::iterator i=sections.begin(); i!=sections.end(); i++) {
        ExecSection *section = *i;
        if (section->is_mapped() && rva>=section->get_mapped() && rva<section->get_mapped()+section->get_size()) {
            retval.push_back(section);
        }
    }
    return retval;
}

/* Given a file address, return the file offset of the following section(s). If there is no following section then return an
 * address of -1 (when signed) */
addr_t
ExecFile::lookup_next_offset(addr_t offset)
{
    addr_t found = ~(addr_t)0;
    for (std::vector<ExecSection*>::iterator i=sections.begin(); i!=sections.end(); i++) {
        if ((*i)->get_offset() >= offset && (*i)->get_offset() < found)
            found = (*i)->get_offset();
    }
    return found;
}

/* Synthesizes sections to describe the parts of the file that are not yet referenced by other sections. */
void
ExecFile::find_holes()
{
    std::vector<std::pair<addr_t,addr_t> > extents;

    addr_t offset = 0;
    while (offset < (uint64_t)sb.st_size) {
        std::vector<ExecSection*> sections = lookup_section_offset(offset, 1);
        if (0==sections.size()) {
            addr_t next_offset = lookup_next_offset(offset);
            if (next_offset==(addr_t)-1) next_offset = sb.st_size;
            extents.push_back(std::pair<addr_t,addr_t>(offset, next_offset-offset));
            offset = next_offset;
        } else {
            offset += sections[0]->get_size();
        }
    }

    for (size_t i=0; i<extents.size(); i++) {
        ExecSection *section = new ExecSection(this, extents[i].first, extents[i].second);
        section->set_synthesized(true);
        section->set_name("hole");
        section->set_purpose(SP_UNSPECIFIED);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExecSection
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
ExecSection::ctor(ExecFile *f, addr_t offset, addr_t size)
{
    ROSE_ASSERT(f);
    if (offset > f->size() || offset+size > f->size())
        throw ShortRead(NULL, offset, size);
    
    this->file = f;
    this->offset = offset;
    this->size = size;
    this->data = f->data + offset;

    f->sections.push_back(this);
}

/* Destructor must remove the section from its ExecFile parent */
ExecSection::~ExecSection()
{
    if (file) {
        std::vector<ExecSection*>::iterator i = file->sections.begin();
        while (i!=file->sections.end()) {
            if (*i==this) {
                i = file->sections.erase(i);
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
    return data + offset;
}

/* Returns ptr to a NUL-terminated string */
const char *
ExecSection::content_str(addr_t offset)
{
    const char *ret = (const char*)content(offset, 0);
    size_t nchars=0;
    while (offset+nchars<size && ret[nchars]) nchars++;
    if (offset+nchars>=size)
        throw ShortRead(this, offset, nchars);
    return ret;
}

/* Extend a section by some number of bytes and return a pointer to the content of the newly extended area. Called with a zero
 * size is a convenient way to return a pointer to the first byte after the section, although depending on the size of the
 * file, this might not be a valid pointer. */
const unsigned char *
ExecSection::extend(addr_t size)
{
    ROSE_ASSERT(file);
    if (offset + this->size + size > file->size()) throw ShortRead(this, offset+this->size, size);
    const unsigned char *retval = data + this->size;
    this->size += size;
    return retval;
}

/* Returns 16-bit little-endian unsigned integer in native format */
uint16_t
ExecSection::u16le(addr_t offset)
{
    ROSE_ASSERT(file);
    if (offset > size || offset+2 > size) throw ShortRead(this, offset, 2);
    return data[offset+0] | (data[offset+1]<<8);
}

/* Returns 16-bit little-endian signed integer in native format or throws short read */
int16_t
ExecSection::s16le(addr_t offset)
{
    ROSE_ASSERT(file);
    if (offset > size || offset+2 > size) throw ShortRead(this, offset, 2);
    return data[offset+0] | (data[offset+1]<<8);
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
    
/* Print some debugging info */
void
ExecSection::dump(FILE *f, const char *prefix)
{
    char p[4096], sbuf[256];
    const char *s;
    sprintf(p, "%sExecSection.", prefix);
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = \"%s\"\n",                      p, w, "name",        name.c_str());
    fprintf(f, "%s%-*s = %d\n",                          p, w, "id",          id);
    fprintf(f, "%s%-*s = %" PRId64 " bytes into file\n", p, w, "offset",      offset);
    fprintf(f, "%s%-*s = %" PRId64 " bytes\n",           p, w, "size",        size);
    fprintf(f, "%s%-*s = %s\n",                          p, w, "synthesized", synthesized?"yes":"no");

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
        fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n", p, w, "mapped_rva", mapped_rva);
    } else {
        fprintf(f, "%s%-*s = <not mapped>\n",    p, w, "mapped_rva");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExecHeader
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
ExecHeader::ctor(ExecFile *f, addr_t offset, addr_t size)
{
    set_synthesized(true);
    set_purpose(SP_HEADER);
}

/* Print some debugging info */
void
ExecHeader::dump(FILE *f, const char *prefix)
{
    char p[4096];
    sprintf(p, "%sExecHeader.", prefix);
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    ExecSection::dump(f, p);
    fileFormat.dump(f, p);

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
    
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n", p, w, "base_va",   base_va);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n", p, w, "entry_rva", entry_rva);
}
    
            


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExecSegment
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Intialize values during construction */
void
ExecSegment::ctor(ExecSection *section, addr_t offset, addr_t size, addr_t rva, addr_t mapped_size)
{
    ROSE_ASSERT(section);
    if (offset > section->size || offset+size > section->size)
        throw ShortRead(section, offset, size);
    
    this->section = section;
    this->offset = offset;
    this->disk_size = size;
    this->mapped_size = mapped_size;
    this->mapped_rva = rva;
    this->writable = false;
    this->executable = false;

    section->segments.push_back(this);
}

/* The destructor removes the section from its ExecSegment parent. */
ExecSegment::~ExecSegment()
{
    if (section) {
        std::vector<ExecSegment*>::iterator i = section->segments.begin();
        while (i!=section->segments.end()) {
            if (*i==this) {
                i = section->segments.erase(i);
            } else {
                i++;
            }
        }
    }
}

/* Print some debugging info about the segment */
void
ExecSegment::dump(FILE *f, const char *prefix)
{
    char p[4096];
    sprintf(p, "%sExecSegment.", prefix);
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = \"%s\"\n",                         p, w, "name",          name.c_str());
    fprintf(f, "%s%-*s = [%d] \"%s\" @%"PRIu64", %"PRIu64" bytes\n", p, w, "section",
            section->get_id(), section->get_name().c_str(), section->get_offset(), section->get_size());
    fprintf(f, "%s%-*s = %" PRIu64 " bytes into section\n", p, w, "offset",        offset);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n",              p, w, "disk_size",     disk_size);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",                p, w, "address (rva)", mapped_rva);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n",              p, w, "mapped_size",   mapped_size);
    fprintf(f, "%s%-*s = %swritable, %sexecutable\n",       p, w, "permissions",   writable?"":"not-", executable?"":"not-");
}
    
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    } else {
        ef->close();
        throw FormatError("unrecognized file format");
    }
    return ef;
}


}; /*namespace*/
