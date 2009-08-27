#include "rose.h"

/* Returns sections in order of their definition in the PE Section Table */
SgAsmGenericSectionPtrList
LoaderPE::order_sections(const SgAsmGenericSectionPtrList &sections) 
{
    SgAsmGenericSectionPtrList retval;
    std::map<int, SgAsmGenericSection*> candidates;
    for (size_t i=0; i<sections.size(); i++) {
        if (sections[i]->get_id()>=0) {
            ROSE_ASSERT(candidates.find(sections[i]->get_id())==candidates.end());
            candidates.insert(std::make_pair(sections[i]->get_id(), sections[i]));
        }
    }
    for (std::map<int, SgAsmGenericSection*>::iterator ci=candidates.begin(); ci!=candidates.end(); ++ci) {
        retval.push_back(ci->second);
    }
    return retval;
}

/* This algorithm was implemented based on an e-mail from Cory Cohen at CERT. [RPM 2009-08-17] */
void
LoaderPE::align_values(SgAsmGenericSection *section, rose_addr_t *va, rose_addr_t *size, rose_addr_t *offset)
{
    SgAsmGenericHeader *header = section->get_header();
    ROSE_ASSERT(header!=NULL);
    ROSE_ASSERT(section->is_mapped());

    rose_addr_t file_alignment = section->get_file_alignment();
    if (file_alignment>0x200 || 0==file_alignment)
        file_alignment = 0x200;

    rose_addr_t mapped_alignment = section->get_mapped_alignment();
    if (0==mapped_alignment)
        mapped_alignment = 0x200;

    rose_addr_t file_size = ALIGN_UP(section->get_size(), file_alignment);

    rose_addr_t mapped_size = section->get_mapped_size();
    if (file_size > mapped_size)
        mapped_size = file_size;

    rose_addr_t file_offset = ALIGN_DN(section->get_offset(), file_alignment);

    rose_addr_t mapped_va = header->get_base_va() + ALIGN_DN(section->get_mapped_rva(), mapped_alignment);

    *va = mapped_va;
    *size = mapped_size;
    *offset = file_offset;
}
