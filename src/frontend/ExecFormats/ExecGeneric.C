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
void
ExecFile::ctor(std::string fileName)
{
    if ((fd=open(fileName.c_str(), O_RDONLY))<0 || fstat(fd, &sb)<0) {
        throw FormatError("Could not open binary file: " + fileName);
    }
    if (NULL==(data=(unsigned char*)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0))) {
        throw FormatError("Could not mmap binary file: " + fileName);
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

/* Returns pointer to the first section with the specified name. */
ExecSection *
ExecFile::get_section_by_name(const std::string &name)
{
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
        if (section->is_mapped() && rva>=section->get_mapped() && rva<section->get_mapped()+section->get_size()) {
            retval.push_back(section);
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

/* Synthesizes sections to describe the parts of the file that are not yet referenced by other sections. */
void
ExecFile::fill_holes()
{
    std::vector<std::pair<addr_t,addr_t> > extents;

    addr_t offset = 0;
    while (offset < (uint64_t)sb.st_size) {
        std::vector<ExecSection*> sections = get_sections_by_offset(offset, 1);
        if (0==sections.size()) {
            addr_t next_offset = get_next_section_offset(offset);
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

/* Destructor must remove the section from its parent file's segment list */
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

/* Adds a new segment to a section. This is called implicitly by the segment constructor. */
void
ExecSection::add_segment(ExecSegment *segment)
{
#ifndef NDEBUG
    /* New segment must not already be present. */
    for (size_t i=0; i<segments.size(); i++) {
        ROSE_ASSERT(segments[i]!=segment);
    }
#endif
    segments.push_back(segment);
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
    if (offset + this->size + size > file->get_size()) throw ShortRead(this, offset+this->size, size);
    const unsigned char *retval = data + this->size;
    this->size += size;
    return retval;
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
    
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n", p, w, "base_va",   base_va);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n", p, w, "entry_rva", entry_rva);

    for (size_t i=0; i<dlls.size(); i++)
        dlls[i]->dump(f, p, i);

    for (size_t i=0; i<symbols.size(); i++)
        symbols[i]->dump(f, p, i);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExecSegment
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor adds segment to its parent section's segment list. */
void
ExecSegment::ctor(ExecSection *section, addr_t offset, addr_t size, addr_t rva, addr_t mapped_size)
{
    ROSE_ASSERT(section);
    if (offset > section->get_size() || offset+size > section->get_size())
        throw ShortRead(section, offset, size);
    
    this->section = section;
    this->offset = offset;
    this->disk_size = size;
    this->mapped_size = mapped_size;
    this->mapped_rva = rva;
    this->writable = false;
    this->executable = false;

    section->add_segment(this);
}

/* The destructor removes the section from its ExecSegment parent. */
ExecSegment::~ExecSegment()
{
    if (section) {
        std::vector<ExecSegment*> &segments = section->get_segments();
        std::vector<ExecSegment*>::iterator i = segments.begin();
        while (i!=segments.end()) {
            if (*i==this) {
                i = segments.erase(i);
            } else {
                i++;
            }
        }
    }
}

/* Print some debugging info about the segment */
void
ExecSegment::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sExecSegment[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sExecSegment.", prefix);
    }
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
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "type", s_type);

    fprintf(f, "%s%-*s = 0x%08"PRIx64, p, w, "value", value);
    if (value>9) {
        fprintf(f, " (unsigned)%"PRIu64, value);
        if ((int64_t)value<0) fprintf(f, " (signed)%"PRId64, (int64_t)value);
    }
    fputc('\n', f);

    fprintf(f, "%s%-*s = %"PRIx64" bytes\n", p, w, "size", size);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    } else {
        delete ef;
        throw FormatError("unrecognized file format");
    }
    return ef;
}


}; /*namespace*/
