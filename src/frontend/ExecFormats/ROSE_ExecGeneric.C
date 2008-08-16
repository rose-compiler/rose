/* Copyright 2008 Lawrence Livermore National Security, LLC */

// #include "ExecELF.h"
// #include "ExecLE.h"
// #include "ExecNE.h"
// #include "ExecPE.h"

#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <algorithm>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

// namespace Exec {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExecFormat
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Print some debugging info */
void
SgAsmGenericFormat::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096], sbuf[256];
    const char *s;
    if (idx>=0) {
        sprintf(p, "%sExecFormat[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sExecFormat.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    switch (get_family()) {
      case FAMILY_UNSPECIFIED:  s = "unspecified";                          break;
      case FAMILY_DOS:          s = "Microsoft DOS";                        break;
      case FAMILY_ELF:          s = "Executable and Linking Format (ELF)";  break;
      case FAMILY_LE:           s = "Microsoft Linear Executable (LE)";     break;
      case FAMILY_LX:           s = "OS/2 Extended Linear Executable (LX)"; break;
      case FAMILY_NE:           s = "Microsoft New Executable (NE)";        break;
      case FAMILY_PE:           s = "Microsoft Portable Executable (PE)";   break;
      default:
        sprintf(sbuf, "%u", get_family());
        s = sbuf;
        break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "family", s);

    switch (get_purpose()) {
      case PURPOSE_UNSPECIFIED:   s = "unspecified";                       break;
      case PURPOSE_OTHER:         s = "other";                             break;
      case PURPOSE_EXECUTABLE:    s = "executable program";                break;
      case PURPOSE_LIBRARY:       s = "library (shared or relocatable)";   break;
      case PURPOSE_CORE_DUMP:     s = "post mortem image (core dump)";     break;
      case PURPOSE_OS_SPECIFIC:   s = "operating system specific purpose"; break;
      case PURPOSE_PROC_SPECIFIC: s = "processor specific purpose";        break;
      default:
        sprintf(sbuf, "%u", get_purpose());
        s = sbuf;
        break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "purpose", s);

    switch (get_sex()) {
      case ORDER_UNSPECIFIED:     s = "unspecified";   break;
      case ORDER_LSB:             s = "little-endian"; break;
      case ORDER_MSB:             s = "big-endian";    break;
      default:
        sprintf(sbuf, "%u", get_sex());
        s = sbuf;
        break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "sex", s);

    fprintf(f, "%s%-*s = %u (%scurrent)\n", p, w, "version", get_version(), get_is_current_version() ? "" : "not-" );
    
    switch (get_abi()) {
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
        sprintf(sbuf, "%u", get_abi());
        s = sbuf;
        break;
    }
    fprintf(f, "%s%-*s = %s\n",  p, w, "ABI",      s);
    fprintf(f, "%s%-*s = %u\n",  p, w, "ABIvers",  get_abi_version());
    fprintf(f, "%s%-*s = %zu\n", p, w, "wordsize", get_word_size());
}
    
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExecFile
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructs by mapping file contents into memory */
void
SgAsmGenericFile::ctor(std::string fileName)
{
 // DQ (8/8/2008): Can we avoid assignments in the predicates?
    if ((p_fd = open(fileName.c_str(), O_RDONLY))<0 || fstat64(p_fd, &p_sb)<0) {
        std::string mesg = "Could not open binary file";
        throw FormatError(mesg + ": " + strerror(errno));
    }

 // DQ (8/8/2008): Can we avoid assignments in the predicates?
 // if (NULL == (p_data = (unsigned char*) mmap(NULL, p_sb.st_size, PROT_READ, MAP_PRIVATE, p_fd, 0))) {
    if (NULL == (p_data = (char*) mmap(NULL, p_sb.st_size, PROT_READ, MAP_PRIVATE, p_fd, 0))) {
        std::string mesg = "Could not mmap binary file";
        throw FormatError(mesg + ": " + strerror(errno));
    }

    ROSE_ASSERT(p_sections == NULL);
    ROSE_ASSERT(p_headers  == NULL);

    p_sections = new SgAsmGenericSectionList();
    p_headers  = new SgAsmGenericHeaderList();

    p_sections->set_parent(this);
    p_headers->set_parent(this);
}

/* Destructs by closing and unmapping the file and destroying all sections, headers, etc. */
SgAsmGenericFile::~SgAsmGenericFile() 
{
    printf ("In ~SgAsmGenericFile() \n");

    /* Delete subclasses before super classes (e.g., ExecHeader before ExecSection) */
    while (p_headers->get_headers().size()) {
        SgAsmGenericHeader *header = p_headers->get_headers().back();
        p_headers->get_headers().pop_back();
        delete header;
    }
    while (p_sections->get_sections().size()) {
        SgAsmGenericSection *section = p_sections->get_sections().back();
        p_sections->get_sections().pop_back();
        delete section;
    }

    ROSE_ASSERT(p_sections->get_sections().empty() == true);
    ROSE_ASSERT(p_headers->get_headers().empty()   == true);
    
    /* Unmap and close */
    if ( p_data != NULL )
        munmap(p_data, p_sb.st_size);

    if ( p_fd >= 0 )
        close(p_fd);

 // Delete the pointers to the IR nodes containing the STL lists
    delete p_sections;
    delete p_headers;

    p_sections = NULL;
    p_headers = NULL;
}

/* Adds a new header to the file. This is called implicitly by the header constructor */
void
SgAsmGenericFile::add_header(SgAsmGenericHeader *header) 
{
    ROSE_ASSERT(p_sections != NULL);
    ROSE_ASSERT(p_headers  != NULL);

#ifndef NDEBUG
    /* New header must not already be present. */
    for (size_t i=0; i< p_headers->get_headers().size(); i++) {
        ROSE_ASSERT(p_headers->get_headers()[i] != header);
    }
#endif
    p_headers->get_headers().push_back(header);

    p_headers->get_headers().back()->set_parent(p_headers);
}

/* Adds a new section to the file. This is called implicitly by the section constructor. */
void
SgAsmGenericFile::add_section(SgAsmGenericSection *section)
{
    ROSE_ASSERT(section != NULL);

    ROSE_ASSERT(p_sections != NULL);
    ROSE_ASSERT(p_headers  != NULL);

    printf ("SgAsmGenericFile::add_section(%p = %s): p_sections->get_sections().size() = %zu \n",section,section->class_name().c_str(),p_sections->get_sections().size());

#ifndef NDEBUG
    /* New section must not already be present. */
    for (size_t i = 0; i < p_sections->get_sections().size(); i++) {
        ROSE_ASSERT(p_sections->get_sections()[i] != section);
    }
#endif
    p_sections->get_sections().push_back(section);

    p_sections->get_sections().back()->set_parent(p_sections);
}

void
SgAsmGenericFile::remove_section(SgAsmGenericSection *section)
{
 // DQ (8/16/2008): Added this support to remove the effects of the SgAsmGenericFile::add_section()
 // bacause get_file() returns NULL in the SgAsmGenericSection destructor now that we have remove the
 // SgAsmGenericFile pointer from the SgAsmGenericSection IR node.

    ROSE_ASSERT(section != NULL);

    ROSE_ASSERT(p_sections != NULL);
    ROSE_ASSERT(p_headers  != NULL);

    printf ("SgAsmGenericFile::remove_section(%p = %s): p_sections->get_sections().size() = %zu \n",section,section->class_name().c_str(),p_sections->get_sections().size());

 // std::vector<SgAsmGenericSection*>::iterator i = p_sections->get_sections().find(section);
    std::vector<SgAsmGenericSection*>::iterator i = find(p_sections->get_sections().begin(),p_sections->get_sections().end(),section);
    if (i != p_sections->get_sections().end())
       {
         printf ("Found section = %p to remove from list \n",section);
         p_sections->get_sections().erase(i);
       }
}

/* Returns the pointer to the first section with the specified ID. */
SgAsmGenericSection *
SgAsmGenericFile::get_section_by_id(int id)
{
    for (std::vector<SgAsmGenericSection*>::iterator i = p_sections->get_sections().begin(); i != p_sections->get_sections().end(); i++) {
        if ((*i)->get_id() == id) {
            return *i;
        }
    }
    return NULL;
}

/* Returns pointer to the first section with the specified name. Any characters in the name after the first occurrence of SEP
 * are ignored (default is NUL). For instance, if sep=='$' then the following names are all equivalent: .idata, .idata$,
 * .idata$1 */
SgAsmGenericSection *
SgAsmGenericFile::get_section_by_name(std::string name, char sep)
{
    if (sep) {
        size_t pos = name.find(sep);
        if (pos!=name.npos)
            name.erase(pos);
    }
    
    for (std::vector<SgAsmGenericSection*>::iterator i = p_sections->get_sections().begin(); i != p_sections->get_sections().end(); i++) {
        if (0==(*i)->get_name().compare(name))
            return *i;
    }
    return NULL;
}

/* Returns a vector of sections that contain the specified portion of the file */
std::vector<SgAsmGenericSection*>
SgAsmGenericFile::get_sections_by_offset(Exec::addr_t offset, Exec::addr_t size)
{
    std::vector<SgAsmGenericSection*> retval;
    for (std::vector<SgAsmGenericSection*>::iterator i = p_sections->get_sections().begin(); i != p_sections->get_sections().end(); i++) {
        SgAsmGenericSection *section = *i;
        if (offset >= section->get_offset() &&
            offset < section->get_offset()+section->get_size() &&
            offset-section->get_offset() + size <= section->get_size())
            retval.push_back(section);
    }
    return retval;
}

/* Returns a vector of sections that are mapped to the specified RVA */
std::vector<SgAsmGenericSection*>
SgAsmGenericFile::get_sections_by_rva(Exec::addr_t rva)
{
    std::vector<SgAsmGenericSection*> retval;
    for (std::vector<SgAsmGenericSection*>::iterator i = p_sections->get_sections().begin(); i != p_sections->get_sections().end(); i++) {
        SgAsmGenericSection *section = *i;
        if (section->is_mapped() && rva >= section->get_mapped_rva() && rva < section->get_mapped_rva() + section->get_mapped_size()) {
            retval.push_back(section);
        }
    }
    return retval;
}

/* Returns a vector of sections that are mapped to the specified virtual address (VA). Sections are mapped by relative virtual
 * addresses (RVAs) that are based on the base VA of the section's file header. If the section is mapped but has no associated
 * file header then we assume zero for the base VA. */
std::vector<SgAsmGenericSection*>
SgAsmGenericFile::get_sections_by_va(Exec::addr_t va)
{
    std::vector<SgAsmGenericSection*> retval;
    for (std::vector<SgAsmGenericSection*>::iterator i = p_sections->get_sections().begin(); i != p_sections->get_sections().end(); i++) {
        SgAsmGenericSection *section = *i;
        if (section->is_mapped()) {
            SgAsmGenericHeader *hdr = section->get_header();
            Exec::addr_t base_va = hdr ? hdr->get_base_va() : 0;
            if (va>=base_va + section->get_mapped_rva() && va < base_va+section->get_mapped_rva() + section->get_mapped_size()) {
                retval.push_back(section);
            }
        }
    }
    return retval;
}

/* Like get_sections_by_va() except it verifies that the specified virtual address maps to the same file offset in all
 * sections containing that VA. It then returns a single section, giving preference to the section with the smallest mapped
 * size and having a non-negative identification number (i.e, appearing in a section table of some sort). */
SgAsmGenericSection *
SgAsmGenericFile::get_section_by_va(Exec::addr_t va)
{
    const std::vector<SgAsmGenericSection*> &possible = get_sections_by_va(va);
    if (0==possible.size()) {
        return NULL;
    } else if (1==possible.size()) {
        return possible[0];
    }

    /* Choose the "best" section to return. */
    SgAsmGenericSection *best = possible[0];
    Exec::addr_t fo0 = possible[0]->get_va_offset(va);
    for (size_t i=1; i<possible.size(); i++) {
        if (fo0 != possible[i]->get_va_offset(va))
            return NULL; /* all possible sections must map the VA to the same file offset */
        if (best->get_id()<0 && possible[i]->get_id()>0) {
            best = possible[i]; /*prefer sections defined in a section or object table*/
        } else if (best->get_mapped_size() > possible[i]->get_mapped_size()) {
            best = possible[i]; /*prefer sections with a smaller mapped size*/
        } else if (best->get_name().size()==0 && possible[i]->get_name().size()>0) {
            best = possible[i]; /*prefer sections having a name*/
        } else {
            /*prefer section defined earlier*/
        }
    }
    return best;
}

/* Given a file address, return the file offset of the following section(s). If there is no following section then return an
 * address of -1 (when signed) */
Exec::addr_t
SgAsmGenericFile::get_next_section_offset(Exec::addr_t offset)
{
    Exec::addr_t found = ~(Exec::addr_t)0;
    for (std::vector<SgAsmGenericSection*>::iterator i = p_sections->get_sections().begin(); i != p_sections->get_sections().end(); i++) {
        if ((*i)->get_offset() >= offset && (*i)->get_offset() < found)
            found = (*i)->get_offset();
    }
    return found;
}

/* Print basic info about the sections of a file */
void
SgAsmGenericFile::dump(FILE *f)
{
    if (p_sections->get_sections().size()==0) {
        fprintf(f, "No sections defined for file.\n");
        return;
    }
    
    /* Sort sections by offset and size */
    std::vector<SgAsmGenericSection*> sections = this->p_sections->get_sections();
    for (size_t i = 1; i < sections.size(); i++) {
        for (size_t j=0; j<i; j++) {
            if (sections[j]->get_offset() == sections[i]->get_offset()) {
                if (sections[j]->get_size() > sections[i]->get_size()) {
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
    fprintf(f, "  Flg File-Addr  File-Size  File-End    Virt-Addr  Virt-Size  Virt-End   Perm  ID Name\n");
    fprintf(f, "  --- ---------- ---------- ----------  ---------- ---------- ---------- ---- --- ----------------------------\n");
    Exec::addr_t high_water = 0;
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
        fprintf(f, " 0x%08"PRIx64" 0x%08"PRIx64" 0x%08"PRIx64, 
                section->get_offset(), section->get_size(), section->get_offset()+section->get_size());

        /* Mapped addresses */
        if (section->is_mapped()) {
            fprintf(f, "  0x%08"PRIx64" 0x%08"PRIx64" 0x%08"PRIx64, 
                    section->get_mapped_rva(), section->get_mapped_size(), section->get_mapped_rva()+section->get_mapped_size());
        } else {
            fprintf(f, " %*s", 3*11, "");
        }

        /* Permissions */
        if (section->is_mapped()) {
            fprintf(f, " %c%c%c ",
                    section->get_rperm()?'r':'-',
                    section->get_wperm()?'w':'-', 
                    section->get_eperm()?'x':'-');
        } else {
            fputs("     ", f);
        }

        /* Section ID, name */
        if (section->get_id()>=0) {
            fprintf(f, " %3d", section->get_id());
        } else {
            fputs("    ", f);
        }
        fprintf(f, " %s\n", section->get_name().c_str());
    }

    char overlap[4] = "   ";
    if (high_water < (Exec::addr_t)p_sb.st_size) {
        overlap[2] = 'H';
    } else if (sections.back()->get_offset() + sections.back()->get_size() < (Exec::addr_t)p_sb.st_size) {
        overlap[2] = 'h';
    }
    fprintf(f, "  %3s 0x%08"PRIx64"%*s EOF\n", overlap, (Exec::addr_t)p_sb.st_size, 65, "");
    fprintf(f, "  --- ---------- ---------- ----------  ---------- ---------- ---------- ---- --- ----------------------------\n");
}

/* Synthesizes sections to describe the parts of the file that are not yet referenced by other sections. */
void
SgAsmGenericFile::fill_holes()
{

    /* Find the holes and store their extent info */
    SgAsmGenericSection::ExtentVector extents;
    Exec::addr_t offset = 0;
    while (offset < (Exec::addr_t)p_sb.st_size) {
        std::vector<SgAsmGenericSection*> sections = get_sections_by_offset(offset, 0); /*all sections at this file offset*/
        
        /* Find the maximum ending offset */
        Exec::addr_t end_offset = 0;
        for (size_t i=0; i<sections.size(); i++) {
            Exec::addr_t tmp = sections[i]->get_offset() + sections[i]->get_size();
            if (tmp>end_offset)
                end_offset = tmp;
        }
        ROSE_ASSERT(end_offset <= (Exec::addr_t)p_sb.st_size);
        
        /* Is there a hole here? */
        if (end_offset<=offset) {
            end_offset = get_next_section_offset(offset+1);
            if (end_offset==(Exec::addr_t)-1)
                end_offset = p_sb.st_size;
            extents.push_back(SgAsmGenericSection::ExtentPair(offset, end_offset-offset));
        }
        
        /* Advance */
        offset = end_offset;
    }

    /* Create the sections representing the holes */
    for (size_t i=0; i<extents.size(); i++) {
        SgAsmGenericSection *section = new SgAsmGenericSection(this, extents[i].first, extents[i].second);
        section->set_synthesized(true);
        section->set_name("hole");
        section->set_purpose(SgAsmGenericSection::SP_UNSPECIFIED);
        section->congeal();
    }
}

/* Deletes "hole" sections */
void
SgAsmGenericFile::unfill_holes()
{
    /* Get a list of holes */
    std::vector<SgAsmGenericSection*> holes;
    for (size_t i=0; i < p_sections->get_sections().size(); i++) {
        if (p_sections->get_sections()[i]->get_id() < 0 && p_sections->get_sections()[i]->get_name().compare("hole")==0)
            holes.push_back(p_sections->get_sections()[i]);
    }

    /* Destroy the holes, removing them from the "sections" vector */
    for (std::vector<SgAsmGenericSection*>::iterator it = holes.begin(); it != holes.end(); it++)
        delete *it;
}

/* Mirror image of parsing an executable file. The result should be identical to the original file. */
void
SgAsmGenericFile::unparse(const char *filename)
{
    FILE *f = fopen(filename, "w");
    ROSE_ASSERT(f);

#if 0
    /* This is only for debugging -- fill the file with something other than zero so we have a better chance of making sure
     * that all data is written back to the file, including things that are zero. */
    Exec::addr_t remaining = p_sb.st_size;
    unsigned char fill=0xaa, buf[4096];
    memset(buf, fill, sizeof buf);
    while (remaining>=sizeof buf) {
        fwrite(buf, sizeof buf, 1, f);
        remaining -= sizeof buf;
    }
    fwrite(buf, remaining, 1, f);
#endif

    /* Write unreferenced sections (i.e., "holes") back to disk */
    for (size_t i=0; i< p_sections->get_sections().size(); i++) {
        if (p_sections->get_sections()[i]->get_id()<0 && p_sections->get_sections()[i]->get_name().compare("hole")==0)
            p_sections->get_sections()[i]->unparse(f);
    }
    
    /* Write file headers (and indirectly, all that they reference) */
    for (size_t i=0; i< p_headers->get_headers().size(); i++) {
        p_headers->get_headers()[i]->unparse(f);
    }
}

/* Return a string describing the file format. We use the last header so that files like PE, NE, LE, LX, etc. which also have
 * a DOS header report the format of the second (PE, etc.) header rather than the DOS header. */
const char *
SgAsmGenericFile::format_name()
{
    return p_headers->get_headers().back()->format_name();
}

/* Returns the header for the specified format. */
SgAsmGenericHeader *
SgAsmGenericFile::get_header(SgAsmGenericFormat::ExecFamily efam)
{
    SgAsmGenericHeader *retval = NULL;
    for (size_t i = 0; i < p_headers->get_headers().size(); i++) {
     // if (p_headers[i]->get_exec_format().get_family() == efam) {
        if (p_headers->get_headers()[i]->get_exec_format()->get_family() == efam) {
            ROSE_ASSERT(NULL == retval);
            retval = p_headers->get_headers()[i];
        }
    }

    return retval;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExecSection
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
SgAsmGenericSection::ctor(SgAsmGenericFile *ef, Exec::addr_t offset, Exec::addr_t size)
{
    ROSE_ASSERT(ef != NULL);
    if (offset > ef->get_size() || offset+size > ef->get_size())
        throw SgAsmGenericFile::ShortRead(NULL, offset, size);

    printf ("In SgAsmGenericSection::ctor() \n");

#if 0
 // This data member is removed from the SgAsmGenericSection, but it is added for the SgAsmGenericHeader.
    this->p_file = ef;
#endif
    ROSE_ASSERT(ef->get_sections() != NULL);
    set_parent(ef->get_sections());

    this->p_offset = offset;
    this->p_size = size;
    this->p_data = ef->content() + offset;

    /* The section is added to the file's list of sections. It may also be added to other lists by the caller. The destructor
     * will remove it from the file's list but does not know about other lists. */
    ef->add_section(this);
}

/* Destructor must remove the section from its parent file's section list */
SgAsmGenericSection::~SgAsmGenericSection()
{
 // if (p_file) {
 //     std::vector<SgAsmGenericSection*> & sections = p_file->get_sections()->get_sections();

    SgAsmGenericFile* genericFile = get_file();
    printf ("In SgAsmGenericSection destructor: genericFile = %p \n",genericFile);

    if (genericFile != NULL) {
        std::vector<SgAsmGenericSection*> & sections = genericFile->get_sections()->get_sections();
        std::vector<SgAsmGenericSection*>::iterator i = sections.begin();
        while (i != sections.end()) {
            if (*i==this) {
                i = sections.erase(i);
            } else {
                i++;
            }
        }
    }
}


/* Returns ptr to content at specified offset after ensuring that the required amount of data is available. One can think of
 * this function as being similar to fseek()+fread() in that it returns contents of part of a file as bytes. The main
 * difference is that instead of the caller supplying the buffer, the callee uses its own buffer (part of the buffer that was
 * returned by the OS from the mmap of the binary file).  The content() functions also keep track of what parts of the section
 * have been returned so that it's easy to find the parts that are apparently unused. */
const unsigned char *
SgAsmGenericSection::content(Exec::addr_t offset, Exec::addr_t size)
{
    if (offset > this->p_size || offset+size > this->p_size)
        throw SgAsmGenericFile::ShortRead(this, offset, size);
    if (!p_congealed && size > 0)
        p_referenced.insert(std::make_pair(offset, offset+size));

 // DQ (8/8/2008): Added cast, but this should be fixed better. FIXME
    return (const unsigned char *) p_data + offset;
}

/* Copies the specified part of the section into a buffer. This is more like fread() than the two-argument version in that the
 * caller must supply the buffer (the two-arg version returns a ptr to the mmap'd memory). Any part of the selected area that
 * is outside the domain of the section will be filled with zero (in contrast to the two-argument version that throws an
 * exception). */
void
SgAsmGenericSection::content(Exec::addr_t offset, Exec::addr_t size, void *buf)
{
    if (offset >= this->p_size) {
        memset(buf, 0, size);
    } else if (offset+size > this->p_size) {
        Exec::addr_t nbytes = this->p_size - offset;
        memcpy(buf, p_data+offset, nbytes);
        memset((char*)buf+nbytes, 0, size-nbytes);
        if (!p_congealed)
            p_referenced.insert(std::make_pair(offset, offset+nbytes));
    } else {
        memcpy(buf, p_data+offset, size);
        if (!p_congealed)
            p_referenced.insert(std::make_pair(offset, offset+size));
    }
}

/* Returns ptr to a NUL-terminated string */
const char *
SgAsmGenericSection::content_str(Exec::addr_t offset)
{
    const char *ret = (const char*) (p_data + offset);
    size_t nchars=0;

#if 0 /*DEBUGGING*/
    printf ("SgAsmGenericSection::content_str(offset): p_data = %p offset = %zu p_size = %zu \n",p_data,offset,p_size);
#endif

    while (offset+nchars < p_size && ret[nchars]) nchars++;
    nchars++; /*NUL*/

    if (offset+nchars > p_size)
        throw SgAsmGenericFile::ShortRead(this, offset, nchars);
    if (!p_congealed)
        p_referenced.insert(std::make_pair(offset, offset+nchars));

    return ret;
}

/* Like the low-level content(addr_t,addr_t) but returns an object rather than a ptr directly into the file content. This is
 * the recommended way to obtain file content for IR nodes that need to point to that content. The other function is more of a
 * low-level, efficient file read operation. */
const SgUnsignedCharList
SgAsmGenericSection::content_ucl(Exec::addr_t offset, Exec::addr_t size)
{
    const unsigned char *data = content(offset, size);
    SgUnsignedCharList returnValue;
    for (addr_t i=0; i<size; i++)
        returnValue.push_back(data[i]);
    return returnValue;
}

/* Write data back to a file section. The data to write may be larger than the file section as long as the extra (which will
 * not be written) is all zero. The offset is relative to the start of the section. */
Exec::addr_t
SgAsmGenericSection::write(FILE *f, Exec::addr_t offset, size_t bufsize, const void *buf)
{
    size_t nwrite, nzero;
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
    
    int status = fseek(f, get_offset()+offset, SEEK_SET);
    ROSE_ASSERT(status>=0);
    size_t nwritten = fwrite(buf, 1, nwrite, f);
    ROSE_ASSERT(nwrite==nwritten);
    
    for (size_t i=nwrite; i<bufsize; i++) {
        if (((const char*)buf)[i]) {
            char mesg[1024];
            sprintf(mesg, "non-zero value truncated: buf[0x%zx]=0x%02x", i, ((const unsigned char*)buf)[i]);
#if 1
            fprintf(stderr, "ROBB: Exec::ExecSection::write(): %s\n", mesg);
            Exec::hexdump(stderr, get_offset()+offset, "      ", (const unsigned char*)buf, bufsize);
            abort(); /*DEBUGGING*/
#endif
            throw SgAsmGenericFile::ShortWrite(this, offset, bufsize, mesg);
        }
    }

    return offset+bufsize;
}

/* Congeal the references to find the unreferenced areas. Once the references are congealed calling content(), content_ucl(),
 * content_str(), etc. will not affect references. This allows us to read the unreferenced areas without turning them into
 * referenced areas. */
const SgAsmGenericSection::ExtentVector &
SgAsmGenericSection::congeal()
{

    if (!p_congealed) {
        p_holes.clear();
        Exec::addr_t old_end = 0;
        for (RefMap::iterator it = p_referenced.begin(); it != p_referenced.end(); it++) {
            ExtentPair value = *it;
            ROSE_ASSERT(value.first <= value.second);
            if (value.first > old_end)
                p_holes.push_back(std::make_pair(old_end, value.first));
            if (value.second > old_end)
                old_end = value.second;
        }
        if (p_size > old_end)
            p_holes.push_back(std::make_pair(old_end, p_size));
        p_referenced.clear();
        p_congealed = true;
    }

    return p_holes;
}

/* Uncongeal the holes */
const SgAsmGenericSection::RefMap &
SgAsmGenericSection::uncongeal()
{
    if (p_congealed) {
        p_referenced.clear();
        Exec::addr_t old_end = 0;
        for (ExtentVector::iterator it = p_holes.begin(); it != p_holes.end(); it++) {
            ExtentPair value = *it;
            ROSE_ASSERT(value.first >= old_end);
            ROSE_ASSERT(value.first <= value.second);
            if (value.first > old_end)
                p_referenced.insert(std::make_pair(old_end, value.first));
            if (value.second > old_end)
                old_end = value.second;
        }
        if (p_size > old_end)
            p_referenced.insert(std::make_pair(old_end, p_size));
        p_congealed = false;
        p_holes.clear();
    }
    return p_referenced;
}

/* Extend a section by some number of bytes. */
void
SgAsmGenericSection::extend(Exec::addr_t size)
{
    ROSE_ASSERT(get_file() != NULL);
    if (p_offset + this->p_size + size > get_file()->get_size())
        throw SgAsmGenericFile::ShortRead(this, p_offset+this->p_size, size);
    this->p_size += size;
}

/* Like extend() but is more relaxed at the end of the file: if extending the section would cause it to go past the end of the
 * file then it is extended to the end of the file and no exception is thrown. */
void
SgAsmGenericSection::extend_up_to(Exec::addr_t size)
{
    ROSE_ASSERT(get_file() != NULL);

    if (p_offset + this->p_size + size > get_file()->get_size()) {
        ROSE_ASSERT(this->p_offset <= get_file()->get_size());
        this->p_size = get_file()->get_size() - this->p_offset;
    } else {
        this->p_size += size;
    }
}

/* True (the ExecHeader pointer) if this section is also a top-level file header, false (NULL) otherwise. */
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

/* Write a section back to the file. This is the generic version that simply writes the content. Subclasses should override
 * this. */
void
SgAsmGenericSection::unparse(FILE *f)
{
#if 0
    /* FIXME: for now we print the names of all sections we dump using this method. Eventually most of these sections will
     *        have subclasses that override this method. */
    fprintf(stderr, "Exec::ExecSection::unparse(FILE*) for section [%d] \"%s\"\n", id, name.c_str());
#endif

    write(f, 0, p_size, p_data);
}

/* Write just the specified regions back to the file */
void
SgAsmGenericSection::unparse(FILE *f, const ExtentVector &ev)
{
    for (size_t i = 0; i < ev.size(); i++) {
        ExtentPair p = ev[i];
        ROSE_ASSERT(p.first <= p.second);
        ROSE_ASSERT(p.second <= p_size);
        Exec::addr_t extent_offset = p.first;
        Exec::addr_t extent_size   = p.second - p.first;
        const unsigned char *extent_data = content(extent_offset, extent_size);
        write(f, extent_offset, extent_size, extent_data);
    }
}

/* Write holes (unreferenced areas) back to the file */
void
SgAsmGenericSection::unparse_holes(FILE *f)
{
    bool was_congealed = p_congealed;
    unparse(f, congeal());
    if (!was_congealed)
        uncongeal();
}

/* Returns the file offset associated with the virtual address of a mapped section. */
Exec::addr_t
SgAsmGenericSection::get_va_offset(Exec::addr_t va)
{
    ROSE_ASSERT(is_mapped());
    SgAsmGenericHeader *hdr = get_header();
    ROSE_ASSERT(hdr);
    ROSE_ASSERT(va >= hdr->get_base_va());
    Exec::addr_t rva = va - hdr->get_base_va();
    ROSE_ASSERT(rva >= get_mapped_rva());
    return get_offset() + (rva - get_mapped_rva());
}

/* Print some debugging info */
void
SgAsmGenericSection::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096], sbuf[256];
    const char *s;
    if (idx>=0) {
        sprintf(p, "%sExecSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sExecSection.", prefix);
    }
    
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = \"%s\"\n",                      p, w, "name",        p_name.c_str());
    fprintf(f, "%s%-*s = %d\n",                          p, w, "id",          p_id);
    fprintf(f, "%s%-*s = %" PRId64 " bytes into file\n", p, w, "offset",      p_offset);
    fprintf(f, "%s%-*s = %" PRId64 " bytes\n",           p, w, "size",        p_size);
    fprintf(f, "%s%-*s = %s\n",                          p, w, "synthesized", p_synthesized?"yes":"no");
    if (p_header) {
        fprintf(f, "%s%-*s = \"%s\"\n",                  p, w, "header",      p_header->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = not associated\n",          p, w, "header");
    }
    
    switch (p_purpose) {
      case SP_UNSPECIFIED: s = "not specified"; break;
      case SP_PROGRAM:     s = "program-supplied data/code/etc"; break;
      case SP_HEADER:      s = "executable format header";       break;
      case SP_SYMTAB:      s = "symbol table";                   break;
      case SP_OTHER:       s = "other";                          break;
      default:
        sprintf(sbuf, "%u", p_purpose);
        s = sbuf;
        break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "purpose", s);

    if (p_mapped) {
        fprintf(f, "%s%-*s = rva=0x%08"PRIx64", size=%"PRIu64" bytes\n", p, w, "mapped",  p_mapped_rva, p_mapped_size);
        fprintf(f, "%s%-*s = %c%c%c\n", p, w, "permissions",
                p_rperm?'r':'-', p_wperm?'w':'-', p_eperm?'x':'-');
    } else {
        fprintf(f, "%s%-*s = <not mapped>\n",    p, w, "mapped");
    }

    /* Show holes based on what's been referenced so far */
    fprintf(f, "%s%-*s = %s\n", p, w, "congealed", p_congealed?"true":"false");
    bool was_congealed = p_congealed;
    const ExtentVector & holes = congeal();
    for (size_t i = 0; i < holes.size(); i++) {
        ExtentPair extent = holes[i];
        Exec::addr_t hole_size = extent.second - extent.first;
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
SgAsmGenericHeader::ctor(SgAsmGenericFile *ef, Exec::addr_t offset, Exec::addr_t size)
{
    set_synthesized(true);
    set_purpose(SP_HEADER);
    ef->add_header(this);

#if 0
 // DQ (8/15/2008): Note that this can fail, but I don't know why!
    ROSE_ASSERT(p_symbols != NULL);
    ROSE_ASSERT(p_dlls    != NULL);
    ROSE_ASSERT(p_target  != NULL);
#endif

 // DQ (8/16/2008): This is defined only for SgAsmGenericHeader, and not for SgAsmGenericSection
    set_file(ef);

    if (p_symbols == NULL)
         p_symbols = new SgAsmGenericSymbolList;

    if (p_dlls == NULL)
         p_dlls    = new SgAsmGenericDLLList;

    if (p_target == NULL)
         p_target  = new SgAsmGenericArchitecture;

    p_symbols->set_parent(this);
    p_dlls->set_parent(this);
    p_target->set_parent(this);

 // The SgAsmGenericFormat is contained as a pointer and not a value data member, 
 // so we have to build one and initialize the pointer.
    SgAsmGenericFormat* local_exec_format = new SgAsmGenericFormat();
    ROSE_ASSERT(local_exec_format != NULL);
    set_exec_format(local_exec_format);
    ROSE_ASSERT(p_exec_format != NULL);

    local_exec_format->set_parent(this);
}

/* Destructor must remove the header from its parent file's headers list. */
SgAsmGenericHeader::~SgAsmGenericHeader() 
{
    if (get_file() != NULL) {
        std::vector<SgAsmGenericHeader*> & headers = get_file()->get_headers()->get_headers();
        std::vector<SgAsmGenericHeader*>::iterator i = headers.begin();
        while (i != headers.end()) {
            if (*i==this) {
                i = headers.erase(i);
            } else {
                i++;
            }
        }
    }

    delete p_symbols;
    delete p_dlls;
    delete p_target;
    delete p_exec_format;

    p_symbols = NULL;
    p_dlls = NULL;
    p_target = NULL;
    p_exec_format = NULL;
}

/* Add a new DLL to the header DLL list */
void
SgAsmGenericHeader::add_dll(SgAsmGenericDLL *dll)
{
#ifndef NDEBUG
 // for (size_t i = 0; i < p_dlls.size(); i++) {
    for (size_t i = 0; i < p_dlls->get_dlls().size(); i++) {
        ROSE_ASSERT(p_dlls->get_dlls()[i] != dll); /*duplicate*/
    }
#endif
    p_dlls->get_dlls().push_back(dll);
}


/* Add a new symbol to the symbol table */
void
SgAsmGenericHeader::add_symbol(SgAsmGenericSymbol *symbol)
{
#ifndef NDEBUG
    for (size_t i = 0; i < p_symbols->get_symbols().size(); i++) {
        ROSE_ASSERT(p_symbols->get_symbols()[i] != symbol); /*duplicate*/
    }
#endif
    p_symbols->get_symbols().push_back(symbol);

    p_symbols->get_symbols().back()->set_parent(p_symbols);
}

/* Print some debugging info */
void
SgAsmGenericHeader::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sExecHeader[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sExecHeader.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    SgAsmGenericSection::dump(f, p, -1);

    ROSE_ASSERT(p_exec_format != NULL);
    p_exec_format->dump(f, p, -1);

    fprintf(f, "%s%-*s = %s\n", p, w, "target",   "<FIXME>");

    fprintf(f, "%s%-*s = \"", p, w, "magic");
    for (size_t i = 0; i < p_magic.size(); i++) {
        switch (p_magic[i]) {
          case '\\': fputs("\\\\", f); break;
          case '\n': fputs("\\n", f); break;
          case '\r': fputs("\\r", f); break;
          case '\t': fputs("\\t", f); break;
          default:
            if (isprint(p_magic[i])) {
                fputc(p_magic[i], f);
            } else {
                fprintf(f, "\\%03o", (unsigned)p_magic[i]);
            }
            break;
        }
    }
    fputs("\"\n", f);
    
    /* Base virtual address and entry addresses */
    char also[1024];
    sprintf(also, "\n%*s also", (int)strlen(p)+w+12, "");
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n", p, w, "base_va",   p_base_va);
    fprintf(f, "%s%-*s = %zu entry points\n", p, w, "entry_rva.size", p_entry_rvas.size());
    for (size_t i = 0; i < p_entry_rvas.size(); i++) {
        fprintf(f, "%s%-*s = [%zu] 0x%08"PRIx64, p, w, "entry_rva", i, p_entry_rvas[i]);
        std::vector<SgAsmGenericSection*> sections = get_file()->get_sections_by_rva(p_entry_rvas[i]);
        for (size_t j = 0; j < sections.size(); j++) {
            fprintf(f, "%s in section [%d] \"%s\"", j?also:"",  sections[j]->get_id(), sections[j]->get_name().c_str());
        }
        fputc('\n', f);
    }

    fprintf(f, "%s%-*s = %zu entries\n", p, w, "ExecDLL.size", p_dlls->get_dlls().size());
    for (size_t i = 0; i < p_dlls->get_dlls().size(); i++)
        p_dlls->get_dlls()[i]->dump(f, p, i);

    fprintf(f, "%s%-*s = %zu entries\n", p, w, "ExecSymbol.size", p_symbols->get_symbols().size());
    for (size_t i = 0; i < p_symbols->get_symbols().size(); i++)
        p_symbols->get_symbols()[i]->dump(f, p, i);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dynamically linked libraries
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Print some debugging info */
void
SgAsmGenericDLL::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sExecDLL[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sExecDLL.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = \"%s\"\n", p, w, "name", p_name.c_str());
    for (size_t i = 0; i < p_funcs.size(); i++)
        fprintf(f, "%s%-*s = [%zd] \"%s\"\n", p, w, "func", i, p_funcs[i].c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Symbols and symbol tables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Print some debugging info */
void
SgAsmGenericSymbol::dump(FILE *f, const char *prefix, ssize_t idx) 
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sExecSymbol[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sExecSymbol.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = \"%s\"\n", p, w, "name", p_name.c_str());

    const char *s_def_state = NULL;
    switch (p_def_state) {
      case SYM_UNDEFINED: s_def_state = "undefined"; break;
      case SYM_TENTATIVE: s_def_state = "tentative"; break;
      case SYM_DEFINED:   s_def_state = "defined";   break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "def_state", s_def_state);

    const char *s_bind = NULL;
    switch (p_binding) {
      case SYM_NO_BINDING: s_bind = "no-binding"; break;
      case SYM_LOCAL:      s_bind = "local";      break;
      case SYM_GLOBAL:     s_bind = "global";     break;
      case SYM_WEAK:       s_bind = "weak";       break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "binding", s_bind);
    
    const char *s_type = NULL;
    switch (p_type) {
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
    if (p_bound) {
        fprintf(f, "%s%-*s = [%d] \"%s\" @%"PRIu64", %"PRIu64" bytes\n", p, w, "bound",
                p_bound->get_id(), p_bound->get_name().c_str(), p_bound->get_offset(), p_bound->get_size());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "bound");
    }
    
    fprintf(f, "%s%-*s = 0x%08"PRIx64, p, w, "value", p_value);
    if (p_value > 9) {
        fprintf(f, " (unsigned)%"PRIu64, p_value);
        if ((int64_t)p_value < 0) fprintf(f, " (signed)%"PRId64, (int64_t)p_value);
    }
    fputc('\n', f);

    fprintf(f, "%s%-*s = %"PRIu64" bytes\n", p, w, "size", p_size);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Works like hexdump -C to display N bytes of DATA */
void
SgAsmExecutableFileFormat::hexdump(FILE *f, Exec::addr_t base_addr, const char *prefix, const unsigned char *data, size_t n)
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

// DQ (11/8/2008): Alternative interface that works better for ROSE IR nodes
void
SgAsmExecutableFileFormat::hexdump(FILE *f, addr_t base_addr, const std::string &prefix, const SgCharList &data)
{
    hexdump(f, base_addr, prefix.c_str(), &(data[0]), data.size());
}

/* Writes a new file from the IR node for a parse executable file.  This is primarily to debug the parser by creating
 * an executable that *should* be identical to the original. */
void
SgAsmExecutableFileFormat::unparseBinaryFormat(const std::string &name, SgAsmFile *asmFile)
{
    FILE *output = fopen(name.c_str(), "w");
    ROSE_ASSERT(output!=NULL);
    
    /* FIXME: executable files may have more than a single file header (e.g., a PE file has a DOS header and a PE header). For
     *        now we just unparse one of the headers. See SgAsmGenericFile::unparse for more info -- it was the old top-level
     *        node. */

 // DQ (8/16/2008): Modified code to support STL container of "SgAsmGenericHeader*" types.
 // SgAsmGenericHeader *file_header = asmFile->get_header();
    ROSE_ASSERT(asmFile->get_headers() != NULL);
    ROSE_ASSERT(asmFile->get_headers()->get_headers().empty() == false);

    SgAsmGenericHeader *file_header = asmFile->get_headers()->get_headers()[0];
    ROSE_ASSERT(file_header != NULL);
    
    file_header->unparse(output);
    fclose(output);
}

// FIXME: This cut-n-pasted version of Exec::ELF::parse() is out-of-date (rpm 2008-07-10)
/* Top-level binary executable file parser. Given the name of a file, open the file, detect the format, parse the file,
 * and return information about the file. */
void
SgAsmExecutableFileFormat::parseBinaryFormat(const std::string & name, SgAsmFile* asmFile)
{
     SgAsmGenericFile *ef = new SgAsmGenericFile(name.c_str());
     ROSE_ASSERT(ef != NULL);

     asmFile->set_name(name);

     SgAsmGenericHeader* executableHeader = NULL;

     if (SgAsmElfFileHeader::is_ELF(ef))
       {
      // ELF::parse(ef);
         executableHeader = SgAsmElfFileHeader::parse(ef);
       }
      else
       {
         if (SgAsmPEFileHeader::is_PE(ef))
            {
           // PE::parse(ef);
              executableHeader = SgAsmPEFileHeader::parse(ef);
            }
           else
            {
              if (SgAsmNEFileHeader::is_NE(ef))
                 {
                // NE::parse(ef);
                   executableHeader = SgAsmNEFileHeader::parse(ef);
                 }
                else
                 {
                   if (SgAsmLEFileHeader::is_LE(ef))
                      {
                     /* or LX */
                     // LE::parse(ef);
                        executableHeader = SgAsmLEFileHeader::parse(ef);
                      }
                     else
                      {
                        if (SgAsmDOSFileHeader::is_DOS(ef))
                           {
                          /* Must be after PE and NE all PE and NE files are also DOS files */
                          // DOS::parse(ef);
                             executableHeader = SgAsmDOSFileHeader::parse(ef);
                           }
                          else
                           {
                             delete ef;

#if 1
                             throw SgAsmGenericFile::FormatError("unrecognized file format");
#else
                          // DQ (8/9/2008): I don't understand the purpose of this code.

                          /* Use file(1) to try to figure out the file type to report in the exception */
                             int child_stdout[2];
                             pipe(child_stdout);
                             pid_t pid = fork();
                             if (0 == pid)
                                {
                                  close(0);
                                  dup2(child_stdout[1], 1);
                                  close(child_stdout[0]);
                                  close(child_stdout[1]);
                                  execlp("file", "file", "-b", name, NULL);
                                  exit(1);
                                }
                               else
                                {
                                  if (pid > 0)
                                     {
                                       char buf[4096];
                                       memset(buf, 0, sizeof buf);
                                       read(child_stdout[0], buf, sizeof buf);
                                       buf[sizeof(buf)-1] = '\0';
                                       if (char *nl = strchr(buf, '\n')) *nl = '\0'; /*keep only first line w/o LF*/
                                       waitpid(pid, NULL, 0);
                                       char mesg[64+sizeof buf];
                                       sprintf(mesg, "unrecognized file format: %s", buf);
                                       throw FormatError(mesg);
                                     }
                                    else
                                     {
                                       throw FormatError("unrecognized file format");
                                     }
                                }
#endif
                           }
                      }
                 }
            }
       }

     ROSE_ASSERT(executableHeader != NULL);

     ROSE_ASSERT(ef->get_parent() == executableHeader);
  // ef->set_parent(executableHeader);

  // asmFile->set_header(executableHeader);
     ROSE_ASSERT(asmFile->get_headers() != NULL);
     asmFile->get_headers()->get_headers().push_back(executableHeader);

     executableHeader->set_parent(asmFile->get_headers());

  // ROSE_ASSERT(asmFile->get_header() != NULL);
     ROSE_ASSERT(asmFile->get_headers()->get_headers().empty() == false);

  // return ef;
}

#if 0
// DQ (6/15/2008): Old function name (confirmed to not be called in ROSE)
/* Top-level binary executable file parser. Given the name of a file, open the file, detect the format, parse the file,
 * and return information about the file. */
SgAsmGenericFile *
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
#endif

// }; /*namespace*/
