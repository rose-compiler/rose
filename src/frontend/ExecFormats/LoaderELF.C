#include "rose.h"

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

/* Load all sections containing code.  Any section marked as executable or explicitly containing code is added to the map. Any
 * ELF Section (but not ELF Segment) that is not added will be subtracted.  Therefore, since ELF Segments are processed before
 * ELF Segments, we first load the ELF Segments and then any ELF Section that doesn't contain code is subtracted out of the
 * containing ELF Segments' virtual address space. */
MemoryMap *
LoaderELF::load_code_sections(const SgAsmGenericSectionPtrList &sections, bool allow_overmap)
{
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
    return create_map(sections, &s1, allow_overmap);
}
