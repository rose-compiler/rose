#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include "BinaryLoaderPe.h"
#include "Diagnostics.h"
#include "MemoryMap.h"

namespace Rose {
namespace BinaryAnalysis {

/* This binary loader can handle all PE files. */
bool
BinaryLoaderPe::canLoad(SgAsmGenericHeader *hdr) const {
    return isSgAsmPEFileHeader(hdr)!=NULL;
}

/* Returns sections to be mapped */
SgAsmGenericSectionPtrList
BinaryLoaderPe::getRemapSections(SgAsmGenericHeader *header) {
    SgAsmGenericSectionPtrList retval;

    /* The NT loader always loads the PE header, so we include that first in the list. */
    retval.push_back(header);

    /* Add the sections in the order they appear in the section table */
    const SgAsmGenericSectionPtrList &sections = header->get_sections()->get_sections();
    std::map<int, SgAsmGenericSection*> candidates;
    for (size_t i=0; i<sections.size(); i++) {
        if (sections[i]->get_id()>=0) {
            ASSERT_require(candidates.find(sections[i]->get_id())==candidates.end());
            candidates.insert(std::make_pair(sections[i]->get_id(), sections[i]));
        }
    }
    for (std::map<int, SgAsmGenericSection*>::iterator ci=candidates.begin(); ci!=candidates.end(); ++ci) {
        retval.push_back(ci->second);
    }
    return retval;
}

/* This algorithm was implemented based on an e-mail from Cory Cohen at CERT and inspection of PE::ConvertRvaToFilePosition()
 * as defined in "PE.cpp 2738 2009-06-05 15:09:11Z murawski_dev". [RPM 2009-08-17] */
BinaryLoader::MappingContribution
BinaryLoaderPe::alignValues(SgAsmGenericSection *section, const MemoryMap::Ptr &map,
                            rose_addr_t *malign_lo_p, rose_addr_t *malign_hi_p,
                            rose_addr_t *va_p, rose_addr_t *mem_size_p,
                            rose_addr_t *offset_p, rose_addr_t *file_size_p, bool *map_private_p,
                            rose_addr_t *va_offset_p, bool *anon_lo_p, bool *anon_hi_p,
                            ConflictResolution *resolve_p) {
    SgAsmGenericHeader *header = isSgAsmPEFileHeader(section);
    if (!header) header = section->get_header();
    ASSERT_not_null(header);

    if (!section->is_mapped())
        return CONTRIBUTE_NONE;

    /* File and memory alignment must be between 1 and 0x200 (512), inclusive */
    rose_addr_t file_alignment = section->get_file_alignment();
    if (file_alignment>0x200 || 0==file_alignment)
        file_alignment = 0x200;
    rose_addr_t mapped_alignment = section->get_mapped_alignment();
    if (0==mapped_alignment)
        mapped_alignment = 0x200;

    /* Align file size upward even before we align the file offset downward. */
    rose_addr_t file_size = alignUp(section->get_size(), file_alignment);

    /* Map the entire section's file content (aligned) or the requested map size, whichever is larger. */
    rose_addr_t mapped_size = std::max(section->get_mapped_size(), file_size);

    /* Align file offset downward but do not adjust file size. */
    rose_addr_t file_offset = alignDown(section->get_offset(), file_alignment);

    /* Align the preferred relative virtual address downward without regard for the base virtual address, and do not adjust
     * mapped size. */
    rose_addr_t mapped_va = header->get_base_va() + alignDown(section->get_mapped_preferred_rva(), mapped_alignment);

    *malign_lo_p = mapped_alignment;
    *malign_hi_p = 1;
    *va_p = mapped_va;
    *mem_size_p = mapped_size;
    *offset_p = file_offset;
    *file_size_p = file_size;
    *map_private_p = false;
    *va_offset_p = 0;
    *anon_lo_p = *anon_hi_p = true;
    *resolve_p = RESOLVE_OVERMAP;
    return CONTRIBUTE_ADD;
}

} // namespace
} // namespace

#endif
