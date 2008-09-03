/* Copyright 2008 Lawrence Livermore National Security, LLC */

#define _FILE_OFFSET_BITS 64
#include <sys/stat.h>

#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <algorithm>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

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
    p_fd = open(fileName.c_str(), O_RDONLY);
    if (p_fd<0 || fstat(p_fd, &p_sb)<0) {
        std::string mesg = "Could not open binary file";
        throw FormatError(mesg + ": " + strerror(errno));
    }

    /* Map the file into memory so we don't have to read it explicitly */
    unsigned char *mapped = (unsigned char*)mmap(NULL, p_sb.st_size, PROT_READ, MAP_PRIVATE, p_fd, 0);
    if (!mapped) {
        std::string mesg = "Could not mmap binary file";
        throw FormatError(mesg + ": " + strerror(errno));
    }

    /* Make file contents available through an STL vector without actually reading the file */
    p_data = SgFileContentList(mapped, p_sb.st_size);

    ROSE_ASSERT(p_headers  == NULL);
    p_headers  = new SgAsmGenericHeaderList();
    p_headers->set_parent(this);
}

/* Destructs by closing and unmapping the file and destroying all sections, headers, etc. */
SgAsmGenericFile::~SgAsmGenericFile() 
{
    /* Delete child headers before this */
    while (p_headers->get_headers().size()) {
        SgAsmGenericHeader *header = p_headers->get_headers().back();
        p_headers->get_headers().pop_back();
        delete header;
    }
    ROSE_ASSERT(p_headers->get_headers().empty()   == true);
    
    /* Unmap and close */
    unsigned char *mapped = p_data.pool();
    if (mapped && p_data.size()>0)
        munmap(mapped, p_data.size());
    p_data.clear();

    if ( p_fd >= 0 )
        close(p_fd);

 // Delete the pointers to the IR nodes containing the STL lists
    delete p_headers;
    p_headers = NULL;
}

/* Returns size of file */
rose_addr_t
SgAsmGenericFile::get_size() const
{
    return p_data.size();
}

/* Returns a vector that points to part of the file content without actually ever referencing the file content until the
 * vector elements are referenced. */
SgFileContentList
SgAsmGenericFile::content(addr_t offset, addr_t size)
{
    if (offset+size > p_data.size())
        throw SgAsmGenericFile::ShortRead(NULL, offset, size);
    return SgFileContentList(p_data, offset, size);
}

/* Adds a new header to the file. This is called implicitly by the header constructor */
void
SgAsmGenericFile::add_header(SgAsmGenericHeader *header) 
{
    ROSE_ASSERT(p_headers  != NULL);

#ifndef NDEBUG
    /* New header must not already be present. */
    for (size_t i=0; i< p_headers->get_headers().size(); i++) {
        ROSE_ASSERT(p_headers->get_headers()[i] != header);
    }
#endif
    header->set_parent(p_headers);
    p_headers->get_headers().push_back(header);
}

/* Removes a header from the header list in a file */
void
SgAsmGenericFile::remove_header(SgAsmGenericHeader *hdr)
{
    if (hdr!=NULL) {
        ROSE_ASSERT(p_headers  != NULL);
        std::vector<SgAsmGenericHeader*>::iterator i = find(p_headers->get_headers().begin(),
                                                            p_headers->get_headers().end(),
							    hdr);
        if (i != p_headers->get_headers().end()) {
            p_headers->get_headers().erase(i);
        }
    }
}

/* Returns list of all sections in the file, including headers. */
SgAsmGenericFile::sections_t
SgAsmGenericFile::get_sections()
{
    sections_t retval;

    retval.insert(retval.end(), p_headers->get_headers().begin(), p_headers->get_headers().end());
    for (headers_t::iterator i=p_headers->get_headers().begin(); i!=p_headers->get_headers().end(); ++i) {
        const sections_t &recurse = (*i)->get_sections();
        retval.insert(retval.end(), recurse.begin(), recurse.end());
    }
    return retval;
}

/* Returns all sections having specified ID in all headers */
SgAsmGenericFile::sections_t
SgAsmGenericFile::get_sections_by_id(int id)
{
    sections_t retval;
    for (headers_t::iterator i=p_headers->get_headers().begin(); i!=p_headers->get_headers().end(); ++i) {
        const sections_t &recurse = (*i)->get_sections_by_id(id);
        retval.insert(retval.end(), recurse.begin(), recurse.end());
    }
    return retval;
}

/* Returns the pointer to section with the specified ID across all headers only if there's exactly one match. */
SgAsmGenericSection *
SgAsmGenericFile::get_section_by_id(int id, size_t *nfound/*optional*/)
{
    sections_t possible = get_sections_by_id(id);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/* Returns all sections having specified name across all headers. */
SgAsmGenericFile::sections_t
SgAsmGenericFile::get_sections_by_name(const std::string &name, char sep/*or NUL*/)
{
    sections_t retval;
    for (headers_t::iterator i=p_headers->get_headers().begin(); i!=p_headers->get_headers().end(); ++i) {
        const sections_t &recurse = (*i)->get_sections_by_name(name, sep);
        retval.insert(retval.end(), recurse.begin(), recurse.end());
    }
    return retval;
}

/* Returns pointer to the section with the specified name, or NULL if there isn't exactly one match. Any characters in the name
 * after the first occurrence of SEP are ignored (default is NUL). For instance, if sep=='$' then the following names are all
 * equivalent: .idata, .idata$, and .idata$1 */
SgAsmGenericSection *
SgAsmGenericFile::get_section_by_name(const std::string &name, char sep/*or NUL*/, size_t *nfound/*optional*/)
{
    sections_t possible = get_sections_by_name(name, sep);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/* Returns all sections that contain all of the specified portion of the file across all headers. */
SgAsmGenericFile::sections_t
SgAsmGenericFile::get_sections_by_offset(addr_t offset, addr_t size)
{
    sections_t retval;
    for (headers_t::iterator i=p_headers->get_headers().begin(); i!=p_headers->get_headers().end(); ++i) {
        const sections_t &recurse = (*i)->get_sections_by_offset(offset, size);
        retval.insert(retval.end(), recurse.begin(), recurse.end());
    }
    return retval;
}

/* Returns single section that contains all of the specified portion of the file across all headers. */
SgAsmGenericSection *
SgAsmGenericFile::get_section_by_offset(addr_t offset, addr_t size, size_t *nfound)
{
    sections_t possible = get_sections_by_offset(offset, size);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/* Returns all sections that are mapped to include the specified relative virtual address across all headers. */
SgAsmGenericFile::sections_t
SgAsmGenericFile::get_sections_by_rva(addr_t rva)
{
    sections_t retval;
    for (headers_t::iterator i=p_headers->get_headers().begin(); i!=p_headers->get_headers().end(); ++i) {
        const sections_t &recurse = (*i)->get_sections_by_rva(rva);
        retval.insert(retval.end(), recurse.begin(), recurse.end());
    }
    return retval;
}

/* Returns single section that is mapped to include the specified relative virtual file address across all headers. */
SgAsmGenericSection *
SgAsmGenericFile::get_section_by_rva(addr_t rva, size_t *nfound/*optional*/)
{
    sections_t possible = get_sections_by_rva(rva);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/* Returns all sections that are mapped to include the specified virtual address across all headers. */
SgAsmGenericFile::sections_t
SgAsmGenericFile::get_sections_by_va(addr_t va)
{
    sections_t retval;
    for (headers_t::iterator i=p_headers->get_headers().begin(); i!=p_headers->get_headers().end(); ++i) {
        const sections_t &recurse = (*i)->get_sections_by_va(va);
        retval.insert(retval.end(), recurse.begin(), recurse.end());
    }
    return retval;
}

/* Returns single section that is mapped to include the specified virtual address across all headers. */
SgAsmGenericSection *
SgAsmGenericFile::get_section_by_va(addr_t va, size_t *nfound)
{
    sections_t possible = get_sections_by_va(va);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/* Similar to get_section_by_va() except when more than one section contains the specified virtual address we choose the
 * "best" one. All candidates must map the virtual address to the same file address or else we fail (return null and number of
 * candidates). See code below for definition of "best". */
SgAsmGenericSection *
SgAsmGenericFile::get_best_section_by_va(addr_t va, size_t *nfound)
{
    const sections_t &candidates = get_sections_by_va(va);
    if (nfound)
        *nfound = candidates.size();
    return best_section_by_va(condidates);
}

/* Definition for "best" as used by
 * SgAsmGenericFile::get_best_section_by_va() and
 * SgAsmGenericHeader::get_best_section_by_va() */
static SgAsmGenericSection *
SgAsmGenericFile::best_section_by_va(const sections_t &sections)
{
    if (1==sections.size()) 
        return sections[0];
    SgAsmGenericSection *best = sections[0];
    addr_t fo0 = sections[0]->get_va_offset(va);
    for (size_t i=1; i<sections.size(); i++) {
        if (fo0 != sections[i]->get_va_offset(va))
            return NULL; /* all sections sections must map the VA to the same file offset */
        if (best->get_mapped_size() > sections[i]->get_mapped_size()) {
            best = sections[i]; /*prefer sections with a smaller mapped size*/
        } else if (best->get_name().size()==0 && sections[i]->get_name().size()>0) {
            best = sections[i]; /*prefer sections having a name*/
        } else {
            /*prefer section defined earlier*/
        }
    }
    return best;
}

SgAsmGenericSection *
SgAsmGenericFile::get_best_possible_section_by_va(addr_t va)
   {
  // This function is implemented for use in:
  //      "DisassemblerCommon::AsmFileWithData::getSectionOfAddress(uint64_t addr)"
  // It supports a more restrictive selection of valid sections to associate with 
  // a given address so that we can avoid disassembly of sections that are not code.

     const std::vector<SgAsmGenericSection*> &possible = get_sections_by_va(va);

     if (0 == possible.size())
        {
          return NULL;
        }
       else
        {
          if (1 == possible.size())
             {
            // printf ("Only one alternative: va = %p possible[0] id = %d name = %s (return %s) \n",
            //      (void*)va,possible[0]->get_id(),possible[0]->get_name().c_str(),(possible[0]->get_id() < 0) ? "NULL" : "it");
            // return possible[0];
               if (possible[0]->get_id() < 0)
                    return NULL;
                 else
                    return possible[0];
             }
        }

#if 0
     printf ("Select from %zu alternatives \n",possible.size());
     for (size_t i = 0; i < possible.size(); i++)
        {
          printf ("   va = %p possible[%zu] id = %d name = %s \n",(void*)va,i,possible[i]->get_id(),possible[i]->get_name().c_str());
        }
#endif

  /* Choose the "best" section to return. */
     SgAsmGenericSection *best = possible[0];
     addr_t fo0 = possible[0]->get_va_offset(va);
     for (size_t i = 1; i < possible.size(); i++)
        {
          if (fo0 != possible[i]->get_va_offset(va))
            return NULL; /* all possible sections must map the VA to the same file offset */

          if (best->get_id() < 0 && possible[i]->get_id() > 0)
             {
               best = possible[i]; /*prefer sections defined in a section or object table*/
             }
            else
               if (best->get_mapped_size() > possible[i]->get_mapped_size())
                  {
                    best = possible[i]; /*prefer sections with a smaller mapped size*/
                  }
                 else
                    if (best->get_name().size()==0 && possible[i]->get_name().size()>0)
                       {
                         best = possible[i]; /*prefer sections having a name*/
                       }
                      else
                       {
                      /* prefer section defined earlier*/

                       }
        }

     ROSE_ASSERT(best != NULL);

  // Add a few things that we just don't want to disassemble
     if (best->get_name() == "ELF Segment Table")
          return NULL;

  // printf ("   best: va = %p id = %d name = %s \n",(void*)va,best->get_id(),best->get_name().c_str());

     return best;
   }

/* Given a file address, return the file offset of the following section(s). If there is no following section then return an
 * address of -1 (when signed) */
rose_addr_t
SgAsmGenericFile::get_next_section_offset(addr_t offset, SgAsmGenericHeader *hdr)
{
    addr_t found = ~(addr_t)0;
    for (std::vector<SgAsmGenericSection*>::iterator i = p_sections->get_sections().begin();
         i != p_sections->get_sections().end();
         i++) {
        if ((!hdr || hdr==(*i)->get_header()) &&
            (*i)->get_offset() >= offset && (*i)->get_offset() < found)
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
    fprintf(f, "  Flg File-Addr  File-Size  File-End    Base-VA    Start-RVA  Virt-Size  End-RVA    Perm  ID Name\n");
    fprintf(f, "  --- ---------- ---------- ----------  ---------- ---------- ---------- ---------- ---- --- -----------------\n");
    addr_t high_water = 0;
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
            fprintf(f, "  0x%08"PRIx64" 0x%08"PRIx64" 0x%08"PRIx64" 0x%08"PRIx64,
                    section->get_base_va(), section->get_mapped_rva(), section->get_mapped_size(),
                    section->get_mapped_rva()+section->get_mapped_size());
        } else {
            fprintf(f, " %*s", 4*11, "");
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
    if (high_water < get_size()) {
        overlap[2] = 'H';
    } else if (sections.back()->get_offset() + sections.back()->get_size() < get_size()) {
        overlap[2] = 'h';
    }
    fprintf(f, "  %3s 0x%08"PRIx64"%*s EOF\n", overlap, get_size(), 76, "");
    fprintf(f, "  --- ---------- ---------- ----------  ---------- ---------- ---------- ---------- ---- --- -----------------\n");
}

/* Synthesizes sections to describe the parts of the file that are not yet referenced by other sections. */
void
SgAsmGenericFile::fill_holes()
{

    /* Find the holes and store their extent info */
    SgAsmGenericSection::ExtentVector extents;
    addr_t offset = 0;
    while (offset < get_size()) {
        std::vector<SgAsmGenericSection*> sections = get_sections_by_offset(offset, 0); /*all sections at this file offset*/
        
        /* Find the maximum ending offset */
        addr_t end_offset = 0;
        for (size_t i=0; i<sections.size(); i++) {
            addr_t tmp = sections[i]->get_offset() + sections[i]->get_size();
            if (tmp>end_offset)
                end_offset = tmp;
        }
        ROSE_ASSERT(end_offset <= get_size());
        
        /* Is there a hole here? */
        if (end_offset<=offset) {
            end_offset = get_next_section_offset(offset+1);
            if (end_offset==(addr_t)-1)
                end_offset = get_size();
            extents.push_back(SgAsmGenericSection::ExtentPair(offset, end_offset-offset));
        }
        
        /* Advance */
        offset = end_offset;
    }

    /* Create the sections representing the holes */
    for (size_t i=0; i<extents.size(); i++) {
        SgAsmGenericSection *section = new SgAsmGenericSection(this, NULL, extents[i].first, extents[i].second);
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
SgAsmGenericFile::unparse(const std::string &filename)
{
    FILE *f = fopen(filename.c_str(), "wb");
    ROSE_ASSERT(f);

#if 0
    /* This is only for debugging -- fill the file with something other than zero so we have a better chance of making sure
     * that all data is written back to the file, including things that are zero. */
    addr_t remaining = get_size();
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
        if (p_sections->get_sections()[i]->get_id() < 0 && p_sections->get_sections()[i]->get_name().compare("hole") == 0)
            p_sections->get_sections()[i]->unparse(f);
    }
    
    /* Write file headers (and indirectly, all that they reference) */
    for (size_t i=0; i< p_headers->get_headers().size(); i++) {
        p_headers->get_headers()[i]->unparse(f);
    }

    fclose(f);
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
SgAsmGenericSection::ctor(SgAsmGenericFile *ef, SgAsmGenericHeader *hdr, addr_t offset, addr_t size)
{
    ROSE_ASSERT(ef != NULL);
    if (offset > ef->get_size() || offset+size > ef->get_size())
        throw SgAsmGenericFile::ShortRead(NULL, offset, size);

    /* Assert that if HDR is null then "this" is a header. */
    if (!hdr) {
        bool this_is_a_header;
        try {
            SgAsmGenericHeader *this_header = dynamic_cast<SgAsmGenericHeader*>(this);
            this_is_a_header = this_header!=NULL;
        } catch(...) {
            this_is_a_header = false;
        }
        ROSE_ASSERT(this_is_a_header);
    }

    /* Initialize data members */
    p_offset = offset;
    p_data = ef->content(offset, size);
    set_header(hdr);

    /* Add this section to either the file's header list or the header's section list. */
    if (hdr) {
        hdr->add_section(this);
    } else {
        ef->add_header(this);
    }
}

/* Destructor must remove the section from its parent file's or header's section list */
SgAsmGenericSection::~SgAsmGenericSection()
{
    SgAsmGenericFile* ef = get_file();
    SgAsmGenericHeader *hdr = get_header();
    
    /* Remove section from file's or header's section list */
    for (int pass=0; pass<2; pass++) {
        std::vector<SgAsmGenericSection*> *sections;
        switch (pass) {
          case 0:
            if (hdr)
                sections = &(hdr->get_sections()->get_sections());
            break;
          case 1:
            if (ef)
                sections = &(ef->get_sections()->get_sections());
            break;
        }
        std::vector<SgAsmGenericSection*>::iterator i = sections->begin();
        while (i != sections->end()) {
            if (*i==this) {
                i = sections->erase(i);
            } else {
                i++;
            }
        }
    }
}

/* Returns the file size of the section in bytes */
rose_addr_t
SgAsmGenericSection::get_size() const
{
    return p_data.size();
}

/* Returns starting byte offset in the file */
rose_addr_t
SgAsmGenericSection::end_offset()
{
    return get_offset() + get_size();
}

/* Returns whether section desires to be mapped to memory */
bool
SgAsmGenericSection::is_mapped()
{
    return p_mapped;
}

/* Causes section to be mapped to memory */
void
SgAsmGenericSection::set_mapped(addr_t rva, addr_t size)
{
    p_mapped = true;
    p_mapped_rva = rva;
    p_mapped_size = size;
}

/* Causes section to not be mapped to memory. */
void
SgAsmGenericSection::clear_mapped()
{
    p_mapped = false;
    p_mapped_rva = p_mapped_size = 0;
}

// DQ (8/8/2008): This is not standard semantics for an access function
rose_addr_t
SgAsmGenericSection::get_mapped_rva()
{
    return p_mapped ? p_mapped_rva  : 0;
}

/* Returns mapped size of section if mapped; zero otherwise */
rose_addr_t
SgAsmGenericSection::get_mapped_size()
{
    return p_mapped ? p_mapped_size : 0;
}

/* Returns base virtual address for a section, or zero if the section is not associated with a header. */
rose_addr_t
SgAsmGenericSection::get_base_va() const
{
    SgAsmGenericHeader *hdr = get_header();
    return hdr ? hdr->get_base_va() : 0;
}

/* Returns ptr to content at specified offset after ensuring that the required amount of data is available. One can think of
 * this function as being similar to fseek()+fread() in that it returns contents of part of a file as bytes. The main
 * difference is that instead of the caller supplying the buffer, the callee uses its own buffer (part of the buffer that was
 * returned by the OS from the mmap of the binary file).  The content() functions also keep track of what parts of the section
 * have been returned so that it's easy to find the parts that are apparently unused. */
const unsigned char *
SgAsmGenericSection::content(addr_t offset, addr_t size)
{
    if (offset > get_size() || offset+size > get_size())
        throw SgAsmGenericFile::ShortRead(this, offset, size);
    if (!p_congealed && size > 0)
        p_referenced.insert(std::make_pair(offset, offset+size));

    return &(p_data[offset]);
}

/* Copies the specified part of the section into a buffer. This is more like fread() than the two-argument version in that the
 * caller must supply the buffer (the two-arg version returns a ptr to the mmap'd memory). Any part of the selected area that
 * is outside the domain of the section will be filled with zero (in contrast to the two-argument version that throws an
 * exception). */
void
SgAsmGenericSection::content(addr_t offset, addr_t size, void *buf)
{
    if (offset >= get_size()) {
        memset(buf, 0, size);
    } else if (offset+size > get_size()) {
        addr_t nbytes = get_size() - offset;
        memcpy(buf, &(p_data[offset]), nbytes);
        memset((char*)buf+nbytes, 0, size-nbytes);
        if (!p_congealed)
            p_referenced.insert(std::make_pair(offset, offset+nbytes));
    } else {
        memcpy(buf, &(p_data[offset]), size);
        if (!p_congealed)
            p_referenced.insert(std::make_pair(offset, offset+size));
    }
}

/* Returns ptr to a NUL-terminated string */
const char *
SgAsmGenericSection::content_str(addr_t offset)
{
    const char *ret = (const char*)&(p_data[offset]);
    size_t nchars=0;

#if 0 /*DEBUGGING*/
    printf ("SgAsmGenericSection::content_str(offset): p_data = %p offset = %zu p_size = %zu \n",p_data,offset,p_size);
#endif

    while (offset+nchars < get_size() && ret[nchars]) nchars++;
    nchars++; /*NUL*/

    if (offset+nchars > get_size())
        throw SgAsmGenericFile::ShortRead(this, offset, nchars);
    if (!p_congealed)
        p_referenced.insert(std::make_pair(offset, offset+nchars));

    return ret;
}

/* Like the low-level content(addr_t,addr_t) but returns an object rather than a ptr directly into the file content. This is
 * the recommended way to obtain file content for IR nodes that need to point to that content. The other function is more of a
 * low-level, efficient file read operation. */
const SgUnsignedCharList
SgAsmGenericSection::content_ucl(addr_t offset, addr_t size)
{
    const unsigned char *data = content(offset, size);
    SgUnsignedCharList returnValue;
    for (addr_t i=0; i<size; i++)
        returnValue.push_back(data[i]);
    return returnValue;
}

/* Write data back to a file section. The data to write may be larger than the file section as long as the extra (which will
 * not be written) is all zero. The offset is relative to the start of the section. */
rose_addr_t
SgAsmGenericSection::write(FILE *f, addr_t offset, size_t bufsize, const void *buf)
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
            hexdump(stderr, get_offset()+offset, "      ", (const unsigned char*)buf, bufsize);
            abort(); /*DEBUGGING*/
#endif
            throw SgAsmGenericFile::ShortWrite(this, offset, bufsize, mesg);
        }
    }

    return offset+bufsize;
}

/* See related method above */
rose_addr_t
SgAsmGenericSection::write(FILE *f, addr_t offset, const SgFileContentList &buf)
{
    if (0==buf.size())
        return 0;
    return write(f, offset, buf.size(), &(buf[0]));
}

/* See related method above */
rose_addr_t
SgAsmGenericSection::write(FILE *f, addr_t offset, const SgUnsignedCharList &buf)
{
    if (0==buf.size())
        return 0;
    return write(f, offset, buf.size(), (void*)&(buf[0]));
}

/* See related method above. */
rose_addr_t
SgAsmGenericSection::write(FILE *f, addr_t offset, const std::string &str)
{
    return write(f, offset, str.size(), str.c_str());
}

/* See related method above. */
rose_addr_t
SgAsmGenericSection::write(FILE *f, addr_t offset, char c)
{
    return write(f, offset, 1, &c);
}


/* Congeal the references to find the unreferenced areas. Once the references are congealed calling content(), content_ucl(),
 * content_str(), etc. will not affect references. This allows us to read the unreferenced areas without turning them into
 * referenced areas. */
const SgAsmGenericSection::ExtentVector &
SgAsmGenericSection::congeal()
{

    if (!p_congealed) {
        p_holes.clear();
        addr_t old_end = 0;
        for (RefMap::iterator it = p_referenced.begin(); it != p_referenced.end(); it++) {
            ExtentPair value = *it;
            ROSE_ASSERT(value.first <= value.second);
            if (value.first > old_end)
                p_holes.push_back(std::make_pair(old_end, value.first));
            if (value.second > old_end)
                old_end = value.second;
        }
        if (get_size() > old_end)
            p_holes.push_back(std::make_pair(old_end, get_size()));
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
        addr_t old_end = 0;
        for (ExtentVector::iterator it = p_holes.begin(); it != p_holes.end(); it++) {
            ExtentPair value = *it;
            ROSE_ASSERT(value.first >= old_end);
            ROSE_ASSERT(value.first <= value.second);
            if (value.first > old_end)
                p_referenced.insert(std::make_pair(old_end, value.first));
            if (value.second > old_end)
                old_end = value.second;
        }
        if (get_size() > old_end)
            p_referenced.insert(std::make_pair(old_end, get_size()));
        p_congealed = false;
        p_holes.clear();
    }
    return p_referenced;
}

/* Extend a section by some number of bytes. */
void
SgAsmGenericSection::extend(addr_t size)
{
    ROSE_ASSERT(get_file() != NULL);
    addr_t new_size = get_size() + size;
    if (p_offset + new_size > get_file()->get_size())
        throw SgAsmGenericFile::ShortRead(this, p_offset+get_size(), size);
    p_data.resize(new_size);
}

/* Like extend() but is more relaxed at the end of the file: if extending the section would cause it to go past the end of the
 * file then it is extended to the end of the file and no exception is thrown. */
void
SgAsmGenericSection::extend_up_to(addr_t size)
{
    ROSE_ASSERT(get_file() != NULL);
    addr_t new_size = get_size() + size;
    if (p_offset + new_size > get_file()->get_size()) {
        ROSE_ASSERT(p_offset <= get_file()->get_size());
        new_size = get_file()->get_size() - p_offset;
    }
    p_data.resize(new_size);
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

    write(f, 0, p_data);
}

/* Write just the specified regions back to the file */
void
SgAsmGenericSection::unparse(FILE *f, const ExtentVector &ev)
{
    for (size_t i = 0; i < ev.size(); i++) {
        ExtentPair p = ev[i];
        ROSE_ASSERT(p.first <= p.second);
        ROSE_ASSERT(p.second <= get_size());
        addr_t extent_offset = p.first;
        addr_t extent_size   = p.second - p.first;
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

/* Returns the file offset associated with the relative virtual address of a mapped section. */
rose_addr_t
SgAsmGenericSection::get_rva_offset(addr_t rva)
{
    return get_va_offset(rva + get_base_va());
}

/* Returns the file offset associated with the virtual address of a mapped section. */
rose_addr_t
SgAsmGenericSection::get_va_offset(addr_t va)
{
    ROSE_ASSERT(is_mapped());
    ROSE_ASSERT(va >= get_base_va());
    addr_t rva = va - get_base_va();
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
    fprintf(f, "%s%-*s = %" PRId64 " bytes\n",           p, w, "size",        get_size());
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
        addr_t hole_size = extent.second - extent.first;
        fprintf(f, "%s%-*s = [%zu] at %"PRIu64", %"PRIu64" bytes\n", p, w, "hole", i, extent.first, hole_size);
    }
    if (!was_congealed)
        uncongeal();

 // DQ (8/31/2008): Output the contents if this not derived from (there is likely a 
 // better implementation if the hexdump function was a virtual member function).
    if (variantT() == V_SgAsmGenericSection)
       {
         fprintf (f, "%sSaved raw data (size = %zu) \n",prefix,p_data.size());
         hexdump(f, get_offset(), "    ", p_data);
       }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExecHeader
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
SgAsmGenericHeader::ctor(SgAsmGenericFile *ef, addr_t offset, addr_t size)
{
    set_synthesized(true);
    set_purpose(SP_HEADER);
    ef->add_header(this);


 // DQ (8/16/2008): This is defined only for SgAsmGenericHeader, and not for SgAsmGenericSection
    set_file(ef);

    /* Initialized in the real constructor */
    ROSE_ASSERT(p_symbols     == NULL);
    ROSE_ASSERT(p_dlls        == NULL);
    ROSE_ASSERT(p_exec_format == NULL);

    /* Create child IR nodes and set their parent */
    p_symbols = new SgAsmGenericSymbolList;
    p_symbols->set_parent(this);

    p_dlls    = new SgAsmGenericDLLList;
    p_dlls->set_parent(this);

    p_exec_format = new SgAsmGenericFormat;
    p_exec_format->set_parent(this);

    p_sections = new SgAsmGenericSectionList;
    p_sections->set_parent(this);
}

/* Destructor must remove the header from its parent file's headers list. */
SgAsmGenericHeader::~SgAsmGenericHeader() 
{
    /* Delete child sections before this */
    while (p_sections->get_sections().size()) {
        SgAsmGenericSection *section = p_sections->get_sections().back();
        p_sections->get_sections().pop_back();
        delete section;
    }
    ROSE_ASSERT(p_sections->get_sections().empty() == true);

    if (get_file() != NULL)
        get_file()->remove_header(this);

    delete p_symbols;
    delete p_dlls;
    delete p_exec_format;
    delete p_sections;

    p_symbols = NULL;
    p_dlls = NULL;
    p_exec_format = NULL;
    p_sections = NULL;

}

/* Adds a new section to the header. This is called implicitly by the section constructor. */
void
SgAsmGenericHeader::add_section(SgAsmGenericSection *section)
{
    ROSE_ASSERT(section != NULL);

    ROSE_ASSERT(p_sections != NULL);

#ifndef NDEBUG
    /* New section must not already be present. */
    for (size_t i = 0; i < p_sections->get_sections().size(); i++) {
        ROSE_ASSERT(p_sections->get_sections()[i] != section);
    }
#endif
    section->set_parent(p_sections);
    p_sections->get_sections().push_back(section);
}

/* Removes a secton from the header's section list. */
void
SgAsmGenericHeader:remove_section(SgAsmGenericSection *section)
{
    if (section!=NULL) {
        ROSE_ASSERT(p_sections != NULL);
        std::vector<SgAsmGenericSection*>::iterator i = find(p_sections->get_sections().begin(),
                                                             p_sections->get_sections().end(),
                                                             section);
        if (i != p_sections->get_sections().end()) {
            p_sections->get_sections().erase(i);
        }
    }
}

/* Add a new DLL to the header DLL list */
void
SgAsmGenericHeader::add_dll(SgAsmGenericDLL *dll)
{
    ROSE_ASSERT(p_dlls != NULL);

#ifndef NDEBUG
 // for (size_t i = 0; i < p_dlls.size(); i++) {
    for (size_t i = 0; i < p_dlls->get_dlls().size(); i++) {
        ROSE_ASSERT(p_dlls->get_dlls()[i] != dll); /*duplicate*/
    }
#endif
    p_dlls->get_dlls().push_back(dll);

    dll->set_parent(p_dlls);
}

/* Add a new symbol to the symbol table. The SgAsmGenericHeader has a list of symbol pointers. These pointers point to symbols
 * that are defined in various sections throughout the executable. It's not absolutely necessary to store them here since the
 * sections where they're defined also point to them--they're here only for convenience.
 * 
 * FIXME: If symbols are stored in one central location we should probably use something other than an
 *        unsorted list. (RPM 2008-08-19) */
void
SgAsmGenericHeader::add_symbol(SgAsmGenericSymbol *symbol)
{
#if 0 /*turned off because too slow!!! (RPM 2008-08-19)*/
#ifndef NDEBUG
    for (size_t i = 0; i < p_symbols->get_symbols().size(); i++) {
        ROSE_ASSERT(p_symbols->get_symbols()[i] != symbol); /*duplicate*/
    }
#endif
#endif
    p_symbols->get_symbols().push_back(symbol);

    /* FIXME: symbols have two parents: the header's p_symbols list and the list in the section where the symbol was defined.
     *        We probably want to keep them only with the section that defines them. For example, SgAsmElfSymbolSection.
     *        (RPM 2008-08-19) */
    p_symbols->get_symbols().back()->set_parent(p_symbols);
}

/* Returns sections in this header that have the specified ID. */
SgAsmGenericFile::sections_t
SgAsmGenericHeader::get_sections_by_id(int id)
{
    SgAsmGenericFile::sections_t retval;
    for (sections_t::iterator i=p_sections->get_sections().begin(); i!=p_sections->get_sections().end(); ++i) {
        if ((*i)->get_id() == id) {
            retval.push_back(*i);
        }
    }
    return retval;
}

/* Returns single section in this header that has the specified ID. */
SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_id(int id, size_t *nfound=0)
{
    sections_t possible = get_sections_by_id(int it);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/* Returns sections in this header that have the specified name. If 'SEP' is a non-null string then ignore any part of name at
 * and after SEP. */
sections_t
SgAsmGenericHeader::get_sections_by_name(std::string name, char *sep=0)
{
    if (sep) {
        size_t pos = name.find(sep);
        if (pos!=name.npos)
            name.erase(pos);
    }

    sections_t retval;
    for (sections_t::iterator i = p_sections->get_sections().begin(); i != p_sections->get_sections().end(); ++i) {
        if (0==(*i)->get_name().compare(name))
            retval.push_back(*i);
    }
    return retval;
}

/* Returns single section in this header that has the specified name. */
SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_name(const std::string &name, char *sep=0, size_t *nfound=0)
{
    sections_t possible = get_sections_by_name(name, sep);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/* Returns sectons in this header that contain all of the specified portion of the file. */
sections_t
SgAsmGenericHeader::get_sections_by_offset(addr_t offset, addr_t size)
{
    sections_t retval;
    for (sections_t::iterator i = p_sections->get_sections().begin(); i != p_sections->get_sections().end(); ++i) {
        SgAsmGenericSection *section = *i;
        if (offset >= section->get_offset() &&
            offset < section->get_offset()+section->get_size() &&
            offset-section->get_offset() + size <= section->get_size())
            retval.push_back(section);
    }
    return retval;
}

/* Returns single section in this header that contains all of the specified portion of the file. */
SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_offset(addr_t offset, addr_t size, size_t *nfound=0)
{
    sections_t possible = get_sections_by_offset(offset, size);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/* Returns sections in this header that are mapped to include the specified relative virtual address. */
sections_t
SgAsmGenericHeader::get_sections_by_rva(addr_t rva)
{
    sections_t retval;
    for (sections_t::iterator i = p_sections->get_sections().begin(); i!=p_sections->get_sections().end(); ++i) {
        SgAsmGenericSection *section = *i;
        if (section->is_mapped() &&
            rva >= section->get_mapped_rva() && rva < section->get_mapped_rva() + section->get_mapped_size()) {
            retval.push_back(section);
        }
    }
    return retval;
}

/* Returns single section in this header that is mapped to include the specified relative virtual address. */
SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_rva(addr_t rva, size_t *nfound=0)
{
    sections_t possible = get_section_by_rva(rva);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/* Returns sections in this header that are mapped to include the specified virtual address */
sections_t
SgAsmGenericHeader::get_sections_by_va(addr_t va)
{
    if (va < get_base_va())
        return sections_t();
    addr_t rva = va - get_base_va();
    return get_sections_by_rva(rva);
}

/* Returns single section in this header that is mapped to include the specified virtual address. */
SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_va_FIXME(addr_t va, size_t *nfound=0)
{
    sections_t possible = get_section_by_va(va);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/* Like SgAsmGenericFile::get_best_section_by_va() except considers only sections defined in this header. */
SgAsmGenericSection *
SgAsmGenericHeader::get_best_section_by_va(addr_t va, size_t *nfound)
{
    const sections_t &candidates = get_sections_by_va(va);
    if (nfound)
        *nfound = candiates.size();
    return SgAsmGenericFile::best_section_by_va(candiates);
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
SgAsmExecutableFileFormat::hexdump(FILE *f, addr_t base_addr, const char *prefix, const unsigned char *data, size_t n)
{
 // The "prefix" can be used for whitespace to intent the output.

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
// void SgAsmExecutableFileFormat::hexdump(FILE *f, addr_t base_addr, const std::string &prefix, const SgCharList &data)
void
SgAsmExecutableFileFormat::hexdump(FILE *f, addr_t base_addr, const std::string &prefix, const SgUnsignedCharList &data)
   {
     if (data.empty() == false)
          hexdump(f, base_addr, prefix.c_str(), &(data[0]), data.size());
   }

// DQ (8/31/2008): This is the newest interface function (could not remove the one based on SgUnsignedCharList since it
// is used in the symbol support).
void
SgAsmExecutableFileFormat::hexdump(FILE *f, addr_t base_addr, const std::string &prefix, const SgFileContentList &data)
   {
     if (data.empty() == false)
          hexdump(f, base_addr, prefix.c_str(), &(data[0]), data.size());
   }

/* Writes a new file from the IR node for a parse executable file.  This is primarily to debug the parser by creating
 * an executable that *should* be identical to the original. */
void
SgAsmExecutableFileFormat::unparseBinaryFormat(const std::string &name, SgAsmFile *asmFile)
{
    FILE *output = fopen(name.c_str(), "wb");
    ROSE_ASSERT(output!=NULL);
    ROSE_ASSERT(asmFile!=NULL);
    ROSE_ASSERT(asmFile->get_genericFile() != NULL);
    asmFile->get_genericFile()->unparse(name);
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

  // printf ("Evaluate what kind of binary format this file is! \n");

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
                             ef = NULL;

                          // DQ (8/20/2008): The code (from Robb) identifies what kind of file this is or 
                          // more specifically what kind of file most tools would think this 
                          // file is (using the system file(1) command as a standard way to identify
                          // file types using their first few bytes.

                          // DQ (8/21/2008): It should be an error to get this far.  Robb's code (below) was copied to sageSupport.C where it is used to detect incorrect file types.
                             printf ("Error: In SgAsmExecutableFileFormat::parseBinaryFormat(%s) evaluation of file type should have been done previously \n",name.c_str());
                             ROSE_ASSERT(false);

#if 0
                          // Use file(1) to try to figure out the file type to report in the exception
                             int child_stdout[2];
                             pipe(child_stdout);
                             pid_t pid = fork();
                             if (0 == pid)
                                {
                                  close(0);
                                  dup2(child_stdout[1], 1);
                                  close(child_stdout[0]);
                                  close(child_stdout[1]);
                                  execlp("file", "file", "-b", name.c_str(), NULL);
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
                                       throw SgAsmGenericFile::FormatError(mesg);
                                     }
                                    else
                                     {
                                       throw SgAsmGenericFile::FormatError("unrecognized file format");
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

#if 0
  // asmFile->set_header(executableHeader);
     ROSE_ASSERT(asmFile->get_headers() != NULL);
     asmFile->get_headers()->get_headers().push_back(executableHeader);

     executableHeader->set_parent(asmFile->get_headers());

  // ROSE_ASSERT(asmFile->get_header() != NULL);
     ROSE_ASSERT(asmFile->get_headers()->get_headers().empty() == false);
#else
  // asmFile->get_genericFile()->get_headers()->get_headers().push_back(executableHeader);
  // executableHeader->set_parent(asmFile->get_genericFile()->get_headers());

     asmFile->set_genericFile(executableHeader->get_file());
     executableHeader->get_file()->set_parent(asmFile);
#endif
}

#if 1
// DQ (8/21/2008): Turn this back on since the disassembler uses it!

// DQ (6/15/2008): Old function name (confirmed to not be called in ROSE)
/* Top-level binary executable file parser. Given the name of a file, open the file, detect the format, parse the file,
 * and return information about the file. */
SgAsmGenericFile *
SgAsmExecutableFileFormat::parse(const char *name)
{
    SgAsmGenericFile *ef = new SgAsmGenericFile(name);
    
    if (SgAsmElfFileHeader::is_ELF(ef)) {
        SgAsmElfFileHeader::parse(ef);
    } else if (SgAsmPEFileHeader::is_PE(ef)) {
        SgAsmPEFileHeader::parse(ef);
    } else if (SgAsmNEFileHeader::is_NE(ef)) {
        SgAsmNEFileHeader::parse(ef);
    } else if (SgAsmLEFileHeader::is_LE(ef)) { /*or LX*/
        SgAsmLEFileHeader::parse(ef);
    } else if (SgAsmDOSFileHeader::is_DOS(ef)) {
        /* Must be after PE and NE all PE and NE files are also DOS files */
        SgAsmDOSFileHeader::parse(ef);
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
