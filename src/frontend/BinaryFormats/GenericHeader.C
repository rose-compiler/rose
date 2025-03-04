/* Generic File Headers (SgAsmGenericHeader and related classes).  Describes a binary file header. Most binary files have at
 * least one header that serves as sort of a superblock for describing the rest of the file.  Many formats have more than one
 * header (e.g., Microsoft Windows files have both a DOS header and a PE header). */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include "stringify.h"
#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>

using namespace Rose;

const char *
SgAsmGenericHeader::format_name() const {
    return formatName();
}

const char *
SgAsmGenericHeader::formatName() const
   {
     return "ASM_GENERIC_HEADER";
   }

Rose::BinaryAnalysis::ByteOrder::Endianness
SgAsmGenericHeader::get_sex() const
   {
     ROSE_ASSERT(get_executableFormat() != NULL);
     return get_executableFormat()->get_sex();
   }

size_t
SgAsmGenericHeader::get_word_size() const {
    return get_wordSize();
}

size_t
SgAsmGenericHeader::get_wordSize() const
   {
     ROSE_ASSERT(get_executableFormat() != NULL);
     return get_executableFormat()->get_wordSize();
   }


SgAsmGenericHeader::SgAsmGenericHeader(SgAsmGenericFile *ef)
    : SgAsmGenericSection(ef, NULL) {
    initializeProperties();

    set_synthesized(true);
    set_purpose(SP_HEADER);

    /* The bidirectional link between file and header */
    ASSERT_not_null(get_file());
    get_file()->addHeader(this);
}

bool
SgAsmGenericHeader::reallocate()
{
    bool reallocated = false;
    for (auto section: p_sections->get_sections()) {
        if (section->reallocate())
            reallocated = true;
    }
    return reallocated;
}
    
void
SgAsmGenericHeader::unparse(std::ostream &f) const
{
    SgAsmGenericSection::unparse(f);

    /* Unparse each section */
    for (auto section: p_sections->get_sections()) {
        section->unparse(f);
    }
}

Rose::BinaryAnalysis::Address
SgAsmGenericHeader::get_entry_rva() const
{
    return get_entryRva();
}

Rose::BinaryAnalysis::Address
SgAsmGenericHeader::get_entryRva() const
{
    if (get_entryRvas().size()==0)
        return Rose::BinaryAnalysis::Address();
    return get_entryRvas()[0].rva();
}

void
SgAsmGenericHeader::add_section(SgAsmGenericSection *section)
{
    addSection(section);
}

void
SgAsmGenericHeader::addSection(SgAsmGenericSection *section)
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
    removeSection(section);
}

void
SgAsmGenericHeader::removeSection(SgAsmGenericSection *section)
{
    if (section && p_sections) {
        SgAsmGenericSectionPtrList::iterator i = find(p_sections->get_sections().begin(),
                                                      p_sections->get_sections().end(),
                                                      section);
        if (i != p_sections->get_sections().end()) {
            if (*i)
                (*i)->set_parent(nullptr);
            p_sections->get_sections().erase(i);
            p_sections->set_isModified(true);
        }
    }
}

void
SgAsmGenericHeader::add_dll(SgAsmGenericDLL *dll)
{
    addDll(dll);
}

void
SgAsmGenericHeader::addDll(SgAsmGenericDLL *dll)
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
    return get_mappedSections();
}

SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_mappedSections() const
{
    SgAsmGenericSectionPtrList retval;
    for (auto section: p_sections->get_sections()) {
        if (section->isMapped()) {
            retval.push_back(section);
        }
    }
    return retval;
}
    
SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_sections_by_id(int id) const
{
    return get_sectionsById(id);
}

SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_sectionsById(int id) const
{
    SgAsmGenericSectionPtrList retval;
    for (auto section: p_sections->get_sections()) {
        if (section->get_id() == id) {
            retval.push_back(section);
        }
    }
    return retval;
}

SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_id(int id, size_t *nfound/*optional*/) const
{
    return get_sectionById(id, nfound);
}

SgAsmGenericSection *
SgAsmGenericHeader::get_sectionById(int id, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sectionsById(id);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_sections_by_name(std::string name, char sep/*or NUL*/) const
{
    return get_sectionsByName(name, sep);
}

SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_sectionsByName(std::string name, char sep/*or NUL*/) const
{
    if (sep) {
        size_t pos = name.find(sep);
        if (pos!=name.npos)
            name.erase(pos);
    }

    SgAsmGenericSectionPtrList retval;
    for (auto section: p_sections->get_sections()) {
        std::string secname = section->get_name()->get_string();
        if (sep) {
            size_t pos = secname.find(sep);
            if (pos!=secname.npos)
                secname.erase(pos);
        }
        if (0==secname.compare(name))
            retval.push_back(section);
    }
    return retval;
}

SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_name(const std::string &name, char sep/*or NUL*/, size_t *nfound/*optional*/) const
{
    return get_sectionByName(name, sep, nfound);
}

SgAsmGenericSection *
SgAsmGenericHeader::get_sectionByName(const std::string &name, char sep/*or NUL*/, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sectionsByName(name, sep);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_sections_by_offset(Rose::BinaryAnalysis::Address offset, Rose::BinaryAnalysis::Address size) const
{
    return get_sectionsByOffset(offset, size);
}

SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_sectionsByOffset(Rose::BinaryAnalysis::Address offset, Rose::BinaryAnalysis::Address size) const
{
    SgAsmGenericSectionPtrList retval;
    for (auto section: p_sections->get_sections()) {
        if (offset >= section->get_offset() &&
            offset < section->get_offset()+section->get_size() &&
            offset-section->get_offset() + size <= section->get_size())
            retval.push_back(section);
    }
    return retval;
}

SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_offset(Rose::BinaryAnalysis::Address offset, Rose::BinaryAnalysis::Address size,
                                          size_t *nfound/*optional*/) const
{
    return get_sectionByOffset(offset, size, nfound);
}

SgAsmGenericSection *
SgAsmGenericHeader::get_sectionByOffset(Rose::BinaryAnalysis::Address offset, Rose::BinaryAnalysis::Address size,
                                        size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sectionsByOffset(offset, size);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_sections_by_rva(Rose::BinaryAnalysis::Address rva) const
{
    return get_sectionsByRva(rva);
}

SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_sectionsByRva(Rose::BinaryAnalysis::Address rva) const
{
    SgAsmGenericSectionPtrList retval;
    for (auto section: p_sections->get_sections()) {
        if (section->isMapped() &&
            rva >= section->get_mappedPreferredRva() && rva < section->get_mappedPreferredRva() + section->get_mappedSize()) {
            retval.push_back(section);
        }
    }
    return retval;
}

SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_rva(Rose::BinaryAnalysis::Address rva, size_t *nfound/*optional*/) const
{
    return get_sectionByRva(rva, nfound);
}

SgAsmGenericSection *
SgAsmGenericHeader::get_sectionByRva(Rose::BinaryAnalysis::Address rva, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sectionsByRva(rva);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_sections_by_va(Rose::BinaryAnalysis::Address va, bool use_preferred) const
{
    return get_sectionsByVa(va, use_preferred);
}

SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_sectionsByVa(Rose::BinaryAnalysis::Address va, bool use_preferred) const
{
    if (use_preferred) {
        if (va < get_baseVa())
            return SgAsmGenericSectionPtrList();
        Rose::BinaryAnalysis::Address rva = va - get_baseVa();
        return get_sectionsByRva(rva);
    }
     
    SgAsmGenericSectionPtrList retval;
    for (size_t i=0; i<p_sections->get_sections().size(); i++) {
        SgAsmGenericSection *section = p_sections->get_sections()[i];
        if (section->isMapped() &&
            va>=section->get_mappedActualVa() && va<section->get_mappedActualVa()+section->get_mappedSize())
            retval.push_back(section);
    }
    return retval;
}

SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_va(Rose::BinaryAnalysis::Address va, bool use_preferred, size_t *nfound/*optional*/) const
{
    return get_sectionByVa(va, use_preferred, nfound);
}

SgAsmGenericSection *
SgAsmGenericHeader::get_sectionByVa(Rose::BinaryAnalysis::Address va, bool use_preferred, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sectionsByVa(va, use_preferred);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

SgAsmGenericSection *
SgAsmGenericHeader::get_best_section_by_va(Rose::BinaryAnalysis::Address va, bool use_preferred, size_t *nfound) const
{
    return get_bestSectionByVa(va, use_preferred, nfound);
}

SgAsmGenericSection *
SgAsmGenericHeader::get_bestSectionByVa(Rose::BinaryAnalysis::Address va, bool use_preferred, size_t *nfound) const
{
    const SgAsmGenericSectionPtrList &candidates = get_sectionsByVa(va, use_preferred);
    if (nfound) *nfound = candidates.size();
    return SgAsmGenericFile::bestSectionByVa(candidates, va);
}

void
SgAsmGenericHeader::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sHeader[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sHeader.", prefix);
    }
    const int w = std::max(size_t{1}, DUMP_FIELD_WIDTH - strlen(p));

    SgAsmGenericSection::dump(f, p, -1);

    ROSE_ASSERT(get_executableFormat() != NULL);
    get_executableFormat()->dump(f, p, -1);

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
    fprintf(f, "%s%-*s = 0x%08" PRIx64 " (%" PRIu64 ")\n", p, w, "base_va", get_baseVa(), get_baseVa());
    fprintf(f, "%s%-*s = %" PRIuPTR " entry points\n", p, w, "entry_rva.size", get_entryRvas().size());
    for (size_t i = 0; i < get_entryRvas().size(); i++) {
        char label[64];
        snprintf(label, sizeof(label), "entry_rva[%" PRIuPTR "]", i);
        Rose::BinaryAnalysis::Address entry_rva = get_entryRvas()[i].rva();
        fprintf(f, "%s%-*s = 0x%08" PRIx64 " (%" PRIu64 ")\n", p, w, label, entry_rva, entry_rva);
        SgAsmGenericSectionPtrList sections = get_file()->get_sections();
        dumpContainingSections(f, std::string(p)+label, entry_rva, sections);
    }

    fprintf(f, "%s%-*s = %" PRIuPTR " sections\n", p, w, "section", p_sections->get_sections().size());
    for (size_t i=0; i<p_sections->get_sections().size(); i++) {
        SgAsmGenericSection *section = p_sections->get_sections()[i];
        char label[1024];
        snprintf(label, sizeof(label), "section[%" PRIuPTR "]", i);
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, label, section->get_id(), section->get_name()->get_string(true).c_str());
    }
    
    fprintf(f, "%s%-*s = %" PRIuPTR " entries\n", p, w, "DLL.size", p_dlls->get_dlls().size());
    for (size_t i = 0; i < p_dlls->get_dlls().size(); i++)
        p_dlls->get_dlls()[i]->dump(f, p, i);
}

SgAsmGenericFormat*
SgAsmGenericHeader::get_exec_format() const {
    return get_executableFormat();
}

void
SgAsmGenericHeader::set_exec_format(SgAsmGenericFormat *x) {
    set_executableFormat(x);
}

Rose::BinaryAnalysis::Address
SgAsmGenericHeader::get_base_va() const {
    return get_baseVa();
}

void
SgAsmGenericHeader::set_base_va(Rose::BinaryAnalysis::Address x) {
    set_baseVa(x);
}

SgRVAList&
SgAsmGenericHeader::get_entry_rvas() {
    return get_entryRvas();
}

const SgRVAList&
SgAsmGenericHeader::get_entry_rvas() const {
    return get_entryRvas();
}

void
SgAsmGenericHeader::set_entry_rvas(const SgRVAList &x) {
    set_entryRvas(x);
}

void
SgAsmGenericHeader::add_entry_rva(const Rose::BinaryAnalysis::RelativeVirtualAddress &rva) {
    addEntryRva(rva);
}

void
SgAsmGenericHeader::addEntryRva(const Rose::BinaryAnalysis::RelativeVirtualAddress &rva) {
    p_entryRvas.push_back(rva);
}

#endif
