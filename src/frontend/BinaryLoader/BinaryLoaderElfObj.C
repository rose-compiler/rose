#include "sage3basic.h"
#include "BinaryLoaderElfObj.h"

/* This binary loader can handle ELF object files. */
bool
BinaryLoaderElfObj::can_load(SgAsmGenericHeader *hdr) const
{
    return isSgAsmElfFileHeader(hdr) && hdr->get_exec_format()->get_purpose()==SgAsmExecutableFileFormat::PURPOSE_LIBRARY;
}

/* Same as parent, but also includes sections that aren't mapped but which contain code. */
SgAsmGenericSectionPtrList
BinaryLoaderElfObj::get_remap_sections(SgAsmGenericHeader *header)
{
    SgAsmGenericSectionPtrList retval = BinaryLoaderElf::get_remap_sections(header);
    const SgAsmGenericSectionPtrList &sections = header->get_sections()->get_sections();
    for (SgAsmGenericSectionPtrList::const_iterator si=sections.begin(); si!=sections.end(); ++si) {
        SgAsmGenericSection *section = *si;
        if (!section->is_mapped() && section->get_contains_code())
            retval.push_back(section);
    }
    return retval;
}

/* Identical to parent class but with this minor difference:  Any section that has no mapping information but is known to
 * contain code (SgAsmGenericSection::get_contains_code() is true) is mapped to an otherwise unused area of the virtual
 * memory. */
BinaryLoader::MappingContribution
BinaryLoaderElfObj::align_values(SgAsmGenericSection *section, MemoryMap *map,
                                 rose_addr_t *malign_lo_p, rose_addr_t *malign_hi_p,
                                 rose_addr_t *va_p, rose_addr_t *mem_size_p,
                                 rose_addr_t *offset_p, rose_addr_t *file_size_p,
                                 rose_addr_t *va_offset_p, bool *anon_lo_p, bool *anon_hi_p, 
                                 ConflictResolution *resolve_p)
{
    if (section->is_mapped())
        return BinaryLoaderElf::align_values(section, map, malign_lo_p, malign_hi_p, va_p, mem_size_p,
                                             offset_p, file_size_p, va_offset_p, anon_lo_p, anon_hi_p,
                                             resolve_p);
    
    if (section->get_contains_code()) {
        /* We don't need to worry about file alignment because the Unix loader isn't going to ever be mapping this object file
         * anyway.  We align memory on our best guess of a page boundary, 4096 bytes. */
        rose_addr_t mem_alignment = 4096;
        rose_addr_t size = section->get_size();

        *malign_lo_p = *malign_hi_p = mem_alignment;
        *va_p = map->find_free(0, size, mem_alignment);
        *mem_size_p = *file_size_p = size;
        *offset_p = section->get_offset();
        *va_offset_p = 0;
        *anon_lo_p = *anon_hi_p = true;
        *resolve_p = RESOLVE_THROW; /*shouldn't have a conflict because we found free space*/
        return CONTRIBUTE_ADD;
    }

    /* Not mapped and not known to contain code, so it should not contribute to the final map. No need to set any of the
     * return values in this case. */
    return CONTRIBUTE_NONE;
}
