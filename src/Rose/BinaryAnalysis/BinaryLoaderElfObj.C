#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/BinaryLoaderElfObj.h>

#include <Rose/AST/Traversal.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>

#include <SgAsmElfFileHeader.h>
#include <SgAsmElfSection.h>
#include <SgAsmGenericSectionList.h>
#include <SgAsmGenericString.h>
#include <Cxx_GrammarDowncast.h>

namespace Rose {
namespace BinaryAnalysis {

/* This binary loader can handle ELF object files. */
bool
BinaryLoaderElfObj::canLoad(SgAsmGenericHeader *hdr) const {
    return isSgAsmElfFileHeader(hdr) && hdr->get_executableFormat()->get_purpose()==SgAsmExecutableFileFormat::PURPOSE_LIBRARY;
}

/* Same as parent, but also includes sections that aren't mapped but which contain code. */
SgAsmGenericSectionPtrList
BinaryLoaderElfObj::getRemapSections(SgAsmGenericHeader *header) {
    SgAsmGenericSectionPtrList retval = BinaryLoaderElf::getRemapSections(header);
    const SgAsmGenericSectionPtrList &sections = header->get_sections()->get_sections();
    for (SgAsmGenericSectionPtrList::const_iterator si=sections.begin(); si!=sections.end(); ++si) {
        if (SgAsmElfSection *section = isSgAsmElfSection(*si)) {
            std::string name = section->get_name() ? section->get_name()->get_string() : std::string();
            if (".text" == name || ".data" == name || ".rodata" == name || ".bss" == name)
                retval.push_back(section);
        }
    }
    return retval;
}

unsigned
BinaryLoaderElfObj::mappingPermissions(SgAsmGenericSection *section_) const {
    SgAsmElfSection *section = isSgAsmElfSection(section_);
    ASSERT_not_null(section);

    SgAsmElfSectionTableEntry *entry = section->get_sectionEntry();
    unsigned retval = BinaryLoaderElf::mappingPermissions(section);

    if (0 == retval && entry != NULL) {
        // By convention...
        if (section->get_name()->get_string() == ".text") {
            retval = MemoryMap::READ_EXECUTE;
        } else if (section->get_name()->get_string() == ".data") {
            retval = MemoryMap::READ_WRITE;
        } else if (section->get_name()->get_string() == ".bss") {
            retval = MemoryMap::READ_WRITE;
        } else if (section->get_name()->get_string() == ".rodata") {
            retval = MemoryMap::READABLE;
        }
    }

    return retval;
}

/* Identical to parent class but with this minor difference:  Any section that has no mapping information but is known to
 * contain code (SgAsmGenericSection::get_containsCode() is true) is mapped to an otherwise unused area of the virtual
 * memory. */
BinaryLoader::MappingContribution
BinaryLoaderElfObj::alignValues(SgAsmGenericSection *section, const MemoryMap::Ptr &map,
                                Address *malign_lo_p, Address *malign_hi_p,
                                Address *va_p, Address *mem_size_p,
                                Address *offset_p, Address *file_size_p, bool *map_private_p,
                                Address *va_offset_p, bool *anon_lo_p, bool *anon_hi_p,
                                ConflictResolution *resolve_p) {
    if (section->isMapped())
        return BinaryLoaderElf::alignValues(section, map, malign_lo_p, malign_hi_p, va_p, mem_size_p,
                                            offset_p, file_size_p, map_private_p, va_offset_p, anon_lo_p, anon_hi_p,
                                            resolve_p);

    if (section->get_containsCode() && section->get_size() > 0) {
        SgAsmGenericHeader *header = AST::Traversal::findParentTyped<SgAsmGenericHeader>(section);
        Address baseVa = header ? header->get_baseVa() : 0;

        /* We don't need to worry about file alignment because the Unix loader isn't going to ever be mapping this object file
         * anyway.  We align memory on our best guess of a page boundary, 4096 bytes. */
        Address mem_alignment = 4096;
        Address size = section->get_size();
        AddressInterval allocationRegion = AddressInterval::hull(baseVa, AddressInterval::whole().greatest());

        *malign_lo_p = *malign_hi_p = mem_alignment;
        *va_p = *(map->findFreeSpace(size, mem_alignment, allocationRegion));
        *mem_size_p = *file_size_p = size;
        *map_private_p = false;
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

} // namespace
} // namespace

#endif
