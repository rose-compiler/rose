/* Windows PE Sections (SgAsmPESection and related classes) */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/BinaryAnalysis/Hexdump.h>

SgAsmPESection::SgAsmPESection(SgAsmPEFileHeader *fhdr)
    : SgAsmGenericSection(fhdr->get_file(), fhdr) {
    set_section_entry(nullptr);
}

/* Pre-unparsing updates */
bool
SgAsmPESection::reallocate()
{
    bool reallocated = false;

    SgAsmPESectionTableEntry *shdr = get_section_entry();
    if (shdr)
        shdr->updateFromSection(this);
    
    return reallocated;
}

SgAsmPESection *
SgAsmPESection::init_from_section_table(SgAsmPESectionTableEntry *entry, int id)
{
    return initFromSectionTable(entry, id);
}

SgAsmPESection *
SgAsmPESection::initFromSectionTable(SgAsmPESectionTableEntry *entry, int id)
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
    set_fileAlignment(fhdr->get_e_file_align());

    /* Memory mapping */
    if (entry->get_rva() > 0) {
        set_mappedPreferredRva(entry->get_rva());
        set_mappedActualVa(0); /*will be asigned by BinaryLoader*/
        set_mappedSize(entry->get_virtual_size());
        set_mappedAlignment(fhdr->get_e_section_align());
        set_mappedReadPermission((entry->get_flags() & SgAsmPESectionTableEntry::OF_READABLE)
                         == SgAsmPESectionTableEntry::OF_READABLE);
        set_mappedWritePermission((entry->get_flags() & SgAsmPESectionTableEntry::OF_WRITABLE)
                         == SgAsmPESectionTableEntry::OF_WRITABLE);
        set_mappedExecutePermission((entry->get_flags() & SgAsmPESectionTableEntry::OF_EXECUTABLE)
                         == SgAsmPESectionTableEntry::OF_EXECUTABLE);
    } else {
        set_mappedPreferredRva(0);
        set_mappedActualVa(0); /*will be assigned by Loader*/
        set_mappedSize(0);
        set_mappedReadPermission(false);
        set_mappedWritePermission(false);
        set_mappedExecutePermission(false);
        set_mappedAlignment(fhdr->get_e_section_align());
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
        snprintf(p, sizeof(p), "%sPESection[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sPESection.", prefix);
    }

    SgAsmGenericSection::dump(f, p, -1);
    if (p_section_entry)
        p_section_entry->dump(f, p, -1);

    if (variantT() == V_SgAsmPESection) //unless a base class
        Rose::BinaryAnalysis::hexdump(f, 0, std::string(p)+"data at ", p_data);
}

#endif
