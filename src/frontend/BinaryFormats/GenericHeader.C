/* Generic File Headers (SgAsmGenericHeader and related classes).  Describes a binary file header. Most binary files have at
 * least one header that serves as sort of a superblock for describing the rest of the file.  Many formats have more than one
 * header (e.g., Microsoft Windows files have both a DOS header and a PE header). */

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>


/** Constructor.
 *  Headers (SgAsmGenericHeader and derived classes) set the file/header relationship--a bidirectional link between this new
 *  header and the single file that contains this new header. This new header points to its file and the file contains a list
 *  that points to this new header. The header-to-file half of the link is deleted by the default destructor by virtue of being
 *  a simple pointer, but we also need to delete the other half of the link in the destructors. */
void
SgAsmGenericHeader::ctor()
{
    set_synthesized(true);
    set_purpose(SP_HEADER);

    /* The bidirectional link between file and header */
    get_file()->add_header(this);

    /* Create child IR nodes and set their parent (initialized to null in real constructor) */
    ROSE_ASSERT(p_symbols == NULL);
    p_symbols = new SgAsmGenericSymbolList;
    p_symbols->set_parent(this);

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

/* Destructor must remove header/file link. */
SgAsmGenericHeader::~SgAsmGenericHeader() 
{
    /* Delete child sections before this */
    SgAsmGenericSectionPtrList to_delete = get_sections()->get_sections();
    for (size_t i=0; i<to_delete.size(); i++) {
        SgAsmGenericSection *section = to_delete[i];
        delete section;
    }

    /* Deletion of section children should have emptied the list of header-to-section links */
    ROSE_ASSERT(p_sections->get_sections().empty() == true);

    /* Destroy the header/file bidirectional link. See comment in constructor. */
    ROSE_ASSERT(get_file()!=NULL);
    get_file()->remove_header(this);
    //set_file(NULL);   -- the file pointer was moved into the superclass in order to be easily available to all sections

    /* Delete children */
    delete p_symbols;     p_symbols     = NULL;
    delete p_dlls;        p_dlls        = NULL;
    delete p_exec_format; p_exec_format = NULL;
    delete p_sections;    p_sections    = NULL;
}

/** Allow all sections to reallocate themselves */
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
    
/** Unparse headers and all they point to */
void
SgAsmGenericHeader::unparse(std::ostream &f) const
{
    SgAsmGenericSection::unparse(f);

    /* Unparse each section */
    for (SgAsmGenericSectionPtrList::iterator i=p_sections->get_sections().begin(); i!=p_sections->get_sections().end(); ++i)
        (*i)->unparse(f);
}

/** Returns the RVA (relative to the header's base virtual address) of the first entry point. If there are no entry points
 *  defined then return a zero RVA. */
rose_addr_t
SgAsmGenericHeader::get_entry_rva() const
{
    if (p_entry_rvas.size()==0)
        return rose_addr_t();
    return p_entry_rvas[0].get_rva();
}

/** Adds a new section to the header. This is called implicitly by the section constructor. */
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

/** Removes a secton from the header's section list. */
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

/** Add a new DLL to the header DLL list */
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

/** Add a new symbol to the symbol table. The SgAsmGenericHeader has a list of symbol pointers. These pointers point to symbols
 *  that are defined in various sections throughout the executable. It's not absolutely necessary to store them here since the
 *  sections where they're defined also point to them--they're here only for convenience.
 * 
 *  FIXME: If symbols are stored in one central location we should probably use something other than an
 *         unsorted list. (RPM 2008-08-19) */
void
SgAsmGenericHeader::add_symbol(SgAsmGenericSymbol *symbol)
{
    ROSE_ASSERT(p_symbols);
    p_symbols->set_isModified(true);

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

/** Returns the list of sections that are memory mapped */
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
    
/** Returns sections in this header that have the specified ID. */
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

/** Returns single section in this header that has the specified ID. */
SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_id(int id, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_id(id);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/** Returns sections in this header that have the specified name. If 'SEP' is a non-null string then ignore any part of name at
 *  and after SEP. */
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

/** Returns single section in this header that has the specified name. */
SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_name(const std::string &name, char sep/*or NUL*/, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_name(name, sep);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/** Returns sectons in this header that contain all of the specified portion of the file. */
SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_sections_by_offset(addr_t offset, addr_t size) const
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

/** Returns single section in this header that contains all of the specified portion of the file. */
SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_offset(addr_t offset, addr_t size, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_offset(offset, size);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/** Returns sections that have a preferred mapping that includes the specified relative virtual address. */
SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_sections_by_rva(addr_t rva) const
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

/** Returns the single section having a preferred mapping that includes the specified relative virtual address. If there are
 *  no sections or multiple sections satisfying this condition then a null pointer is returned. */
SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_rva(addr_t rva, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_rva(rva);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/** Returns sections having a preferred or actual mapping that includes the specified virtual address.  If @p use_preferred is
 *  set, then the condition is evaluated by looking at the section's preferred mapping, otherwise the actual mapping is used.
 *  If an actual mapping is used, the specified virtual address must be part of the actual mapped section, not merely in the
 *  memory region that was also mapped to satisfy alignment constraints. */
SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_sections_by_va(addr_t va, bool use_preferred) const
{
    if (use_preferred) {
        if (va < get_base_va())
            return SgAsmGenericSectionPtrList();
        addr_t rva = va - get_base_va();
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

/** Returns the section having a preferred or actual mapping that includes the specified virtual address. If @p use_preferred
 *  is set, then the condition is evaluated by looking at the section's preferred mapping, otherwise the actual mapping is
 *  used. If an actual mapping is used, the specified virtual address must be part of the actual mapped section, not merely in
 *  the memory region that was also mapped to satisfy alignment constraints.  If there are no sections or multiple sections
 *  satisfying this condition then a null pointer is returned. */
SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_va(addr_t va, bool use_preferred, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_va(va, use_preferred);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/** Like SgAsmGenericFile::get_best_section_by_va() except considers only sections defined in this header. */
SgAsmGenericSection *
SgAsmGenericHeader::get_best_section_by_va(addr_t va, bool use_preferred, size_t *nfound) const
{
    const SgAsmGenericSectionPtrList &candidates = get_sections_by_va(va, use_preferred);
    if (nfound) *nfound = candidates.size();
    return SgAsmGenericFile::best_section_by_va(candidates, va);
}

/* Print some debugging info */
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

    fprintf(f, "%s%-*s = 0x%x\n", p, w, "ins_arch", p_isa);

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
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64")\n", p, w, "base_va", get_base_va(), get_base_va());
    fprintf(f, "%s%-*s = %zu entry points\n", p, w, "entry_rva.size", p_entry_rvas.size());
    for (size_t i = 0; i < p_entry_rvas.size(); i++) {
        char label[64];
        sprintf(label, "entry_rva[%zu]", i);
        addr_t entry_rva = p_entry_rvas[i].get_rva();
        fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64")\n", p, w, label, entry_rva, entry_rva);
        SgAsmGenericSectionPtrList sections = get_file()->get_sections();
        dump_containing_sections(f, std::string(p)+label, entry_rva, sections);
    }

    fprintf(f, "%s%-*s = %zu sections\n", p, w, "section", p_sections->get_sections().size());
    for (size_t i=0; i<p_sections->get_sections().size(); i++) {
        SgAsmGenericSection *section = p_sections->get_sections()[i];
        char label[1024];
        sprintf(label, "section[%zu]", i);
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, label, section->get_id(), section->get_name()->c_str());
    }
    
    fprintf(f, "%s%-*s = %zu entries\n", p, w, "DLL.size", p_dlls->get_dlls().size());
    for (size_t i = 0; i < p_dlls->get_dlls().size(); i++)
        p_dlls->get_dlls()[i]->dump(f, p, i);

    fprintf(f, "%s%-*s = %zu entries\n", p, w, "Symbol.size", p_symbols->get_symbols().size());
    for (size_t i = 0; i < p_symbols->get_symbols().size(); i++)
        p_symbols->get_symbols()[i]->dump(f, p, i);
}

