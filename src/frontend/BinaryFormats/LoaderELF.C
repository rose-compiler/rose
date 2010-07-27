// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "Loader.h"
#include "LoaderELF.h"
/* Returns ELF Segments followed by ELF Sections */
SgAsmGenericSectionPtrList
LoaderELF::order_sections(const SgAsmGenericSectionPtrList &sections)
{
    SgAsmGenericSectionPtrList retval;
    for (int pass=0; pass<2; pass++) {
        for (size_t i=0; i<sections.size(); i++) {
            SgAsmElfSection *section = isSgAsmElfSection(sections[i]);
            if (!section)
                continue;
            if (0==pass && section->get_segment_entry()!=NULL) {
                retval.push_back(section);
            } else if (1==pass && section->get_section_entry()!=NULL) {
                retval.push_back(section);
            }
        }
    }
    return retval;
}

/* Same as superclass, but if we are mapping/unmapping an ELF Section (that is not an ELF Segment) then don't bother to align
 * it. This is used for ELF code mapping because code is mapped by ELF Segments and then the ELF Sections fine tune it. */
rose_addr_t
LoaderELF::align_values(SgAsmGenericSection *_section, Contribution contrib,
                           rose_addr_t *va_p/*out*/, rose_addr_t *mem_size_p/*out*/,
                           rose_addr_t *offset_p/*out*/, rose_addr_t *file_size_p/*out*/,
                           const MemoryMap *current)
{
    SgAsmElfSection *section = isSgAsmElfSection(_section);

    rose_addr_t retval = 0;
    rose_addr_t old_va_align = section->get_mapped_alignment();

    try {
        if (NULL==section->get_segment_entry()) {
            if (get_debug())
                fprintf(get_debug(), "    Temporarily relaxing memory alignment constraints.\n");
            section->set_mapped_alignment(1);
        }
        retval = Loader::align_values(section, contrib, va_p, mem_size_p, offset_p, file_size_p, current);
    } catch (...) {
        section->set_mapped_alignment(old_va_align);
        throw;
    }
    section->set_mapped_alignment(old_va_align);
    return retval;
}

/* Load all sections containing code.  Any section marked as executable or explicitly containing code is added to the map. Any
 * ELF Section (but not ELF Segment) that is not added will be subtracted.  Therefore, since ELF Segments are processed before
 * ELF Sections, we first load the ELF Segments and then any ELF Section that doesn't contain code is subtracted out of the
 * containing ELF Segments' virtual address space. */
MemoryMap *
LoaderELF::load_code_sections(MemoryMap *map, const SgAsmGenericSectionPtrList &sections, bool allow_overmap)
{
#ifdef USE_ROSE
   return NULL;
#else
    struct: public Selector {
        Contribution contributes(SgAsmGenericSection *section) {
            SgAsmElfSection *s = isSgAsmElfSection(section);
            if (!s) {
                return CONTRIBUTE_NONE;
            } else if (s->get_contains_code() || (s->is_mapped() && section->get_mapped_xperm())) {
                return CONTRIBUTE_ADD;
            } else if (s->get_segment_entry()==NULL) {
                return CONTRIBUTE_SUB;
            } else {
                return CONTRIBUTE_NONE;
            }
        }
    } s1;
    return create_map(map, sections, &s1, allow_overmap);
#endif
}
