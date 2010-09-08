// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "Loader.h"
#include "LoaderPE.h"

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
rose_addr_t
LoaderPE::align_values(SgAsmGenericSection *section, rose_addr_t *va_p, rose_addr_t *mem_size_p,
                       rose_addr_t *offset_p, rose_addr_t *file_size_p, const MemoryMap*)
{
    SgAsmGenericHeader *header = section->get_header();
    ROSE_ASSERT(header!=NULL);

    if (!section->is_mapped()) {
        *va_p = *mem_size_p = *offset_p = *file_size_p = 0;
        return 0;
    }

    /* File and memory alignment must be between 1 and 0x200 (512), inclusive */
    rose_addr_t file_alignment = section->get_file_alignment();
    if (file_alignment>0x200 || 0==file_alignment)
        file_alignment = 0x200;
    rose_addr_t mapped_alignment = section->get_mapped_alignment();
    if (0==mapped_alignment)
        mapped_alignment = 0x200;

    /* Align file size upward even before we align the file offset downward. */
    rose_addr_t file_size = ALIGN_UP(section->get_size(), file_alignment);

    /* Map the entire section's file content (aligned) or the requested map size, whichever is larger. */

	rose_addr_t mapped_size = std::max(section->get_mapped_size(), file_size);

    /* Align file offset downward but do not adjust file size. */
    rose_addr_t file_offset = ALIGN_DN(section->get_offset(), file_alignment);

    /* Align the preferred relative virtual address downward without regard for the base virtual address, and do not adjust
     * mapped size. */
    rose_addr_t mapped_va = header->get_base_va() + ALIGN_DN(section->get_mapped_preferred_rva(), mapped_alignment);

    *va_p = mapped_va;
    *mem_size_p = mapped_size;
    *offset_p = file_offset;
    *file_size_p = mapped_size;

    /* Not sure about this; it should be the VA for the first byte of the section; see Loader.C version. [RPM 2009-09-11] */
    return header->get_base_va() + section->get_mapped_preferred_rva();
}
