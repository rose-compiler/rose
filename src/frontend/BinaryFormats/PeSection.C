/* Windows PE Sections (SgAsmPESection and related classes) */

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* Pre-unparsing updates */
bool
SgAsmPESection::reallocate()
{
    bool reallocated = false;

    SgAsmPESectionTableEntry *shdr = get_section_entry();
    if (shdr)
        shdr->update_from_section(this);
    
    return reallocated;
}

/** Initializes the section from data parsed from the PE Section Table. This includes the section offset, size, memory mapping,
 *  alignments, permissions, etc. This function complements SgAsmPESectionTable::add_section(): this function initializes this
 *  section from the section table while add_section() initializes the section table from the section. */
SgAsmPESection *
SgAsmPESection::init_from_section_table(SgAsmPESectionTableEntry *entry, int id)
{
    ROSE_ASSERT(entry);
    ROSE_ASSERT(id>=0);
    
    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);

    set_synthesized(false);
    set_name(new SgAsmBasicString(entry->get_name()));
    set_id(id);
    set_purpose(SP_PROGRAM);

    /* File mapping */
    set_offset(entry->get_physical_offset());
    set_size(entry->get_physical_size());
    set_file_alignment(fhdr->get_e_file_align());

    /* Memory mapping */
    if (entry->get_rva() > 0) {
        set_mapped_preferred_rva(entry->get_rva());
        set_mapped_actual_va(0); /*will be asigned by Loader*/
        set_mapped_size(entry->get_virtual_size());
        set_mapped_alignment(fhdr->get_e_section_align());
        set_mapped_rperm((entry->get_flags() & SgAsmPESectionTableEntry::OF_READABLE)
                         == SgAsmPESectionTableEntry::OF_READABLE);
        set_mapped_wperm((entry->get_flags() & SgAsmPESectionTableEntry::OF_WRITABLE)
                         == SgAsmPESectionTableEntry::OF_WRITABLE);
        set_mapped_xperm((entry->get_flags() & SgAsmPESectionTableEntry::OF_EXECUTABLE)
                         == SgAsmPESectionTableEntry::OF_EXECUTABLE);
    } else {
        set_mapped_preferred_rva(0);
        set_mapped_actual_va(0); /*will be assigned by Loader*/
        set_mapped_size(0);
        set_mapped_rperm(false);
        set_mapped_wperm(false);
        set_mapped_xperm(false);
        set_mapped_alignment(fhdr->get_e_section_align());
    }
    
    /* Add section table entry to section */
    set_section_entry(entry);
    entry->set_parent(this);

    return this;
}

/* Print some debugging info. */
void
SgAsmPESection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPESection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPESection.", prefix);
    }

    SgAsmGenericSection::dump(f, p, -1);
    if (p_section_entry)
        p_section_entry->dump(f, p, -1);

    if (variantT() == V_SgAsmPESection) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}
