/* Generic File Headers (SgAsmGenericHeader and related classes).  Describes a binary file header. Most binary files have at
 * least one header that serves as sort of a superblock for describing the rest of the file.  Many formats have more than one
 * header (e.g., Microsoft Windows files have both a DOS header and a PE header). */
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include "stringify.h"

using namespace Rose;

const char *
SgAsmGenericHeader::format_name() const
   {
     return "ASM_GENERIC_HEADER";
   }

ByteOrder::Endianness
SgAsmGenericHeader::get_sex() const
   {
     ROSE_ASSERT(p_exec_format != NULL);
     return p_exec_format->get_sex();
   }

size_t
SgAsmGenericHeader::get_word_size() const
   {
     ROSE_ASSERT(p_exec_format != NULL);
     return p_exec_format->get_word_size();
   }


void
SgAsmGenericHeader::ctor()
{
    set_synthesized(true);
    set_purpose(SP_HEADER);

    /* The bidirectional link between file and header */
    get_file()->add_header(this);

    /* Create child IR nodes and set their parent (initialized to null in real constructor) */
    ROSE_ASSERT(p_dlls == NULL);
    p_dlls    = new SgAsmGenericDLLList;
    p_dlls->set_parent(this);

    ROSE_ASSERT(p_exec_format == NULL);
    p_exec_format = new SgAsmGenericFormat;
    p_exec_format->set_parent(this);

    ROSE_ASSERT(p_sections == NULL);
    p_sections = new SgAsmGenericSectionList;
    p_sections->set_parent(this);
}

/* Destructor must remove header/file link. Children in the AST have already been deleted when called from
 * SageInterface::deleteAST() */
SgAsmGenericHeader::~SgAsmGenericHeader() 
{
    /* Deletion of section children should have emptied the list of header-to-section links */
    ROSE_ASSERT(p_sections->get_sections().empty() == true);

    /* Destroy the header/file bidirectional link. See comment in constructor. */
    ROSE_ASSERT(get_file()!=NULL);
    get_file()->remove_header(this);
}

bool
SgAsmGenericHeader::reallocate()
{
    bool reallocated = false;
    for (SgAsmGenericSectionPtrList::iterator i=p_sections->get_sections().begin(); i!=p_sections->get_sections().end(); ++i) {
        if ((*i)->reallocate())
            reallocated = true;
    }
    return reallocated;
}
    
void
SgAsmGenericHeader::unparse(std::ostream &f) const
{
    SgAsmGenericSection::unparse(f);

    /* Unparse each section */
    for (SgAsmGenericSectionPtrList::iterator i=p_sections->get_sections().begin(); i!=p_sections->get_sections().end(); ++i)
        (*i)->unparse(f);
}

rose_addr_t
SgAsmGenericHeader::get_entry_rva() const
{
    if (p_entry_rvas.size()==0)
        return rose_addr_t();
    return p_entry_rvas[0].get_rva();
}

void
SgAsmGenericHeader::add_section(SgAsmGenericSection *section)
{
    ROSE_ASSERT(section != NULL);
    ROSE_ASSERT(p_sections != NULL);
    p_sections->set_isModified(true);

#ifndef NDEBUG
    /* New section must not already be present. */
    for (size_t i = 0; i < p_sections->get_sections().size(); i++) {
        ROSE_ASSERT(p_sections->get_sections()[i] != section);
    }
#endif
    section->set_header(this);
    section->set_parent(p_sections);
    p_sections->get_sections().push_back(section);
}

void
SgAsmGenericHeader::remove_section(SgAsmGenericSection *section)
{
    if (section!=NULL) {
        ROSE_ASSERT(p_sections != NULL);
        SgAsmGenericSectionPtrList::iterator i = find(p_sections->get_sections().begin(),
                                                        p_sections->get_sections().end(),
                                                        section);
        if (i != p_sections->get_sections().end()) {
            p_sections->get_sections().erase(i);
            p_sections->set_isModified(true);
        }
    }
}

void
SgAsmGenericHeader::add_dll(SgAsmGenericDLL *dll)
{
    ROSE_ASSERT(p_dlls != NULL);
    p_dlls->set_isModified(true);

#ifndef NDEBUG
 // for (size_t i = 0; i < p_dlls.size(); i++) {
    for (size_t i = 0; i < p_dlls->get_dlls().size(); i++) {
        ROSE_ASSERT(p_dlls->get_dlls()[i] != dll); /*duplicate*/
    }
#endif
    p_dlls->get_dlls().push_back(dll);

    dll->set_parent(p_dlls);
}

SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_mapped_sections() const
{
    SgAsmGenericSectionPtrList retval;
    for (SgAsmGenericSectionPtrList::iterator i=p_sections->get_sections().begin(); i!=p_sections->get_sections().end(); ++i) {
        if ((*i)->is_mapped()) {
            retval.push_back(*i);
        }
    }
    return retval;
}
    
SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_sections_by_id(int id) const
{
    SgAsmGenericSectionPtrList retval;
    for (SgAsmGenericSectionPtrList::iterator i=p_sections->get_sections().begin(); i!=p_sections->get_sections().end(); ++i) {
        if ((*i)->get_id() == id) {
            retval.push_back(*i);
        }
    }
    return retval;
}

SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_id(int id, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_id(id);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_sections_by_name(std::string name, char sep/*or NUL*/) const
{
    if (sep) {
        size_t pos = name.find(sep);
        if (pos!=name.npos)
            name.erase(pos);
    }

    SgAsmGenericSectionPtrList retval;
    for (SgAsmGenericSectionPtrList::iterator i=p_sections->get_sections().begin(); i!=p_sections->get_sections().end(); ++i) {
        std::string secname = (*i)->get_name()->get_string();
        if (sep) {
            size_t pos = secname.find(sep);
            if (pos!=secname.npos)
                secname.erase(pos);
        }
        if (0==secname.compare(name))
            retval.push_back(*i);
    }
    return retval;
}

SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_name(const std::string &name, char sep/*or NUL*/, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_name(name, sep);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_sections_by_offset(rose_addr_t offset, rose_addr_t size) const
{
    SgAsmGenericSectionPtrList retval;
    for (SgAsmGenericSectionPtrList::iterator i=p_sections->get_sections().begin(); i!=p_sections->get_sections().end(); ++i) {
        SgAsmGenericSection *section = *i;
        if (offset >= section->get_offset() &&
            offset < section->get_offset()+section->get_size() &&
            offset-section->get_offset() + size <= section->get_size())
            retval.push_back(section);
    }
    return retval;
}

SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_offset(rose_addr_t offset, rose_addr_t size, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_offset(offset, size);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_sections_by_rva(rose_addr_t rva) const
{
    SgAsmGenericSectionPtrList retval;
    for (SgAsmGenericSectionPtrList::iterator i = p_sections->get_sections().begin(); i!=p_sections->get_sections().end(); ++i) {
        SgAsmGenericSection *section = *i;
        if (section->is_mapped() &&
            rva >= section->get_mapped_preferred_rva() && rva < section->get_mapped_preferred_rva() + section->get_mapped_size()) {
            retval.push_back(section);
        }
    }
    return retval;
}

SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_rva(rose_addr_t rva, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_rva(rva);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_sections_by_va(rose_addr_t va, bool use_preferred) const
{
    if (use_preferred) {
        if (va < get_base_va())
            return SgAsmGenericSectionPtrList();
        rose_addr_t rva = va - get_base_va();
        return get_sections_by_rva(rva);
    }
     
    SgAsmGenericSectionPtrList retval;
    for (size_t i=0; i<p_sections->get_sections().size(); i++) {
        SgAsmGenericSection *section = p_sections->get_sections()[i];
        if (section->is_mapped() &&
            va>=section->get_mapped_actual_va() && va<section->get_mapped_actual_va()+section->get_mapped_size())
            retval.push_back(section);
    }
    return retval;
}

SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_va(rose_addr_t va, bool use_preferred, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_va(va, use_preferred);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

SgAsmGenericSection *
SgAsmGenericHeader::get_best_section_by_va(rose_addr_t va, bool use_preferred, size_t *nfound) const
{
    const SgAsmGenericSectionPtrList &candidates = get_sections_by_va(va, use_preferred);
    if (nfound) *nfound = candidates.size();
    return SgAsmGenericFile::best_section_by_va(candidates, va);
}

void
SgAsmGenericHeader::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sHeader[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sHeader.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    SgAsmGenericSection::dump(f, p, -1);

    ROSE_ASSERT(p_exec_format != NULL);
    p_exec_format->dump(f, p, -1);

    fprintf(f, "%s%-*s = 0x%x (%s)\n", p, w, "ins_arch",
            p_isa, stringifySgAsmExecutableFileFormatInsSetArchitecture(p_isa).c_str());

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
    fprintf(f, "%s%-*s = 0x%08" PRIx64 " (%" PRIu64 ")\n", p, w, "base_va", get_base_va(), get_base_va());
    fprintf(f, "%s%-*s = %" PRIuPTR " entry points\n", p, w, "entry_rva.size", p_entry_rvas.size());
    for (size_t i = 0; i < p_entry_rvas.size(); i++) {
        char label[64];
        sprintf(label, "entry_rva[%" PRIuPTR "]", i);
        rose_addr_t entry_rva = p_entry_rvas[i].get_rva();
        fprintf(f, "%s%-*s = 0x%08" PRIx64 " (%" PRIu64 ")\n", p, w, label, entry_rva, entry_rva);
        SgAsmGenericSectionPtrList sections = get_file()->get_sections();
        dump_containing_sections(f, std::string(p)+label, entry_rva, sections);
    }

    fprintf(f, "%s%-*s = %" PRIuPTR " sections\n", p, w, "section", p_sections->get_sections().size());
    for (size_t i=0; i<p_sections->get_sections().size(); i++) {
        SgAsmGenericSection *section = p_sections->get_sections()[i];
        char label[1024];
        sprintf(label, "section[%" PRIuPTR "]", i);
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, label, section->get_id(), section->get_name()->get_string(true).c_str());
    }
    
    fprintf(f, "%s%-*s = %" PRIuPTR " entries\n", p, w, "DLL.size", p_dlls->get_dlls().size());
    for (size_t i = 0; i < p_dlls->get_dlls().size(); i++)
        p_dlls->get_dlls()[i]->dump(f, p, i);
}

#endif
